#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

LoopGeneratorAudioProcessor::LoopGeneratorAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "Parameters", createParameters())
{
    audioLoop.setSize(2, 44100 * 4); // 4 seconds of audio buffer
    audioLoop.clear();
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

juce::MidiMessageSequence LoopGeneratorAudioProcessor::generateMidiLoop(int numBars, int beatsPerBar, float bpm)
{
    midiLoop.clear();
    float freq = *parameters.getRawParameterValue("frequency");
    // MIDI note = 69 ⟶ A4 = 440 Hz
    int midiNote = static_cast<int>(std::round(69.0 + 12.0 * std::log2(freq / 440.0)));
    const double ticksPerBeat = 960.0; // one quarter-note
    for (int bar = 0; bar < numBars; ++bar)
    {
        for (int beat = 0; beat < beatsPerBar; ++beat)
        {
            double time = (bar * beatsPerBar + beat) * ticksPerBeat;

            // Note on
            auto on = juce::MidiMessage::noteOn(1, midiNote, 1.0f);
            on.setTimeStamp(time);
            midiLoop.addEvent(on);

            // Note off one quarter-note later
            auto off = juce::MidiMessage::noteOff(1, midiNote);
            off.setTimeStamp(time + ticksPerBeat);
            midiLoop.addEvent(off);
        }
    }

    midiLoop.updateMatchedPairs();
    return midiLoop;
}

juce::AudioBuffer<float> LoopGeneratorAudioProcessor::generateAudioLoop(int lengthInSamples, double sampleRate)
{
    // Get parameters
    int waveformType = *parameters.getRawParameterValue("waveform");
    float frequency = *parameters.getRawParameterValue("frequency");
    float bpm = *parameters.getRawParameterValue("bpm");

    // Resize the buffer if needed
    if (audioLoop.getNumSamples() != lengthInSamples)
        audioLoop.setSize(2, lengthInSamples);

    audioLoop.clear();

    // Generate a simple audio loop based on the selected waveform
    for (int channel = 0; channel < audioLoop.getNumChannels(); ++channel)
    {
        float *channelData = audioLoop.getWritePointer(channel);

        for (int sample = 0; sample < lengthInSamples; ++sample)
        {
            double time = sample / sampleRate;
            double phase = std::fmod(time * frequency, 1.0);

            // Generate different waveforms
            float value = 0.0f;
            switch (waveformType)
            {
            case 0: // Sine
                value = std::sin(2.0 * juce::MathConstants<double>::pi * phase);
                break;

            case 1: // Square
                value = phase < 0.5 ? 1.0f : -1.0f;
                break;

            case 2: // Saw
                value = 2.0f * (float)phase - 1.0f;
                break;

            case 3: // Triangle
                value = phase < 0.5 ? 4.0f * (float)phase - 1.0f : 3.0f - 4.0f * (float)phase;
                break;
            }

            // Apply some amplitude modulation based on beat position for rhythmic effect
            double beatsPerSecond = bpm / 60.0;
            double beatPosition = std::fmod(time * beatsPerSecond, 1.0);
            float envelope = 0.7f + 0.3f * std::exp(-beatPosition * 4.0);

            channelData[sample] = value * 0.5f * envelope;
        }
    }

    return audioLoop;
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new LoopGeneratorAudioProcessor();
}
