/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JBDrumsAudioProcessorEditor::JBDrumsAudioProcessorEditor(JBDrumsAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(400, 300);

  // make the player visible in the editor
  addAndMakeVisible(audioProcessor.midiPlayer);
}

JBDrumsAudioProcessorEditor::~JBDrumsAudioProcessorEditor()
{
}

//==============================================================================
void JBDrumsAudioProcessorEditor::paint(juce::Graphics &g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void JBDrumsAudioProcessorEditor::resized()
{
  audioProcessor.midiPlayer.setBounds(getLocalBounds());
}
