#pragma once

#include <JuceHeader.h>
#include <map>
#include <any>

namespace Service
{
    enum ParameterType
    {
        ParameterTypeFloatEqualizerFrequency, // from 20 to 20000
        ParameterTypeFloatEqualizerGain,      // from -24 to 24
        ParameterTypeFloatEqualizerQuality,   // from 0.1 to 10
        ParameterTypeFloatEqualizerSlope,     // from 0 to 3 (12, 24, 36, 48)
        //
        ParameterTypeFloatCompressorGain,    // from -60 to 10
        ParameterTypeFloatCompressorRatio,   // from 1 to 20
        ParameterTypeFloatCompressorAttack,  // from 0 to 200
        ParameterTypeFloatCompressorRelease, // from 5 to 5000
        //
        ParameterTypeBool,   // true or false
        ParameterTypeChoice, // choice from a list
    };

    class ParameterManager
    {
    public:
        ParameterManager();
        bool addParameter(const juce::String &name, std::any defaultValue, Service::ParameterType type, std::any extras = std::any());
        const juce::AudioProcessorValueTreeState::ParameterLayout getLayout() { return std::move(layout); }

    private:
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        int parameterIndex;
        std::map<juce::String, std::any> parameterNames;
    };
}
