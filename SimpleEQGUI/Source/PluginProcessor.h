/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <array>

template <typename T>
struct Fifo
{

  void prepare(int numChannels, int numSamples)
  {
    static_assert(std::is_same_v<T, juce::AudioBuffer<float>>,
                  "Fifo only works with AudioBuffer<float>");
    for (auto &buffer : buffers)
    {
      buffer.setSize(
          numChannels,
          numSamples,
          false,      // clear everything?
          true,       // including the extra space?
          true);      // avoid reallocation
      buffer.clear(); // clear the buffer
    }
  }

  void prepare(size_t numElements)
  {
    static_assert(std::is_same_v<T, std::vector<float>>,
                  "Fifo only works with std::vector<float>");
    for (auto &buffer : buffers)
    {
      buffer.clear();                // clear the buffer
      buffer.resize(numElements, 0); // resize the buffer and fill with 0s
    }
  }

  bool push(const T &t)
  {
    auto write = fifo.write(1);
    if (write.blockSize1 > 0)
    {
      buffers[write.startIndex1] = t;
      return true;
    }
    return false;
  }

  bool pull(T &t)
  {
    auto read = fifo.read(1);
    if (read.blockSize1 > 0)
    {
      t = buffers[read.startIndex1];
      return true;
    }
    return false;
  }

  int getNumAvailableForReading() const
  {
    return fifo.getNumReady();
  }

private:
  static constexpr int Capacity = 30; // number of buffers
  std::array<T, Capacity> buffers;    // array of buffers
  juce::AbstractFifo fifo{Capacity};  // fifo to manage the buffers
};

// enum for channels
enum Channel
{
  Right, // 0
  Left   // 1
};

template <typename BlockType>
struct SingleChannelSampleFifo
{
  SingleChannelSampleFifo(Channel ch) : channelToUse(ch)
  {
    prepared.set(false);
  }

  void update(const BlockType &buffer)
  {
    jassert(prepared.get());
    jassert(buffer.getNumChannels() > channelToUse);        // make sure the buffer is valid
    auto *channelPtr = buffer.getReadPointer(channelToUse); // get the channel data

    for (int i = 0; i < buffer.getNumSamples(); ++i) // loop through the samples
    {
      pushNextSampleIntoFifo(channelPtr[i]); // push the sample into the fifo
    }
  }

  void prepare(int bufferSize)
  {
    prepared.set(false);  // set the prepared flag to false
    size.set(bufferSize); // set the size of the buffer

    // create the buffer
    bufferToFill.setSize(
        1,          // channel
        bufferSize, // num samples
        false,      // keep exising data
        true,       // clear extra space
        true);      // avoid reallocation

    audioBufferFifo.prepare(1, bufferSize); // prepare the fifo
    fifoIndex = 0;                          // set the fifo index to 0
    prepared.set(true);                     // set the prepared flag to true
  }

  int getNumCompleteBuffersAvailable() const { return audioBufferFifo.getNumAvailableForReading(); } // get the number of complete buffers available
  bool isPrepared() const { return prepared.get(); }                                                 // check if the fifo is prepared
  int getSize() const { return size.get(); }                                                         // get the size of the buffer
  bool getAudioBuffer(BlockType &buf) { return audioBufferFifo.pull(buf); }                          // get the audio buffer from the fifo
private:
  Channel channelToUse;                // the channel to use
  int fifoIndex = 0;                   // the index of the fifo
  Fifo<BlockType> audioBufferFifo;     // the fifo to use
  BlockType bufferToFill;              // the buffer to fill
  juce::Atomic<bool> prepared = false; // flag to check if the fifo is prepared
  juce::Atomic<int> size = 0;          // size of the buffer

