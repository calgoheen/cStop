#include "Components.h"

void CTextButton::paintButton(juce::Graphics& g, bool mouseIsOver, bool mouseIsDown) {
    int offset = 0;
    int shadowOffset = 6;

    juce::Colour fill, outline, text;

    if (mouseIsOver && !mouseIsDown) 
    {
        fill = UI::TextButton::overFillColour;
        outline = UI::TextButton::overOutlineColour;
        text = UI::TextButton::overTextColour;
    }
    else if (mouseIsDown) 
    {
        offset = 3;
        fill = UI::TextButton::downFillColour;
        outline = UI::TextButton::downOutlineColour;
        text = UI::TextButton::downTextColour;
    }
    else 
    {
        fill = UI::TextButton::idleFillColour;
        outline = UI::TextButton::idleOutlineColour;
        text = UI::TextButton::idleTextColour;
    }

    auto width = getWidth();
    auto height = getHeight();

    g.setColour(UI::TextButton::shadowColour);
    g.fillRoundedRectangle(0.0f, (shadowOffset + offset), width, height - (shadowOffset + offset), 5.0f);

    auto fillArea = juce::Rectangle<float>(0.0f, offset, width, height - shadowOffset);
    auto outlineArea = juce::Rectangle<float>(2.0f, 2.0f + offset, width - 4, height - (shadowOffset + 4));

    g.setColour(fill);
    g.fillRoundedRectangle(fillArea, 5.0f);

    g.setColour(outline);
    g.drawRoundedRectangle(outlineArea, 4.0f, 1.5f);

    g.setColour(text);
    g.drawText(getButtonText(), outlineArea, juce::Justification::centred, false);
}

