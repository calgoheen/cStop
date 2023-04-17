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

    CTextButton _bypass, _stop, _start;
    CToggleButton _bpm, _filterState;
    CRadioGroup _filterType, _filterOrder;
    CurveControl _slowdownCurve, _speedupCurve;

    Knob _slowdownTime, _speedupTime;
    Knob _slowdownNote, _speedupNote;
    Knob _filterFreq, _filterQ;

    SemiTransparentBox _filterBox;

    std::unique_ptr<juce::ButtonParameterAttachment> _filterStateAttachment, _bpmAttachment;
    std::unique_ptr<juce::ParameterAttachment> _filterTypeAttachment, _filterOrderAttachment;

    juce::Rectangle<int> _titleBounds, _filterBoxBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CStopEditor)
};
