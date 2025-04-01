/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQAudioProcessorEditor::SimpleEQAudioProcessorEditor(SimpleEQAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
  for (auto *comp : getComps())
  {
    addAndMakeVisible(comp); // add the components to the editor
  }

  setSize(600, 400); // size of the editor window
}

SimpleEQAudioProcessorEditor::~SimpleEQAudioProcessorEditor()
{
}

//==============================================================================
void SimpleEQAudioProcessorEditor::paint(juce::Graphics &g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  g.setColour(juce::Colours::white);
  g.setFont(juce::FontOptions(15.0f));
  g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void SimpleEQAudioProcessorEditor::resized()
{
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..

  // first 3rd of the display for showing the frequency response of the EQ
  auto bounds = getLocalBounds();                                      // bounding box of the editor window
  auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33); // remove the top 1/3 of the display for showing the frequency response

  // bottom 2/3 of the display for showing the sliders
  auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);  // remove the left half of the display for showing the low cut filter
  auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5); // remove the right half of the display for showing the high cut filter

  lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5)); // set the bounds of the low cut filter slider
  lowCutSlopeSlider.setBounds(lowCutArea);                                            // set the bounds of the low cut filter slope slider

  highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5)); // set the bounds of the high cut filter slider
  highCutSlopeSlider.setBounds(highCutArea);                                             // set the bounds of the high cut filter slope slider

  peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33)); // set the bounds of the peak frequency slider
  peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));  // set the bounds of the peak gain slider
  peakQualitySlider.setBounds(bounds);                                       // set the bounds of the peak quality slider
}

std::vector<juce::Component *> SimpleEQAudioProcessorEditor::getComps()
{
  return {
      &peakFreqSlider,
      &peakGainSlider,
      &peakQualitySlider,
      &lowCutFreqSlider,
      &highCutFreqSlider,
      &lowCutSlopeSlider,
      &highCutSlopeSlider,
  };
}
