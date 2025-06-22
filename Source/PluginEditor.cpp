#include "PluginProcessor.h"
#include "PluginEditor.h"

CStopEditor::CStopEditor (CStop& p)
    : AudioProcessorEditor (&p), 
    audioProcessor (p), 
    filterType (3, 1),
    filterOrder (2, 2),
    slowdownCurve (true),
    speedupCurve (false)
{
    setSize (UI::totalWidth + 6, UI::totalHeight + 9);

    auto& state = audioProcessor.getState();

    addAndMakeVisible (&bypass);
    bypass.setButtonText ("Bypass");
    bypass.onClick = [&] { chowdsp::ParameterTypeHelpers::setValue (0, *state.params.tapeStop.mode); };

    addAndMakeVisible (&stop);
    stop.setButtonText ("Stop");
    stop.onClick = [&] { chowdsp::ParameterTypeHelpers::setValue (1, *state.params.tapeStop.mode); };

    addAndMakeVisible (&start);
    start.setButtonText ("Start");
    start.onClick = [&] { chowdsp::ParameterTypeHelpers::setValue (2, *state.params.tapeStop.mode); };

    addAndMakeVisible (&slowdownTime);
    slowdownTime.label.setText ("Stop Time", juce::dontSendNotification);
    slowdownTime.attachParameter (*state.params.slowdownLengthFree, state);

    addAndMakeVisible (&speedupTime);
    speedupTime.label.setText ("Start Time", juce::dontSendNotification);
    speedupTime.attachParameter (*state.params.speedupLengthFree, state);

    addAndMakeVisible (&slowdownNote);
    slowdownNote.label.setText ("Stop Time", juce::dontSendNotification);
    slowdownNote.setVisible (false);
    slowdownNote.attachParameter (*state.params.slowdownLengthSync, state);

    addAndMakeVisible (&speedupNote);
    speedupNote.label.setText ("Start Time", juce::dontSendNotification);
    speedupNote.setVisible (false);
    speedupNote.attachParameter (*state.params.speedupLengthSync, state);

    addAndMakeVisible (&bpm);
    bpm.setButtonText ("BPM");
    bpm.onClick = [this] {
        if (bpm.getToggleState())
        {
            slowdownTime.setVisible (false);
            speedupTime.setVisible (false);
            slowdownNote.setVisible (true);
            speedupNote.setVisible (true);
        }
        else
        {
            slowdownTime.setVisible (true);
            speedupTime.setVisible (true);
            slowdownNote.setVisible (false);
            speedupNote.setVisible (false);
        }
    };
    bpmAttachment.reset (new juce::ButtonParameterAttachment (*state.params.sync, bpm));
    bpm.onClick();

    // filter frequency knob
    addAndMakeVisible (&filterFreq);
    filterFreq.label.setText ("Cutoff", juce::dontSendNotification);
    filterFreq.attachParameter (*state.params.tapeStop.filter.cutoff, state);

    // filter Q knob
    addAndMakeVisible (&filterQ);
    filterQ.label.setText ("Q", juce::dontSendNotification);
    filterQ.attachParameter (*state.params.tapeStop.filter.resonance, state);

    // filter type radio group
    addAndMakeVisible (&filterType);

    auto& lowpassButton = *filterType.getToggleButton (0);
    auto& bandpassButton = *filterType.getToggleButton (1);
    auto& highpassButton = *filterType.getToggleButton (2);

    filterTypeAttachment.reset (new juce::ParameterAttachment (*state.params.tapeStop.filter.type, [&](float x) {
        if (x == 0.0f)
            lowpassButton.setToggleState (true, juce::sendNotification);
        else if (x == 1.0f)
            bandpassButton.setToggleState (true, juce::sendNotification);
        else
            highpassButton.setToggleState (true, juce::sendNotification);
    }));
    
    lowpassButton.setButtonText ("LP");
    lowpassButton.onClick = [&]
    {
        if (lowpassButton.getToggleState() == true)
            chowdsp::ParameterTypeHelpers::setValue (0, *state.params.tapeStop.filter.type);
    };
    
    bandpassButton.setButtonText ("BP");
    bandpassButton.onClick = [&]
    {
        if (bandpassButton.getToggleState() == true)
            chowdsp::ParameterTypeHelpers::setValue (1, *state.params.tapeStop.filter.type);
    };
    
    highpassButton.setButtonText ("HP");
    highpassButton.onClick = [&]
    {
        if (highpassButton.getToggleState() == true)
            chowdsp::ParameterTypeHelpers::setValue (2, *state.params.tapeStop.filter.type);
    };

    filterTypeAttachment->sendInitialUpdate();

    // filter order radio group
    addAndMakeVisible (&filterOrder);

    auto& orderButton_12db = *filterOrder.getToggleButton (0);
    auto& orderButton_24db = *filterOrder.getToggleButton (1);

    filterOrderAttachment.reset (new juce::ParameterAttachment (*state.params.tapeStop.filter.order, [&](float x) {
        if (x == 0.0f)
            orderButton_12db.setToggleState (true, juce::sendNotification);
        else
            orderButton_24db.setToggleState (true, juce::sendNotification);
    }));

    orderButton_12db.setButtonText ("12");
    orderButton_12db.onClick = [&] 
    {
        if (orderButton_12db.getToggleState() == true)
            chowdsp::ParameterTypeHelpers::setValue (0, *state.params.tapeStop.filter.order);
    };
    
    orderButton_24db.setButtonText ("24");
    orderButton_24db.onClick = [&] 
    {
        if (orderButton_24db.getToggleState() == true) 
            chowdsp::ParameterTypeHelpers::setValue (1, *state.params.tapeStop.filter.order);
    };

    filterOrderAttachment->sendInitialUpdate();

    // filter hider (goes over the filter sliders/radio groups but under the filter on/off button)
    addAndMakeVisible (&filterBox);

    // filter state button
    addAndMakeVisible (&filterState);
    filterState.setButtonText ("Filter");
    filterState.makeHorizontal();
    filterState.onClick = [this]
    {
        auto state = filterState.getToggleState();
        if (state) 
            filterBox.setVisible (false);
        else 
            filterBox.setVisible (true);
    };
    filterStateAttachment.reset (new juce::ButtonParameterAttachment (*state.params.tapeStop.filterEnable, filterState));
    filterState.onClick();

    // curve plots
    addAndMakeVisible (&slowdownCurve);
    slowdownCurve.attachParameters (*state.params.tapeStop.slowdownStart, *state.params.tapeStop.slowdownEnd, *state.params.tapeStop.slowdownCurve);
    
    addAndMakeVisible (&speedupCurve);
    speedupCurve.attachParameters (*state.params.tapeStop.speedupStart, *state.params.tapeStop.speedupEnd, *state.params.tapeStop.speedupCurve);
}