  void pushNextSampleIntoFifo(float sample) // push the sample into the fifo
  {
    if (fifoIndex == bufferToFill.getNumSamples()) // check if the fifo is full
    {
      auto ok = audioBufferFifo.push(bufferToFill); // push the buffer into the fifo
      juce::ignoreUnused(ok);
      fifoIndex = 0; // reset the fifo index
    }
    bufferToFill.setSample(0, fifoIndex, sample); // set the sample in the buffer
    ++fifoIndex;                                  // increment the fifo index
  }
};

// enum to switch between different slope settings
enum Slope
{
  Slope_12,
  Slope_24,
  Slope_36,
  Slope_48
};

// struct for parameter values
struct ChainSettings
{
  float peakFreq{0}, peakGainInDecibels{0}, peakQuality{1};
  float lowCutFreq{0}, highCutFreq{0};
  Slope lowCutSlope{Slope_12}, highCutSlope{Slope_12};
};

// helper funciton that will give us all of the values in the struct
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState &apvts);

// create filter types
using Filter = juce::dsp::IIR::Filter<float>;
using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

// enum that represents each link's position in the chain
enum ChainPositions
{
  LowCut,
  Peak,
  HighCut
};

using Coefficients = Filter::CoefficientsPtr;
void updateCoefficients(Coefficients &old, const Coefficients &replcements);
Coefficients makePeakFilter(const ChainSettings &chainSettings, double sampleRate);
template <int Index, typename ChainType, typename CoefficientsType>
void update(ChainType &chain, const CoefficientsType &coefficients)
{
  updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]); // set the coefficients for the chain
  chain.template setBypassed<Index>(false);                                          // set the chain to not bypassed
}
template <typename ChainType, typename CoefficientsType>
void updateCutFilter(ChainType &chain, const CoefficientsType &coefficients, const Slope &slope)
{
  // bypass all of the links in the chain
  chain.template setBypassed<0>(true); // set the first link to bypassed
  chain.template setBypassed<1>(true); // set the second link to bypassed
  chain.template setBypassed<2>(true); // set the third link to bypassed
  chain.template setBypassed<3>(true); // set the fourth link to bypassed

  switch (slope) // set the bypassed links according to the slope
  {
  case Slope_48:
    update<3>(chain, coefficients); // set the coefficients for the left low cut filter
  case Slope_36:
    update<2>(chain, coefficients); // set the coefficients for the left low cut filter
  case Slope_24:
    update<1>(chain, coefficients); // set the coefficients for the left low cut filter
  case Slope_12:
    update<0>(chain, coefficients); // set the coefficients for the left low cut filter
  };
}

inline auto makeLowCutFilter(const ChainSettings &chainSettings, double sampleRate)
{
  return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq,
                                                                                     sampleRate,
                                                                                     2 * (1 + chainSettings.lowCutSlope)); // create the low cut filter coefficients
}
inline auto makeHighCutFilter(const ChainSettings &chainSettings, double sampleRate)
{
  return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCutFreq,
                                                                                    sampleRate,
                                                                                    2 * (1 + chainSettings.highCutSlope)); // create the high cut filter coefficients
}
//==============================================================================
/**
 */
class SimpleEQAudioProcessor : public juce::AudioProcessor
{
public:
  //==============================================================================
  SimpleEQAudioProcessor();
  ~SimpleEQAudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  //==============================================================================
  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  //==============================================================================
  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  // function to create the parameter layout for the AudioProcessorValueTreeState
  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  // list of parameters we will use in the EQ
  juce::AudioProcessorValueTreeState apvts{*this, nullptr, "Parameters", createParameterLayout()};

  using BlockType = juce::AudioBuffer<float>;

  SingleChannelSampleFifo<BlockType> leftChannelFifo{Channel::Left};
  SingleChannelSampleFifo<BlockType> rightChannelFifo{Channel::Right};

private:
  MonoChain leftChain, rightChain;

  void updatePeakFilter(const ChainSettings &chainSettings);

  void updateLowCutFilters(const ChainSettings &chainSettings);
  void updateHighCutFilters(const ChainSettings &chainSettings);

  void updateFilters();

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQAudioProcessor)
};
