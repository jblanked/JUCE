/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct LookAndFeel : juce::LookAndFeel_V4
{
  void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                        float sliderPosProportional, float rotaryStartAngle,
                        float rotaryEndAngle, juce::Slider &slider) override;
};

struct RotarySliderWithLabels : juce::Slider
{
  RotarySliderWithLabels(juce::RangedAudioParameter &rap, const juce::String &unitSuffix) : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                                                                                         juce::Slider::TextEntryBoxPosition::NoTextBox),
                                                                                            param(&rap), suffix(unitSuffix)
  {
    setLookAndFeel(&lnf); // set the custom look and feel for the slider
  }

  ~RotarySliderWithLabels()
  {
    setLookAndFeel(nullptr); // reset the look and feel to the default
  }

  struct LabelPos
  {
    float pos;          // position of the label
    juce::String label; // label text
  };

  juce::Array<LabelPos> labels; // array of labels for the slider

  void paint(juce::Graphics &g) override;
  juce::Rectangle<int> getSliderBounds() const;
  int getTextHeight() const { return 14; } // height of the text label
  juce::String getDisplayString() const;

private:
  LookAndFeel lnf;                   // custom look and feel for the slider
  juce::RangedAudioParameter *param; // pointer to the parameter that the slider is controlling
  juce::String suffix;               // suffix for the parameter value
};

struct ResponseCurveComponent : public juce::Component, juce::AudioProcessorParameter::Listener, juce::Timer
{
  ResponseCurveComponent(SimpleEQAudioProcessor &);
  ~ResponseCurveComponent();
  void parameterValueChanged(int parameterIndex, float newValue) override;
  void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {};
  void timerCallback() override; // query atomic flag to decide if the chain needs updating and our components need repainting
  void paint(juce::Graphics &g) override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  SimpleEQAudioProcessor &audioProcessor;
  juce::Atomic<bool> parametersChanged{false}; // atomic flag to indicate if the parameters have changed
  MonoChain monoChain;                         // chain to hold the filter coefficients

  void updateChain(); // update the chain with the new parameters
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

  std::vector<juce::Component *> getComps();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQAudioProcessorEditor)
};
