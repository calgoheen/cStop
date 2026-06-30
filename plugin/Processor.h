#pragma once

#include <JuceHeader.h>

class Processor : public juce::AudioProcessor
{
public:
    Processor();
    ~Processor() override = default;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    /** Serializes the complete user-facing plug-in state into the host's preset/project chunk.
        The chunk contains the AudioProcessorValueTreeState tree named "PARAMS", so every
        parameter created in cgo::TapeStop::Params is persisted using JUCE's XML binary format.
        Hosts may call this on save, preset export, duplication, or initial state capture, and the
        returned block is expected to be non-empty and self-contained.
    */
    void getStateInformation (juce::MemoryBlock& destData) override;

    /** Restores a state block previously produced by getStateInformation().
        Empty, null, malformed, or wrong-root data is ignored by the processor, leaving defaults
        untouched when a wrapper or host still calls into this method with unusable state data.
        Valid "PARAMS" trees replace the full APVTS state and therefore update all automatable
        parameters; no transient DSP buffers, playhead position, or in-progress tape motion are
        restored from the host chunk.
    */
    void setStateInformation (const void* data, int sizeInBytes) override;

    bool hasEditor() const override;
    juce::AudioProcessorEditor* createEditor() override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    cgo::TapeStop::Params parameters;
    juce::AudioProcessorValueTreeState state;

private:
    cgo::TapeStop tapeStop;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Processor)
};
