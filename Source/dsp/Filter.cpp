#include "Filter.h"

Filter::Filter (FilterParams& p) : params (p)
{
}

void Filter::prepare (double sampleRate, int blockSize, int channels)
{
    numChannels = channels;

    juce::dsp::ProcessSpec spec { sampleRate, juce::uint32 (blockSize), juce::uint32 (numChannels) };

    for (auto& f : filters)
        f.prepare (spec);

    updateParams (true);
}

void Filter::process (float* const* buffer, int startSample, int numSamples)
{
    updateParams();

    juce::dsp::AudioBlock<float> block (buffer, numChannels, startSample, numSamples);
    juce::dsp::ProcessContextReplacing<float> context (block);

    for (int k = 0; k < numFilters; k++)
        filters[k].process (context);
}

void Filter::reset()
{
    for (auto& f : filters)
        f.reset();
}

void Filter::updateParams (bool force)
{
    auto newCutoff = float (*params.cutoff);
    auto newResonance = float (*params.resonance);
    auto newType = Type (params.type->getIndex());
    auto newNumFilters = params.order->getIndex() + 1;

    const float mode = newType == Type::Lowpass ? 0.0f : newType == Type::Bandpass ? 0.5f : 1.0f;

    if (force)
    {
        cutoff = newCutoff;
        resonance = newResonance;
        type = newType;
        numFilters = newNumFilters;

        for (auto& f : filters)
        {
            f.setCutoffFrequency (cutoff);
            f.setQValue (resonance);
            f.setMode (mode);
        }

        reset();
    }
    else
    {
        if (newCutoff != cutoff)
        {
            cutoff = newCutoff;
            for (auto& f : filters)
                f.setCutoffFrequency (cutoff);
        }

        if (newResonance != resonance)
        {
            resonance = newResonance;
            for (auto& f : filters)
                f.setQValue (resonance);
        }

        if (newType != type)
        {
            type = newType;
            for (auto& f : filters)
                f.setMode (mode);
        }

        if (newNumFilters != numFilters)
        {
            numFilters = newNumFilters;
            reset();
        }
    }
}
