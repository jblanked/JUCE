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
        float lowCutFreq{0.0f};
        float highCutFreq{0.0f};
        Slope lowCutSlope{Slope_12};
        Slope highCutSlope{Slope_12};
        bool lowCutBypassed{false};
        bool peakBypassed{false};
        bool highCutBypassed{false};
    };

    // Helper function that extracts settings from the AudioProcessorValueTreeState
    ChainSettings getChainSettings(juce::AudioProcessorValueTreeState &apvts);

    //=====================================================================
    // Filter Type Aliases
    //=====================================================================
    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

    //=====================================================================
    // Enum for Chain Positions
    //=====================================================================
    enum ChainPositions
    {
        LowCut,
        Peak,
        HighCut
    };

    using Coefficients = Filter::CoefficientsPtr;

    //=====================================================================
    // DSP Utility Functions
    //=====================================================================
    void updateCoefficients(Coefficients &old, const Coefficients &replacements);
    Coefficients makePeakFilter(const ChainSettings &chainSettings, double sampleRate);

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
        return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
            chainSettings.lowCutFreq,
            sampleRate,
            2 * (1 + static_cast<int>(chainSettings.lowCutSlope)) // Using explicit cast for clarity
        );
    }

    inline auto makeHighCutFilter(const ChainSettings &chainSettings, double sampleRate)
    {
        return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
            chainSettings.highCutFreq,
            sampleRate,
            2 * (1 + static_cast<int>(chainSettings.highCutSlope)));
    }

} // namespace DSP
