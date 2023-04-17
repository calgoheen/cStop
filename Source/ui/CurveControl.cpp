#include "CurveControl.h"
#include "../dsp/TapeStop.h"

Plot::Plot(juce::Colour bgColour, juce::Colour pathColour, juce::Colour gridColour,
    float pathStrokeSize, float gridStrokeSize, float numGridDivisions)
    : bgColour(bgColour),
    pathColour(pathColour),
    gridColour(gridColour),
    pathStrokeSize(pathStrokeSize),
    gridStrokeSize(gridStrokeSize),
    gridDivisor(numGridDivisions + 1) 
{
}

void Plot::paint(juce::Graphics& g) 
{
    g.fillAll(bgColour);

    auto width = getWidth();
    auto height = getHeight();
    auto xInc = (float)width / gridDivisor;
    auto yInc = (float)height / gridDivisor;

    g.setColour(gridColour);
    g.setOpacity(0.65);

    for (float x = 0; x <= width; x += xInc)
        g.drawLine(x, 0.0f, x, height, 1.0f);

    for (float y = 0; y <= height; y += yInc)
        g.drawLine(0.0f, y, width, y, 1.0f);

    g.setColour(pathColour);
    g.setOpacity(1.0);
    g.strokePath(path, juce::PathStrokeType(pathStrokeSize,
        juce::PathStrokeType::JointStyle::curved));
}

void Plot::setPathStrokeSize(float size)
{
    pathStrokeSize = size;
    repaint();
}

void Plot::setGridStrokeSize(float size) 
{
    gridStrokeSize = size;
    repaint();
}

void Plot::setPathColour(juce::Colour colour)
{
    pathColour = colour;
    repaint();
}

void Plot::setGridColour(juce::Colour colour)
{
    gridColour = colour;
    repaint();
}

void Plot::setBackgroundColour(juce::Colour colour) 
{
    bgColour = colour;
    repaint();
}

void Plot::transformPath() 
{
    path.scaleToFit(2, 2, getWidth() - 4, getHeight() - 4, false);
    repaint();
}

void Plot::setPath(const juce::Path& p) 
{
    path = p;
    transformPath();
}

void Plot::resized() 
{
    transformPath();
}

Dot::Dot() 
{
    addMouseListener(this, true);
}

void Dot::paint(juce::Graphics& g) 
{
    auto thumbColour = UI::CurveControl::thumbColour;

    switch (state) {
    case Dot::State::IDLE:
        g.setColour(thumbColour);
        break;

    case Dot::State::OVER:
        g.setColour(thumbColour.brighter(0.5));
        break;

    case Dot::State::DOWN:
        g.setColour(thumbColour.darker(0.25));
        break;
    }

    g.drawEllipse(1, 1, size - 2, size - 2, 1.5);
    g.fillEllipse(4, 4, size - 8, size - 8);
}

void Dot::mouseEnter(const juce::MouseEvent& e)
{
    state = Dot::State::OVER;
    repaint();
}

void Dot::mouseExit(const juce::MouseEvent& e)
{
    state = Dot::State::IDLE;
    repaint();
}

void Dot::mouseDown(const juce::MouseEvent& e)
{
    state = Dot::State::DOWN;
    repaint();
}

void Dot::mouseUp(const juce::MouseEvent& e)
{
    if (state == State::DOWN)
    {
        state = State::OVER;
        repaint();
    }
}

void Dot::resized()
{
    size = getWidth() - 2;
}

void Endpoints::paint(juce::Graphics& g)
{
    g.setColour(UI::CurveControl::highlightColour);
    g.setOpacity(UI::CurveControl::highlightOpacity);

    int w = getWidth();
    int leftBound = std::round(_left * w);
    int rightBound = std::round(_right * w);

    g.fillRect(0, 0, leftBound, getHeight());
    g.fillRect(rightBound, 0, getWidth() - rightBound, getHeight());

    g.setColour(UI::CurveControl::outlineColour);
    g.drawRect(getLocalBounds(), 2.0f);
}

void Endpoints::setLeftEndpoint(float newValue)
{
    _left = newValue;
    repaint();
}
void Endpoints::setRightEndpoint(float newValue)
{
    _right = newValue;
    repaint();
}

CurveControl::CurveControl(bool isSlowdown)
    : _plot(UI::CurveControl::bgColour,
        UI::CurveControl::pathColour,
        UI::CurveControl::gridColour,
        UI::CurveControl::pathStrokeSize,
        UI::CurveControl::gridStrokeSize,
        UI::CurveControl::numGridDivisions),
    _isSlowdownCurve(isSlowdown)
{
    addAndMakeVisible(&_plot);
    addAndMakeVisible(&_endpoints);

    addAndMakeVisible(&_curveSlider);
    _curveSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    makeSliderTransparent(&_curveSlider);

    addAndMakeVisible(&_leftDot);
    _leftDot.addMouseListener(this, false);

    addAndMakeVisible(&_rightDot);
    _rightDot.addMouseListener(this, false);

    _curveValues.resize(_numPoints);
}

void CurveControl::setCurve(float curveFactor, int direction)
{
    auto ptr = _curveValues.data();
    auto alpha = TapeStop::calculateAlpha(curveFactor);

    if (direction == -1)
        for (int i = 0; i < _numPoints; i++)
            ptr[i] = TapeStop::applyExpScale(1 - float(i) / _numPoints, alpha);

    else if (direction == 1)
        for (int i = 0; i < _numPoints; i++)
            ptr[i] = TapeStop::applyExpScale(float(i) / _numPoints, alpha);

    arrayToPath(ptr);
}

