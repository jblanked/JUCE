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
        settings.peak2Freq = apvts.getRawParameterValue("peak2Frequency")->load();                      // get the second peak frequency from the apvts
        settings.peak2GainInDecibels = apvts.getRawParameterValue("peak2Gain")->load();                 // get the second peak gain from the apvts
        settings.peak2Quality = apvts.getRawParameterValue("peak2Quality")->load();                     // get the second peak quality from the apvt
        settings.peak3Freq = apvts.getRawParameterValue("peak3Frequency")->load();                      // get the third peak frequency from the apvts
        settings.peak3GainInDecibels = apvts.getRawParameterValue("peak3Gain")->load();                 // get the third peak gain from the apvts
        settings.peak3Quality = apvts.getRawParameterValue("peak3Quality")->load();                     // get the third peak quality from the apvts
        settings.peak4Freq = apvts.getRawParameterValue("peak4Frequency")->load();                      // get the fourth peak frequency from the apvts
        settings.peak4GainInDecibels = apvts.getRawParameterValue("peak4Gain")->load();                 // get the fourth peak gain from the apvts
        settings.peak4Quality = apvts.getRawParameterValue("peak4Quality")->load();                     // get the fourth peak quality from the apvts
        settings.lowShelfFreq = apvts.getRawParameterValue("lowShelfFrequency")->load();                // get the low shelf frequency from the apvts
        settings.lowShelfGain = apvts.getRawParameterValue("lowShelfGain")->load();                     // get the low shelf gain from the apvts
        settings.lowShelfQ = apvts.getRawParameterValue("lowShelfQ")->load();                           // get the low shelf Q from the apvts
        settings.highShelfFreq = apvts.getRawParameterValue("highShelfFrequency")->load();              // get the high shelf frequency from the apvts
        settings.highShelfGain = apvts.getRawParameterValue("highShelfGain")->load();                   // get the high shelf gain from the apvts
        settings.highShelfQ = apvts.getRawParameterValue("highShelfQ")->load();                         // get the high shelf Q from the apvts
        settings.lowCutSlope = static_cast<Slope>(apvts.getRawParameterValue("lowCutSlope")->load());   // get the low cut slope from the apvts
        settings.highCutSlope = static_cast<Slope>(apvts.getRawParameterValue("highCutSlope")->load()); // get the high cut slope from the apvts

        settings.lowCutBypassed = apvts.getRawParameterValue("lowCutBypass")->load() > 0.5f;   // get the low cut bypassed state from the apvts
        settings.lowShelfFreq = apvts.getRawParameterValue("lowShelfFrequency")->load();       // get the low shelf frequency from the apvts
        settings.peakBypassed = apvts.getRawParameterValue("peakBypass")->load() > 0.5f;       // get the peak bypassed state from the apvts
        settings.peak2Bypassed = apvts.getRawParameterValue("peak2Bypass")->load() > 0.5f;     // get the second peak bypassed state from the apvts
        settings.peak3Bypassed = apvts.getRawParameterValue("peak3Bypass")->load() > 0.5f;     // get the third peak bypassed state from the apvts
        settings.peak4Bypassed = apvts.getRawParameterValue("peak4Bypass")->load() > 0.5f;     // get the fourth peak bypassed state from the apvts
        settings.lowShelfGain = apvts.getRawParameterValue("lowShelfGain")->load();            // get the low shelf gain from the apvts
        settings.highCutBypassed = apvts.getRawParameterValue("highCutBypass")->load() > 0.5f; // get the high cut bypassed state from the apvts

        return settings; // return the settings struct
    }

    Coefficients makePeakFilter(const ChainSettings &chainSettings, double sampleRate, int peakIndex)
    {
        if (peakIndex < 1 || peakIndex > 4)
            return nullptr;

        auto nyquist = static_cast<float>(sampleRate / 2.0);
        float clampedPeakFreq = 0.0f;
        auto gain = 0.0f;
        juce::dsp::IIR::Coefficients<float>::Ptr peakFilter = nullptr;

        switch (peakIndex)
        {
        case 1:
            clampedPeakFreq = std::min(chainSettings.peakFreq, nyquist * 0.95f);
            gain = juce::Decibels::decibelsToGain(chainSettings.peakGainInDecibels);
            if (std::abs(gain - 1.0f) < 1e-5f)
                gain = 1.0f;
            peakFilter = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                sampleRate, clampedPeakFreq, chainSettings.peakQuality, gain);
            break;
        case 2:
            clampedPeakFreq = std::min(chainSettings.peak2Freq, nyquist * 0.95f);
            gain = juce::Decibels::decibelsToGain(chainSettings.peak2GainInDecibels);
            if (std::abs(gain - 1.0f) < 1e-5f)
                gain = 1.0f;
            peakFilter = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                sampleRate, clampedPeakFreq, chainSettings.peak2Quality, gain);
            break;
        case 3:
            clampedPeakFreq = std::min(chainSettings.peak3Freq, nyquist * 0.95f);
            gain = juce::Decibels::decibelsToGain(chainSettings.peak3GainInDecibels);
            if (std::abs(gain - 1.0f) < 1e-5f)
                gain = 1.0f;
            peakFilter = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                sampleRate, clampedPeakFreq, chainSettings.peak3Quality, gain);
            break;
        case 4:
            clampedPeakFreq = std::min(chainSettings.peak4Freq, nyquist * 0.95f);
            gain = juce::Decibels::decibelsToGain(chainSettings.peak4GainInDecibels);
            if (std::abs(gain - 1.0f) < 1e-5f)
                gain = 1.0f;
            peakFilter = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                sampleRate, clampedPeakFreq, chainSettings.peak4Quality, gain);
            break;
        default:
            return nullptr;
        }

        return peakFilter;
    }

    void updateCoefficients(Coefficients &old, const Coefficients &replacements)
    {
        // Update the old coefficients to match the new ones by dereferencing the pointers
        *old = *replacements;
    }
}
