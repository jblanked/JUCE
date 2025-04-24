#include "CompressorVisualizer.h"

namespace GUI
{
    CompressorVisualizer::CompressorVisualizer(juce::AudioProcessorValueTreeState &apvts,
                                               JBCompressorAudioProcessor &p)
        : state(apvts), processor(p)
    {
        startTimerHz(30);
    }

    CompressorVisualizer::~CompressorVisualizer()
    {
        stopTimer();
    }

    void CompressorVisualizer::paint(juce::Graphics &g)
    {
        auto areaInt = getLocalBounds();
        auto meterInt = areaInt.removeFromRight(120);
        auto curveArea = areaInt.toFloat();
        auto meterTotal = meterInt.toFloat();

        drawBackgroundGrid(g, curveArea);
        drawCompressorCurve(g, curveArea);

        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.drawLine(curveArea.getRight(), curveArea.getY(),
                   curveArea.getRight(), curveArea.getBottom(), 1.0f);

        constexpr int labelHeight = 16;
        auto meterArea = meterTotal.removeFromTop(meterTotal.getHeight() - labelHeight);
        juce::Rectangle<float> labelArea{
            meterTotal.getX(), meterArea.getBottom(),
            meterTotal.getWidth(), (float)labelHeight};

        float wThird = meterArea.getWidth() / 3.0f;
        auto inArea = meterArea.removeFromLeft(wThird);
        auto grArea = meterArea.removeFromLeft(wThird);
        auto outArea = meterArea;

        drawInputMeter(g, inArea);
        drawGainReduction(g, grArea);
        drawOutputMeter(g, outArea);

        g.setColour(juce::Colours::white);
        g.setFont(10.0f);
        float labelW = labelArea.getWidth() / 3.0f;
        g.drawText("IN", juce::Rectangle<int>((int)labelArea.getX(), (int)labelArea.getY(), (int)labelW, labelHeight), juce::Justification::centred);
        g.drawText("GR", juce::Rectangle<int>((int)(labelArea.getX() + labelW), (int)labelArea.getY(), (int)labelW, labelHeight), juce::Justification::centred);
        g.drawText("OUT", juce::Rectangle<int>((int)(labelArea.getX() + 2 * labelW), (int)labelArea.getY(), (int)labelW, labelHeight), juce::Justification::centred);
    }

    void CompressorVisualizer::drawBackgroundGrid(juce::Graphics &g,
                                                  juce::Rectangle<float> b)
    {
        g.setColour(juce::Colours::darkgrey);
        g.drawRect(b);
        g.setColour(juce::Colours::darkgrey.darker());
        for (int i = 0; i <= 4; ++i)
        {
            float x = b.getX() + i * (b.getWidth() / 4.0f);
            float y = b.getY() + i * (b.getHeight() / 4.0f);
            g.drawVerticalLine(x, b.getY(), b.getBottom());
            g.drawHorizontalLine(y, b.getX(), b.getRight());
        }
    }

    void CompressorVisualizer::drawCompressorCurve(juce::Graphics &g,
                                                   juce::Rectangle<float> b)
    {
        auto threshold = state.getRawParameterValue("threshold")->load();
        auto ratio = state.getRawParameterValue("ratio")->load();
        juce::Path curve;
        curve.startNewSubPath(b.getX(), b.getBottom());
        for (int px = 0; px < (int)b.getWidth(); ++px)
        {
            float inDb = juce::jmap((float)px, 0.f, b.getWidth(), -60.f, 10.f);
            float outDb = inDb;
            if (inDb > threshold)
                outDb = threshold + (inDb - threshold) / ratio;
            float y = juce::jmap(outDb, -60.f, 10.f, b.getBottom(), b.getY());
            curve.lineTo(b.getX() + px, y);
        }
        g.setColour(juce::Colours::grey);
        g.strokePath(curve, juce::PathStrokeType(2.f));
    }

