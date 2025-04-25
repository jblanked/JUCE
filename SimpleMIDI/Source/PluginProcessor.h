#pragma once

#include <JuceHeader.h>

class SimpleMIDIAudioProcessor : public juce::AudioProcessor
{
public:
  SimpleMIDIAudioProcessor();
  ~SimpleMIDIAudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  //==============================================================================
  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override { return false; }

  //==============================================================================
  const juce::String getName() const override { return JucePlugin_Name; }
  bool acceptsMidi() const override { return true; }
  bool producesMidi() const override { return false; }
  bool isMidiEffect() const override { return false; }
  double getTailLengthSeconds() const override { return 0; }

  //==============================================================================
  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  const juce::String getProgramName(int) override { return {}; }
  void changeProgramName(int, const juce::String &) override {}

  //==============================================================================
  void getStateInformation(juce::MemoryBlock &) override {}
  void setStateInformation(const void *, int) override {}

private:
  juce::Synthesiser synth;
  juce::AudioFormatManager formatManager;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleMIDIAudioProcessor)
};