CStopEditor::~CStopEditor()
{
}

void CStopEditor::paint (juce::Graphics& g)
{
    g.fillAll (UI::bgColour);

    g.setColour (UI::titleColour);
    g.setFont (28);
    g.drawText ("cStop", titleBounds.translated (-1, 0), juce::Justification::centred);

    auto area = juce::Rectangle<float>(filterBoxBounds.getX(),
                                       filterBoxBounds.getY(),
                                       filterBoxBounds.getWidth(),
                                       filterBoxBounds.getHeight());

    g.setColour (UI::filterBoxFillColour);
    g.fillRoundedRectangle (area, 5.0f);
}

void CStopEditor::resized()
{
    // use a slightly smaller rectangle than the plugin window size
    auto area = getLocalBounds().reduced (3);

    // chop a little more off the top
    area.removeFromTop (2);

    // area to draw title text in
    titleBounds = area.removeFromTop (UI::textButtonHeight);

    // set bounds of main control buttons
    auto buttonArea = titleBounds.removeFromRight (3 * UI::textButtonWidth);
    bypass.setBounds (buttonArea.removeFromLeft (UI::textButtonWidth).reduced (2, 0));
    stop.setBounds (buttonArea.removeFromLeft (UI::textButtonWidth).reduced (2, 0));
    start.setBounds (buttonArea.removeFromLeft (UI::textButtonWidth).reduced (2, 0));

    // leave a small gap
    area.removeFromTop (4);

    // set bounds of slowdown/speedup time knobs & BPM button
    auto knobArea = area.removeFromTop (UI::Knob::totalHeight + 3).withTrimmedTop (3);
    auto fixedModeArea = knobArea.withSizeKeepingCentre (UI::verticalToggleButtonWidth, UI::verticalToggleButtonHeight);
    auto downKnobArea = knobArea.removeFromLeft (getWidth() / 2).withSizeKeepingCentre (UI::Knob::totalWidth, UI::Knob::totalHeight);
    auto upKnobArea = knobArea.removeFromLeft (getWidth() / 2).withSizeKeepingCentre (UI::Knob::totalWidth, UI::Knob::totalHeight);
    slowdownTime.setBounds (downKnobArea);
    slowdownNote.setBounds (downKnobArea);
    speedupTime.setBounds (upKnobArea);
    speedupNote.setBounds (upKnobArea);
    bpm.setBounds (fixedModeArea);

    // set bounds of curve plots
    auto curveArea = area.removeFromTop (UI::CurveControl::totalHeight);
    slowdownCurve.setBounds (curveArea.removeFromLeft (getWidth() / 2).withSizeKeepingCentre (UI::CurveControl::totalWidth, UI::CurveControl::totalHeight));
    speedupCurve.setBounds (curveArea.removeFromLeft (getWidth() / 2).withSizeKeepingCentre (UI::CurveControl::totalWidth, UI::CurveControl::totalHeight));

    // area for filter
    auto filterArea = area.removeFromTop (UI::Knob::totalHeight +
        UI::horizontalToggleButtonHeight + 10).reduced (20, 5).translated (0, 4);

    // area to paint a box around filter stuff
    filterBoxBounds = filterArea.expanded (3, 3);

    // component that hides the filter when it is inactive
    filterBox.setBounds (filterBoxBounds);

    // filter on/off switch bounds
    filterState.setBounds (filterArea.removeFromTop (UI::horizontalToggleButtonHeight)
        .withTrimmedLeft (2).translated (0, 2)
        .removeFromLeft (UI::horizontalToggleButtonWidth));

    // area for filter type & filter order
    auto filterSelectArea = filterArea.removeFromLeft (UI::comboBoxWidth).withTrimmedTop (2).withTrimmedLeft (2);

    // area for filter freq & Q knobs
    auto filterKnobArea = filterArea.removeFromRight (2 * UI::Knob::totalWidth + 4).reduced (2, 0).translated (0, -5);

    // filter type & filter order
    filterType.setBounds (filterSelectArea.removeFromLeft (filterSelectArea.getWidth() / 2));
    filterOrder.setBounds (filterSelectArea);

    // filter freq & Q
    filterFreq.setBounds (filterKnobArea.removeFromLeft (UI::Knob::totalWidth));
    filterQ.setBounds (filterKnobArea.removeFromLeft (UI::Knob::totalWidth));
}
