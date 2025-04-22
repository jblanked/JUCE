#include "ParameterManager.h"

namespace Service
{
    ParameterManager::ParameterManager()
        : layout(), parameterIndex(1)
    {
    }

    bool ParameterManager::addParameter(const juce::String &name, std::any defaultValue, Service::ParameterType type, std::any extras)
    {
        if (name.isEmpty() || !defaultValue.has_value())
        {
            DBG("ParameterManager::addParameter: Invalid name or default value");
            return false;
        }

        juce::String newName = name.trim();

        // Check if the name already exists
        if (parameterNames.find(newName) != parameterNames.end())
        {
            // no duplicate names
            DBG("ParameterManager::addParameter: Duplicate parameter name: " << newName);
            jassert("Duplicate parameter name");
            return false;
        }

        switch (type)
        {
        case Service::ParameterTypeBool:
            layout.add(std::make_unique<juce::AudioParameterBool>(
                juce::ParameterID(newName, parameterIndex),
                name,
                std::any_cast<bool>(defaultValue)));
            parameterNames[newName] = name;
            parameterIndex++;
            return true;
        case Service::ParameterTypeFloatEqualizerFrequency:
            layout.add(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(newName, parameterIndex),
                name,
                juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.5f),
                std::any_cast<float>(defaultValue)));
            parameterNames[newName] = name;
            parameterIndex++;
            return true;
        case Service::ParameterTypeFloatEqualizerGain:
            layout.add(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(newName, parameterIndex),
                name,
                juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                std::any_cast<float>(defaultValue)));
            parameterNames[newName] = name;
            parameterIndex++;
            return true;
        case Service::ParameterTypeFloatEqualizerQuality:
            layout.add(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(newName, parameterIndex),
                name,
                juce::NormalisableRange<float>(0.1f, 10.f, 0.1f, 1.f),
                std::any_cast<float>(defaultValue)));
            parameterNames[newName] = name;
            parameterIndex++;
            return true;
        case Service::ParameterTypeFloatEqualizerSlope:
        {
            juce::StringArray filterTypes{"12 dB/Oct", "24 dB/Oct", "36 dB/Oct", "48 dB/Oct"};
            layout.add(std::make_unique<juce::AudioParameterChoice>(
                juce::ParameterID(newName, parameterIndex),
                name,
                filterTypes,
                std::any_cast<int>(defaultValue)));
            parameterNames[newName] = name;
            parameterIndex++;
            return true;
        }
        case Service::ParameterTypeChoice:
            if (extras.has_value())
            {
                juce::StringArray choices = std::any_cast<juce::StringArray>(extras);
                layout.add(std::make_unique<juce::AudioParameterChoice>(
                    juce::ParameterID(newName, parameterIndex),
                    name,
                    choices,
                    std::any_cast<int>(defaultValue)));
                parameterNames[newName] = name;
                parameterIndex++;
                return true;
            }
            DBG("ParameterManager::addParameter: No choices provided for choice type");
            break;
        case Service::ParameterTypeFloatCompressorGain:
            layout.add(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(newName, parameterIndex),
                name,
                juce::NormalisableRange<float>(-60.f, 10.f, 0.5f, 1.f),
                std::any_cast<float>(defaultValue)));
            parameterNames[newName] = name;
            parameterIndex++;
            return true;
        case Service::ParameterTypeFloatCompressorRatio:
            layout.add(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(newName, parameterIndex),
                name,
                juce::NormalisableRange<float>(1.f, 20.f, 0.5f, 1.f),
                std::any_cast<float>(defaultValue)));
            parameterNames[newName] = name;
            parameterIndex++;
            return true;
        case Service::ParameterTypeFloatCompressorAttack:
        {
            juce::NormalisableRange<float> attackRange = juce::NormalisableRange<float>(0.f, 200.f, 1.f);
            attackRange.setSkewForCentre(50.f);
            layout.add(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(newName, parameterIndex),
                name,
                attackRange,
                std::any_cast<float>(defaultValue)));
            parameterNames[newName] = name;
            parameterIndex++;
            return true;
        }
        case Service::ParameterTypeFloatCompressorRelease:
        {
            juce::NormalisableRange<float> releaseRange = juce::NormalisableRange<float>(5.f, 5000.f, 1.f);
            releaseRange.setSkewForCentre(160.f);
            layout.add(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID(newName, parameterIndex),
                name,
                releaseRange,
                std::any_cast<float>(defaultValue)));
            parameterNames[newName] = name;
            parameterIndex++;
            return true;
        }
        default:
            DBG("ParameterManager::addParameter: Unknown parameter type");
            break;
        }
        return false;
    }
}
