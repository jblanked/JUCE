#pragma once
#include <JuceHeader.h>
#include <array>
#include "Service/PresetManager.h"
#define DEFAULT_PRESET_COUNT 2
#define JUCE_OFF 0.0
#define JUCE_ON 1.0
const Service::DefaultPreset defaultPresets[DEFAULT_PRESET_COUNT] =
    {
        {
            "Default",
            {
                {"inputGain", 0.f},
                {"threshold", 0.f},
                {"ratio", 1.f},
                {"attack", 50.f},
                {"release", 160.f},
                {"outputGain", 0.f},
            },
        }, // Default
        {
            "Vocal Bus",
            {
                {"inputGain", 0.f},
                {"threshold", -11.5f},
                {"ratio", 2.49f},
                {"attack", 5.02f},
                {"release", 50.2f},
                {"outputGain", 0.f},
            },
        }, // Vocal Bus
};
