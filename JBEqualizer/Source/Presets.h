#pragma once
#include <JuceHeader.h>
#include <array>
#include "Service/PresetManager.h"
#define DEFAULT_PRESET_COUNT 4
#define JUCE_OFF 0.0
#define JUCE_ON 1.0
// frequency values in Hz from 20Hz to 20kHz
// gain values in dB from -24dB to +24dB
// quality values from 0.1 to 10
// slope values from 12, 24, 36, 48
// bypass values 0 or 1
const Service::DefaultPreset defaultPresets[] =
    {
        {
            "Main Vocals",
            {{"lowCutFrequency", 106.f},     // 1
             {"highCutFrequency", 17000.f},  // 2
             {"peakFrequency", 280.f},       // 3
             {"peakGain", 2.4f},             // 4
             {"peakQuality", 4.3f},          // 5
             {"peak2Frequency", 340.f},      // 6
             {"peak2Gain", -3.5f},           // 7
             {"peak2Quality", 5.6f},         // 8
             {"peak3Frequency", 1600.f},     // 9
             {"peak3Gain", 3.5f},            // 10
             {"peak3Quality", 4.5f},         // 11
             {"peak4Frequency", 12000.f},    // 12
             {"peak4Gain", 3.f},             // 13
             {"peak4Quality", 3.4f},         // 14
             {"lowShelfFrequency", 80.f},    // 15
             {"lowShelfGain", 0.f},          // 16
             {"lowShelfQ", 1.1f},            // 17
             {"highShelfFrequency", 2000.f}, // 18
             {"highShelfGain", 3.f},         // 19
             {"highShelfQ", 0.63f},          // 20
             {"lowCutSlope", 24.0},          // 21
             {"highCutSlope", 12.0},         // 22
             {"lowCutBypass", JUCE_OFF},     // 23
             {"peakBypass", JUCE_OFF},       // 24
             {"peak2Bypass", JUCE_OFF},      // 25
             {"peak3Bypass", JUCE_OFF},      // 26
             {"peak4Bypass", JUCE_OFF},      // 27
             {"lowShelfBypass", JUCE_OFF},   // 28
             {"highShelfBypass", JUCE_OFF},  // 29
             {"highCutBypass", JUCE_ON},     // 30
             {"analyzerEnabled", JUCE_ON}},  // 31
        },                                   // Main Vocals
        {
            "Background Vocals",
            {{"lowCutFrequency", 246.f},     // 1
             {"highCutFrequency", 17000.f},  // 2
             {"peakFrequency", 220.f},       // 3
             {"peakGain", -2.f},             // 4
             {"peakQuality", 2.f},           // 5
             {"peak2Frequency", 340.f},      // 6
             {"peak2Gain", -3.5f},           // 7
             {"peak2Quality", 5.6f},         // 8
             {"peak3Frequency", 940.f},      // 9
             {"peak3Gain", -2.f},            // 10
             {"peak3Quality", 11.f},         // 11
             {"peak4Frequency", 2200.f},     // 12
             {"peak4Gain", -3.5f},           // 13
             {"peak4Quality", 2.f},          // 14
             {"lowShelfFrequency", 198.f},   // 15
             {"lowShelfGain", -24.f},        // 16
             {"lowShelfQ", 1.1f},            // 17
             {"highShelfFrequency", 8500.f}, // 18
             {"highShelfGain", 7.5f},        // 19
             {"highShelfQ", 0.28f},          // 20
             {"lowCutSlope", 12.0},          // 21
             {"highCutSlope", 12.0},         // 22
             {"lowCutBypass", JUCE_OFF},     // 23
             {"peakBypass", JUCE_OFF},       // 24
             {"peak2Bypass", JUCE_ON},       // 25
             {"peak3Bypass", JUCE_OFF},      // 26
             {"peak4Bypass", JUCE_OFF},      // 27
             {"lowShelfBypass", JUCE_OFF},   // 28
             {"highShelfBypass", JUCE_OFF},  // 29
             {"highCutBypass", JUCE_ON},     // 30
             {"analyzerEnabled", JUCE_ON}},  // 31
        },                                   // Background Vocals
        {
            "Vocal Bus 1",
            {{"lowCutFrequency", 100.f},     // 1
             {"highCutFrequency", 20000.f},  // 2
             {"peakFrequency", 35.f},        // 3
             {"peakGain", -11.5f},           // 4
             {"peakQuality", 1.f},           // 5
             {"peak2Frequency", 346.f},      // 6
             {"peak2Gain", -5.5f},           // 7
             {"peak2Quality", 1.f},          // 8
             {"peak3Frequency", 1000.f},     // 9
             {"peak3Gain", -1.5f},           // 10
             {"peak3Quality", 1.f},          // 11
             {"peak4Frequency", 3000.f},     // 12
             {"peak4Gain", 2.f},             // 13
             {"peak4Quality", 1.f},          // 14
             {"lowShelfFrequency", 75.f},    // 15
             {"lowShelfGain", 0.f},          // 16
             {"lowShelfQ", 1.f},             // 17
             {"highShelfFrequency", 6200.f}, // 18
             {"highShelfGain", 4.5f},        // 19
             {"highShelfQ", 1.f},            // 20
             {"lowCutSlope", 24.0},          // 21
             {"highCutSlope", 24.0},         // 22
             {"lowCutBypass", JUCE_OFF},     // 23
             {"peakBypass", JUCE_OFF},       // 24
             {"peak2Bypass", JUCE_OFF},      // 25
             {"peak3Bypass", JUCE_OFF},      // 26
             {"peak4Bypass", JUCE_OFF},      // 27
             {"lowShelfBypass", JUCE_OFF},   // 28
             {"highShelfBypass", JUCE_OFF},  // 29
             {"highCutBypass", JUCE_ON},     // 30
             {"analyzerEnabled", JUCE_ON}},  // 31
        },                                   // Vocal Bus 1
        {
            "Vocal Bus 2",
            {{"lowCutFrequency", 20.f},      // 1
             {"highCutFrequency", 20000.f},  // 2
             {"peakFrequency", 70.f},        // 3
             {"peakGain", -24.f},            // 4
             {"peakQuality", 0.19f},         // 5
             {"peak2Frequency", 250.f},      // 6
             {"peak2Gain", 0.f},             // 7
             {"peak2Quality", 0.3f},         // 8
             {"peak3Frequency", 750.f},      // 9
             {"peak3Gain", 0.f},             // 10
             {"peak3Quality", 0.3f},         // 11
             {"peak4Frequency", 2500.f},     // 12
             {"peak4Gain", 0.f},             // 13
             {"peak4Quality", 0.2f},         // 14
             {"lowShelfFrequency", 54.f},    // 15
             {"lowShelfGain", -2.5f},        // 16
             {"lowShelfQ", 1.f},             // 17
             {"highShelfFrequency", 7500.f}, // 18
             {"highShelfGain", 0.f},         // 19
             {"highShelfQ", 1.f},            // 20
             {"lowCutSlope", 24.0},          // 21
             {"highCutSlope", 24.0},         // 22
             {"lowCutBypass", JUCE_ON},      // 23
             {"peakBypass", JUCE_OFF},       // 24
             {"peak2Bypass", JUCE_ON},       // 25
             {"peak3Bypass", JUCE_ON},       // 26
             {"peak4Bypass", JUCE_ON},       // 27
             {"lowShelfBypass", JUCE_OFF},   // 28
             {"highShelfBypass", JUCE_ON},   // 29
             {"highCutBypass", JUCE_ON},     // 30
             {"analyzerEnabled", JUCE_ON}},  // 31
        } // Vocal Bus 2
};
/*
Name: LowCut Freq
ID: lowCutFrequency
Index: 1
Type: float
//
Name: HighCut Freq
ID: highCutFrequency
Index: 2
Type: float
//
Name: Peak Freq
ID: peakFrequency
Index: 3
Type: float
//
Name: Peak Gain
ID: peakGain
Index: 4
Type: float
//
Name: Peak Quality
ID: peakQuality
Index: 5
//
Name: Peak 2 Freq
ID: peak2Frequency
Index: 6
Type: float
//
Name: Peak 2 Gain
ID: peak2Gain
Index: 7
Type: float
//
Name: Peak 2 Quality
ID: peak2Quality
Index: 8
//
Name: Peak 3 Freq
ID: peak3Frequency
Index: 9
Type: float
//
Name: Peak 3 Gain
ID: peak3Gain
Index: 10
Type: float
//
Name: Peak 3 Quality
ID: peak3Quality
Index: 11
//
Name: Peak 4 Freq
ID: peak4Frequency
Index: 12
Type: float
//
Name: Peak 4 Gain
ID: peak4Gain
Index: 13
Type: float
//
Name: Peak 4 Quality
ID: peak4Quality
Index: 14
Type: float
//
Name: Low Shelf Freq
ID: lowShelfFrequency
Index: 15
Type: float
//
Name: Low Shelf Gain
ID: lowShelfGain
Index: 16
Type: float
//
Name: Low Shelf Q
ID: lowShelfQ
Index: 17
Type: float
//
Name: High Shelf Freq
ID: highShelfFrequency
Index: 18
Type: float
//
Name: High Shelf Gain
ID: highShelfGain
Index: 19
Type: float
//
Name: High Shelf Q
ID: highShelfQ
Index: 20
//
Name: LowCut Slope
ID: lowCutSlope
Index: 21
Type: int
//
Name: HighCut Slope
ID: highCutSlope
Index: 22
Type: int
//
Name: LowCut Bypass
ID: lowCutBypass
Index: 23
Type: bool
//
Name: Peak Bypass
ID: peakBypass
Index: 24
Type: bool
//
Name: Peak 2 Bypass
ID: peak2Bypass
Index: 25
Type: bool
//
Name: Peak 3 Bypass
ID: peak3Bypass
Index: 26
Type: bool
//
Name: Peak 4 Bypass
ID: peak4Bypass
Index: 27
Type: bool
//
Name: Low Shelf Bypass
ID: lowShelfBypass
Index: 28
Type: bool
//
Name: High Shelf Bypass
ID: highShelfBypass
Index: 29
Type: bool
//
Name: HighCut Bypass
ID: highCutBypass
Index: 30
Type: bool
//
Name: Analyzer Enabled
ID: analyzerEnabled
Index: 31
Type: bool
*/
