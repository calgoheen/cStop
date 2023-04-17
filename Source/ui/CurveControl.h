#pragma once

#include <JuceHeader.h>
#include "Constants.h"

class Plot : public juce::Component
{
public:
    Plot(juce::Colour bgColour, juce::Colour pathColour, juce::Colour gridColour,
        float pathStrokeSize, float gridStrokeSize, float numGridDivisions);
    ~Plot() = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setPathStrokeSize(float size);
    void setGridStrokeSize(float size);
    void setPathColour(juce::Colour colour);
    void setGridColour(juce::Colour colour);
    void setBackgroundColour(juce::Colour colour);

    void setPath(const juce::Path& p);

private:
    void transformPath();

    juce::Path path;
    juce::Colour bgColour;
    juce::Colour pathColour;
    juce::Colour gridColour;

    float pathStrokeSize;
    float gridStrokeSize;
    float gridDivisor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Plot)
};

class Dot : public juce::Component
{
public:
    enum class State { IDLE = 0, OVER = 1, DOWN = 2 };

    Dot();
    ~Dot() = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

private:
    int size = 0;
    Dot::State state = Dot::State::IDLE;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Dot)
};

class Endpoints : public juce::Component
{
public:
    Endpoints() = default;
    ~Endpoints() = default;

    void paint(juce::Graphics& g) override;

    void setLeftEndpoint(float newValue);
    void setRightEndpoint(float newValue);

private:
    float _left{ 0.0f };
    float _right{ 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Endpoints)
};

class CurveControl : public juce::Component
{
public:
    CurveControl(bool isSlowdown);
    ~CurveControl() = default;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void resized() override;
    
    void setCurve(float curveFactor, int direction);
    
    void attachParameters(juce::RangedAudioParameter& startParam,
                          juce::RangedAudioParameter& endParam,
                          juce::RangedAudioParameter& curveParam);
    
private:
    void makeSliderTransparent(juce::Slider* slider);
    void setLeftEndpoint(float newValue);
    void setRightEndpoint(float newValue);
    juce::Rectangle<int> getDotBounds(int screenPos) const;
    void arrayToPath(float* values);

    const bool _isSlowdownCurve;
    
    Plot _plot;
    std::vector<float> _curveValues;
    juce::Slider _curveSlider;
    
    Endpoints _endpoints;
    Dot _leftDot, _rightDot;

    std::unique_ptr<juce::ParameterAttachment> _curveStartAttachment;
    std::unique_ptr<juce::ParameterAttachment> _curveEndAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> _curveAttachment;
    
    bool _ignoreCallbacksLeft{ false };
    bool _ignoreCallbacksRight{ false };
    
    float _leftDotPosRelative{ 0.0f };
    float _rightDotPosRelative{ 1.0f };
    int _leftDotPos;
    int _rightDotPos;
    int _numPoints{ 512 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CurveControl)
};
