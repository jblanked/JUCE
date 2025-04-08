#include "../DSP/DSPUtilities.h"

namespace DSP
{
    ChainSettings getChainSettings(juce::AudioProcessorValueTreeState &apvts)
    {
        ChainSettings settings; // initialize the settings struct

        settings.lowCutFreq = apvts.getRawParameterValue("lowCutFrequency")->load();                    // get the low cut frequency from the apvts
        settings.highCutFreq = apvts.getRawParameterValue("highCutFrequency")->load();                  // get the high cut frequency from the apvts
        settings.peakFreq = apvts.getRawParameterValue("peakFrequency")->load();                        // get the peak frequency from the apvts
        settings.peakGainInDecibels = apvts.getRawParameterValue("peakGain")->load();                   // get the peak gain from the apvts
        settings.peakQuality = apvts.getRawParameterValue("peakQuality")->load();                       // get the peak quality from the apvts
        settings.lowCutSlope = static_cast<Slope>(apvts.getRawParameterValue("lowCutSlope")->load());   // get the low cut slope from the apvts
        settings.highCutSlope = static_cast<Slope>(apvts.getRawParameterValue("highCutSlope")->load()); // get the high cut slope from the apvts

        settings.lowCutBypassed = apvts.getRawParameterValue("lowCutBypass")->load() > 0.5f;   // get the low cut bypassed state from the apvts
        settings.peakBypassed = apvts.getRawParameterValue("peakBypass")->load() > 0.5f;       // get the peak bypassed state from the apvts
        settings.highCutBypassed = apvts.getRawParameterValue("highCutBypass")->load() > 0.5f; // get the high cut bypassed state from the apvts

        return settings; // return the settings struct
    }

    Coefficients makePeakFilter(const ChainSettings &chainSettings, double sampleRate)
    {
        auto nyquist = static_cast<float>(sampleRate / 2.0);
        float clampedPeakFreq = std::min(chainSettings.peakFreq, nyquist * 0.95f);

        auto gain = juce::Decibels::decibelsToGain(chainSettings.peakGainInDecibels);
        if (std::abs(gain - 1.0f) < 1e-5f)
            gain = 1.0f;

        return juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
                                                                   clampedPeakFreq,
                                                                   gain,
                                                                   chainSettings.peakQuality);
    }

    void updateCoefficients(Coefficients &old, const Coefficients &replacements)
    {
        // Update the old coefficients to match the new ones by dereferencing the pointers
        *old = *replacements;
    }
}
