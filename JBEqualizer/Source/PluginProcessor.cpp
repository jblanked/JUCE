/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JBEqualizerAudioProcessor::JBEqualizerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : BasicAudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                              .withInput("Input", AudioChannelSet::stereo(), true)
#endif
                              .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
      )
#endif
{
    // Set any desired properties on the apvts state.
    apvts.state.setProperty(Service::PresetManager::presetNameProperty, "", nullptr);
    apvts.state.setProperty("version", ProjectInfo::versionString, nullptr);

    // Use the apvts member for your preset manager.
    presetManager = std::make_unique<Service::PresetManager>(apvts, defaultPresets, DEFAULT_PRESET_COUNT);
}

JBEqualizerAudioProcessor::~JBEqualizerAudioProcessor()
{
}

//==============================================================================
const juce::String JBEqualizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JBEqualizerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool JBEqualizerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool JBEqualizerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double JBEqualizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JBEqualizerAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int JBEqualizerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JBEqualizerAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String JBEqualizerAudioProcessor::getProgramName(int index)
{
    return {};
}

void JBEqualizerAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void JBEqualizerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
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

void JBEqualizerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JBEqualizerAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
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

void JBEqualizerAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
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
bool JBEqualizerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *JBEqualizerAudioProcessor::createEditor()
{
    return new JBEqualizerAudioProcessorEditor(*this);
    // return new juce::GenericAudioProcessorEditor(*this); // This creates a generic editor so we can visualize this in standalone mode.
}

//==============================================================================
void JBEqualizerAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // Save the entire apvts state (parameters + preset info)
    juce::MemoryOutputStream stream(destData, true);
    apvts.state.writeToStream(stream);
}

void JBEqualizerAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // Read the state data from the memory block
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        // Replace the apvts state, which restores both parameters and preset settings.
        apvts.replaceState(tree);
        updateFilters(); // Update filters with the new settings.
    }
}

void JBEqualizerAudioProcessor::updatePeakFilters(const ChainSettings &chainSettings)
{
    auto peakCoefficients = makePeakFilter(chainSettings, getSampleRate(), 1);                  // create the peak filter coefficients
    leftChain.setBypassed<ChainPositions::Peak1>(chainSettings.peakBypassed);                   // set the left peak filter to bypassed or not
    rightChain.setBypassed<ChainPositions::Peak1>(chainSettings.peakBypassed);                  // set the right peak filter to bypassed or not
    updateCoefficients(leftChain.get<ChainPositions::Peak1>().coefficients, peakCoefficients);  // set the coefficients for the left chain
    updateCoefficients(rightChain.get<ChainPositions::Peak1>().coefficients, peakCoefficients); // set the coefficients for the right chain
    //
    peakCoefficients = makePeakFilter(chainSettings, getSampleRate(), 2);                       // create the peak 2 filter coefficients
    leftChain.setBypassed<ChainPositions::Peak2>(chainSettings.peakBypassed);                   // set the left peak filter to bypassed or not
    rightChain.setBypassed<ChainPositions::Peak2>(chainSettings.peakBypassed);                  // set the right peak filter to bypassed or not
    updateCoefficients(leftChain.get<ChainPositions::Peak2>().coefficients, peakCoefficients);  // set the coefficients for the left chain
    updateCoefficients(rightChain.get<ChainPositions::Peak2>().coefficients, peakCoefficients); // set the coefficients for the right chain
    //
    peakCoefficients = makePeakFilter(chainSettings, getSampleRate(), 3);                       // create the peak 3 filter coefficients
    leftChain.setBypassed<ChainPositions::Peak3>(chainSettings.peakBypassed);                   // set the left peak filter to bypassed or not
    rightChain.setBypassed<ChainPositions::Peak3>(chainSettings.peakBypassed);                  // set the right peak filter to bypassed or not
    updateCoefficients(leftChain.get<ChainPositions::Peak3>().coefficients, peakCoefficients);  // set the coefficients for the left chain
    updateCoefficients(rightChain.get<ChainPositions::Peak3>().coefficients, peakCoefficients); // set the coefficients for the right chain
    //
    peakCoefficients = makePeakFilter(chainSettings, getSampleRate(), 4);                       // create the peak 4 filter coefficients
    leftChain.setBypassed<ChainPositions::Peak4>(chainSettings.peakBypassed);                   // set the left peak filter to bypassed or not
    rightChain.setBypassed<ChainPositions::Peak4>(chainSettings.peakBypassed);                  // set the right peak filter to bypassed or not
    updateCoefficients(leftChain.get<ChainPositions::Peak4>().coefficients, peakCoefficients);  // set the coefficients for the left chain
    updateCoefficients(rightChain.get<ChainPositions::Peak4>().coefficients, peakCoefficients); // set the coefficients for the right chain
}

void JBEqualizerAudioProcessor::updateLowCutFilters(const ChainSettings &chainSettings)
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

void JBEqualizerAudioProcessor::updateHighCutFilters(const ChainSettings &chainSettings)
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

