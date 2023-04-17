#pragma once

#include <JuceHeader.h>
#include "dsp/TapeStop.h"

struct Params : chowdsp::ParamHolder
{
    Params()
    {
        add(sync,
            slowdownLengthFree,
            speedupLengthFree,
            slowdownLengthSync,
            speedupLengthSync,
            tapeStop.mode,
            tapeStop.slowdownCurve,
            tapeStop.speedupCurve,
            tapeStop.slowdownStart,
            tapeStop.speedupStart,
            tapeStop.slowdownEnd,
            tapeStop.speedupEnd,
            tapeStop.fadeLength,
            tapeStop.crossfadeLength,
            tapeStop.filter.cutoff,
            tapeStop.filter.resonance,
            tapeStop.filter.type,
            tapeStop.filter.order,
            tapeStop.filterEnable);
    }

private:
    const juce::StringArray notes = { "1/16", "1/12", "1/8", "1/6", "1/4", "1/3", "1/2", "1", "2" };
    const juce::NormalisableRange<float> noteRange{ 0.0f, float(notes.size() - 1), 1.0f };
    const juce::NormalisableRange<float> lengthRange{ TapeStop::MIN_TIME, 5.0f, 0.0f, 0.25f };
    const std::function<juce::String(float)> floatValToNote{ [this](float val) { return notes[int(val)]; } };

public:
    TapeStopParams tapeStop;

    chowdsp::ChoiceParameter::Ptr sync{
        chowdsp::ParameterID("sync", 100),
        "Sync",
        juce::StringArray("OFF", "ON"),
        0
    };

    chowdsp::FloatParameter::Ptr slowdownLengthFree{
        chowdsp::ParameterID("slowdown_length_free", 100), "Slowdown Length (Free)",
        lengthRange, 1.0f,
        &chowdsp::ParamUtils::floatValToString, &chowdsp::ParamUtils::stringToFloatVal
    };

    chowdsp::FloatParameter::Ptr speedupLengthFree{
        chowdsp::ParameterID("speedup_length_free", 100), "Speedup Length (Free)",
        lengthRange, 1.0f,
        &chowdsp::ParamUtils::floatValToString, &chowdsp::ParamUtils::stringToFloatVal
    };

    chowdsp::FloatParameter::Ptr slowdownLengthSync{
        chowdsp::ParameterID("slowdown_length_sync", 100), "Slowdown Length (Sync)",
        noteRange, 4.0f,
        floatValToNote, &chowdsp::ParamUtils::stringToFloatVal
    };

    chowdsp::FloatParameter::Ptr speedupLengthSync{
        chowdsp::ParameterID("speedup_length_sync", 100), "Speedup Length (Sync)",
        noteRange, 4.0f,
        floatValToNote, &chowdsp::ParamUtils::stringToFloatVal
    };
};

using CStopState = chowdsp::PluginStateImpl<Params>;

class CStop : public chowdsp::PluginBase<CStopState>
{
public:
    CStop();
    ~CStop() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void processAudioBlock(juce::AudioBuffer<float>&) override;
    void releaseResources() override {}

    juce::AudioProcessorEditor* createEditor() override;

private:
    void updateParams();

    TapeStop tapeStop;

    const float noteLengths[9] = {
            1.0 / 16.0,   1.0 / 12.0,   1.0 / 8.0,
            1.0 / 6.0,    1.0 / 4.0,    1.0 / 3.0,
            1.0 / 2.0,    1.0,          2.0
    };

    double tempo{ 120.0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CStop)
};
