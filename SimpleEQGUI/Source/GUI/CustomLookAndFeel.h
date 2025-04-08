#pragma once

#include <JuceHeader.h>

namespace GUI
{
    struct PowerButton : juce::ToggleButton
    {
    };

    struct AnalyzerButton : juce::ToggleButton
    {
        void resized() override
        {
            auto bounds = getLocalBounds();
            auto insetRect = bounds.reduced(4);

            randomPath.clear();

            juce::Random r;

            randomPath.startNewSubPath(insetRect.getX(),
                                       insetRect.getY() + insetRect.getHeight() * r.nextFloat());

            for (auto x = insetRect.getX() + 1; x < insetRect.getRight(); x += 2)
            {
                randomPath.lineTo(x,
                                  insetRect.getY() + insetRect.getHeight() * r.nextFloat());
            }
        }

        juce::Path randomPath;
    };

    struct LookAndFeel : juce::LookAndFeel_V4
    {
        void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                              float sliderPosProportional, float rotaryStartAngle,
                              float rotaryEndAngle, juce::Slider &slider) override;

        void drawToggleButton(juce::Graphics &g, juce::ToggleButton &button,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;
    };
}