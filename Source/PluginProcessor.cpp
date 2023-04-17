#include "PluginProcessor.h"
#include "PluginEditor.h"

CStop::CStop() : tapeStop(state.params.tapeStop)
{
}

void CStop::updateParams()
{
    float slowdownLength, speedupLength;

    if (int(*state.params.sync))
    {
        auto slowdownNote = noteLengths[int(*state.params.slowdownLengthSync)];
        auto speedupNote = noteLengths[int(*state.params.speedupLengthSync)];

        slowdownLength = slowdownNote * (240.0 / tempo);
        speedupLength = speedupNote * (240.0 / tempo);
    }
    else
    {
        slowdownLength = *state.params.slowdownLengthFree;
        speedupLength = *state.params.speedupLengthFree;
    }

    // Need to cast to AudioParameterFloat because operator=() is disabled by JUCE_DECLARE_NON_COPYABLE
    // in chowdsp::FloatParameter (maybe this could be changed?)
    *static_cast<juce::AudioParameterFloat*>(state.params.tapeStop.slowdownLength.get()) = slowdownLength;
    *static_cast<juce::AudioParameterFloat*>(state.params.tapeStop.speedupLength.get()) = speedupLength;
}

void CStop::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    tapeStop.prepare(sampleRate, samplesPerBlock, getMainBusNumOutputChannels());
}

void CStop::processAudioBlock(juce::AudioBuffer<float>& buffer)
{
    // Update the tempo if it is different
    juce::AudioPlayHead::CurrentPositionInfo info;
    info.bpm = tempo;

    // TODO: Use new JUCE method of getting tempo
    if (auto playHead = getPlayHead())
        playHead->getCurrentPosition(info);

    tempo = info.bpm;

    updateParams();

    tapeStop.process(buffer.getArrayOfWritePointers(), 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor* CStop::createEditor()
{
    return new CStopEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CStop();
}
