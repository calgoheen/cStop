#include "Constants.h"

const int UI::textButtonWidth                 = 60;
const int UI::textButtonHeight                = 45;
const int UI::comboBoxWidth                   = 80;
const int UI::comboBoxHeight                  = 25;
const int UI::verticalToggleButtonWidth       = 32;
const int UI::verticalToggleButtonHeight      = 30;
const int UI::horizontalToggleButtonWidth     = 60;
const int UI::horizontalToggleButtonHeight    = 16;

const juce::Colour UI::BaseColours::lightblue     = { 188, 208, 229 };
const juce::Colour UI::BaseColours::cyan          = { 117, 185, 190 };
const juce::Colour UI::BaseColours::bluegrey      = { 60, 62, 77 };
const juce::Colour UI::BaseColours::mediumgrey    = { 77, 80, 97 };
const juce::Colour UI::BaseColours::darkgrey      = { 48, 50, 61 };

const int UI::CurveControl::totalWidth        = 120;
const int UI::CurveControl::totalHeight       = 80;
const int UI::CurveControl::plotBufferX       = 9;
const int UI::CurveControl::plotBufferY       = 7;
const int UI::CurveControl::plotWidth         = UI::CurveControl::totalWidth - 2 * (UI::CurveControl::plotBufferX);
const int UI::CurveControl::plotHeight        = UI::CurveControl::totalHeight - 2 * (UI::CurveControl::plotBufferY);
const int UI::CurveControl::numPoints         = 1024;
const int UI::CurveControl::dotSize           = 16;
const int UI::CurveControl::numGridDivisions  = 4;

const float UI::CurveControl::pathStrokeSize      = 3.0f;
const float UI::CurveControl::outlineStrokeSize   = 2.2f;
const float UI::CurveControl::gridStrokeSize      = 0.4f;
const float UI::CurveControl::highlightOpacity    = 0.5f;

const juce::Colour UI::CurveControl::bgColour         = UI::BaseColours::mediumgrey;
const juce::Colour UI::CurveControl::outlineColour    = UI::BaseColours::mediumgrey.brighter(0.7);
const juce::Colour UI::CurveControl::pathColour       = UI::BaseColours::lightblue;
const juce::Colour UI::CurveControl::highlightColour  = UI::BaseColours::lightblue;
const juce::Colour UI::CurveControl::thumbColour      = UI::BaseColours::lightblue.brighter(0.4);
const juce::Colour UI::CurveControl::gridColour       = UI::BaseColours::mediumgrey.brighter(0.7);

const int UI::Knob::knobWidth         = 65;
const int UI::Knob::knobHeight        = 60;
const int UI::Knob::valueBoxWidth     = UI::Knob::knobWidth;
const int UI::Knob::valueBoxHeight    = 20;
const int UI::Knob::labelWidth        = UI::Knob::knobWidth;
const int UI::Knob::labelHeight       = 15;
const int UI::Knob::sliderWidth       = UI::Knob::knobWidth;
const int UI::Knob::sliderHeight      = UI::Knob::knobHeight + UI::Knob::valueBoxHeight;
const int UI::Knob::totalWidth        = UI::Knob::knobWidth;
const int UI::Knob::totalHeight       = UI::Knob::sliderHeight + UI::Knob::labelHeight;

const float UI::Knob::fontSize = 13.0f;

const juce::Colour UI::Knob::textColour   = UI::BaseColours::lightblue;
const juce::Colour UI::Knob::bgColour     = UI::BaseColours::darkgrey;

const int UI::Slider::valueBoxWidth   = UI::Knob::valueBoxWidth;
const int UI::Slider::valueBoxHeight  = UI::Knob::valueBoxHeight;

const float UI::Slider::strokeSize = 2.0f;

const juce::Colour UI::Slider::bottomArcColour        = UI::BaseColours::mediumgrey;
const juce::Colour UI::Slider::topArcColour           = UI::BaseColours::lightblue;
const juce::Colour UI::Slider::valueBoxTextColour     = UI::BaseColours::lightblue;

const juce::Colour UI::TextButton::idleFillColour         = UI::BaseColours::lightblue.darker(0.1);
const juce::Colour UI::TextButton::overFillColour         = UI::BaseColours::lightblue.brighter(0.2);
const juce::Colour UI::TextButton::downFillColour         = UI::BaseColours::lightblue.darker(0.2);
const juce::Colour UI::TextButton::idleOutlineColour      = UI::BaseColours::darkgrey;
const juce::Colour UI::TextButton::overOutlineColour      = UI::BaseColours::darkgrey;
const juce::Colour UI::TextButton::downOutlineColour      = UI::BaseColours::darkgrey;
const juce::Colour UI::TextButton::idleTextColour         = UI::BaseColours::darkgrey;
const juce::Colour UI::TextButton::overTextColour         = UI::BaseColours::darkgrey;
const juce::Colour UI::TextButton::downTextColour         = UI::BaseColours::darkgrey;
const juce::Colour UI::TextButton::shadowColour           = UI::BaseColours::mediumgrey;

const float UI::ToggleButton::outlineStrokeSize = 1.5f;

const int UI::ToggleButton::boxSize = 17;

const juce::Colour UI::ToggleButton::idleFillColour       = UI::BaseColours::mediumgrey;
const juce::Colour UI::ToggleButton::overFillColour       = UI::ToggleButton::idleFillColour;
const juce::Colour UI::ToggleButton::downFillColour       = UI::ToggleButton::idleFillColour;
const juce::Colour UI::ToggleButton::idleTextColour       = UI::BaseColours::lightblue;
const juce::Colour UI::ToggleButton::overTextColour       = UI::ToggleButton::idleTextColour;
const juce::Colour UI::ToggleButton::downTextColour       = UI::ToggleButton::idleTextColour;
const juce::Colour UI::ToggleButton::idleOutlineColour    = UI::BaseColours::mediumgrey.brighter(0.7);
const juce::Colour UI::ToggleButton::overOutlineColour    = UI::BaseColours::lightblue.darker(0.2);
const juce::Colour UI::ToggleButton::downOutlineColour    = UI::BaseColours::lightblue.darker(0.5);
const juce::Colour UI::ToggleButton::checkmarkColour      = UI::BaseColours::lightblue.darker(0.1);

const float UI::SemiTransparentBox::boxAlpha = 0.6f;

const juce::Colour UI::SemiTransparentBox::boxColour = UI::BaseColours::darkgrey;

const juce::Colour UI::bgColour               = UI::BaseColours::darkgrey;
const juce::Colour UI::titleColour            = UI::BaseColours::lightblue;
const juce::Colour UI::filterBoxFillColour    = UI::BaseColours::bluegrey;
const juce::Colour UI::filterBoxOutlineColour = UI::BaseColours::mediumgrey.brighter(0.7);

const int UI::totalWidth  = 2 * UI::CurveControl::totalWidth + 16;
const int UI::totalHeight = 2 * UI::Knob::totalHeight + UI::CurveControl::totalHeight + UI::textButtonHeight + 40;
