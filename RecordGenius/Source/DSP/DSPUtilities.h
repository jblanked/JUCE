#pragma once

#include <JuceHeader.h>

namespace DSP
{
    //=====================================================================
    // Filter Type Aliases
    //=====================================================================
    using Filter = juce::dsp::IIR::Filter<float>;

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

    using Coefficients = Filter::CoefficientsPtr;
    using CutCoefficients = juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>>;

    void updateCoefficients(Coefficients &old, const Coefficients &replacements);
    Coefficients makePeakFilter(float peakFreq, float peakGain, float peakQuality, double sampleRate);
    CutCoefficients makeCutFilter(float cutFreq, Slope slope, double sampleRate, bool isHighCut);
    Coefficients makeShelfFilter(float shelfFreq, float shelfGain, float shelfQuality, double sampleRate, bool isHighShelf);

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

} // namespace DSP
