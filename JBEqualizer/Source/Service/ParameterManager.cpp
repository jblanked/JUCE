#include "ParameterManager.h"

namespace Service
{
    ParameterManager::ParameterManager()
        : layout(), parameterIndex(0)
    {
    }

    bool ParameterManager::addParameter(const juce::String &name, std::any defaultValue, Service::ParameterType type, std::any extras)
    {
        if (name.isEmpty() || !defaultValue.has_value())
        {
            DBG("ParameterManager::addParameter: Invalid name or default value");
            return false;
        }
        switch (type)
        {
        case Service::ParameterTypeBool:
            layout.add(std::make_unique<juce::AudioParameterBool>(
                juce::ParameterID(name.trim() + juce::String(parameterIndex), parameterIndex),
                name,
                std::any_cast<bool>(defaultValue)));
            parameterIndex++;
            return true;
        case Service::ParameterTypeFloatFrequency:
            layout.add(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(name.trim() + juce::String(parameterIndex), parameterIndex),
                name,
                juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.5f),
                std::any_cast<float>(defaultValue)));
            parameterIndex++;
            return true;
        case Service::ParameterTypeFloatGain:
            layout.add(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(name.trim() + juce::String(parameterIndex), parameterIndex),
                name,
                juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                std::any_cast<float>(defaultValue)));
            parameterIndex++;
            return true;
        case Service::ParameterTypeFloatQuality:
            layout.add(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(name.trim() + juce::String(parameterIndex), parameterIndex),
                name,
                juce::NormalisableRange<float>(0.1f, 10.f, 0.1f, 1.f),
                std::any_cast<float>(defaultValue)));
            parameterIndex++;
            return true;
        case Service::ParameterTypeFloatSlope:
        {
            juce::StringArray filterTypes{"12 dB/Oct", "24 dB/Oct", "36 dB/Oct", "48 dB/Oct"};
            layout.add(std::make_unique<juce::AudioParameterChoice>(
                juce::ParameterID(name.trim() + juce::String(parameterIndex), parameterIndex),
                name,
                filterTypes,
                std::any_cast<int>(defaultValue)));
            parameterIndex++;
            return true;
        }
        case Service::ParameterTypeChoice:
            if (extras.has_value())
            {
                juce::StringArray choices = std::any_cast<juce::StringArray>(extras);
                layout.add(std::make_unique<juce::AudioParameterChoice>(
                    juce::ParameterID(name.trim() + juce::String(parameterIndex), parameterIndex),
                    name,
                    choices,
                    std::any_cast<int>(defaultValue)));
                parameterIndex++;
                return true;
            }
            DBG("ParameterManager::addParameter: No choices provided for choice type");
            break;
        }
        return false;
    }
}
