#pragma once

#include <JuceHeader.h>

struct FilterParams
{
    chowdsp::FreqHzParameter::Ptr cutoff {
        chowdsp::ParameterID { "filter_cutoff", 100 },
        "Filter Cutoff",
        chowdsp::ParamUtils::createNormalisableRange (20.0f, 20e3f, 2e3f),
        20e3f
    };

    chowdsp::FloatParameter::Ptr resonance {
        chowdsp::ParameterID { "filter_resonance", 100 },
        "Filter Resonance",
        juce::NormalisableRange<float>(0.1f, 5.0f, 0.0f, 0.25f),
        0.71f,
        &chowdsp::ParamUtils::floatValToString,
        &chowdsp::ParamUtils::stringToFloatVal
    };

    chowdsp::ChoiceParameter::Ptr type {
        chowdsp::ParameterID { "filter_type", 100 },
        "Filter Type",
        juce::StringArray ("LP", "BP", "HP"),
        0
    };

    chowdsp::ChoiceParameter::Ptr order {
        chowdsp::ParameterID { "filter_order", 100 },
        "Filter Order",
        juce::StringArray ("2", "4"),
        0
    };
};

class Filter
{
public:
    enum class Type { Lowpass = 0, Bandpass, Highpass };
    enum class Order { Twelve = 0, TwentyFour };

    Filter (FilterParams& params);
    ~Filter() = default;

    void prepare (double sampleRate, int blockSize, int numChannels);
    void process (float* const* buffer, int startSample, int numSamples);
    void reset();

private:
    void updateParams (bool force = false);

    FilterParams& params;

    static constexpr int maxNumFilters { 2 };
    int numChannels { 0 };

    using SVF = chowdsp::StateVariableFilter<float, chowdsp::StateVariableFilterType::MultiMode>;
    std::array<SVF, maxNumFilters> filters;

    float cutoff { 0.0f };
    float resonance { 0.0f };
    int numFilters { 1 };
    Type type { Type::Lowpass };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Filter)
};
