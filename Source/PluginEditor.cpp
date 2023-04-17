#include "PluginProcessor.h"
#include "PluginEditor.h"

CStopEditor::CStopEditor (CStop& p)
    : AudioProcessorEditor (&p), 
    audioProcessor (p), 
    _filterType(3, 1),
    _filterOrder(2, 2),
    _slowdownCurve(true),
    _speedupCurve(false)
{
    setSize(UI::totalWidth + 6, UI::totalHeight + 9);

    auto& state = audioProcessor.getState();

    addAndMakeVisible(&_bypass);
    _bypass.setButtonText("Bypass");
    _bypass.onClick = [&] { chowdsp::ParameterTypeHelpers::setValue(0, *state.params.tapeStop.mode); };

    addAndMakeVisible(&_stop);
    _stop.setButtonText("Stop");
    _stop.onClick = [&] { chowdsp::ParameterTypeHelpers::setValue(1, *state.params.tapeStop.mode); };

    addAndMakeVisible(&_start);
    _start.setButtonText("Start");
    _start.onClick = [&] { chowdsp::ParameterTypeHelpers::setValue(2, *state.params.tapeStop.mode); };

    addAndMakeVisible(&_slowdownTime);
    _slowdownTime.label.setText("Stop Time", juce::dontSendNotification);
    _slowdownTime.attachParameter(*state.params.slowdownLengthFree, state);

    addAndMakeVisible(&_speedupTime);
    _speedupTime.label.setText("Start Time", juce::dontSendNotification);
    _speedupTime.attachParameter(*state.params.speedupLengthFree, state);

    addAndMakeVisible(&_slowdownNote);
    _slowdownNote.label.setText("Stop Time", juce::dontSendNotification);
    _slowdownNote.setVisible(false);
    _slowdownNote.attachParameter(*state.params.slowdownLengthSync, state);

    addAndMakeVisible(&_speedupNote);
    _speedupNote.label.setText("Start Time", juce::dontSendNotification);
    _speedupNote.setVisible(false);
    _speedupNote.attachParameter(*state.params.speedupLengthSync, state);

    addAndMakeVisible(&_bpm);
    _bpm.setButtonText("BPM");
    _bpm.onClick = [this] {
        if (_bpm.getToggleState())
        {
            _slowdownTime.setVisible(false);
            _speedupTime.setVisible(false);
            _slowdownNote.setVisible(true);
            _speedupNote.setVisible(true);
        }
        else
        {
            _slowdownTime.setVisible(true);
            _speedupTime.setVisible(true);
            _slowdownNote.setVisible(false);
            _speedupNote.setVisible(false);
        }
    };
    _bpmAttachment.reset(new juce::ButtonParameterAttachment(*state.params.sync, _bpm));
    _bpm.onClick();

    // filter frequency knob
    addAndMakeVisible(&_filterFreq);
    _filterFreq.label.setText("Cutoff", juce::dontSendNotification);
    _filterFreq.attachParameter(*state.params.tapeStop.filter.cutoff, state);

    // filter Q knob
    addAndMakeVisible(&_filterQ);
    _filterQ.label.setText("Q", juce::dontSendNotification);
    _filterQ.attachParameter(*state.params.tapeStop.filter.resonance, state);

    // filter type radio group
    addAndMakeVisible(&_filterType);

    auto& lowpassButton = *_filterType.getToggleButton(0);
    auto& bandpassButton = *_filterType.getToggleButton(1);
    auto& highpassButton = *_filterType.getToggleButton(2);

    _filterTypeAttachment.reset(new juce::ParameterAttachment(*state.params.tapeStop.filter.type, [&](float x) {
        if (x == 0.0f)
            lowpassButton.setToggleState(true, juce::sendNotification);
        else if (x == 1.0f)
            bandpassButton.setToggleState(true, juce::sendNotification);
        else
            highpassButton.setToggleState(true, juce::sendNotification);
    }));
    
    lowpassButton.setButtonText("LP");
    lowpassButton.onClick = [&]
    {
        if (lowpassButton.getToggleState() == true)
            chowdsp::ParameterTypeHelpers::setValue(0, *state.params.tapeStop.filter.type);
    };
    
    bandpassButton.setButtonText("BP");
    bandpassButton.onClick = [&]
    {
        if (bandpassButton.getToggleState() == true)
            chowdsp::ParameterTypeHelpers::setValue(1, *state.params.tapeStop.filter.type);
    };
    
    highpassButton.setButtonText("HP");
    highpassButton.onClick = [&]
    {
        if (highpassButton.getToggleState() == true)
            chowdsp::ParameterTypeHelpers::setValue(2, *state.params.tapeStop.filter.type);
    };

    _filterTypeAttachment->sendInitialUpdate();

    // filter order radio group
    addAndMakeVisible(&_filterOrder);

    auto& orderButton_12db = *_filterOrder.getToggleButton(0);
    auto& orderButton_24db = *_filterOrder.getToggleButton(1);

    _filterOrderAttachment.reset(new juce::ParameterAttachment(*state.params.tapeStop.filter.order, [&](float x) {
        if (x == 0.0f)
            orderButton_12db.setToggleState(true, juce::sendNotification);
        else
            orderButton_24db.setToggleState(true, juce::sendNotification);
    }));

    orderButton_12db.setButtonText("12");
    orderButton_12db.onClick = [&] 
    {
        if (orderButton_12db.getToggleState() == true)
            chowdsp::ParameterTypeHelpers::setValue(0, *state.params.tapeStop.filter.order);
    };
    
    orderButton_24db.setButtonText("24");
    orderButton_24db.onClick = [&] 
    {
        if (orderButton_24db.getToggleState() == true) 
            chowdsp::ParameterTypeHelpers::setValue(1, *state.params.tapeStop.filter.order);
    };

    _filterOrderAttachment->sendInitialUpdate();

    // filter hider (goes over the filter sliders/radio groups but under the filter on/off button)
    addAndMakeVisible(&_filterBox);

    // filter state button
    addAndMakeVisible(&_filterState);
    _filterState.setButtonText("Filter");
    _filterState.makeHorizontal();
    _filterState.onClick = [this]
    {
        auto state = _filterState.getToggleState();
        if (state) 
            _filterBox.setVisible(false);
        else 
            _filterBox.setVisible(true);
    };
    _filterStateAttachment.reset(new juce::ButtonParameterAttachment(*state.params.tapeStop.filterEnable, _filterState));
    _filterState.onClick();

    // curve plots
    addAndMakeVisible(&_slowdownCurve);
    _slowdownCurve.attachParameters(*state.params.tapeStop.slowdownStart, *state.params.tapeStop.slowdownEnd, *state.params.tapeStop.slowdownCurve);
    
    addAndMakeVisible(&_speedupCurve);
    _speedupCurve.attachParameters(*state.params.tapeStop.speedupStart, *state.params.tapeStop.speedupEnd, *state.params.tapeStop.speedupCurve);
}

