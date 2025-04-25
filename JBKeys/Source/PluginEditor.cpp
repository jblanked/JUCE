/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JBKeysAudioProcessorEditor::JBKeysAudioProcessorEditor(JBKeysAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(400, 400);

  // Make the ComboBox visible
  addAndMakeVisible(audioProcessor.player);
}

JBKeysAudioProcessorEditor::~JBKeysAudioProcessorEditor()
{
}

//==============================================================================
void JBKeysAudioProcessorEditor::paint(juce::Graphics &g)
{
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void JBKeysAudioProcessorEditor::resized()
{
  audioProcessor.player.setBounds(getLocalBounds());
}
