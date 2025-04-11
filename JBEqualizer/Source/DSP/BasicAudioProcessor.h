#pragma once
#include <JuceHeader.h>
#include "../DSP/Fifo.h"
#include "../DSP/DSPUtilities.h"

class BasicAudioProcessor : public juce::AudioProcessor
{
public:
    virtual ~BasicAudioProcessor() = default;

    virtual juce::AudioProcessorValueTreeState &getAPVTS() = 0;

    using BlockType = juce::AudioBuffer<float>;
    // SingleChannelSampleFifo<BlockType> leftChannelFifo{Channel::Left};
    // SingleChannelSampleFifo<BlockType> rightChannelFifo{Channel::Right};
    virtual SingleChannelSampleFifo<BlockType> &getLeftChannelFifo() = 0;
    virtual SingleChannelSampleFifo<BlockType> &getRightChannelFifo() = 0;

    // Provide default implementations of the required accessors.
    virtual double getSampleRate() const { return juce::AudioProcessor::getSampleRate(); }

    virtual juce::Array<juce::AudioProcessorParameter *> getParameters() { return juce::AudioProcessor::getParameters(); }

protected:
    // Add a constructor that forwards the BusesProperties to the AudioProcessor base class.
    BasicAudioProcessor(const BusesProperties &props)
        : juce::AudioProcessor(props)
    {
    }
};
