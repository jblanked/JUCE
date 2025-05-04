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

            // —– INPUT GAIN & MEASURE INPUT LEVEL —–
            inputModule.process(juce::dsp::ProcessContextReplacing<float>(block));
            float maxLevel = 0.0f;
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                auto *data = buffer.getReadPointer(ch);
                for (int i = 0; i < buffer.getNumSamples(); ++i)
                    maxLevel = std::max(maxLevel, std::abs(data[i]));
            }
            float inputLevelDb = juce::Decibels::gainToDecibels(maxLevel);
            currentInputLevel = inputLevelDb;

            // —– COMPRESSOR —–
            compressorModule.process(juce::dsp::ProcessContextReplacing<float>(block));

            // —– MEASURE OUTPUT LEVEL —–
            maxLevel = 0.0f;
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                auto *data = buffer.getReadPointer(ch);
                for (int i = 0; i < buffer.getNumSamples(); ++i)
                    maxLevel = std::max(maxLevel, std::abs(data[i]));
            }
            float outputLevelDb = juce::Decibels::gainToDecibels(maxLevel);
            currentOutputLevel = outputLevelDb;

            // —– GAIN REDUCTION —–
            currentGainReduction = inputLevelDb - outputLevelDb;

            // —– OUTPUT GAIN —–
            outputModule.process(juce::dsp::ProcessContextReplacing<float>(block));
        }

        // Accessors for visualizer
        float getGainReduction() const noexcept { return currentGainReduction; }
        float getInputLevel() const noexcept { return currentInputLevel; }
        float getOutputLevel() const noexcept { return currentOutputLevel; }

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

        float currentGainReduction = 0.0f;
        float currentInputLevel = 0.0f;
        float currentOutputLevel = 0.0f;
    };

    struct CompressorSettings
    {
        float inputGain{0.f}, threshold{0.f}, ratio{0.f},
            attack{0.f}, release{0.f}, outputGain{0.f};
    };

    inline auto getCompressorSettings(juce::AudioProcessorValueTreeState &apvts,
                                      const juce::String &inID = "inputGain",
                                      const juce::String &threshID = "threshold",
                                      const juce::String &ratioID = "ratio",
                                      const juce::String &attackID = "attack",
                                      const juce::String &releaseID = "release",
                                      const juce::String &outGainID = "outputGain")
    {
        CompressorSettings s;
        s.inputGain = apvts.getRawParameterValue(inID)->load();
        s.threshold = apvts.getRawParameterValue(threshID)->load();
        s.ratio = apvts.getRawParameterValue(ratioID)->load();
        s.attack = apvts.getRawParameterValue(attackID)->load();
        s.release = apvts.getRawParameterValue(releaseID)->load();
        s.outputGain = apvts.getRawParameterValue(outGainID)->load();
        return s;
    }
}
