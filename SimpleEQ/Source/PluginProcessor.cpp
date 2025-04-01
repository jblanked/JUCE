/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQAudioProcessor::SimpleEQAudioProcessor()
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
}

SimpleEQAudioProcessor::~SimpleEQAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleEQAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool SimpleEQAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool SimpleEQAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double SimpleEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleEQAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleEQAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String SimpleEQAudioProcessor::getProgramName(int index)
{
    return {};
}

void SimpleEQAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void SimpleEQAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    // prepare filters before we use them by passing a ProcessSpec object to the chain,
    // which will then pass it to each link in the chain
    juce::dsp::ProcessSpec spec;

    // maximum samples we will press at one time
    spec.maximumBlockSize = samplesPerBlock;

    // numbers of channels
    spec.numChannels = 1; // mono

    // sample rate
    spec.sampleRate = sampleRate;

    // prepare the chain with the spec
    leftChain.prepare(spec);
    rightChain.prepare(spec);

    // produce coefficients using the static helper functions that are part of the coefficients class
    auto chainSettings = getChainSettings(apvts); // get the chain settings from the apvts

    updatePeakFilter(chainSettings); // update the peak filter with the new settings

    // set the low and high cut frequencies and slopes
    auto cutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq,
                                                                                                       sampleRate,
                                                                                                       2 * (1 + chainSettings.lowCutSlope)); // create the low cut filter coefficients

    auto &leftLowCut = leftChain.get<ChainPositions::LowCut>();

    // bypass all of the links in the chain
    leftLowCut.setBypassed<0>(true); // set the first link to bypassed
    leftLowCut.setBypassed<1>(true); // set the second link to bypassed
    leftLowCut.setBypassed<2>(true); // set the third link to bypassed
    leftLowCut.setBypassed<3>(true); // set the fourth link to bypassed

    switch (chainSettings.lowCutSlope) // set the bypassed links according to the slope
    {
    case Slope_12:
        *leftLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        leftLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        break;
    case Slope_24:
        *leftLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        leftLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        *leftLowCut.get<1>().coefficients = *cutCoefficients[1]; // set the coefficients for the second link
        leftLowCut.setBypassed<1>(false);                        // set the second link to not bypassed
        break;
    case Slope_36:
        *leftLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        leftLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        *leftLowCut.get<1>().coefficients = *cutCoefficients[1]; // set the coefficients for the second link
        leftLowCut.setBypassed<1>(false);                        // set the second link to not bypassed
        *leftLowCut.get<2>().coefficients = *cutCoefficients[2]; // set the coefficients for the third link
        leftLowCut.setBypassed<2>(false);                        // set the third link to not bypassed
        break;
    case Slope_48:
        *leftLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        leftLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        *leftLowCut.get<1>().coefficients = *cutCoefficients[1]; // set the coefficients for the second link
        leftLowCut.setBypassed<1>(false);                        // set the second link to not bypassed
        *leftLowCut.get<2>().coefficients = *cutCoefficients[2]; // set the coefficients for the third link
        leftLowCut.setBypassed<2>(false);                        // set the third link to not bypassed
        *leftLowCut.get<3>().coefficients = *cutCoefficients[3]; // set the coefficients for the fourth link
        leftLowCut.setBypassed<3>(false);                        // set the fourth link to not bypassed
        break;
    }

    auto &rightLowCut = rightChain.get<ChainPositions::LowCut>();

    // bypass all of the links in the chain
    rightLowCut.setBypassed<0>(true); // set the first link to bypassed
    rightLowCut.setBypassed<1>(true); // set the second link to bypassed
    rightLowCut.setBypassed<2>(true); // set the third link to bypassed
    rightLowCut.setBypassed<3>(true); // set the fourth link to bypassed

    switch (chainSettings.lowCutSlope) // set the bypassed links according to the slope
    {
    case Slope_12:
        *rightLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        rightLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        break;
    case Slope_24:
        *rightLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        rightLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        *rightLowCut.get<1>().coefficients = *cutCoefficients[1]; // set the coefficients for the second link
        rightLowCut.setBypassed<1>(false);                        // set the second link to not bypassed
        break;
    case Slope_36:
        *rightLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        rightLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        *rightLowCut.get<1>().coefficients = *cutCoefficients[1]; // set the coefficients for the second link
        rightLowCut.setBypassed<1>(false);                        // set the second link to not bypassed
        *rightLowCut.get<2>().coefficients = *cutCoefficients[2]; // set the coefficients for the third link
        rightLowCut.setBypassed<2>(false);                        // set the third link to not bypassed
        break;
    case Slope_48:
        *rightLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        rightLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        *rightLowCut.get<1>().coefficients = *cutCoefficients[1]; // set the coefficients for the second link
        rightLowCut.setBypassed<1>(false);                        // set the second link to not bypassed
        *rightLowCut.get<2>().coefficients = *cutCoefficients[2]; // set the coefficients for the third link
        rightLowCut.setBypassed<2>(false);                        // set the third link to not bypassed
        *rightLowCut.get<3>().coefficients = *cutCoefficients[3]; // set the coefficients for the fourth link
        rightLowCut.setBypassed<3>(false);                        // set the fourth link to not bypassed
        break;
    }
}

void SimpleEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleEQAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
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

void SimpleEQAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // update parameters before processing audio to them
    // produce coefficients using the static helper functions that are part of the coefficients class
    auto chainSettings = getChainSettings(apvts); // get the chain settings from the apvts

    updatePeakFilter(chainSettings); // update the peak filter with the new settings

    // set the low and high cut frequencies and slopes
    auto cutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq,
                                                                                                       getSampleRate(),
                                                                                                       2 * (1 + chainSettings.lowCutSlope)); // create the low cut filter coefficients

    auto &leftLowCut = leftChain.get<ChainPositions::LowCut>();

    // bypass all of the links in the chain
    leftLowCut.setBypassed<0>(true); // set the first link to bypassed
    leftLowCut.setBypassed<1>(true); // set the second link to bypassed
    leftLowCut.setBypassed<2>(true); // set the third link to bypassed
    leftLowCut.setBypassed<3>(true); // set the fourth link to bypassed

    switch (chainSettings.lowCutSlope) // set the bypassed links according to the slope
    {
    case Slope_12:
        *leftLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        leftLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        break;
    case Slope_24:
        *leftLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        leftLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        *leftLowCut.get<1>().coefficients = *cutCoefficients[1]; // set the coefficients for the second link
        leftLowCut.setBypassed<1>(false);                        // set the second link to not bypassed
        break;
    case Slope_36:
        *leftLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        leftLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        *leftLowCut.get<1>().coefficients = *cutCoefficients[1]; // set the coefficients for the second link
        leftLowCut.setBypassed<1>(false);                        // set the second link to not bypassed
        *leftLowCut.get<2>().coefficients = *cutCoefficients[2]; // set the coefficients for the third link
        leftLowCut.setBypassed<2>(false);                        // set the third link to not bypassed
        break;
    case Slope_48:
        *leftLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        leftLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        *leftLowCut.get<1>().coefficients = *cutCoefficients[1]; // set the coefficients for the second link
        leftLowCut.setBypassed<1>(false);                        // set the second link to not bypassed
        *leftLowCut.get<2>().coefficients = *cutCoefficients[2]; // set the coefficients for the third link
        leftLowCut.setBypassed<2>(false);                        // set the third link to not bypassed
        *leftLowCut.get<3>().coefficients = *cutCoefficients[3]; // set the coefficients for the fourth link
        leftLowCut.setBypassed<3>(false);                        // set the fourth link to not bypassed
        break;
    }

    auto &rightLowCut = rightChain.get<ChainPositions::LowCut>();

    // bypass all of the links in the chain
    rightLowCut.setBypassed<0>(true); // set the first link to bypassed
    rightLowCut.setBypassed<1>(true); // set the second link to bypassed
    rightLowCut.setBypassed<2>(true); // set the third link to bypassed
    rightLowCut.setBypassed<3>(true); // set the fourth link to bypassed

    switch (chainSettings.lowCutSlope) // set the bypassed links according to the slope
    {
    case Slope_12:
        *rightLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        rightLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        break;
    case Slope_24:
        *rightLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        rightLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        *rightLowCut.get<1>().coefficients = *cutCoefficients[1]; // set the coefficients for the second link
        rightLowCut.setBypassed<1>(false);                        // set the second link to not bypassed
        break;
    case Slope_36:
        *rightLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        rightLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        *rightLowCut.get<1>().coefficients = *cutCoefficients[1]; // set the coefficients for the second link
        rightLowCut.setBypassed<1>(false);                        // set the second link to not bypassed
        *rightLowCut.get<2>().coefficients = *cutCoefficients[2]; // set the coefficients for the third link
        rightLowCut.setBypassed<2>(false);                        // set the third link to not bypassed
        break;
    case Slope_48:
        *rightLowCut.get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
        rightLowCut.setBypassed<0>(false);                        // set the first link to not bypassed
        *rightLowCut.get<1>().coefficients = *cutCoefficients[1]; // set the coefficients for the second link
        rightLowCut.setBypassed<1>(false);                        // set the second link to not bypassed
        *rightLowCut.get<2>().coefficients = *cutCoefficients[2]; // set the coefficients for the third link
        rightLowCut.setBypassed<2>(false);                        // set the third link to not bypassed
        *rightLowCut.get<3>().coefficients = *cutCoefficients[3]; // set the coefficients for the fourth link
        rightLowCut.setBypassed<3>(false);                        // set the fourth link to not bypassed
        break;
    }

    // process chain requries a processing context in order to run the audio in the links of the chain
    // in order to make a processing context, we need to create an AudioBlock object
    // the processBlock function is called by the host

    // create an audio block object to hold the buffer
    juce::dsp::AudioBlock<float> block(buffer);

    // extract the left and right channels from the block
    auto leftBlock = block.getSingleChannelBlock(0);  // left channel
    auto rightBlock = block.getSingleChannelBlock(1); // right channel

    // create process context objects for the left and right channels
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);   // process context for left channel
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock); // process context for right channel

    // pass context to the chain to process the audio in the links of the chain
    leftChain.process(leftContext);   // process the left channel
    rightChain.process(rightContext); // process the right channel
}

