/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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

private:
  MonoChain leftChain, rightChain;

  void updatePeakFilter(const ChainSettings &chainSettings);

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

  void updateLowCutFilters(const ChainSettings &chainSettings);
  void updateHighCutFilters(const ChainSettings &chainSettings);

  void updateFilters();
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQAudioProcessor)
};
