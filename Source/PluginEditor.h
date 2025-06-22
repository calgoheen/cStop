#pragma once

#include "PluginProcessor.h"
#include "ui/Constants.h"
#include "ui/Components.h"
#include "ui/CurveControl.h"

/** Main plugin UI Component. Contains all the knobs, buttons, etc. and 
*   sets their bounds according to the data in the Constants.h file.
* 
*   Each UI element is linked to its corresponding parameter in the constructor 
*   using ParameterAttachment objects and/or Component lambda functions.
*/
class CStopEditor  : public juce::AudioProcessorEditor
{
public:
    CStopEditor (CStop&);
    ~CStopEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    CStop& audioProcessor;

    CTextButton bypass, stop, start;
    CToggleButton bpm, filterState;
    CRadioGroup filterType, filterOrder;
    CurveControl slowdownCurve, speedupCurve;

    Knob slowdownTime, speedupTime;
    Knob slowdownNote, speedupNote;
    Knob filterFreq, filterQ;

    SemiTransparentBox filterBox;

    std::unique_ptr<juce::ButtonParameterAttachment> filterStateAttachment, bpmAttachment;
    std::unique_ptr<juce::ParameterAttachment> filterTypeAttachment, filterOrderAttachment;

    juce::Rectangle<int> titleBounds, filterBoxBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CStopEditor)
};
