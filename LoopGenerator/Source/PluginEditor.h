#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Service/DraggableLoopComponent.h"

class LoopGeneratorAudioProcessorEditor : public juce::AudioProcessorEditor,
                                          public juce::DragAndDropContainer
{
public:
  LoopGeneratorAudioProcessorEditor(LoopGeneratorAudioProcessor &, juce::AudioProcessorValueTreeState &);
  ~LoopGeneratorAudioProcessorEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  LoopGeneratorAudioProcessor &audioProcessor;
  juce::AudioProcessorValueTreeState &parameters;

  // UI Components
  juce::TextButton generateMidiButton{"Generate MIDI Loop"};
  juce::TextButton generateAudioButton{"Generate Audio Loop"};

  // Parameter sliders
  juce::Slider numBarsSlider;
  juce::Slider beatsPerBarSlider;
  juce::Slider bpmSlider;
  juce::ComboBox waveformSelector;
  juce::Slider frequencySlider;

  // Labels
  juce::Label numBarsLabel{"", "Number of Bars:"};
  juce::Label beatsPerBarLabel{"", "Beats Per Bar:"};
  juce::Label bpmLabel{"", "BPM:"};
  juce::Label waveformLabel{"", "Waveform:"};
  juce::Label frequencyLabel{"", "Frequency (Hz):"};

  // Draggable components
  std::unique_ptr<DraggableLoopComponent> midiLoopComponent;
  std::unique_ptr<DraggableLoopComponent> audioLoopComponent;

  // Attachments for parameters
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> numBarsAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> beatsPerBarAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bpmAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveformAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencyAttachment;

  void setupSliders();
  void generateMidiLoop();
  void generateAudioLoop();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoopGeneratorAudioProcessorEditor)
};
