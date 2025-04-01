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
  // create filter types
  using Filter = juce::dsp::IIR::Filter<float>;
  using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
  using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;
  MonoChain leftChain, rightChain;

  // enum that represents each link's position in the chain
  enum ChainPositions
  {
    LowCut,
    Peak,
    HighCut
  };

  void updatePeakFilter(const ChainSettings &chainSettings);
  using Coefficients = Filter::CoefficientsPtr;
  static void updateCoefficients(Coefficients &old, const Coefficients &replcements);
  template <typename ChainType, typename CoefficientsType>
  void updateCutFilter(ChainType &leftLowCut, const CoefficientsType &cutCoefficients, const Slope &lowCutSlope)
  {
    // bypass all of the links in the chain
    leftLowCut.template setBypassed<0>(true); // set the first link to bypassed
    leftLowCut.template setBypassed<1>(true); // set the second link to bypassed
    leftLowCut.template setBypassed<2>(true); // set the third link to bypassed
    leftLowCut.template setBypassed<3>(true); // set the fourth link to bypassed

    switch (lowCutSlope) // set the bypassed links according to the slope
    {
    case Slope_12:
      *leftLowCut.template get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
      leftLowCut.template setBypassed<0>(false);                        // set the first link to not bypassed
      break;
    case Slope_24:
      *leftLowCut.template get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
      leftLowCut.template setBypassed<0>(false);                        // set the first link to not bypassed
      *leftLowCut.template get<1>().coefficients = *cutCoefficients[1]; // set the coefficients for the second link
      leftLowCut.template setBypassed<1>(false);                        // set the second link to not bypassed
      break;
    case Slope_36:
      *leftLowCut.template get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
      leftLowCut.template setBypassed<0>(false);                        // set the first link to not bypassed
      *leftLowCut.template get<1>().coefficients = *cutCoefficients[1]; // set the coefficients for the second link
      leftLowCut.template setBypassed<1>(false);                        // set the second link to not bypassed
      *leftLowCut.template get<2>().coefficients = *cutCoefficients[2]; // set the coefficients for the third link
      leftLowCut.template setBypassed<2>(false);                        // set the third link to not bypassed
      break;
    case Slope_48:
      *leftLowCut.template get<0>().coefficients = *cutCoefficients[0]; // set the coefficients for the first link
      leftLowCut.template setBypassed<0>(false);                        // set the first link to not bypassed
      *leftLowCut.template get<1>().coefficients = *cutCoefficients[1]; // set the coefficients for the second link
      leftLowCut.template setBypassed<1>(false);                        // set the second link to not bypassed
      *leftLowCut.template get<2>().coefficients = *cutCoefficients[2]; // set the coefficients for the third link
      leftLowCut.template setBypassed<2>(false);                        // set the third link to not bypassed
      *leftLowCut.template get<3>().coefficients = *cutCoefficients[3]; // set the coefficients for the fourth link
      leftLowCut.template setBypassed<3>(false);                        // set the fourth link to not bypassed
      break;
    }
  }
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQAudioProcessor)
};
