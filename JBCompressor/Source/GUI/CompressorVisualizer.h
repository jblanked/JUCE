// GUI/CompressorVisualizer.h
#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"

namespace GUI
{
    class CompressorVisualizer : public juce::Component,
                                 private juce::Timer
    {
    public:
        CompressorVisualizer(juce::AudioProcessorValueTreeState &apvts,
                             JBCompressorAudioProcessor &processor);
        ~CompressorVisualizer() override;

        void paint(juce::Graphics &g) override;
        void resized() override {}

    private:
        void timerCallback() override;

        void drawBackgroundGrid(juce::Graphics &g, juce::Rectangle<float> bounds);
        void drawCompressorCurve(juce::Graphics &g, juce::Rectangle<float> bounds);
        void drawGainReduction(juce::Graphics &g, juce::Rectangle<float> bounds);
        void drawInputMeter(juce::Graphics &g, juce::Rectangle<float> bounds);
        void drawOutputMeter(juce::Graphics &g, juce::Rectangle<float> bounds);

        juce::AudioProcessorValueTreeState &state;
        JBCompressorAudioProcessor &processor;

        float currentGainReduction = 0.f;
        float currentInputLevel = 0.f;
        float currentOutputLevel = 0.f;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorVisualizer)
    };
}