//==============================================================================
bool SimpleEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *SimpleEQAudioProcessor::createEditor()
{
    // return new SimpleEQAudioProcessorEditor(*this);
    return new juce::GenericAudioProcessorEditor(*this);
    // This creates a generic editor so we can visualize this in standalone mode.
}

//==============================================================================
void SimpleEQAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SimpleEQAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState &apvts)
{
    ChainSettings settings; // initialize the settings struct

    settings.lowCutFreq = apvts.getRawParameterValue("lowCutFrequency")->load();                    // get the low cut frequency from the apvts
    settings.highCutFreq = apvts.getRawParameterValue("highCutFrequency")->load();                  // get the high cut frequency from the apvts
    settings.peakFreq = apvts.getRawParameterValue("peakFrequency")->load();                        // get the peak frequency from the apvts
    settings.peakGainInDecibels = apvts.getRawParameterValue("peakGain")->load();                   // get the peak gain from the apvts
    settings.peakQuality = apvts.getRawParameterValue("peakQuality")->load();                       // get the peak quality from the apvts
    settings.lowCutSlope = static_cast<Slope>(apvts.getRawParameterValue("lowCutSlope")->load());   // get the low cut slope from the apvts
    settings.highCutSlope = static_cast<Slope>(apvts.getRawParameterValue("highCutSlope")->load()); // get the high cut slope from the apvts

    return settings; // return the settings struct
}

void SimpleEQAudioProcessor::updatePeakFilter(const ChainSettings &chainSettings)
{
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
                                                                                chainSettings.peakFreq,
                                                                                juce::Decibels::decibelsToGain(chainSettings.peakGainInDecibels),
                                                                                chainSettings.peakQuality); // create the peak filter coefficients

    // now set the coefficients accordingly
    updateCoefficients(leftChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);  // set the coefficients for the left chain
    updateCoefficients(rightChain.get<ChainPositions::Peak>().coefficients, peakCoefficients); // set the coefficients for the right chain
}

void SimpleEQAudioProcessor::updateCoefficients(Coefficients &old, const Coefficients &replcements)
{
    // set the old coefficients to the new coefficients
    // we need to dereference them to get the actual values
    *old = *replcements;
}

juce::AudioProcessorValueTreeState::ParameterLayout
SimpleEQAudioProcessor::createParameterLayout()
{
    // define the layout of the parameters we will use in the EQ
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // add a low cut frequency parameter
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("lowCutFrequency", 1), "LowCut Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), 20.f));

    // add a high cut frequency parameter
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("highCutFrequency", 2), "HighCut Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), 20000.f));

    // add a peak frequency parameter
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("peakFrequency", 3), "Peak Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.5f), 750.f));

    // add a peak gain parameter
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("peakGain", 4), "Peak Gain", juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f), 0.f));

    // add a peak quality parameter
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("peakQuality", 5), "Peak Quality", juce::NormalisableRange<float>(0.1f, 10.f, 0.1f, 1.f), 1.f));

    // define a list of filter types for the low and high cut filters
    juce::StringArray filterTypes{"12 dB/Oct", "24 dB/Oct", "36 dB/Oct", "48 dB/Oct"};

    // add the list of filter types to the layout
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("lowCutSlope", 6), "LowCut Slope", filterTypes, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("highCutSlope", 7), "HighCut Slope", filterTypes, 0));

    // return the layout
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleEQAudioProcessor();
}
