/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JBDrumsAudioProcessor::JBDrumsAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
#endif
{
    // Rock
    midiPlayer.addPreset("Rock Hi-Hat", 0.0, 0.1, 10.0, BinaryData::Rock_HiHat_1_wav, BinaryData::Rock_HiHat_1_wavSize);
    midiPlayer.addPreset("Rock Hi-Hat 2", 0.0, 0.1, 10.0, BinaryData::Rock_HiHat_3_wav, BinaryData::Rock_HiHat_3_wavSize);
    midiPlayer.addPreset("Rock Kick", 0.0, 0.1, 10.0, BinaryData::Rock_Kick_1_wav, BinaryData::Rock_Kick_1_wavSize);
    midiPlayer.addPreset("Rock Snare", 0.0, 0.1, 10.0, BinaryData::Rock_Snare_1_wav, BinaryData::Rock_Snare_1_wavSize);
    midiPlayer.addPreset("Rock Snare 2", 0.0, 0.1, 10.0, BinaryData::Rock_Snare_3_wav, BinaryData::Rock_Snare_3_wavSize);
    // Boom Bap
    midiPlayer.addPreset("Boom Bap Hi-Hat", 0.0, 0.1, 10.0, BinaryData::Boom_Bap_HiHat_wav, BinaryData::Boom_Bap_HiHat_wavSize);
    midiPlayer.addPreset("Boom Bap Open Hi-Hat", 0.0, 0.1, 10.0, BinaryData::Boom_Bap_Open_HiHat_wav, BinaryData::Boom_Bap_Open_HiHat_wavSize);
    midiPlayer.addPreset("Boom Bap Kick", 0.0, 0.1, 10.0, BinaryData::Boom_Bap_Kick_wav, BinaryData::Boom_Bap_Kick_wavSize);
    midiPlayer.addPreset("Boom Bap Snare", 0.0, 0.1, 10.0, BinaryData::Boom_Bap_Snare_wav, BinaryData::Boom_Bap_Snare_wavSize);
    // Trap
    midiPlayer.addPreset("Trap Hi-Hat", 0.0, 0.1, 10.0, BinaryData::Trap_HiHat_wav, BinaryData::Trap_HiHat_wavSize);
    midiPlayer.addPreset("Trap Hi-Hat 2", 0.0, 0.1, 10.0, BinaryData::Trap_HiHat_2_wav, BinaryData::Trap_HiHat_2_wavSize);
    midiPlayer.addPreset("Trap Kick", 0.0, 0.1, 10.0, BinaryData::Trap_Kick_wav, BinaryData::Trap_Kick_wavSize);
    midiPlayer.addPreset("Trap Snare", 0.0, 0.1, 10.0, BinaryData::Trap_Snare_wav, BinaryData::Trap_Snare_wavSize);
    midiPlayer.addPreset("Trap 808", 0.0, 0.1, 10.0, BinaryData::Trap_808_C3_wav, BinaryData::Trap_808_C3_wavSize);
}

JBDrumsAudioProcessor::~JBDrumsAudioProcessor()
{
}

//==============================================================================
const juce::String JBDrumsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JBDrumsAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool JBDrumsAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool JBDrumsAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double JBDrumsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JBDrumsAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int JBDrumsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JBDrumsAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String JBDrumsAudioProcessor::getProgramName(int index)
{
    return {};
}

void JBDrumsAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void JBDrumsAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    // For the MidiPlayer, we need to prepare it for playback
    midiPlayer.prepareToPlay(sampleRate);
}

void JBDrumsAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JBDrumsAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
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

void JBDrumsAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    midiPlayer.processBlock(buffer, midiMessages);
}

//==============================================================================
bool JBDrumsAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *JBDrumsAudioProcessor::createEditor()
{
    return new JBDrumsAudioProcessorEditor(*this);
}

//==============================================================================
void JBDrumsAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void JBDrumsAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new JBDrumsAudioProcessor();
}
