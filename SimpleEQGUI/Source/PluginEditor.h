/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/CustomLookAndFeel.h"
#include "GUI/FFTComponents.h"
#include "GUI/ResponseCurveComponent.h"
#include "GUI/RotarySliderWithLabels.h"

using namespace GUI;

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

  RotarySliderWithLabels peakFreqSlider,
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

  ResponseCurveComponent responseCurveComponent; // component to show the frequency response of the EQ

  PowerButton lowCutBypassButton, peakBypassButton, highCutBypassButton; // bypass buttons for the filters

  AnalyzerButton analyzerEnabledButton; // button to enable/disable the analyzer

  using ButtonAttachment = APVTS::ButtonAttachment; // typename alias to help with readability of the function
  ButtonAttachment lowCutBypassButtonAttachment,
      peakBypassButtonAttachment,
      highCutBypassButtonAttachment,
      analyzerEnabledButtonAttachment; // declare an attachment for each button

  std::vector<juce::Component *> getComps();

  LookAndFeel lnf; // custom look and feel for the editor

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQAudioProcessorEditor)
};
