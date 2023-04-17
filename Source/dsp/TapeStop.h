#pragma once

#include <JuceHeader.h>
#include "Filter.h"

struct TapeStopParams
{
    FilterParams filter;

    chowdsp::ChoiceParameter::Ptr filterEnable{
            chowdsp::ParameterID("filter_enable", 100),
            "Filter Enable",
            juce::StringArray("OFF", "ON"),
            0
    };

    chowdsp::ChoiceParameter::Ptr mode{
        chowdsp::ParameterID("mode", 100),
        "Mode",
        juce::StringArray("BYPASS", "STOP", "START"),
        0
    };

    chowdsp::FloatParameter::Ptr slowdownLength{
        chowdsp::ParameterID("slowdown_length", 100), "Slowdown Length",
        juce::NormalisableRange<float>(0.05f, 5.0f, 0.0f, 0.25f), 1.0f,
        &chowdsp::ParamUtils::floatValToString, &chowdsp::ParamUtils::stringToFloatVal
    };

    chowdsp::FloatParameter::Ptr speedupLength{
        chowdsp::ParameterID("speedup_length", 100), "Speedup Length",
        juce::NormalisableRange<float>(0.05f, 5.0f, 0.0f, 0.25f), 1.0f,
        &chowdsp::ParamUtils::floatValToString, &chowdsp::ParamUtils::stringToFloatVal
    };

    chowdsp::FloatParameter::Ptr slowdownCurve{
        chowdsp::ParameterID("slowdown_curve", 100), "Slowdown Curve",
        juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f,
        &chowdsp::ParamUtils::floatValToString, &chowdsp::ParamUtils::stringToFloatVal
    };

    chowdsp::FloatParameter::Ptr speedupCurve{
        chowdsp::ParameterID("speedup_curve", 100), "Speedup Curve",
        juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f,
        &chowdsp::ParamUtils::floatValToString, &chowdsp::ParamUtils::stringToFloatVal
    };

    chowdsp::FloatParameter::Ptr slowdownStart{
        chowdsp::ParameterID("slowdown_start", 100), "Slowdown Start",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f,
        &chowdsp::ParamUtils::floatValToString, &chowdsp::ParamUtils::stringToFloatVal
    };

    chowdsp::FloatParameter::Ptr speedupStart{
        chowdsp::ParameterID("speedup_start", 100), "Speedup Start",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f,
        &chowdsp::ParamUtils::floatValToString, &chowdsp::ParamUtils::stringToFloatVal
    };

    chowdsp::FloatParameter::Ptr slowdownEnd{
        chowdsp::ParameterID("slowdown_end", 100), "Slowdown End",
        juce::NormalisableRange<float>(0.0f, 1.0f), 1.0f,
        &chowdsp::ParamUtils::floatValToString, &chowdsp::ParamUtils::stringToFloatVal
    };

    chowdsp::FloatParameter::Ptr speedupEnd{
        chowdsp::ParameterID("speedup_end", 100), "Speedup End",
        juce::NormalisableRange<float>(0.0f, 1.0f), 1.0f,
        &chowdsp::ParamUtils::floatValToString, &chowdsp::ParamUtils::stringToFloatVal
    };

    chowdsp::FloatParameter::Ptr fadeLength{
        chowdsp::ParameterID("fade_length", 100), "Fade Length",
        juce::NormalisableRange<float>(0.01f, 0.5f, 0.0f, 0.5f), 0.03f,
        &chowdsp::ParamUtils::floatValToString, &chowdsp::ParamUtils::stringToFloatVal
    };

    chowdsp::FloatParameter::Ptr crossfadeLength{
        chowdsp::ParameterID("crossfade_length", 100), "Crossfade Length",
        juce::NormalisableRange<float>(0.01f, 0.5f, 0.0f, 0.5f), 0.035f,
        &chowdsp::ParamUtils::floatValToString, &chowdsp::ParamUtils::stringToFloatVal
    };
};

class TapeStop
{
public:
    enum class State
    {
        Bypass = 0,
        Slowdown,
        Speedup
    };

    static constexpr float MAX_TIME = 10.0f;
    static constexpr float MIN_TIME = 0.05f;
    static constexpr float MIN_ALPHA = 5e-3f;

    static float applyExpScale(float val, float alpha);
    static float calculateAlpha(float curveFactor);

    TapeStop(TapeStopParams& params);
    ~TapeStop() = default;

    void prepare(double sampleRate, int blockSize, int numChannels);
    void process(float* const* buffer, int startIdx, int numSamples);
    void reset();

private:
    void updateParams();
    TapeStopParams& params;

    struct CurveSettings
    {
        double alpha{ 0.0 };
        double startPos{ 0.0 };
        double endPos{ 1.0 };
    };

    struct Instance
    {
        State state{ State::Bypass };
        CurveSettings settings;
        int length{ 0 };
        int counter{ 0 };
        double readIndex{ 0.0 };
        int filterIndex{ 0 };
    };

    static double getIncrement(const Instance &inst);
    void incrementCounters(Instance &inst);
    void resetInstance(Instance &inst);
    void processStateChanged();
    void getFractionalSampleFromBuffer(float* const* destBuffer, int destSample, float readIdx, float gain = 1.0f) const;
    void processSlowdown(float* const* outputBuffer, int sampleIdx, int numSamples, Instance &inst);
    void processSpeedup(float* const* outputBuffer, int sampleIdx, int numSamples, Instance &inst);
    void processCurrentState(float* const* processBuffer, int sampleIdx, int numSamples);
    void processPrevState(float* const* processBuffer, int sampleIdx, int numSamples);

    double sampleRate;
    int numChannels;

    juce::AudioBuffer<float> mainBuffer;
    int bufferLength{ 0 };
    float* const* bufferPtr;
    int writeIndex{ 0 };

    juce::AudioBuffer<float> prevStateBuffer;
    float* const* prevStateBufferPtr;
    
    std::array<std::optional<Filter>, 2> filters;

    State state{ State::Bypass };

    Instance instances[2];
    Instance* currentInstance{ &instances[0] };
    Instance* prevInstance{ &instances[1] };

    float slowdownLengthSeconds{ 0.0f };
    float speedupLengthSeconds{ 0.0f };

    CurveSettings slowdownSettings;
    CurveSettings speedupSettings;

    float fadeLengthSeconds{ 0.01f };
    float crossfadeLengthSeconds{ 0.03f };
    int fadeLength{ 0 };
    int crossfadeLength{ 0 };
    int crossfadeCounter{ 0 };

    bool filterEnable{ true };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TapeStop)
};
