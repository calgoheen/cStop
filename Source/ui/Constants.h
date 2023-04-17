#pragma once

#include <JuceHeader.h>

struct UI
{
    static const int textButtonWidth;
    static const int textButtonHeight;
    static const int comboBoxWidth;
    static const int comboBoxHeight;
    static const int verticalToggleButtonWidth;
    static const int verticalToggleButtonHeight;
    static const int horizontalToggleButtonWidth;
    static const int horizontalToggleButtonHeight;

    struct BaseColours
    {
        static const juce::Colour lightblue;
        static const juce::Colour cyan;
        static const juce::Colour bluegrey;
        static const juce::Colour mediumgrey;
        static const juce::Colour darkgrey;
    };

    struct CurveControl 
    {
        static const int totalWidth;
        static const int totalHeight;
        static const int plotBufferX;
        static const int plotBufferY;
        static const int plotWidth;
        static const int plotHeight;
        static const int numGridDivisions;

        static const int numPoints;
        static const int dotSize;

        static const float pathStrokeSize;
        static const float outlineStrokeSize;
        static const float gridStrokeSize;
        static const float highlightOpacity;

        static const juce::Colour bgColour;
        static const juce::Colour outlineColour;
        static const juce::Colour pathColour;
        static const juce::Colour highlightColour;
        static const juce::Colour thumbColour;
        static const juce::Colour gridColour;
    };

    struct Knob 
    {
        static const int knobWidth;
        static const int knobHeight;
        static const int valueBoxWidth;
        static const int valueBoxHeight;
        static const int labelWidth;
        static const int labelHeight;
        static const int sliderHeight;
        static const int sliderWidth;
        static const int totalWidth;
        static const int totalHeight;

        static const float fontSize;

        static const juce::Colour textColour;
        static const juce::Colour bgColour;
    };

    struct Slider 
    {
        static const int valueBoxWidth;
        static const int valueBoxHeight;
        static const float strokeSize;

        static const juce::Colour bottomArcColour;
        static const juce::Colour topArcColour;
        static const juce::Colour valueBoxTextColour;
    };

    struct TextButton 
    {
        static const juce::Colour idleFillColour;
        static const juce::Colour overFillColour;
        static const juce::Colour downFillColour;

        static const juce::Colour idleOutlineColour;
        static const juce::Colour overOutlineColour;
        static const juce::Colour downOutlineColour;

        static const juce::Colour idleTextColour;
        static const juce::Colour overTextColour;
        static const juce::Colour downTextColour;

        static const juce::Colour shadowColour;
    };

    struct ToggleButton 
    {
        static const float outlineStrokeSize;

        static const int boxSize;

        static const juce::Colour idleFillColour;
        static const juce::Colour overFillColour;
        static const juce::Colour downFillColour;
        static const juce::Colour idleTextColour;
        static const juce::Colour overTextColour;
        static const juce::Colour downTextColour;
        static const juce::Colour idleOutlineColour;
        static const juce::Colour overOutlineColour;
        static const juce::Colour downOutlineColour;
        static const juce::Colour checkmarkColour;
    };

    struct SemiTransparentBox 
    {
        static const float boxAlpha;
        static const juce::Colour boxColour;
    };

    static const juce::Colour bgColour;
    static const juce::Colour titleColour;
    static const juce::Colour filterBoxFillColour;
    static const juce::Colour filterBoxOutlineColour;

    static const int totalWidth;
    static const int totalHeight;
};
