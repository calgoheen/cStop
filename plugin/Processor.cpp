#include "Processor.h"
#include "Editor.h"

Processor::Processor()
    : juce::AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                             .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
    state (*this, nullptr, "PARAMS", {
        #define X(name) parameters.name.release(),
        TAPESTOP_PARAMETERS (X)
        #undef X
    }),
    tapeStop (parameters)
{
}

bool Processor::isBusesLayoutSupported (const juce::AudioProcessor::BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void Processor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    tapeStop.prepareToPlay (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void Processor::releaseResources()
{
}

void Processor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    tapeStop.processBlock (buffer, midi, getPlayHead());
}

void Processor::getStateInformation (juce::MemoryBlock&)
{
}

void Processor::setStateInformation (const void*, int)
{
}

bool Processor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* Processor::createEditor()
{
    return new Editor (*this);
}

const juce::String Processor::getName() const
{
    return JucePlugin_Name;
}

bool Processor::acceptsMidi() const
{
    return false;
}

bool Processor::producesMidi() const
{
    return false;
}

bool Processor::isMidiEffect() const
{
    return false;
}

double Processor::getTailLengthSeconds() const
{
    return 0.0;
}

int Processor::getNumPrograms()
{
    return 1;
}

int Processor::getCurrentProgram()
{
    return 0;
}

void Processor::setCurrentProgram (int)
{
}

const juce::String Processor::getProgramName (int)
{
    return {};
}

void Processor::changeProgramName (int, const juce::String&)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Processor();
}
