#pragma once

#include "Processor.h"

class Editor : public juce::AudioProcessorEditor
{
public:
    Editor (Processor&);
    ~Editor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::WebBrowserComponent::Options getWebViewOptions();

    Processor& audioProcessor;

    #define X(name) juce::WebSliderRelay name##Relay; juce::WebSliderParameterAttachment name##Attachment;
    TAPESTOP_PARAMETERS (X)
    #undef X

    juce::WebBrowserComponent webView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Editor)
};
