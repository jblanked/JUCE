/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JBCompressorAudioProcessor::JBCompressorAudioProcessor()
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
      ,
      treeState(*this, nullptr, "Parameters", createParameterLayout()),
      compressorModule()
{
    // add listeners to the treeState state
    treeState.addParameterListener("inputGain", this);
    treeState.addParameterListener("threshold", this);
    treeState.addParameterListener("ratio", this);
    treeState.addParameterListener("attack", this);
    treeState.addParameterListener("release", this);
    treeState.addParameterListener("outputGain", this);

    // Set any desired properties on the treeState state.
    treeState.state.setProperty(Service::PresetManager::presetNameProperty, "", nullptr);
    treeState.state.setProperty("version", ProjectInfo::versionString, nullptr);
    presetManager = std::make_unique<Service::PresetManager>(treeState, defaultPresets, DEFAULT_PRESET_COUNT);
}

JBCompressorAudioProcessor::~JBCompressorAudioProcessor()
{
    // remove listeners from the treeState state
    treeState.removeParameterListener("inputGain", this);
    treeState.removeParameterListener("threshold", this);
    treeState.removeParameterListener("ratio", this);
    treeState.removeParameterListener("attack", this);
    treeState.removeParameterListener("release", this);
    treeState.removeParameterListener("outputGain", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout JBCompressorAudioProcessor::createParameterLayout()
{
    using namespace Service; // for the parameter manager

    Service::ParameterManager manager; // create a parameter manager to help with the parameters

    manager.addParameter("inputGain", 0.f, ParameterTypeFloatCompressorGain);    // Input Gain
    manager.addParameter("threshold", 0.f, ParameterTypeFloatCompressorGain);    // Threshold
    manager.addParameter("ratio", 1.f, ParameterTypeFloatCompressorRatio);       // Ratio
    manager.addParameter("attack", 50.f, ParameterTypeFloatCompressorAttack);    // Attack
    manager.addParameter("release", 160.f, ParameterTypeFloatCompressorRelease); // Release
    manager.addParameter("outputGain", 0.f, ParameterTypeFloatCompressorGain);   // Output Gain

    return manager.getLayout(); // return the layout from the parameter manager
}

void JBCompressorAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    updateParameters();
}

void JBCompressorAudioProcessor::updateParameters()
{
    compressorModule.updateParameters(treeState.getRawParameterValue("inputGain")->load(),
                                      treeState.getRawParameterValue("threshold")->load(),
                                      treeState.getRawParameterValue("ratio")->load(),
                                      treeState.getRawParameterValue("attack")->load() / 1000.f,
                                      treeState.getRawParameterValue("release")->load() / 1000.f,
                                      treeState.getRawParameterValue("outputGain")->load());
}

//==============================================================================
const juce::String JBCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JBCompressorAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool JBCompressorAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool JBCompressorAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double JBCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JBCompressorAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int JBCompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JBCompressorAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String JBCompressorAudioProcessor::getProgramName(int index)
{
    return {};
}

void JBCompressorAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void JBCompressorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // initial spec for dsp modules
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    compressorModule.prepareToPlay(spec); // prepare compressor module

    updateParameters(); // initialise parameters
}

void JBCompressorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JBCompressorAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
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

void JBCompressorAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    // juce::ScopedNoDenormals noDenormals;
    // auto totalNumInputChannels = getTotalNumInputChannels();
    // auto totalNumOutputChannels = getTotalNumOutputChannels();

    compressorModule.processBlock(buffer);
}

//==============================================================================
bool JBCompressorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *JBCompressorAudioProcessor::createEditor()
{
    return new JBCompressorAudioProcessorEditor(*this);
    // return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void JBCompressorAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream stream(destData, false);
    treeState.state.writeToStream(stream);
}

void JBCompressorAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));
    if (tree.isValid())
    {
        treeState.state = tree;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new JBCompressorAudioProcessor();
}
