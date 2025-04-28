/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Service/DraggableLoopComponent.h"

//==============================================================================
/**
 */
class JBProducerAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       public juce::DragAndDropContainer
{
public:
  JBProducerAudioProcessorEditor(JBProducerAudioProcessor &);
  ~JBProducerAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

private:
  JBProducerAudioProcessor &audioProcessor;

  juce::Label loopLabel{{}, "Loop:"};
  juce::ComboBox loopSelector;

  juce::TextButton generateMidiButton{"Generate MIDI Loop"};
  juce::TextButton generateAudioButton{"Generate Audio Loop"};

  std::unique_ptr<DraggableLoopComponent> midiLoopComponent;
  std::unique_ptr<DraggableLoopComponent> audioLoopComponent;

  juce::TextButton previewButton{"Preview"};
  juce::TextButton stopButton{"Stop"};

  // Audio playback in the editor
  juce::AudioDeviceManager deviceManager;
  juce::AudioSourcePlayer audioPlayer;
  juce::AudioTransportSource transportSource;
  std::unique_ptr<juce::AudioFormatReaderSource> transportReaderSource;

  int currentLoopId = 1;

  void generateMidiLoop();
  void generateAudioLoop();
  void previewAudio();
  void stopPreview() { transportSource.stop(); }

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JBProducerAudioProcessorEditor)
};
