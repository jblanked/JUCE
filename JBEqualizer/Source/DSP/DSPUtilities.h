#pragma once

#include <JuceHeader.h>

namespace DSP
{

    //=====================================================================
    // Enum for Slope Settings
    //=====================================================================
    enum Slope
    {
        Slope_12,
        Slope_24,
        Slope_36,
        Slope_48
    };

    //=====================================================================
    // Chain Settings Structure
    //=====================================================================
    struct ChainSettings
    {
        float peakFreq{0.0f};
        float peakGainInDecibels{0.0f};
        float peakQuality{1.0f};
        //
        float peak2Freq{0.0f};
        float peak2GainInDecibels{0.0f};
        float peak2Quality{1.0f};
        //
        float peak3Freq{0.0f};
        float peak3GainInDecibels{0.0f};
        float peak3Quality{1.0f};
        //
        float peak4Freq{0.0f};
        float peak4GainInDecibels{0.0f};
        float peak4Quality{1.0f};
        //
        float lowShelfFreq{0.0f};
        float lowShelfGain{0.0f};
        float lowShelfQ{1.0f};
        //
        float highShelfFreq{0.0f};
        float highShelfGain{0.0f};
        float highShelfQ{1.0f};
        //
        float lowCutFreq{0.0f};
        float highCutFreq{0.0f};
        //
        Slope lowCutSlope{Slope_12};
        Slope highCutSlope{Slope_12};
        //
        bool lowCutBypassed{false};
        bool highCutBypassed{false};
        //
        bool peakBypassed{false};
        bool peak2Bypassed{false};
        bool peak3Bypassed{false};
        bool peak4Bypassed{false};
        //
        bool lowShelfBypassed{false};
        bool highShelfBypassed{false};
        //
        bool analyzerEnabled{true};
    };

    // Helper function that extracts settings from the AudioProcessorValueTreeState
    ChainSettings getChainSettings(juce::AudioProcessorValueTreeState &apvts);

    //=====================================================================
    // Filter Type Aliases
    //=====================================================================
    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, Filter, Filter, Filter, Filter, Filter, CutFilter>;
    // Low pass filter (for the low end)
    // Low shelf filter
    // Peak filter 1
    // Peak filter 2
    // Peak filter 3
    // Peak filter 4
    // High shelf filter
    // High pass filter (for the high end)

    //=====================================================================
    // Enum for Chain Positions
    //=====================================================================
    enum ChainPositions
    {
        LowCut,
        LowShelf,
        Peak1,
        Peak2,
        Peak3,
        Peak4,
        HighShelf,
        HighCut
    };

    using Coefficients = Filter::CoefficientsPtr;

    //=====================================================================
    // DSP Utility Functions
    //=====================================================================
    void updateCoefficients(Coefficients &old, const Coefficients &replacements);
    Coefficients makePeakFilter(const ChainSettings &chainSettings, double sampleRate, int peakIndex);

    template <int Index, typename ChainType, typename CoefficientsType>
    void update(ChainType &chain, const CoefficientsType &coefficients)
    {
        updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]); // Update coefficients
        chain.template setBypassed<Index>(false);                                          // Ensure the chain is active
    }

    template <typename ChainType, typename CoefficientsType>
    void updateCutFilter(ChainType &chain, const CoefficientsType &coefficients, const Slope &slope)
    {
        // Bypass all filter stages initially
        chain.template setBypassed<0>(true);
        chain.template setBypassed<1>(true);
        chain.template setBypassed<2>(true);
        chain.template setBypassed<3>(true);

        // Intentionally fall through to update the necessary stages based on the slope
        switch (slope)
        {
        case Slope_48:
            update<3>(chain, coefficients);
            [[fallthrough]];
        case Slope_36:
            update<2>(chain, coefficients);
            [[fallthrough]];
        case Slope_24:
            update<1>(chain, coefficients);
            [[fallthrough]];
        case Slope_12:
            update<0>(chain, coefficients);
            break;
        }
    }

    inline auto makeLowCutFilter(const ChainSettings &chainSettings, double sampleRate)
    {
        auto nyquist = static_cast<float>(sampleRate / 2.0);
        // Clamp the low cut frequency to be at most 95% of Nyquist
        float clampedLowCut = std::min(chainSettings.lowCutFreq, nyquist * 0.95f);

        return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
            clampedLowCut,
            sampleRate,
            2 * (1 + static_cast<int>(chainSettings.lowCutSlope)));
    }

    inline auto makeHighCutFilter(const ChainSettings &chainSettings, double sampleRate)
    {
        auto nyquist = static_cast<float>(sampleRate / 2.0);
        // Clamp the high cut frequency to be at most 95% of Nyquist
        float clampedHighCut = std::min(chainSettings.highCutFreq, nyquist * 0.95f);

        return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
            clampedHighCut,
            sampleRate,
            2 * (1 + static_cast<int>(chainSettings.highCutSlope)));
    }

    inline auto makeLowShelfFilter(const ChainSettings &settings, double sampleRate)
    {
        float nyquist = static_cast<float>(sampleRate / 2.0);
        // Use a default minimum (e.g., 20 Hz) if the value is 0 or less
        float freqToUse = (settings.lowShelfFreq <= 0.0f) ? 20.0f : settings.lowShelfFreq;

        // Clamp frequency if necessary (e.g., to 95% of Nyquist)
        float clampedFreq = std::min(freqToUse, nyquist * 0.95f);

        float linearGain = juce::Decibels::decibelsToGain(settings.lowShelfGain);

        return juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, clampedFreq, settings.lowShelfQ, linearGain);
    }

    inline auto makeHighShelfFilter(const ChainSettings &settings, double sampleRate)
    {
        float nyquist = static_cast<float>(sampleRate / 2.0);
        // Use a safe minimum (20 Hz) if highShelfFreq is 0 or less
        float freqToUse = (settings.highShelfFreq <= 0.0f) ? 20.0f : settings.highShelfFreq;
        // Clamp the frequency to be at most 95% of Nyquist
        float clampedFreq = std::min(freqToUse, nyquist * 0.95f);
        float linearGain = juce::Decibels::decibelsToGain(settings.highShelfGain);
        return juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, clampedFreq, settings.highShelfQ, linearGain);
    }

} // namespace DSP
