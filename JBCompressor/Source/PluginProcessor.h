/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP/Compressor.h"
#include "Service/PresetManager.h"
#include "Service/ParameterManager.h"
#include "Presets.h"

//==============================================================================
/**
 */
class JBCompressorAudioProcessor : public juce::AudioProcessor,
                                   public juce::AudioProcessorValueTreeState::Listener
{
public:
  //==============================================================================
  JBCompressorAudioProcessor();
  ~JBCompressorAudioProcessor() override;

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

  juce::AudioProcessorValueTreeState treeState;

  Service::PresetManager &getPresetManager() { return *presetManager; }

  // Accessors for meters
  float getGainReduction() const { return compressorModule.getGainReduction(); }
  float getInputLevel() const { return compressorModule.getInputLevel(); }
  float getOutputLevel() const { return compressorModule.getOutputLevel(); }

private:
  juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
  void parameterChanged(const juce::String &parameterID, float newValue) override;
  void updateParameters();
  DSP::Compressor compressorModule;
  std::unique_ptr<Service::PresetManager> presetManager;
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JBCompressorAudioProcessor)
};
