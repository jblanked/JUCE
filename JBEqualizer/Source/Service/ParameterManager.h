#pragma once

#include <JuceHeader.h>
#include <map>
#include <any>

namespace Service
{
    enum ParameterType
    {
        ParameterTypeFloatFrequency,
        ParameterTypeFloatGain,
        ParameterTypeFloatQuality,
        ParameterTypeBool,
        ParameterTypeFloatSlope,
        ParameterTypeChoice,
    };

    class ParameterManager
    {
    public:
        ParameterManager();
        bool addParameter(const juce::String &name, std::any defaultValue, Service::ParameterType type, std::any extras = std::any());
        const juce::AudioProcessorValueTreeState::ParameterLayout &getLayout() const { return layout; }

    private:
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        int parameterIndex;
    };
}