CStopEditor::~CStopEditor()
{
}

void CStopEditor::paint (juce::Graphics& g)
{
    g.fillAll(UI::bgColour);

    g.setColour(UI::titleColour);
    g.setFont(28);
    g.drawText("cStop", _titleBounds.translated(-1, 0), juce::Justification::centred);

    auto area = juce::Rectangle<float>(_filterBoxBounds.getX(),
                                       _filterBoxBounds.getY(),
                                       _filterBoxBounds.getWidth(),
                                       _filterBoxBounds.getHeight());

    g.setColour(UI::filterBoxFillColour);
    g.fillRoundedRectangle(area, 5.0f);
}

void CStopEditor::resized()
{
    // use a slightly smaller rectangle than the plugin window size
    auto area = getLocalBounds().reduced(3);

    // chop a little more off the top
    area.removeFromTop(2);

    // area to draw title text in
    _titleBounds = area.removeFromTop(UI::textButtonHeight);

    // set bounds of main control buttons
    auto buttonArea = _titleBounds.removeFromRight(3 * UI::textButtonWidth);
    _bypass.setBounds(buttonArea.removeFromLeft(UI::textButtonWidth).reduced(2, 0));
    _stop.setBounds(buttonArea.removeFromLeft(UI::textButtonWidth).reduced(2, 0));
    _start.setBounds(buttonArea.removeFromLeft(UI::textButtonWidth).reduced(2, 0));

    // leave a small gap
    area.removeFromTop(4);

    // set bounds of slowdown/speedup time knobs & BPM button
    auto knobArea = area.removeFromTop(UI::Knob::totalHeight + 3).withTrimmedTop(3);
    auto fixedModeArea = knobArea.withSizeKeepingCentre(UI::verticalToggleButtonWidth, UI::verticalToggleButtonHeight);
    auto downKnobArea = knobArea.removeFromLeft(getWidth() / 2).withSizeKeepingCentre(UI::Knob::totalWidth, UI::Knob::totalHeight);
    auto upKnobArea = knobArea.removeFromLeft(getWidth() / 2).withSizeKeepingCentre(UI::Knob::totalWidth, UI::Knob::totalHeight);
    _slowdownTime.setBounds(downKnobArea);
    _slowdownNote.setBounds(downKnobArea);
    _speedupTime.setBounds(upKnobArea);
    _speedupNote.setBounds(upKnobArea);
    _bpm.setBounds(fixedModeArea);

    // set bounds of curve plots
    auto curveArea = area.removeFromTop(UI::CurveControl::totalHeight);
    _slowdownCurve.setBounds(curveArea.removeFromLeft(getWidth() / 2).withSizeKeepingCentre(UI::CurveControl::totalWidth, UI::CurveControl::totalHeight));
    _speedupCurve.setBounds(curveArea.removeFromLeft(getWidth() / 2).withSizeKeepingCentre(UI::CurveControl::totalWidth, UI::CurveControl::totalHeight));

    // area for filter
    auto filterArea = area.removeFromTop(UI::Knob::totalHeight +
        UI::horizontalToggleButtonHeight + 10).reduced(20, 5).translated(0, 4);

    // area to paint a box around filter stuff
    _filterBoxBounds = filterArea.expanded(3, 3);

    // component that hides the filter when it is inactive
    _filterBox.setBounds(_filterBoxBounds);

    // filter on/off switch bounds
    _filterState.setBounds(filterArea.removeFromTop(UI::horizontalToggleButtonHeight)
        .withTrimmedLeft(2).translated(0, 2)
        .removeFromLeft(UI::horizontalToggleButtonWidth));

    // area for filter type & filter order
    auto filterSelectArea = filterArea.removeFromLeft(UI::comboBoxWidth).withTrimmedTop(2).withTrimmedLeft(2);

    // area for filter freq & Q knobs
    auto filterKnobArea = filterArea.removeFromRight(2 * UI::Knob::totalWidth + 4).reduced(2, 0).translated(0, -5);

    // filter type & filter order
    _filterType.setBounds(filterSelectArea.removeFromLeft(filterSelectArea.getWidth() / 2));
    _filterOrder.setBounds(filterSelectArea);

    // filter freq & Q
    _filterFreq.setBounds(filterKnobArea.removeFromLeft(UI::Knob::totalWidth));
    _filterQ.setBounds(filterKnobArea.removeFromLeft(UI::Knob::totalWidth));
}
