/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JBKeysAudioProcessor::JBKeysAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         ),
      player(*this)
#endif
{
    player.addPreset("Violin", 0.0, 0.1, 10.0, BinaryData::Violin_C3_wav, BinaryData::Violin_C3_wavSize);
    player.addPreset("Viola", 0.0, 0.1, 10.0, BinaryData::Viola_C3_wav, BinaryData::Viola_C3_wavSize);
    player.addPreset("Soft Keys", 0.0, 0.1, 10.0, BinaryData::Soft_Keys_C3_wav, BinaryData::Soft_Keys_C3_wavSize);
    player.addPreset("Piano", 0.0, 0.1, 10.0, BinaryData::Piano_C3_wav, BinaryData::Piano_C3_wavSize);
    player.addPreset("Bass Rock", 0.0, 0.1, 10.0, BinaryData::Bass_Rock_C2_wav, BinaryData::Bass_Rock_C2_wavSize);
    player.addPreset("Electric Guitar", 0.0, 0.1, 10.0, BinaryData::Electric_Guitar_C3_wav, BinaryData::Electric_Guitar_C3_wavSize);
    player.addPreset("Bass", 0.0, 0.1, 10.0, BinaryData::Bass_C2_wav, BinaryData::Bass_C2_wavSize);

    player.finalize();
}

JBKeysAudioProcessor::~JBKeysAudioProcessor()
{
}

//==============================================================================
const juce::String JBKeysAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JBKeysAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool JBKeysAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool JBKeysAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double JBKeysAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JBKeysAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int JBKeysAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JBKeysAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String JBKeysAudioProcessor::getProgramName(int index)
{
    return {};
}

void JBKeysAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void JBKeysAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    // For the MidiPlayer, we need to prepare it for playback
    player.prepareToPlay(sampleRate);
}

void JBKeysAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JBKeysAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void JBKeysAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    player.processBlock(buffer, midiMessages);
}

//==============================================================================
bool JBKeysAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *JBKeysAudioProcessor::createEditor()
{
    return new JBKeysAudioProcessorEditor(*this);
}

//==============================================================================
void JBKeysAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    player.getStateInformation(destData);
}

void JBKeysAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    player.setStateInformation(data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new JBKeysAudioProcessor();
}