CSlider::CSlider() 
{
    setSliderStyle(CSlider::SliderStyle::RotaryVerticalDrag);
    setTextBoxStyle(CSlider::TextEntryBoxPosition::TextBoxBelow,
        true,
        UI::Slider::valueBoxWidth,
        UI::Slider::valueBoxHeight);
    setColour(CSlider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(CSlider::ColourIds::textBoxTextColourId, UI::Slider::valueBoxTextColour);
}

void CSlider::paint(juce::Graphics& g) {
    auto sliderPos = valueToProportionOfLength(getValue());
    auto rotaryParams = getRotaryParameters();
    auto bounds = getLocalBounds().withTrimmedBottom(UI::Slider::valueBoxHeight).withTop(6);
    auto arcRadius = std::min(bounds.getWidth(), bounds.getHeight()) / 2.0f - 4;

    juce::Path bottomArc, topArc;

    bottomArc.addCentredArc(bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryParams.startAngleRadians,
        rotaryParams.endAngleRadians,
        true);

    auto topArcEndAngle = sliderPos * (rotaryParams.endAngleRadians - rotaryParams.startAngleRadians)
        + rotaryParams.startAngleRadians;

    topArc.addCentredArc(bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryParams.startAngleRadians,
        topArcEndAngle,
        true);
    topArc.lineTo(bounds.getCentreX(), bounds.getCentreY());

    g.setColour(UI::Slider::bottomArcColour);
    g.strokePath(bottomArc, juce::PathStrokeType(UI::Slider::strokeSize,
        juce::PathStrokeType::JointStyle::curved));

    g.setColour(UI::Slider::topArcColour);
    g.strokePath(topArc, juce::PathStrokeType(UI::Slider::strokeSize,
        juce::PathStrokeType::JointStyle::curved));
}

void CToggleButton::paintButton(juce::Graphics& g, bool mouseIsOver, bool mouseIsDown) 
{
    juce::Colour fill, outline, text;
    auto bounds = getLocalBounds();
    juce::Rectangle<int> textArea, boxArea;

    auto size = UI::ToggleButton::boxSize;

    if (isVertical) 
    {
        textArea = bounds.removeFromTop(15);
        boxArea = bounds.withSizeKeepingCentre(size, size).reduced(2);
    }
    else 
    {
        boxArea = bounds.withSize(size, size).reduced(2);
        textArea = bounds.withTrimmedLeft(size + 2);
    }

    if (mouseIsOver && !mouseIsDown) 
    {
        fill = UI::ToggleButton::overFillColour;
        outline = UI::ToggleButton::overOutlineColour;
        text = UI::ToggleButton::overTextColour;
    }
    else if (mouseIsDown) 
    {
        fill = UI::ToggleButton::downFillColour;
        outline = UI::ToggleButton::downOutlineColour;
        text = UI::ToggleButton::downTextColour;
    }
    else 
    {
        fill = UI::ToggleButton::idleFillColour;
        outline = UI::ToggleButton::idleOutlineColour;
        text = UI::ToggleButton::idleTextColour;
    }

    g.setColour(text);

    if (isVertical) 
        g.drawText(getButtonText(), textArea, juce::Justification::centred);
    else 
        g.drawText(getButtonText(), textArea, juce::Justification::centredLeft);

    if (getToggleState() == true)
        g.setColour(UI::ToggleButton::checkmarkColour);
    else
        g.setColour(fill);

    g.fillRoundedRectangle(boxArea.getX(),
        boxArea.getY(),
        boxArea.getWidth(),
        boxArea.getHeight(),
        4.0f);

    g.setColour(outline);

    g.drawRoundedRectangle(boxArea.getX(),
        boxArea.getY(),
        boxArea.getWidth(),
        boxArea.getHeight(),
        3.0f,
        UI::ToggleButton::outlineStrokeSize);
}

CRadioGroup::CRadioGroup(int numberOfButtons, int radioGroupId)
{
    this->numberOfButtons = numberOfButtons;

    for (int i = 0; i < numberOfButtons; i++) 
    {
        toggleButtons.add(new CToggleButton());
        toggleButtons[i]->makeHorizontal();
        toggleButtons[i]->setRadioGroupId(radioGroupId);
        addAndMakeVisible(toggleButtons[i]);
    }
}

void CRadioGroup::resized()
{
    auto area = getLocalBounds().withSizeKeepingCentre(toggleButtonWidth,
        numberOfButtons * (toggleButtonHeight + 1));

    for (int i = 0; i < numberOfButtons; i++)
        toggleButtons[i]->setBounds(area.removeFromTop(toggleButtonHeight + 1));
}

CToggleButton* CRadioGroup::getToggleButton(int index)
{
    return toggleButtons[index];
}

void SemiTransparentBox::setColour(juce::Colour c)
{
    colour = c;
}

void SemiTransparentBox::setAlpha(float a)
{
    alpha = a;
}

void SemiTransparentBox::paint(juce::Graphics& g)
{
    g.setColour(colour.withAlpha(alpha));
    g.fillRoundedRectangle(0.0f, 0.0f, width, height, 5.0f);
}

void SemiTransparentBox::resized()
{
    height = getHeight();
    width = getWidth();
}

Knob::Knob()
    : labelFontSize(UI::Knob::fontSize),
    labelColour(UI::Knob::textColour)
{
    label.setFont(labelFontSize);
    label.setColour(juce::Label::ColourIds::textColourId, labelColour);
    label.setJustificationType(juce::Justification::centredBottom);

    slider.reset(new CSlider());
    
    addAndMakeVisible(label);
    addAndMakeVisible(*slider);
}

void Knob::attachParameter(chowdsp::FloatParameter& param, chowdsp::PluginState& state)
{
    attachment.emplace(param, state, *slider);
}

void Knob::resized()
{
    auto area = getLocalBounds().removeFromTop(UI::Knob::totalHeight)
        .removeFromLeft(UI::Knob::totalWidth);

    labelBounds = area.removeFromTop(UI::Knob::labelHeight);
    sliderBounds = area.removeFromTop(UI::Knob::sliderHeight);

    label.setBounds(labelBounds);
    slider->setBounds(sliderBounds);
}
