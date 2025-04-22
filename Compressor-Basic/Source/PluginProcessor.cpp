/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CompressorBasicAudioProcessor::CompressorBasicAudioProcessor()
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
      treeState(*this, nullptr, "Parameters", createParameterLayout())
{
    treeState.addParameterListener("inputGain", this);
    treeState.addParameterListener("threshold", this);
    treeState.addParameterListener("ratio", this);
    treeState.addParameterListener("attack", this);
    treeState.addParameterListener("release", this);
    treeState.addParameterListener("outputGain", this);
}

CompressorBasicAudioProcessor::~CompressorBasicAudioProcessor()
{
    treeState.removeParameterListener("inputGain", this);
    treeState.removeParameterListener("threshold", this);
    treeState.removeParameterListener("ratio", this);
    treeState.removeParameterListener("attack", this);
    treeState.removeParameterListener("release", this);
    treeState.removeParameterListener("outputGain", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout CompressorBasicAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    juce::NormalisableRange<float> attackRange = juce::NormalisableRange<float>(0.f, 200.f, 1.f);
    attackRange.setSkewForCentre(50.f);
    juce::NormalisableRange<float> releaseRange = juce::NormalisableRange<float>(5.f, 5000.f, 1.f);
    releaseRange.setSkewForCentre(160.f);
    auto pInputGain = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("inputGain", 1), "Input Gain", -60.0f, 10.0f, 0.f);
    auto pThresh = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("threshold", 2), "Threshold", -60.0f, 10.0f, 0.f);
    auto pRatio = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("ratio", 3), "Ratio", 1.0f, 20.0f, 1.f);
    auto pAttack = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("attack", 4), "Attack", attackRange, 50.f);
    auto pRelease = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("release", 5), "Release", releaseRange, 160.f);
    auto pOutputGain = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("outputGain", 6), "Output Gain", -60.0f, 10.0f, 0.f);
    params.push_back(std::move(pInputGain));
    params.push_back(std::move(pThresh));
    params.push_back(std::move(pRatio));
    params.push_back(std::move(pAttack));
    params.push_back(std::move(pRelease));
    params.push_back(std::move(pOutputGain));
    return {params.begin(), params.end()};
}

void CompressorBasicAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    updateParameters();
}

void CompressorBasicAudioProcessor::updateParameters()
{
    inputModule.setGainDecibels(treeState.getRawParameterValue("inputGain")->load());
    compressorModule.setThreshold(treeState.getRawParameterValue("threshold")->load());
    compressorModule.setRatio(treeState.getRawParameterValue("ratio")->load());
    compressorModule.setAttack(treeState.getRawParameterValue("attack")->load() / 1000.f);
    compressorModule.setRelease(treeState.getRawParameterValue("release")->load() / 1000.f);
    outputModule.setGainDecibels(treeState.getRawParameterValue("outputGain")->load());
}

//==============================================================================
const juce::String CompressorBasicAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CompressorBasicAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool CompressorBasicAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool CompressorBasicAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double CompressorBasicAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CompressorBasicAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int CompressorBasicAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CompressorBasicAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String CompressorBasicAudioProcessor::getProgramName(int index)
{
    return {};
}

void CompressorBasicAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void CompressorBasicAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // initial spec for dsp modules
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    inputModule.prepare(spec);
    inputModule.setRampDurationSeconds(0.02); // 20ms ramp time
    outputModule.prepare(spec);
    outputModule.setRampDurationSeconds(0.02); // 20ms ramp time
    compressorModule.prepare(spec);

    updateParameters(); // initialise parameters
}

void CompressorBasicAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CompressorBasicAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
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

void CompressorBasicAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    juce::dsp::AudioBlock<float> block{buffer};
    inputModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    compressorModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    outputModule.process(juce::dsp::ProcessContextReplacing<float>(block));
}

//==============================================================================
bool CompressorBasicAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *CompressorBasicAudioProcessor::createEditor()
{
    // return new CompressorBasicAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void CompressorBasicAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block,
    // so that when the host calls getStateInformation, your parameters can be loaded
    // back into your plugin when it is created again.
    juce::MemoryOutputStream stream(destData, false);
    treeState.state.writeToStream(stream);
}

void CompressorBasicAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
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
    return new CompressorBasicAudioProcessor();
}
