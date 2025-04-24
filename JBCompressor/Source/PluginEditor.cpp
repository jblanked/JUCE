/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JBCompressorAudioProcessorEditor::JBCompressorAudioProcessorEditor(JBCompressorAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      //
      presetPanel(p.getPresetManager()),
      //
      thresholdSlider(*audioProcessor.treeState.getParameter("threshold"), "dB"),
      ratioSlider(*audioProcessor.treeState.getParameter("ratio"), ""),
      attackSlider(*audioProcessor.treeState.getParameter("attack"), "ms"),
      releaseSlider(*audioProcessor.treeState.getParameter("release"), "ms"),
      inputGainSlider(*audioProcessor.treeState.getParameter("inputGain"), "dB"),
      outputGainSlider(*audioProcessor.treeState.getParameter("outputGain"), "dB"),
      //
      inputGainSliderAttachment(audioProcessor.treeState, "inputGain", inputGainSlider),
      thresholdSliderAttachment(audioProcessor.treeState, "threshold", thresholdSlider),
      ratioSliderAttachment(audioProcessor.treeState, "ratio", ratioSlider),
      attackSliderAttachment(audioProcessor.treeState, "attack", attackSlider),
      releaseSliderAttachment(audioProcessor.treeState, "release", releaseSlider),
      outputGainSliderAttachment(audioProcessor.treeState, "outputGain", outputGainSlider),
      //
      visualizer(audioProcessor.treeState, p)
{
  thresholdSlider.labels.add({0.f, "-60dB"});
  thresholdSlider.labels.add({1.f, "10dB"});

  ratioSlider.labels.add({0.f, "1:1"});
  ratioSlider.labels.add({1.f, "20:1"});

  attackSlider.labels.add({0.f, "0ms"});
  attackSlider.labels.add({1.f, "200ms"});

  releaseSlider.labels.add({0.f, "5ms"});
  releaseSlider.labels.add({1.f, "5000ms"});

  // Add components
  for (auto *comp : getComps())
    addAndMakeVisible(comp);

  addAndMakeVisible(presetPanel);
  addAndMakeVisible(visualizer);
  setResizable(true, false);
  setSize(700, 500);
}

JBCompressorAudioProcessorEditor::~JBCompressorAudioProcessorEditor()
{
}

//==============================================================================
void JBCompressorAudioProcessorEditor::paint(juce::Graphics &g)
{
  using namespace juce;
  g.fillAll(Colours::black); // fill the background with black
  g.setColour(Colours::white);
  g.setFont(14);
  g.drawFittedText("Input", inputGainSlider.getBounds(), Justification::centredBottom, 1);
  g.drawFittedText("Threshold", thresholdSlider.getBounds(), Justification::centredBottom, 1);
  g.drawFittedText("Ratio", ratioSlider.getBounds(), Justification::centredBottom, 1);
  g.drawFittedText("Attack", attackSlider.getBounds(), Justification::centredBottom, 1);
  g.drawFittedText("Release", releaseSlider.getBounds(), Justification::centredBottom, 1);
  g.drawFittedText("Output", outputGainSlider.getBounds(), Justification::centredBottom, 1);
}

void JBCompressorAudioProcessorEditor::resized()
{
  auto area = getLocalBounds();
  auto presetArea = area.removeFromTop(proportionOfHeight(0.1f));
  presetPanel.setBounds(presetArea);
  area.removeFromTop(5);

  // Add visualizer at the top
  auto visualizerArea = area.removeFromTop(area.getHeight() * 0.3f);
  visualizer.setBounds(visualizerArea.reduced(10));

  // left/right sliders each 1/4 width
  int quarterW = area.getWidth() / 4;
  auto leftArea = area.removeFromLeft(quarterW);
  auto rightArea = area.removeFromRight(quarterW);
  inputGainSlider.setBounds(leftArea.reduced(10));
  outputGainSlider.setBounds(rightArea.reduced(10));

  // center area split into 2x2 grid
  auto centerArea = area;
  int cellW = centerArea.getWidth() / 2;
  int cellH = centerArea.getHeight() / 2;

  auto makeCenteredCell = [&](juce::Rectangle<int> cell, juce::Slider &slider)
  {
    // inset cell
    auto r = cell.reduced(10);

    // center the slider in that rect
    slider.setBounds(r.withSizeKeepingCentre(r.getWidth(), r.getHeight()));
  };

  // first row
  auto row1 = centerArea.removeFromTop(cellH);
  auto cell1 = row1.removeFromLeft(cellW);
  auto cell2 = row1;
  makeCenteredCell(cell1, thresholdSlider);
  makeCenteredCell(cell2, ratioSlider);

  // second row
  auto row2 = centerArea;
  auto cell3 = row2.removeFromLeft(cellW);
  auto cell4 = row2;
  makeCenteredCell(cell3, releaseSlider);
  makeCenteredCell(cell4, attackSlider);
}

std::vector<juce::Component *> JBCompressorAudioProcessorEditor::getComps()
{
  return {
      &inputGainSlider,
      &thresholdSlider,
      &ratioSlider,
      &attackSlider,
      &releaseSlider,
      &outputGainSlider,
      &visualizer};
}
