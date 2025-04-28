/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "LoopDataHelpers.h"

//==============================================================================
JBProducerAudioProcessorEditor::JBProducerAudioProcessorEditor(JBProducerAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
  // ——— Loop Selector ———
  addAndMakeVisible(loopLabel);
  addAndMakeVisible(loopSelector);
  //
  auto names = LoopDataHelpers::getLoopNames();
  for (int i = 0; i < names.size(); ++i)
    loopSelector.addItem(names[i], i + 1);
  //
  loopSelector.setSelectedId(1);
  loopSelector.onChange = [this]
  {
    currentLoopId = loopSelector.getSelectedId();
  };

  // ——— Generate buttons ———
  addAndMakeVisible(generateMidiButton);
  generateMidiButton.onClick = [this]
  { generateMidiLoop(); };

  addAndMakeVisible(generateAudioButton);
  generateAudioButton.onClick = [this]
  { generateAudioLoop(); };

  // ——— Draggable components ———
  midiLoopComponent.reset(new DraggableLoopComponent("MIDI Loop", juce::Colours::blue.withAlpha(0.6f)));
  audioLoopComponent.reset(new DraggableLoopComponent("Audio Loop", juce::Colours::green.withAlpha(0.6f)));
  addAndMakeVisible(midiLoopComponent.get());
  addAndMakeVisible(audioLoopComponent.get());

  // — Preview button —
  addAndMakeVisible(previewButton);
  previewButton.onClick = [this]
  { previewAudio(); };

  // Stop button
  addAndMakeVisible(stopButton);
  stopButton.onClick = [this]
  { stopPreview(); };

  // — Audio playback setup —
  deviceManager.initialise(
      /* numInputChannels  */ 0,
      /* numOutputChannels */ 2,
      /* xmlState           */ nullptr,
      /* selectDefaultDevice*/ true);
  deviceManager.addAudioCallback(&audioPlayer);
  audioPlayer.setSource(&transportSource);

  setSize(500, 400);
}

JBProducerAudioProcessorEditor::~JBProducerAudioProcessorEditor()
{
  // teardown audio playback
  audioPlayer.setSource(nullptr);
  deviceManager.removeAudioCallback(&audioPlayer);
  transportSource.setSource(nullptr);
}

//==============================================================================
void JBProducerAudioProcessorEditor::paint(juce::Graphics &g)
{
  // Fill the background
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void JBProducerAudioProcessorEditor::resized()
{
  int y = 10, pad = 10;
  // Loop label + combo:
  loopLabel.setBounds(10, y + pad, 40, 25);
  loopSelector.setBounds(55, y + pad, getWidth() - 65, 25);
  y += pad + 25 + pad;

  auto buttonWidth = (getWidth() - 3 * 10) / 2; // pad = 10
  previewButton.setBounds(10, y, buttonWidth, 30);
  stopButton.setBounds(20 + buttonWidth, y, buttonWidth, 30);

  y += 30 + pad;

  // Buttons:
  int btnW = (getWidth() - 3 * pad) / 2;
  generateMidiButton.setBounds(pad, y, btnW, 30);
  generateAudioButton.setBounds(2 * pad + btnW, y, btnW, 30);
  y += 30 + pad;

  // Draggable Loop Components:
  int dragH = juce::jmax(80, getHeight() - y - pad);
  int halfW = (getWidth() - 3 * pad) / 2;
  midiLoopComponent->setBounds(pad, y, halfW, dragH);
  audioLoopComponent->setBounds(2 * pad + halfW, y, halfW, dragH);
}

void JBProducerAudioProcessorEditor::generateMidiLoop()
{
  auto bpm = audioProcessor.getHostBPM();
  auto block = LoopDataHelpers::makeMidiBlock(currentLoopId, bpm);
  midiLoopComponent->setLoopData(block, "mid");
}

void JBProducerAudioProcessorEditor::generateAudioLoop()
{
  auto bpm = audioProcessor.getHostBPM();
  auto sr = audioProcessor.getSampleRate() < 8000 ? 44100.0 : audioProcessor.getSampleRate();
  auto block = LoopDataHelpers::makeAudioBlock(currentLoopId,
                                               bpm,
                                               2,
                                               sr);
  audioLoopComponent->setLoopData(block, "wav");
}
void JBProducerAudioProcessorEditor::previewAudio()
{
  transportSource.stop();
  transportSource.setSource(nullptr);
  transportReaderSource.reset();
  auto bpm = audioProcessor.getHostBPM();
  double sr = audioProcessor.getSampleRate() < 8000 ? 44100.0
                                                    : audioProcessor.getSampleRate();
  auto block = LoopDataHelpers::makeAudioBlock(currentLoopId,
                                               bpm,
                                               /* channels */ 2,
                                               sr);
  auto *memStream = new juce::MemoryInputStream(block, /* keepInternalCopy */ false);

  juce::WavAudioFormat wavFormat;
  if (auto *reader = wavFormat.createReaderFor(memStream, /* deleteStreamWhenDone */ true))
  {
    transportReaderSource.reset(new juce::AudioFormatReaderSource(reader, /* takeOwnership */ true));
    transportSource.setSource(transportReaderSource.get(),
                              /* readAheadBufferSize */ 0,
                              nullptr,
                              sr);
    transportSource.setPosition(0.0);
    transportSource.start();
  }
}
