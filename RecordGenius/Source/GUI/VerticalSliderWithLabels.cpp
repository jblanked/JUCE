#include "VerticalSliderWithLabels.h"

namespace GUI
{
    void VerticalSliderWithLabels::paint(juce::Graphics &g)
    {
        using namespace juce;
        auto bounds = getLocalBounds();
        auto sliderArea = getSliderBounds();

        float sliderPos = jmap((float)getValue(),
                               (float)getMinimum(),
                               (float)getMaximum(),
                               (float)sliderArea.getBottom(),
                               (float)sliderArea.getY());

        getLookAndFeel().drawLinearSlider(g,
                                          sliderArea.getX(), sliderArea.getY(),
                                          sliderArea.getWidth(), sliderArea.getHeight(),
                                          sliderPos,
                                          0.f, 1.f,
                                          Slider::SliderStyle::LinearVertical,
                                          *this);

        g.setColour(Colours::white);
        g.setFont((float)getTextHeight());

        const int textH = getTextHeight();
        const int padding = 2;

        for (auto &lp : labels)
        {
            jassert(lp.pos >= 0.f && lp.pos <= 1.f);
            auto text = lp.label;
            auto tw{juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), text)};
            int x = sliderArea.getCentreX() - tw / 2;

            if (lp.pos >= 0.999f) // top
            {
                int y = sliderArea.getY() - textH - padding;
                g.drawFittedText(text, x, y, tw + 4, textH,
                                 Justification::centred, 1);
            }
            else if (lp.pos <= 0.001f) // bottom
            {
                int y = sliderArea.getBottom() + padding;
                g.drawFittedText(text, x, y, tw + 4, textH,
                                 Justification::centred, 1);
            }
        }

        auto disp = getDisplayString();
        if (disp.isNotEmpty())
        {
            auto tw{juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), disp)};
            int boxWidth = tw + 10;
            int x = bounds.getCentreX() - boxWidth / 2;
            int y = bounds.getY();

            g.drawFittedText(disp,
                             x, y,
                             boxWidth, textH,
                             Justification::centred, 1);
        }
    }

    juce::Rectangle<int> VerticalSliderWithLabels::getSliderBounds() const
    {
        auto bounds = getLocalBounds();
        int w = bounds.getWidth() / 3;
        int h = bounds.getHeight() - getTextHeight() * 2;
        juce::Rectangle<int> r(w, h);
        r.setSize(w, h - 20);
        r.setCentre(bounds.getCentreX(), bounds.getCentreY() + getTextHeight() / 2);
        return r;
    }

    juce::String VerticalSliderWithLabels::getDisplayString() const
    {
        juce::String str;
        if (auto choiceParam = dynamic_cast<juce::AudioParameterChoice *>(param))
            str = choiceParam->getCurrentChoiceName();
        else if (auto floatParam = dynamic_cast<juce::AudioParameterFloat *>(param))
        {
            float v = (float)getValue();
            str = juce::String(v, v > 999.0f ? 2 : 0);
        }

        if (str.isNotEmpty() && suffix.isNotEmpty())
            str << " " << suffix;

        return str;
    }

}