void CurveControl::attachParameters(juce::RangedAudioParameter& startParam,
    juce::RangedAudioParameter& endParam,
    juce::RangedAudioParameter& curveParam)
{
    _curveStartAttachment.reset(new juce::ParameterAttachment(startParam,
        [this](float x) { setLeftEndpoint(x); }));
    _curveStartAttachment->sendInitialUpdate();

    _curveEndAttachment.reset(new juce::ParameterAttachment(endParam,
        [this](float x) { setRightEndpoint(x); }));
    _curveEndAttachment->sendInitialUpdate();

    _curveAttachment.reset(new juce::SliderParameterAttachment(curveParam, _curveSlider));
    _curveSlider.onValueChange = [this]() {
        setCurve(_curveSlider.getValue(), 1 - 2 * _isSlowdownCurve);
    };
    _curveAttachment->sendInitialUpdate();
    _curveSlider.onValueChange();
}

void CurveControl::arrayToPath(float* values)
{
    juce::Path path;
    path.startNewSubPath(0, values[0]);

    for (int i = 0; i < _numPoints; i++)
        path.lineTo(i, values[i]);

    auto height = path.getBounds().getHeight();
    path.applyTransform(juce::AffineTransform::verticalFlip(height));

    _plot.setPath(path);
}

void CurveControl::mouseDown(const juce::MouseEvent& e)
{
    if (e.eventComponent == &_leftDot)
        _curveStartAttachment->beginGesture();
    else if (e.eventComponent == &_rightDot)
        _curveEndAttachment->beginGesture();
}

void CurveControl::mouseUp(const juce::MouseEvent& e)
{
    if (e.eventComponent == &_leftDot)
        _curveStartAttachment->endGesture();
    else if (e.eventComponent == &_rightDot)
        _curveEndAttachment->endGesture();
}

void CurveControl::mouseDrag(const juce::MouseEvent& e)
{
    auto newEvent = e.getEventRelativeTo(&_endpoints);

    int pos = newEvent.getPosition().getX();
    auto d = UI::CurveControl::dotSize;

    if (e.eventComponent == &_leftDot)
    {
        if (pos < 0)
            pos = 0;
        else if (pos > _rightDotPos - (d / 2 + 1))
            pos = _rightDotPos - (d / 2 + 1);

        auto relativePos = static_cast<float>(pos) / static_cast<float>(_endpoints.getWidth());
        setLeftEndpoint(relativePos);
    }
    else if (e.eventComponent == &_rightDot)
    {
        if (pos < _leftDotPos + (d / 2 + 1))
            pos = _leftDotPos + (d / 2 + 1);
        else if (pos > _endpoints.getWidth())
            pos = _endpoints.getWidth();

        auto relativePos = static_cast<float>(pos) / static_cast<float>(_endpoints.getWidth());
        setRightEndpoint(relativePos);
    }
}

void CurveControl::setLeftEndpoint(float newValue)
{
    _leftDotPosRelative = newValue;
    _leftDotPos = newValue * _endpoints.getWidth();
    _leftDot.setBounds(getDotBounds(_leftDotPos));
    _endpoints.setLeftEndpoint(newValue);

    if (!_ignoreCallbacksLeft)
    {
        const juce::ScopedValueSetter<bool> svs(_ignoreCallbacksLeft, true);
        _curveStartAttachment->setValueAsPartOfGesture(newValue);
    }
}

void CurveControl::setRightEndpoint(float newValue)
{
    _rightDotPosRelative = newValue;
    _rightDotPos = newValue * _endpoints.getWidth();
    _rightDot.setBounds(getDotBounds(_rightDotPos));
    _endpoints.setRightEndpoint(newValue);

    if (!_ignoreCallbacksRight)
    {
        const juce::ScopedValueSetter<bool> svs(_ignoreCallbacksRight, true);
        _curveEndAttachment->setValueAsPartOfGesture(newValue);
    }
}

juce::Rectangle<int> CurveControl::getDotBounds(int screenPos) const
{
    auto d = UI::CurveControl::dotSize;
    auto buff = UI::CurveControl::plotBufferX;

    return {
        screenPos - d / 2 + 1 + buff,
        getHeight() / 2 - d / 2 + 1, 
        d, 
        d
    };
}

void CurveControl::resized()
{
    auto area = getLocalBounds();
    auto plotArea = area.reduced(UI::CurveControl::plotBufferX,
        UI::CurveControl::plotBufferY);

    _plot.setBounds(plotArea);
    _curveSlider.setBounds(plotArea);
    _endpoints.setBounds(plotArea);

    _leftDotPos = _leftDotPosRelative * _endpoints.getWidth();
    _leftDot.setBounds(getDotBounds(_leftDotPos));

    _rightDotPos = _rightDotPosRelative * _endpoints.getWidth();
    _rightDot.setBounds(getDotBounds(_rightDotPos));

    setCurve(_curveSlider.getValue(), 1 - 2 * _isSlowdownCurve);
}

void CurveControl::makeSliderTransparent(juce::Slider* slider)
{
    juce::Colour c{ juce::Colours::transparentBlack };

    slider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    slider->setColour(juce::Slider::backgroundColourId, c);
    slider->setColour(juce::Slider::trackColourId, c);
    slider->setColour(juce::Slider::rotarySliderFillColourId, c);
    slider->setColour(juce::Slider::rotarySliderOutlineColourId, c);
    slider->setColour(juce::Slider::thumbColourId, c);
}
