#include "TapeStop.h"

float TapeStop::applyExpScale(float val, float alpha)
{
    return (1.0f / alpha - 1.0f) * (std::pow(1.0f / (1.0f - alpha), val) - 1.0f);
}

float TapeStop::calculateAlpha(float curveFactor)
{
    if (std::abs(curveFactor) < MIN_ALPHA)
        return MIN_ALPHA;
    else
        return 1 - std::pow(MIN_ALPHA, -1 * curveFactor);
}

TapeStop::TapeStop(TapeStopParams& p) : params(p)
{
    filters[0].emplace(params.filter);
    filters[1].emplace(params.filter);

    instances[0].filterIndex = 0;
    instances[1].filterIndex = 1;
}

void TapeStop::prepare(double sr, int blockSize, int channels)
{
    sampleRate = sr;
    numChannels = channels;

    bufferLength = int(sampleRate * MAX_TIME);
    mainBuffer.setSize(numChannels, bufferLength);
    mainBuffer.clear();
    bufferPtr = mainBuffer.getArrayOfWritePointers();

    prevStateBuffer.setSize(numChannels, blockSize);
    prevStateBuffer.clear();
    prevStateBufferPtr = prevStateBuffer.getArrayOfWritePointers();

    for (auto& f : filters)
        f->prepare(sampleRate, blockSize, numChannels);

    reset();
}

void TapeStop::process(float* const* buffer, int startIdx, int numSamples)
{
    updateParams();

    int endSample = startIdx + numSamples;

    // Save the new samples to the internal buffer
    for (int i = startIdx; i < endSample; i++)
    {
        for (int j = 0; j < numChannels; j++)
            bufferPtr[j][writeIndex] = buffer[j][i];

        writeIndex = (writeIndex + 1) % bufferLength;
    }

    // If currently crossfading between two states, process the previous state as well and add the
    // output to the buffer.
    if (crossfadeCounter < crossfadeLength)
    {
        processPrevState(buffer, startIdx, numSamples);
        processCurrentState(buffer, startIdx, numSamples);
        
        for (int i = startIdx; i < numSamples; i++)
        {
            auto fadePos = juce::jlimit(0.0f, 1.0f, static_cast<float>(crossfadeCounter++) / static_cast<float>(crossfadeLength));
            for (int j = 0; j < numChannels; j++)
                buffer[j][i] = std::sqrt(fadePos) * buffer[j][i] + std::sqrt(1 - fadePos) * prevStateBufferPtr[j][i];
        }
    }
    else
    {
        processCurrentState(buffer, startIdx, numSamples);
    }
}

void TapeStop::reset()
{
    writeIndex = 0;

    for (auto& f : filters)
        f->reset();
}

void TapeStop::updateParams()
{
    slowdownLengthSeconds = *params.slowdownLength;
    speedupLengthSeconds = *params.speedupLength;
    slowdownSettings.alpha = calculateAlpha(*params.slowdownCurve);
    speedupSettings.alpha = calculateAlpha(*params.speedupCurve);
    slowdownSettings.startPos = *params.slowdownStart;
    slowdownSettings.endPos = *params.slowdownEnd;
    speedupSettings.startPos = *params.speedupStart;
    speedupSettings.endPos = *params.speedupEnd;

    auto nextState = State(int(*params.mode));
    if (state != nextState)
    {
        state = nextState;
        processStateChanged();
    }

    fadeLengthSeconds = *params.fadeLength;
    fadeLength = static_cast<int>(fadeLengthSeconds * static_cast<float>(sampleRate));

    crossfadeLengthSeconds = *params.crossfadeLength;
    crossfadeLength = static_cast<int>(crossfadeLengthSeconds * static_cast<float>(sampleRate));

    bool filterEnabled = bool(*params.filterEnable);
    if (filterEnable != filterEnabled)
    {
        filterEnable = filterEnabled;

        if (!filterEnable)
            for (auto& f : filters)
                f->reset();
    }
}

double TapeStop::getIncrement(const Instance& inst)
{
    float inc = (inst.settings.endPos - inst.settings.startPos) * inst.counter / static_cast<double>(inst.length) + inst.settings.startPos;

    if (inst.state == State::Slowdown)
        inc = 1.0f - inc;

    return applyExpScale(inc, inst.settings.alpha);
}

void TapeStop::incrementCounters(Instance& inst)
{
    inst.readIndex += getIncrement(inst);

    if (inst.readIndex >= bufferLength)
        inst.readIndex -= bufferLength;

    ++inst.counter;
}

void TapeStop::resetInstance(Instance& inst)
{
    inst.readIndex = writeIndex;
    inst.counter = 0;
    filters[inst.filterIndex]->reset();

    switch (inst.state)
    {
    case State::Slowdown:
        inst.length = sampleRate * slowdownLengthSeconds;
        inst.settings = slowdownSettings;
        break;

    case State::Speedup:
        inst.length = sampleRate * speedupLengthSeconds;
        inst.settings = speedupSettings;
        break;

    default:
        break;
    }
}

