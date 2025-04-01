/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

// generic class for all sliders in GUI

struct CustomRotarySlider : public juce::Slider
{
  CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                      juce::Slider::TextEntryBoxPosition::NoTextBox)
  {
    //
  }
};

//==============================================================================
/**
 */
class SimpleEQAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
  SimpleEQAudioProcessorEditor(SimpleEQAudioProcessor &);
  ~SimpleEQAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  SimpleEQAudioProcessor &audioProcessor;

  CustomRotarySlider peakFreqSlider,
      peakGainSlider,
      peakQualitySlider,
      lowCutFreqSlider,
      highCutFreqSlider,
      lowCutSlopeSlider,
      highCutSlopeSlider;

  // typename alias to help with readability of the function
  using APVTS = juce::AudioProcessorValueTreeState;
  using Attachment = APVTS::SliderAttachment;

  // declare an attachment for each slider
  Attachment peakFreqSliderAttachment,
      peakGainSliderAttachment,
      peakQualitySliderAttachment,
      lowCutFreqSliderAttachment,
      highCutFreqSliderAttachment,
      lowCutSlopeSliderAttachment,
      highCutSlopeSliderAttachment;

  std::vector<juce::Component *> getComps();

  MonoChain monoChain;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQAudioProcessorEditor)
};
