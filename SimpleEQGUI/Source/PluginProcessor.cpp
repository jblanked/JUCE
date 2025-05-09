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
    : BasicAudioProcessor(BusesProperties()
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

    updateFilters(); // update the filters with the new settings

    // prepare our fifos
    leftChannelFifo.prepare(samplesPerBlock);  // prepare the left channel fifo
    rightChannelFifo.prepare(samplesPerBlock); // prepare the right channel fifo

    spec.numChannels = getTotalNumOutputChannels(); // set the number of channels to the number of output channels
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

    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear extra output channels
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    updateFilters(); // Update filters with latest parameters

    if (buffer.getNumChannels() == 1)
    {
        // If mono, duplicate the channel into a temporary stereo buffer.
        const int numSamples = buffer.getNumSamples();
        juce::AudioBuffer<float> stereoBuffer;
        // Set up stereo buffer with 2 channels
        stereoBuffer.setSize(2, numSamples, false, true, true);
        // Copy the mono channel into channel 0 and duplicate to channel 1.
        stereoBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
        stereoBuffer.copyFrom(1, 0, buffer, 0, 0, numSamples);

        // Create an audio block from the stereo buffer.
        juce::dsp::AudioBlock<float> block(stereoBuffer);

        auto leftBlock = block.getSingleChannelBlock(0);
        auto rightBlock = block.getSingleChannelBlock(1);

        juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
        juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

        // Process each channel
        leftChain.process(leftContext);
        rightChain.process(rightContext);

        // Here, we average the two channels back into one channel.
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float leftSample = stereoBuffer.getSample(0, sample);
            float rightSample = stereoBuffer.getSample(1, sample);
            buffer.setSample(0, sample, (leftSample + rightSample) * 0.5f);
        }

        // Push the processed buffer into the FIFOs
        leftChannelFifo.update(buffer);
        rightChannelFifo.update(buffer);
    }
    else
    {
        // For stereo or more channels, process normally.
        juce::dsp::AudioBlock<float> block(buffer);
        auto leftBlock = block.getSingleChannelBlock(0);
        auto rightBlock = block.getSingleChannelBlock(1);

        juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
        juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

        leftChain.process(leftContext);
        rightChain.process(rightContext);

        // Push buffers into the FIFOs
        leftChannelFifo.update(buffer);
        rightChannelFifo.update(buffer);
    }
}

//==============================================================================
bool SimpleEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *SimpleEQAudioProcessor::createEditor()
{
    return new SimpleEQAudioProcessorEditor(*this);
    // return new juce::GenericAudioProcessorEditor(*this); // This creates a generic editor so we can visualize this in standalone mode.
}

//==============================================================================
void SimpleEQAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    // write apvts to the memory block
    juce::MemoryOutputStream stream(destData, true); // create a memory output stream to write to the memory block
    apvts.state.writeToStream(stream);               // write the state of the apvts to the stream
}

void SimpleEQAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // restore plugin state from the memory block

    // check if the data is valid
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes); // read the data from the memory block
    if (tree.isValid())                                           // check if the tree is valid
    {
        apvts.replaceState(tree); // replace the state of the apvts with the new state
        updateFilters();          // update the filters with the new settings
    }
}

void SimpleEQAudioProcessor::updatePeakFilter(const ChainSettings &chainSettings)
{
    auto peakCoefficients = makePeakFilter(chainSettings, getSampleRate());                    // create the peak filter coefficients
    leftChain.setBypassed<ChainPositions::Peak>(chainSettings.peakBypassed);                   // set the left peak filter to bypassed or not
    rightChain.setBypassed<ChainPositions::Peak>(chainSettings.peakBypassed);                  // set the right peak filter to bypassed or not
    updateCoefficients(leftChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);  // set the coefficients for the left chain
    updateCoefficients(rightChain.get<ChainPositions::Peak>().coefficients, peakCoefficients); // set the coefficients for the right chain
}

void SimpleEQAudioProcessor::updateLowCutFilters(const ChainSettings &chainSettings)
{
    // set the low cut frequency and slope
    auto lowCutCoefficients = makeLowCutFilter(chainSettings, getSampleRate()); // create the low cut filter coefficients
    auto &leftLowCut = leftChain.get<ChainPositions::LowCut>();
    auto &rightLowCut = rightChain.get<ChainPositions::LowCut>();

    leftChain.setBypassed<ChainPositions::LowCut>(chainSettings.lowCutBypassed);  // set the left low cut filter to bypassed or not
    rightChain.setBypassed<ChainPositions::LowCut>(chainSettings.lowCutBypassed); // set the right low cut filter to bypassed or not

    updateCutFilter(leftLowCut, lowCutCoefficients, chainSettings.lowCutSlope);  // update the left low cut filter with the new settings
    updateCutFilter(rightLowCut, lowCutCoefficients, chainSettings.lowCutSlope); // update the right low cut filter with the new settings
}

void SimpleEQAudioProcessor::updateHighCutFilters(const ChainSettings &chainSettings)
{
    // set the high cut frequency and slope
    auto highCutCoefficients = makeHighCutFilter(chainSettings, getSampleRate()); // create the high cut filter coefficients
    auto &leftHighCut = leftChain.get<ChainPositions::HighCut>();
    auto &rightHighCut = rightChain.get<ChainPositions::HighCut>();

    leftChain.setBypassed<ChainPositions::HighCut>(chainSettings.highCutBypassed);  // set the left high cut filter to bypassed or not
    rightChain.setBypassed<ChainPositions::HighCut>(chainSettings.highCutBypassed); // set the right high cut filter to bypassed or not

    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);  // update the left high cut filter with the new settings
    updateCutFilter(rightHighCut, highCutCoefficients, chainSettings.highCutSlope); // update the right high cut filter with the new settings
}

void SimpleEQAudioProcessor::updateFilters()
{
    // update the filters with the new settings
    auto chainSettings = getChainSettings(apvts); // get the chain settings from the apvts

    updateLowCutFilters(chainSettings);  // update the low cut filters with the new settings
    updatePeakFilter(chainSettings);     // update the peak filter with the new settings
    updateHighCutFilters(chainSettings); // update the high cut filters with the new settings
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
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("peakGain", 4), "Peak Gain", juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f), 0.0f));

    // add a peak quality parameter
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("peakQuality", 5), "Peak Quality", juce::NormalisableRange<float>(0.1f, 10.f, 0.1f, 1.f), 1.f));

    // define a list of filter types for the low and high cut filters
    juce::StringArray filterTypes{"12 dB/Oct", "24 dB/Oct", "36 dB/Oct", "48 dB/Oct"};

    // add the list of filter types to the layout
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("lowCutSlope", 6), "LowCut Slope", filterTypes, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("highCutSlope", 7), "HighCut Slope", filterTypes, 0));

    // add a bypass parameter
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("lowCutBypass", 8), "LowCut Bypass", false));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("peakBypass", 9), "Peak Bypass", false));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("highCutBypass", 10), "HighCut Bypass", false));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("analyzerEnabled", 11), "Analyzer Enabled", true)); // add an analyzer enabled parameter

    // return the layout
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleEQAudioProcessor();
}