void TapeStop::processStateChanged()
{
    auto temp = prevInstance;
    prevInstance = currentInstance;
    currentInstance = temp;

    currentInstance->state = state;
    resetInstance(*currentInstance);

    crossfadeCounter = 0;
}

template <int k>
struct LagrangeResampleHelper
{
    static forcedinline void calc(float& a, float b) noexcept { a *= b * (1.0f / k); }
};

template <>
struct LagrangeResampleHelper<0>
{
    static forcedinline void calc(float&, float) noexcept {}
};

template <int k>
static float calcCoefficient(float input, float offset) noexcept
{
    LagrangeResampleHelper<0 - k>::calc(input, -2.0f - offset);
    LagrangeResampleHelper<1 - k>::calc(input, -1.0f - offset);
    LagrangeResampleHelper<2 - k>::calc(input, 0.0f - offset);
    LagrangeResampleHelper<3 - k>::calc(input, 1.0f - offset);
    LagrangeResampleHelper<4 - k>::calc(input, 2.0f - offset);
    return input;
}

void TapeStop::getFractionalSampleFromBuffer(float* const* destBuffer, int destSample, float readIdx, float gain) const
{
    int idx0 = std::floor(readIdx);
    int idx1 = (bufferLength + idx0 - 1) % bufferLength;
    int idx2 = (bufferLength + idx0 - 2) % bufferLength;
    int idx3 = (bufferLength + idx0 - 3) % bufferLength;
    int idx4 = (bufferLength + idx0 - 4) % bufferLength;

    float offset = readIdx - float(idx0);

    for (int i = 0; i < numChannels; i++)
    {
        float x0 = bufferPtr[i][idx4];
        float x1 = bufferPtr[i][idx3];
        float x2 = bufferPtr[i][idx2];
        float x3 = bufferPtr[i][idx1];
        float x4 = bufferPtr[i][idx0];

        float result = 0.0f;

        result += calcCoefficient<0>(x0, offset);
        result += calcCoefficient<1>(x1, offset);
        result += calcCoefficient<2>(x2, offset);
        result += calcCoefficient<3>(x3, offset);
        result += calcCoefficient<4>(x4, offset);

        destBuffer[i][destSample] = result * gain;
    }
}

void TapeStop::processSlowdown(float* const* outputBuffer, int sampleIdx, int numSamples, Instance& inst)
{
    int endSample = sampleIdx + numSamples;

    for (int i = sampleIdx; i < endSample; i++)
    {
        if (inst.counter < inst.length)
        {
            auto samplesRemaining = inst.length - inst.counter;
            float g = juce::jlimit(0.0f, 1.0f, static_cast<float>(samplesRemaining) / static_cast<float>(fadeLength));

            getFractionalSampleFromBuffer(outputBuffer, i, float(inst.readIndex), g);
            incrementCounters(inst);
        }
        else
        {
            for (int j = 0; j < numChannels; j++)
                outputBuffer[j][i] = 0.0f;
        }
    }
}

void TapeStop::processSpeedup(float* const* outputBuffer, int sampleIdx, int numSamples, Instance& inst)
{
    int endSample = sampleIdx + numSamples;
    
    for (int i = sampleIdx; i < endSample; i++)
    {
        if (inst.counter < inst.length)
        {
            float g = juce::jlimit(0.0f, 1.0f, static_cast<float>(inst.counter) / static_cast<float>(fadeLength));

            getFractionalSampleFromBuffer(outputBuffer, i, float(inst.readIndex), g);
            incrementCounters(inst);
        }
        else
        {
            getFractionalSampleFromBuffer(outputBuffer, i, float(inst.readIndex));

            inst.readIndex += 1.0;
            if (inst.readIndex >= bufferLength)
                inst.readIndex -= bufferLength;
        }
    }
}

void TapeStop::processCurrentState(float* const* ioBuffer, int sampleIdx, int numSamples)
{
    switch (currentInstance->state)
    {
    case State::Bypass:
        return;

    case State::Slowdown:
        processSlowdown(ioBuffer, sampleIdx, numSamples, *currentInstance);
        break;

    case State::Speedup:
        processSpeedup(ioBuffer, sampleIdx, numSamples, *currentInstance);
        break;
    }

    if (filterEnable)
        filters[currentInstance->filterIndex]->process(ioBuffer, sampleIdx, numSamples);
}

void TapeStop::processPrevState(float* const* inputBuffer, int sampleIdx, int numSamples)
{
    int end = sampleIdx + numSamples;

    switch (prevInstance->state)
    {
    case State::Bypass:
        for (int i = sampleIdx; i < end; i++)
            for (int j = 0; j < numChannels; j++)
                prevStateBufferPtr[j][i] = inputBuffer[j][i];
        return;

    case State::Slowdown:
        processSlowdown(prevStateBufferPtr, sampleIdx, numSamples, *prevInstance);
        break;

    case State::Speedup:
        processSpeedup(prevStateBufferPtr, sampleIdx, numSamples, *prevInstance);
        break;
    }

    if (filterEnable)
        filters[prevInstance->filterIndex]->process(prevStateBufferPtr, sampleIdx, numSamples);
}
