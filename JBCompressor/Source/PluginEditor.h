/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/CustomLookAndFeel.h"
#include "GUI/RotarySliderWithLabels.h"
#include "GUI/VerticalSliderWithLabels.h"
#include "GUI/PresetPanel.h"
#include "GUI/CompressorVisualizer.h"

//==============================================================================
/**
 */
class JBCompressorAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
  JBCompressorAudioProcessorEditor(JBCompressorAudioProcessor &);
  ~JBCompressorAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  JBCompressorAudioProcessor &audioProcessor;

  GUI::PresetPanel presetPanel; // panel to show the presets

  GUI::RotarySliderWithLabels thresholdSlider, ratioSlider, attackSlider, releaseSlider;

  GUI::VerticalSliderWithLabels inputGainSlider, outputGainSlider;

  // typename alias to help with readability of the function
  using APVTS = juce::AudioProcessorValueTreeState;
  using Attachment = APVTS::SliderAttachment;

  // declare an attachment for each slider
  Attachment inputGainSliderAttachment,
      thresholdSliderAttachment,
      ratioSliderAttachment,
      attackSliderAttachment,
      releaseSliderAttachment,
      outputGainSliderAttachment;

  std::vector<juce::Component *> getComps();

  GUI::CustomLookAndFeel lnf; // custom look and feel for the editor

  GUI::CompressorVisualizer visualizer;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JBCompressorAudioProcessorEditor)
};
