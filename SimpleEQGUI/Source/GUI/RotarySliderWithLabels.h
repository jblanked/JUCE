#pragma once
#include <JuceHeader.h>
#include "../GUI/CustomLookAndFeel.h"

namespace GUI
{
    struct RotarySliderWithLabels : juce::Slider
    {
        RotarySliderWithLabels(juce::RangedAudioParameter &rap, const juce::String &unitSuffix)
            : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                           juce::Slider::TextEntryBoxPosition::NoTextBox),
              param(&rap), suffix(unitSuffix)
        {
            setLookAndFeel(&lnf); // set the custom look and feel for the slider
        }

        ~RotarySliderWithLabels()
        {
            setLookAndFeel(nullptr); // reset to the default look and feel
        }

        struct LabelPos
        {
            float pos;          // Relative position (0 to 1) along the slider arc
            juce::String label; // Text for the label
        };

        juce::Array<LabelPos> labels; // Array of labels for the slider

        void paint(juce::Graphics &g) override;
        juce::Rectangle<int> getSliderBounds() const;
        int getTextHeight() const { return 14; } // Height for text labels
        juce::String getDisplayString() const;

    private:
        LookAndFeel lnf;                   // Custom look and feel for drawing
        juce::RangedAudioParameter *param; // Parameter controlled by this slider
        juce::String suffix;               // Unit suffix to display with the parameter value
    };
}
