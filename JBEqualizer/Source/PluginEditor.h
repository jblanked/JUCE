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
#include "GUI/PresetPanel.h"

using namespace GUI;

//==============================================================================
/**
 */
class JBEqualizerAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
  JBEqualizerAudioProcessorEditor(JBEqualizerAudioProcessor &);
  ~JBEqualizerAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  JBEqualizerAudioProcessor &audioProcessor;

  RotarySliderWithLabels peakFreqSlider,
      peakGainSlider,
      peakQualitySlider,
      peak2FreqSlider,
      peak2GainSlider,
      peak2QualitySlider,
      peak3FreqSlider,
      peak3GainSlider,
      peak3QualitySlider,
      peak4FreqSlider,
      peak4GainSlider,
      peak4QualitySlider,
      lowShelfFreqSlider,
      lowShelfGainSlider,
      lowShelfQSlider,
      highShelfFreqSlider,
      highShelfGainSlider,
      highShelfQSlider,
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
      peak2FreqSliderAttachment,
      peak2GainSliderAttachment,
      peak2QualitySliderAttachment,
      peak3FreqSliderAttachment,
      peak3GainSliderAttachment,
      peak3QualitySliderAttachment,
      peak4FreqSliderAttachment,
      peak4GainSliderAttachment,
      peak4QualitySliderAttachment,
      lowShelfFreqSliderAttachment,
      lowShelfGainSliderAttachment,
      lowShelfQSliderAttachment,
      highShelfFreqSliderAttachment,
      highShelfGainSliderAttachment,
      highShelfQSliderAttachment,
      lowCutFreqSliderAttachment,
      highCutFreqSliderAttachment,
      lowCutSlopeSliderAttachment,
      highCutSlopeSliderAttachment;

  ResponseCurveComponent responseCurveComponent; // component to show the frequency response of the EQ

  PowerButton lowCutBypassButton,
      lowShelfBypassButton,
      peakBypassButton,
      peak2BypassButton,
      peak3BypassButton,
      peak4BypassButton,
      highShelfBypassButton,
      highCutBypassButton; // bypass buttons for the filters

  AnalyzerButton analyzerEnabledButton; // button to enable/disable the analyzer

  using ButtonAttachment = APVTS::ButtonAttachment; // typename alias to help with readability of the function
  ButtonAttachment lowCutBypassButtonAttachment,
      lowShelfBypassButtonAttachment,
      peakBypassButtonAttachment,
      peak2BypassButtonAttachment,
      peak3BypassButtonAttachment,
      peak4BypassButtonAttachment,
      highShelfBypassButtonAttachment,
      highCutBypassButtonAttachment,
      analyzerEnabledButtonAttachment; // declare an attachment for each button

  std::vector<juce::Component *> getComps();

  CustomLookAndFeel lnf; // custom look and feel for the editor

  PresetPanel presetPanel; // panel to show the presets

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JBEqualizerAudioProcessorEditor)
};
