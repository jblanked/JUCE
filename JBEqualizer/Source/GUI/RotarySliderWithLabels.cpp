#include "../GUI/RotarySliderWithLabels.h"

namespace GUI
{
    void RotarySliderWithLabels::paint(juce::Graphics &g)
    {
        using namespace juce;

        // Define start and end angles for the rotary slider
        auto startAng = degreesToRadians(180.f + 45.f);
        auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<double>::twoPi;

        auto range = getRange();               // Retrieve the slider's value range
        auto sliderBounds = getSliderBounds(); // Get the area where the slider is drawn

        // Draw the rotary slider using the custom look-and-feel
        getLookAndFeel().drawRotarySlider(g,
                                          sliderBounds.getX(), sliderBounds.getY(),
                                          sliderBounds.getWidth(), sliderBounds.getHeight(),
                                          jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                          startAng, endAng, *this);

        auto center = sliderBounds.toFloat().getCentre();
        auto radius = sliderBounds.getWidth() * 0.5f;

        g.setColour(Colours::lightblue);
        g.setFont(getTextHeight());

        // Draw the labels around the slider's circumference
        auto numChoices = labels.size();
        for (int i = 0; i < numChoices; ++i)
        {
            auto pos = labels[i].pos;
            jassert(0.f <= pos && pos <= 1.f);

            auto ang = jmap(pos, 0.f, 1.f, static_cast<float>(startAng), static_cast<float>(endAng));
            auto c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f + 1, ang);

            Rectangle<float> r;
            auto str = labels[i].label;
            r.setSize(g.getCurrentFont().getStringWidth(str), getTextHeight());
            r.setCentre(c);
            r.setY(r.getY() + getTextHeight());

            g.drawFittedText(str, r.toNearestInt(), Justification::centred, 1);
        }
    }

    juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
    {
        auto bounds = getLocalBounds();
        auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
        size -= getTextHeight() * 2; // Reduce the size by a margin for text
        juce::Rectangle<int> r;
        r.setSize(size, size);
        r.setCentre(bounds.getCentreX(), 0);
        r.setY(2); // Position the slider 2 pixels from the top
        return r;
    }

    juce::String RotarySliderWithLabels::getDisplayString() const
    {
        if (auto choiceParam = dynamic_cast<juce::AudioParameterChoice *>(param))
        {
            return choiceParam->getCurrentChoiceName(); // Return choice parameter string
        }

        juce::String str;
        bool addK = false;

        if (auto *floatParam = dynamic_cast<juce::AudioParameterFloat *>(param))
        {
            // Get the current value and adjust for display
            float val = static_cast<float>(getValue());

            if (val > 999.f)
            {
                val /= 1000.f;
                addK = true;
            }

            str = juce::String(val, (addK ? 2 : 0));
        }
        else
        {
            jassertfalse; // Unexpected parameter type
        }

        if (suffix.isNotEmpty())
        {
            str << " ";
            if (addK)
                str << "k";
            str << suffix;
        }
        return str;
    }
}
