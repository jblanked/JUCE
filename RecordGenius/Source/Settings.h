#pragma once

#include <JuceHeader.h>
#include "DSP/DSPUtilities.h"

using namespace DSP; // for the DSP utilities
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
inline auto getChainSettings(juce::AudioProcessorValueTreeState &apvts)
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
