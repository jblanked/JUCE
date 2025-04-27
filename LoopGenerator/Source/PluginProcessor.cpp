#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

LoopGeneratorAudioProcessor::LoopGeneratorAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "Parameters", createParameters())
{
}

LoopGeneratorAudioProcessor::~LoopGeneratorAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout LoopGeneratorAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Loop parameters
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("numBars", 1), "Number of Bars", 1, 8, 4));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("beatsPerBar", 2), "Beats Per Bar", 3, 7, 4));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("bpm", 3), "BPM", 60.0f, 200.0f, 120.0f));

    // Audio generation parameters
    params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("waveform", 4), "Waveform",
                                                                  juce::StringArray("Sine", "Square", "Saw", "Triangle"), 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("frequency", 5), "Frequency", 20.0f, 20000.0f, 440.0f));

    return {params.begin(), params.end()};
}

void LoopGeneratorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Nothing to prepare for this specific plugin
}

void LoopGeneratorAudioProcessor::releaseResources()
{
    // Nothing to release for this specific plugin
}

void LoopGeneratorAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    // This plugin doesn't process audio in real-time - it generates loops on demand
    // Clear the buffer to ensure silence
    buffer.clear();
}

juce::AudioProcessorEditor *LoopGeneratorAudioProcessor::createEditor()
{
    return new LoopGeneratorAudioProcessorEditor(*this, parameters);
}

bool LoopGeneratorAudioProcessor::hasEditor() const
{
    return true;
}

const juce::String LoopGeneratorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int LoopGeneratorAudioProcessor::getNumPrograms()
{
    return 1;
}

int LoopGeneratorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LoopGeneratorAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String LoopGeneratorAudioProcessor::getProgramName(int index)
{
    return {};
}

void LoopGeneratorAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

void LoopGeneratorAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void LoopGeneratorAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new LoopGeneratorAudioProcessor();
}
