/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <array>
#include "DSP/BasicAudioProcessor.h"
#include "Service/PresetManager.h"
#include "Service/ParameterManager.h"
#include "Presets.h"
#include "Settings.h"
using namespace DSP;
//==============================================================================
/**
 */
class JBEqualizerAudioProcessor : public BasicAudioProcessor
{
public:
  //==============================================================================
  JBEqualizerAudioProcessor();
  ~JBEqualizerAudioProcessor() override;

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

  // Implement the accessor from BasicAudioProcessor:
  juce::AudioProcessorValueTreeState &getAPVTS() override { return apvts; }

  using BlockType = juce::AudioBuffer<float>;

  SingleChannelSampleFifo<BlockType> leftChannelFifo{Channel::Left};
  SingleChannelSampleFifo<BlockType> rightChannelFifo{Channel::Right};

  SingleChannelSampleFifo<BlockType> &getLeftChannelFifo() override { return leftChannelFifo; }
  SingleChannelSampleFifo<BlockType> &getRightChannelFifo() override { return rightChannelFifo; }

  Service::PresetManager &getPresetManager() { return *presetManager; }

private:
  MonoChain leftChain, rightChain;

  void updatePeakFilters(const ChainSettings &chainSettings);
  void updateLowCutFilters(const ChainSettings &chainSettings);
  void updateHighCutFilters(const ChainSettings &chainSettings);
  void updateShelfFilters(const ChainSettings &chainSettings);

  void updateFilters();

  std::unique_ptr<Service::PresetManager> presetManager;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JBEqualizerAudioProcessor)
};