void JBEqualizerAudioProcessor::updateShelfFilters(const ChainSettings &chainSettings)
{
    auto lowShelfCoefficients = makeLowShelfFilter(chainSettings, getSampleRate());   // create the low shelf filter coefficients
    auto highShelfCoefficients = makeHighShelfFilter(chainSettings, getSampleRate()); // create the high shelf filter coefficients

    auto &leftLowShelf = leftChain.get<ChainPositions::LowShelf>();
    auto &rightLowShelf = rightChain.get<ChainPositions::LowShelf>();

    auto &leftHighShelf = leftChain.get<ChainPositions::HighShelf>();
    auto &rightHighShelf = rightChain.get<ChainPositions::HighShelf>();

    leftChain.setBypassed<ChainPositions::LowShelf>(chainSettings.lowShelfBypassed);  // set the left low shelf filter to bypassed or not
    rightChain.setBypassed<ChainPositions::LowShelf>(chainSettings.lowShelfBypassed); // set the right low shelf filter to bypassed or not

    leftChain.setBypassed<ChainPositions::HighShelf>(chainSettings.highShelfBypassed);  // set the left high shelf filter to bypassed or not
    rightChain.setBypassed<ChainPositions::HighShelf>(chainSettings.highShelfBypassed); // set the right high shelf filter to bypassed or not

    updateCoefficients(leftLowShelf.coefficients, lowShelfCoefficients);  // set the coefficients for the left chain
    updateCoefficients(rightLowShelf.coefficients, lowShelfCoefficients); // set the coefficients for the right chain

    updateCoefficients(leftHighShelf.coefficients, highShelfCoefficients);  // set the coefficients for the left chain
    updateCoefficients(rightHighShelf.coefficients, highShelfCoefficients); // set the coefficients for the right chain
}

void JBEqualizerAudioProcessor::updateFilters()
{
    // update the filters with the new settings
    auto chainSettings = getChainSettings(apvts); // get the chain settings from the apvts
    updateLowCutFilters(chainSettings);           // update the low cut filters with the new settings
    updatePeakFilters(chainSettings);             // update the peak filter with the new settings
    updateHighCutFilters(chainSettings);          // update the high cut filters with the new settings
    updateShelfFilters(chainSettings);            // update the shelf filters with the new settings
}

juce::AudioProcessorValueTreeState::ParameterLayout
JBEqualizerAudioProcessor::createParameterLayout()
{
    using namespace Service; // for the parameter manager

    Service::ParameterManager manager; // create a parameter manager to help with the parameters

    manager.addParameter("lowCutFrequency", 20.f, ParameterTypeFloatFrequency);       // Low Cut Frequency
    manager.addParameter("highCutFrequency", 20000.f, ParameterTypeFloatFrequency);   // High Cut Frequency
    manager.addParameter("peakFrequency", 100.f, ParameterTypeFloatFrequency);        // Peak Frequency
    manager.addParameter("peakGain", 0.f, ParameterTypeFloatGain);                    // Peak Gain
    manager.addParameter("peakQuality", 1.f, ParameterTypeFloatQuality);              // Peak Quality
    manager.addParameter("peak2Frequency", 250.f, ParameterTypeFloatFrequency);       // Peak 2 Frequency
    manager.addParameter("peak2Gain", 0.f, ParameterTypeFloatGain);                   // Peak 2 Gain
    manager.addParameter("peak2Quality", 1.f, ParameterTypeFloatQuality);             // Peak 2 Quality
    manager.addParameter("peak3Frequency", 1040.f, ParameterTypeFloatFrequency);      // Peak 3 Frequency
    manager.addParameter("peak3Gain", 0.f, ParameterTypeFloatGain);                   // Peak 3 Gain
    manager.addParameter("peak3Quality", 1.f, ParameterTypeFloatQuality);             // Peak 3 Quality
    manager.addParameter("peak4Frequency", 2500.f, ParameterTypeFloatFrequency);      // Peak 4 Frequency
    manager.addParameter("peak4Gain", 0.f, ParameterTypeFloatGain);                   // Peak 4 Gain
    manager.addParameter("peak4Quality", 1.f, ParameterTypeFloatQuality);             // Peak 4 Quality
    manager.addParameter("lowShelfFrequency", 20.f, ParameterTypeFloatFrequency);     // Low Shelf Frequency
    manager.addParameter("lowShelfGain", 0.f, ParameterTypeFloatGain);                // Low Shelf Gain
    manager.addParameter("lowShelfQ", 1.f, ParameterTypeFloatQuality);                // Low Shelf Q
    manager.addParameter("highShelfFrequency", 20000.f, ParameterTypeFloatFrequency); // High Shelf Frequency
    manager.addParameter("highShelfGain", 0.f, ParameterTypeFloatGain);               // High Shelf Gain
    manager.addParameter("highShelfQ", 1.f, ParameterTypeFloatQuality);               // High Shelf Q
    manager.addParameter("lowCutSlope", 0, ParameterTypeFloatSlope);                  // Low Cut Slope
    manager.addParameter("highCutSlope", 0, ParameterTypeFloatSlope);                 // High Cut Slope
    manager.addParameter("lowCutBypass", false, ParameterTypeBool);                   // Low Cut Bypass
    manager.addParameter("peakBypass", false, ParameterTypeBool);                     // Peak Bypass
    manager.addParameter("peak2Bypass", false, ParameterTypeBool);                    // Peak 2 Bypass
    manager.addParameter("peak3Bypass", false, ParameterTypeBool);                    // Peak 3 Bypass
    manager.addParameter("peak4Bypass", false, ParameterTypeBool);                    // Peak 4 Bypass
    manager.addParameter("lowShelfBypass", false, ParameterTypeBool);                 // Low Shelf Bypass
    manager.addParameter("highShelfBypass", false, ParameterTypeBool);                // High Shelf Bypass
    manager.addParameter("highCutBypass", false, ParameterTypeBool);                  // High Cut Bypass
    manager.addParameter("analyzerEnabled", true, ParameterTypeBool);                 // Analyzer Enabled

    return manager.getLayout(); // return the layout from the parameter manager
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new JBEqualizerAudioProcessor();
}
