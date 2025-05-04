#include "../DSP/DSPUtilities.h"

namespace DSP
{

    Coefficients makePeakFilter(float peakFreq, float peakGain, float peakQuality, double sampleRate)
    {
        // Clamp the peak frequency to a maximum of 95% of Nyquist frequency
        float nyquist = static_cast<float>(sampleRate / 2.0);
        float clampedPeakFreq = std::min(peakFreq, nyquist * 0.95f);

        // Convert gain from decibels to linear scale
        float gain = juce::Decibels::decibelsToGain(peakGain);

        if (std::abs(gain - 1.0f) < 1e-5f)
            gain = 1.0f;

        // Create and return the peak filter coefficients
        return juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, clampedPeakFreq, peakQuality, gain);
    }

    CutCoefficients makeCutFilter(float cutFreq, Slope slope, double sampleRate, bool isHighCut)
    {
        // Clamp the cut frequency to a maximum of 95% of Nyquist frequency
        float nyquist = static_cast<float>(sampleRate / 2.0);
        float clampedCutFreq = std::min(cutFreq, nyquist * 0.95f);

        if (isHighCut)
            return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(clampedCutFreq, sampleRate, 2 * (1 + static_cast<int>(slope)));
        else
            return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(clampedCutFreq, sampleRate, 2 * (1 + static_cast<int>(slope)));
    }

    Coefficients makeShelfFilter(float shelfFreq, float shelfGain, float shelfQuality, double sampleRate, bool isHighShelf)
    {
        float nyquist = static_cast<float>(sampleRate / 2.0);
        // Use a default minimum (e.g., 20 Hz) if the value is 0 or less
        float freqToUse = (shelfFreq <= 0.0f) ? 20.0f : shelfFreq;

        // Clamp frequency if necessary (e.g., to 95% of Nyquist)
        float clampedFreq = std::min(freqToUse, nyquist * 0.95f);

        float linearGain = juce::Decibels::decibelsToGain(shelfGain);

        return isHighShelf ? juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, clampedFreq, shelfQuality, linearGain)
                           : juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, clampedFreq, shelfQuality, linearGain);
    }

    void updateCoefficients(Coefficients &old, const Coefficients &replacements)
    {
        // Update the old coefficients to match the new ones by dereferencing the pointers
        *old = *replacements;
    }
}
