#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"

namespace GUI
{
    struct VerticalSliderWithLabels : juce::Slider
    {
        VerticalSliderWithLabels(juce::RangedAudioParameter &rap, const juce::String &unitSuffix)
            : juce::Slider(juce::Slider::SliderStyle::LinearVertical, juce::Slider::TextEntryBoxPosition::NoTextBox),
              param(&rap), suffix(unitSuffix)
        {
            setLookAndFeel(&lnf);
        }

        ~VerticalSliderWithLabels()
        {
            setLookAndFeel(nullptr);
        }

        struct LabelPos
        {
            float pos;          // 0 = bottom, 1 = top
            juce::String label; // text label
        };

        juce::Array<LabelPos> labels;

        void paint(juce::Graphics &g) override;
        juce::Rectangle<int> getSliderBounds() const;
        int getTextHeight() const { return 14; }
        juce::String getDisplayString() const;

    private:
        GUI::CustomLookAndFeel lnf;
        juce::RangedAudioParameter *param;
        juce::String suffix;
    };

}