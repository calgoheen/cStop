#pragma once

#include <JuceHeader.h>
#include "Constants.h"

/** This file contains custom implementations of the JUCE UI classes
*   Slider, TextButton, and ToggleButton, along with a few other Component
*   classes used in the plugin UI.
*/

class CTextButton : public juce::TextButton 
{
public:
    CTextButton() = default;
    ~CTextButton() = default;

    void paintButton(juce::Graphics& g, bool mouseIsOver, bool mouseIsDown) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CTextButton)
};

class CSlider : public juce::Slider
{
public:
    CSlider();
    ~CSlider() = default;

    void paint(juce::Graphics& g) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CSlider)
};

class CToggleButton : public juce::ToggleButton
{
public:
    CToggleButton() = default;
    ~CToggleButton() = default;

    void paintButton(juce::Graphics& g, bool mouseIsOver, bool mouseIsDown) override;

    void makeVertical() { isVertical = true; }
    void makeHorizontal() { isVertical = false; }

private:
    bool isVertical{ true };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CToggleButton)
};

/** A radio group made with CToggleButton & predefined resizing method. */
class CRadioGroup : public juce::Component
{
public:
    CRadioGroup(int numberOfButtons, int radioGroupId);
    ~CRadioGroup() = default;

    void resized() override;

    CToggleButton* getToggleButton(int index);

private:
    juce::OwnedArray<CToggleButton> toggleButtons;

    int numberOfButtons{ 0 };
    int toggleButtonHeight{ UI::horizontalToggleButtonHeight };
    int toggleButtonWidth{ 40 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CRadioGroup)
};

/** Used to cover the filter UI when it is inactive. */
class SemiTransparentBox : public juce::Component
{
public:
    SemiTransparentBox() = default;
    ~SemiTransparentBox() = default;

    void setColour(juce::Colour c);
    void setAlpha(float a);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Colour colour{ UI::SemiTransparentBox::boxColour };
    float alpha = UI::SemiTransparentBox::boxAlpha;

    int height = 0;
    int width = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SemiTransparentBox)
};

/** Bundles together a few Component classes that are used when 
*   adding a parameter knob to the plugin UI.
*/
class Knob : public juce::Component
{
public:
    Knob();
    ~Knob() = default;

    void attachParameter(chowdsp::FloatParameter& param, chowdsp::PluginState& state);
    void resized() override;

    juce::Label label;
    std::unique_ptr<CSlider> slider;
    std::optional<chowdsp::SliderAttachment> attachment;

private:
    juce::Rectangle<int> labelBounds, sliderBounds;
    float labelFontSize;
    juce::Colour labelColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Knob)
};
