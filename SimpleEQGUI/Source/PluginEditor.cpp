/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQAudioProcessorEditor::SimpleEQAudioProcessorEditor(SimpleEQAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      //
      peakFreqSlider(*audioProcessor.apvts.getParameter("peakFrequency"), "Hz"),
      peakGainSlider(*audioProcessor.apvts.getParameter("peakGain"), "dB"),
      peakQualitySlider(*audioProcessor.apvts.getParameter("peakQuality"), ""),
      lowCutFreqSlider(*audioProcessor.apvts.getParameter("lowCutFrequency"), "Hz"),
      highCutFreqSlider(*audioProcessor.apvts.getParameter("highCutFrequency"), "Hz"),
      lowCutSlopeSlider(*audioProcessor.apvts.getParameter("lowCutSlope"), "dB/Oct"),
      highCutSlopeSlider(*audioProcessor.apvts.getParameter("highCutSlope"), "dB/Oct"),
      //
      responseCurveComponent(audioProcessor),
      //
      peakFreqSliderAttachment(audioProcessor.apvts, "peakFrequency", peakFreqSlider),
      peakGainSliderAttachment(audioProcessor.apvts, "peakGain", peakGainSlider),
      peakQualitySliderAttachment(audioProcessor.apvts, "peakQuality", peakQualitySlider),
      lowCutFreqSliderAttachment(audioProcessor.apvts, "lowCutFrequency", lowCutFreqSlider),
      highCutFreqSliderAttachment(audioProcessor.apvts, "highCutFrequency", highCutFreqSlider),
      lowCutSlopeSliderAttachment(audioProcessor.apvts, "lowCutSlope", lowCutSlopeSlider),
      highCutSlopeSliderAttachment(audioProcessor.apvts, "highCutSlope", highCutSlopeSlider),
      //
      lowCutBypassButtonAttachment(audioProcessor.apvts, "lowCutBypass", lowCutBypassButton),
      peakBypassButtonAttachment(audioProcessor.apvts, "peakBypass", peakBypassButton),
      highCutBypassButtonAttachment(audioProcessor.apvts, "highCutBypass", highCutBypassButton),
      analyzerEnabledButtonAttachment(audioProcessor.apvts, "analyzerEnabled", analyzerEnabledButton)
{
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.

  peakFreqSlider.labels.add({0.f, "20Hz"});  // add a label to the peak frequency slider
  peakFreqSlider.labels.add({1.f, "20kHz"}); // add a label to the peak frequency slider

  peakGainSlider.labels.add({0.f, "-24dB"}); // add a label to the peak gain slider
  peakGainSlider.labels.add({1.f, "24dB"});  // add a label to the peak gain slider

  peakQualitySlider.labels.add({0.f, "0.1"});  // add a label to the peak quality slider
  peakQualitySlider.labels.add({1.f, "10.0"}); // add a label to the peak quality slider

  lowCutFreqSlider.labels.add({0.f, "20Hz"});  // add a label to the low cut frequency slider
  lowCutFreqSlider.labels.add({1.f, "20kHz"}); // add a label to the low cut frequency slider

  highCutFreqSlider.labels.add({0.f, "20Hz"});  // add a label to the high cut frequency slider
  highCutFreqSlider.labels.add({1.f, "20kHz"}); // add a label to the high cut frequency slider

  lowCutSlopeSlider.labels.add({0.0f, "12"}); // add a label to the low cut slope slider
  lowCutSlopeSlider.labels.add({1.f, "48"});  // add a label to the low cut slope slider

  highCutSlopeSlider.labels.add({0.0f, "12"}); // add a label to the high cut slope slider
  highCutSlopeSlider.labels.add({1.f, "48"});  // add a label to the high cut slope slider

  for (auto *comp : getComps())
  {
    addAndMakeVisible(comp); // add the components to the editor
  }

  peakBypassButton.setLookAndFeel(&lnf);      // set the look and feel of the peak bypass button
  lowCutBypassButton.setLookAndFeel(&lnf);    // set the look and feel of the low cut bypass button
  highCutBypassButton.setLookAndFeel(&lnf);   // set the look and feel of the high cut bypass button
  analyzerEnabledButton.setLookAndFeel(&lnf); // set the look and feel of the analyzer enabled button

  auto safePtr = juce::Component::SafePointer<SimpleEQAudioProcessorEditor>(this);
  peakBypassButton.onClick = [safePtr]()
  {
    if (auto *comp = safePtr.getComponent())
    {
      auto bypassed = comp->peakBypassButton.getToggleState();

      comp->peakFreqSlider.setEnabled(!bypassed);
      comp->peakGainSlider.setEnabled(!bypassed);
      comp->peakQualitySlider.setEnabled(!bypassed);
    }
  };

  lowCutBypassButton.onClick = [safePtr]()
  {
    if (auto *comp = safePtr.getComponent())
    {
      auto bypassed = comp->lowCutBypassButton.getToggleState();

      comp->lowCutFreqSlider.setEnabled(!bypassed);
      comp->lowCutSlopeSlider.setEnabled(!bypassed);
    }
  };

  highCutBypassButton.onClick = [safePtr]()
  {
    if (auto *comp = safePtr.getComponent())
    {
      auto bypassed = comp->highCutBypassButton.getToggleState();

      comp->highCutFreqSlider.setEnabled(!bypassed);
      comp->highCutSlopeSlider.setEnabled(!bypassed);
    }
  };

  analyzerEnabledButton.onClick = [safePtr]()
  {
    if (auto *comp = safePtr.getComponent())
    {
      auto enabled = comp->analyzerEnabledButton.getToggleState();
      comp->responseCurveComponent.toggleAnalyzerEnabled(enabled);
    }
  };

  setSize(600, 480); // size of the editor window
}

SimpleEQAudioProcessorEditor::~SimpleEQAudioProcessorEditor()
{
  // This will be deleted by the compiler automatically, but you can do it here
  // if you want to be explicit about it.
  peakBypassButton.setLookAndFeel(nullptr);      // set the look and feel of the peak bypass button to nullptr
  lowCutBypassButton.setLookAndFeel(nullptr);    // set the look and feel of the low cut bypass button to nullptr
  highCutBypassButton.setLookAndFeel(nullptr);   // set the look and feel of the high cut bypass button to nullptr
  analyzerEnabledButton.setLookAndFeel(nullptr); // set the look and feel of the analyzer enabled button to nullptr
}

//==============================================================================
void SimpleEQAudioProcessorEditor::paint(juce::Graphics &g)
{
  using namespace juce;

  g.fillAll(Colours::black); // fill the background with black

  g.setColour(Colours::white);
  g.setFont(14);
  g.drawFittedText("LowCut", lowCutSlopeSlider.getBounds(), juce::Justification::centredBottom, 1);
  g.drawFittedText("Peak", peakQualitySlider.getBounds(), juce::Justification::centredBottom, 1);
  g.drawFittedText("HighCut", highCutSlopeSlider.getBounds(), juce::Justification::centredBottom, 1);
}

void SimpleEQAudioProcessorEditor::resized()
{
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..

  // first 3rd of the display for showing the frequency response of the EQ
  auto bounds = getLocalBounds(); // bounding box of the editor window
  bounds.removeFromTop(4);        // remove the top 4 pixels from the bounds for spacing

  auto analyzerEnabledArea = bounds.removeFromTop(25);

  analyzerEnabledArea.setWidth(50);
  analyzerEnabledArea.setX(5);
  analyzerEnabledArea.removeFromTop(2);

  analyzerEnabledButton.setBounds(analyzerEnabledArea); // set the bounds of the analyzer enabled button

  float hRatio = 25.f / 100.f;                                           // JUCE_LIVE_CONSTANT(33) / 100.f;                      // height ratio of the response area
  auto responseArea = bounds.removeFromTop(bounds.getHeight() * hRatio); // remove the top 1/3 of the display for showing the frequency response

  responseCurveComponent.setBounds(responseArea); // set the bounds of the response curve component

  bounds.removeFromTop(5); // remove the top 5 pixels from the bounds for spacing

  // bottom 2/3 of the display for showing the sliders
  auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);  // remove the left half of the display for showing the low cut filter
  auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5); // remove the right half of the display for showing the high cut filter

  lowCutBypassButton.setBounds(lowCutArea.removeFromTop(25));                         // set the bounds of the low cut bypass button
  lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5)); // set the bounds of the low cut filter slider
  lowCutSlopeSlider.setBounds(lowCutArea);                                            // set the bounds of the low cut filter slope slider

  highCutBypassButton.setBounds(highCutArea.removeFromTop(25));                          // set the bounds of the high cut bypass button
  highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5)); // set the bounds of the high cut filter slider
  highCutSlopeSlider.setBounds(highCutArea);                                             // set the bounds of the high cut filter slope slider

  peakBypassButton.setBounds(bounds.removeFromTop(25));                      // set the bounds of the peak bypass button
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
      &responseCurveComponent,
      &lowCutBypassButton,
      &peakBypassButton,
      &highCutBypassButton,
      &analyzerEnabledButton

  };
}