    void CompressorVisualizer::drawGainReduction(juce::Graphics &g,
                                                 juce::Rectangle<float> b)
    {
        constexpr float maxDb = 40.f;
        float grDb = juce::jlimit(0.f, maxDb, currentGainReduction);
        float norm = grDb / maxDb;

        float w = b.getWidth() * 0.5f;
        float x = b.getX() + (b.getWidth() - w) / 2;
        float h = b.getHeight() * norm;
        float y = b.getBottom() - h;

        g.setColour(juce::Colours::lightblue);
        g.fillRect(x, y, w, h);

        g.setColour(juce::Colours::white);
        g.drawRect(x, b.getY(), w, b.getHeight(), 1.f);

        // steps of 3 up to 12, spaced evenly
        constexpr float tickMax = 12.f;
        g.saveState();
        g.reduceClipRegion(b.toNearestInt());
        g.setFont(10.f);
        g.setColour(juce::Colours::grey.withAlpha(0.7f));
        for (int d = 0; d <= 12; d += 3)
        {
            float yy = juce::jmap((float)d, 0.f, tickMax, b.getBottom(), b.getY());
            juce::String label = juce::String(d);
            g.drawText(label,
                       (int)x, (int)(yy - 4),
                       (int)w, 10,
                       juce::Justification::centred);
        }
        g.restoreState();
    }

    void CompressorVisualizer::drawInputMeter(juce::Graphics &g,
                                              juce::Rectangle<float> b)
    {
        constexpr float minDb = -60.f, maxDb = 10.f;
        float lvl = juce::jlimit(minDb, maxDb, currentInputLevel);
        float norm = (lvl - minDb) / (maxDb - minDb);

        float w = b.getWidth() * 0.5f;
        float x = b.getX() + (b.getWidth() - w) / 2;
        float h = b.getHeight() * norm;
        float y = b.getBottom() - h;

        g.setColour(juce::Colours::lightblue);
        g.fillRect(x, y, w, h);

        g.setColour(juce::Colours::white);
        g.drawRect(x, b.getY(), w, b.getHeight(), 1.f);

        g.saveState();
        g.reduceClipRegion(b.toNearestInt());
        g.setFont(10.f);
        g.setColour(juce::Colours::grey.withAlpha(0.7f));
        for (int i = 0; i <= 70; i += 10)
        {
            float dB = minDb + (maxDb - minDb) * (i / 70.f);
            float yy = juce::jmap(dB, minDb, maxDb, b.getBottom(), b.getY());
            juce::String label = juce::String((int)dB);
            g.drawText(label,
                       (int)x, (int)(yy - 4),
                       (int)w, 10,
                       juce::Justification::centred);
        }
        g.restoreState();
    }

    void CompressorVisualizer::drawOutputMeter(juce::Graphics &g,
                                               juce::Rectangle<float> b)
    {
        constexpr float minDb = -60.f, maxDb = 10.f;
        float lvl = juce::jlimit(minDb, maxDb, currentOutputLevel);
        float norm = (lvl - minDb) / (maxDb - minDb);

        float w = b.getWidth() * 0.5f;
        float x = b.getX() + (b.getWidth() - w) / 2;
        float h = b.getHeight() * norm;
        float y = b.getBottom() - h;

        g.setColour(juce::Colours::lightblue);
        g.fillRect(x, y, w, h);

        g.setColour(juce::Colours::white);
        g.drawRect(x, b.getY(), w, b.getHeight(), 1.f);

        g.saveState();
        g.reduceClipRegion(b.toNearestInt());
        g.setFont(10.f);
        g.setColour(juce::Colours::grey.withAlpha(0.7f));
        for (int i = 0; i <= 70; i += 10)
        {
            float dB = minDb + (maxDb - minDb) * (i / 70.f);
            float yy = juce::jmap(dB, minDb, maxDb, b.getBottom(), b.getY());
            juce::String label = juce::String((int)dB);
            g.drawText(label,
                       (int)x, (int)(yy - 4),
                       (int)w, 10,
                       juce::Justification::centred);
        }
        g.restoreState();
    }

    void CompressorVisualizer::timerCallback()
    {
        currentGainReduction = processor.getGainReduction();
        currentInputLevel = processor.getInputLevel();
        currentOutputLevel = processor.getOutputLevel();
        repaint();
    }
}
