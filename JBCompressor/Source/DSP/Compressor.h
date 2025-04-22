#pragma once
#include <JuceHeader.h>

namespace DSP
{
    class Compressor
    {
    public:
        Compressor() = default;

        void prepareToPlay(const juce::dsp::ProcessSpec &spec)
        {
            inputModule.prepare(spec);
            inputModule.setRampDurationSeconds(0.02);
            compressorModule.prepare(spec);
            outputModule.prepare(spec);
            outputModule.setRampDurationSeconds(0.02);
        }

        void processBlock(juce::AudioBuffer<float> &buffer)
        {
            juce::dsp::AudioBlock<float> block{buffer};
            inputModule.process(juce::dsp::ProcessContextReplacing<float>(block));
            compressorModule.process(juce::dsp::ProcessContextReplacing<float>(block));
            outputModule.process(juce::dsp::ProcessContextReplacing<float>(block));
        }

        void updateParameters(float inGain, float thresh, float ratio,
                              float attackSec, float releaseSec, float outGain)
        {
            inputModule.setGainDecibels(inGain);
            compressorModule.setThreshold(thresh);
            compressorModule.setRatio(ratio);
            compressorModule.setAttack(attackSec);
            compressorModule.setRelease(releaseSec);
            outputModule.setGainDecibels(outGain);
        }

    private:
        juce::dsp::Gain<float> inputModule;
        juce::dsp::Compressor<float> compressorModule;
        juce::dsp::Gain<float> outputModule;
    };

    struct CompressorSettings
    {
        float inputGain{0.f};
        float threshold{0.f};
        float ratio{0.f};
        float attack{0.f};
        float release{0.f};
        float outputGain{0.f};
    };

    // Helper function that extracts settings from the AudioProcessorValueTreeState
    inline auto getCompressorSettings(juce::AudioProcessorValueTreeState &apvts,
                                      const juce::String &inputGainID = "inputGain",
                                      const juce::String &thresholdID = "threshold",
                                      const juce::String &ratioID = "ratio",
                                      const juce::String &attackID = "attack",
                                      const juce::String &releaseID = "release",
                                      const juce::String &outputGainID = "outputGain")
    {
        CompressorSettings settings;
        settings.inputGain = apvts.getRawParameterValue(inputGainID)->load();
        settings.threshold = apvts.getRawParameterValue(thresholdID)->load();
        settings.ratio = apvts.getRawParameterValue(ratioID)->load();
        settings.attack = apvts.getRawParameterValue(attackID)->load();
        settings.release = apvts.getRawParameterValue(releaseID)->load();
        settings.outputGain = apvts.getRawParameterValue(outputGainID)->load();
        return settings;
    }

}
