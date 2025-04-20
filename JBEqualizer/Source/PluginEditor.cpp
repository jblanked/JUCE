/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JBEqualizerAudioProcessorEditor::JBEqualizerAudioProcessorEditor(JBEqualizerAudioProcessor &p)
: AudioProcessorEditor(&p), audioProcessor(p), peakFreqSlider(*audioProcessor.apvts.getParameter("peakFrequency"), "Hz"),
// Peaks
peakGainSlider(*audioProcessor.apvts.getParameter("peakGain"), "dB"),
peakQualitySlider(*audioProcessor.apvts.getParameter("peakQuality"), ""),
peak2FreqSlider(*audioProcessor.apvts.getParameter("peak2Frequency"), "Hz"),
peak2GainSlider(*audioProcessor.apvts.getParameter("peak2Gain"), "dB"),
peak2QualitySlider(*audioProcessor.apvts.getParameter("peak2Quality"), ""),
peak3FreqSlider(*audioProcessor.apvts.getParameter("peak3Frequency"), "Hz"),
peak3GainSlider(*audioProcessor.apvts.getParameter("peak3Gain"), "dB"),
peak3QualitySlider(*audioProcessor.apvts.getParameter("peak3Quality"), ""),
peak4FreqSlider(*audioProcessor.apvts.getParameter("peak4Frequency"), "Hz"),
peak4GainSlider(*audioProcessor.apvts.getParameter("peak4Gain"), "dB"),
peak4QualitySlider(*audioProcessor.apvts.getParameter("peak4Quality"), ""),
lowShelfFreqSlider(*audioProcessor.apvts.getParameter("lowShelfFrequency"), "Hz"),
// Shelves
lowShelfGainSlider(*audioProcessor.apvts.getParameter("lowShelfGain"), "dB"),
lowShelfQSlider(*audioProcessor.apvts.getParameter("lowShelfQ"), ""),
highShelfFreqSlider(*audioProcessor.apvts.getParameter("highShelfFrequency"), "Hz"),
highShelfGainSlider(*audioProcessor.apvts.getParameter("highShelfGain"), "dB"),
highShelfQSlider(*audioProcessor.apvts.getParameter("highShelfQ"), ""),
lowCutFreqSlider(*audioProcessor.apvts.getParameter("lowCutFrequency"), "Hz"),
// Cuts
highCutFreqSlider(*audioProcessor.apvts.getParameter("highCutFrequency"), "Hz"),
lowCutSlopeSlider(*audioProcessor.apvts.getParameter("lowCutSlope"), "dB/Oct"),
highCutSlopeSlider(*audioProcessor.apvts.getParameter("highCutSlope"), "dB/Oct"),
peakFreqSliderAttachment(audioProcessor.apvts, "peakFrequency", peakFreqSlider),
// Analyzer
peakGainSliderAttachment(audioProcessor.apvts, "peakGain", peakGainSlider),
// Sliders
peakQualitySliderAttachment(audioProcessor.apvts, "peakQuality", peakQualitySlider),
peak2FreqSliderAttachment(audioProcessor.apvts, "peak2Frequency", peak2FreqSlider),
peak2GainSliderAttachment(audioProcessor.apvts, "peak2Gain", peak2GainSlider),
peak2QualitySliderAttachment(audioProcessor.apvts, "peak2Quality", peak2QualitySlider),
peak3FreqSliderAttachment(audioProcessor.apvts, "peak3Frequency", peak3FreqSlider),
peak3GainSliderAttachment(audioProcessor.apvts, "peak3Gain", peak3GainSlider),
peak3QualitySliderAttachment(audioProcessor.apvts, "peak3Quality", peak3QualitySlider),
peak4FreqSliderAttachment(audioProcessor.apvts, "peak4Frequency", peak4FreqSlider),
peak4GainSliderAttachment(audioProcessor.apvts, "peak4Gain", peak4GainSlider),
peak4QualitySliderAttachment(audioProcessor.apvts, "peak4Quality", peak4QualitySlider),
lowShelfFreqSliderAttachment(audioProcessor.apvts, "lowShelfFrequency", lowShelfFreqSlider),
lowShelfGainSliderAttachment(audioProcessor.apvts, "lowShelfGain", lowShelfGainSlider),
//
lowShelfQSliderAttachment(audioProcessor.apvts, "lowShelfQ", lowShelfQSlider),
highShelfFreqSliderAttachment(audioProcessor.apvts, "highShelfFrequency", highShelfFreqSlider),
highShelfGainSliderAttachment(audioProcessor.apvts, "highShelfGain", highShelfGainSlider),
highShelfQSliderAttachment(audioProcessor.apvts, "highShelfQ", highShelfQSlider),
lowCutFreqSliderAttachment(audioProcessor.apvts, "lowCutFrequency", lowCutFreqSlider),
highCutFreqSliderAttachment(audioProcessor.apvts, "highCutFrequency", highCutFreqSlider),
//
lowCutSlopeSliderAttachment(audioProcessor.apvts, "lowCutSlope", lowCutSlopeSlider),
highCutSlopeSliderAttachment(audioProcessor.apvts, "highCutSlope", highCutSlopeSlider),
responseCurveComponent(audioProcessor),
lowCutBypassButtonAttachment(audioProcessor.apvts, "lowCutBypass", lowCutBypassButton),
// Bypass Buttons
lowShelfBypassButtonAttachment(audioProcessor.apvts, "lowShelfBypass", lowShelfBypassButton),
peakBypassButtonAttachment(audioProcessor.apvts, "peakBypass", peakBypassButton),
peak2BypassButtonAttachment(audioProcessor.apvts, "peak2Bypass", peak2BypassButton),
peak3BypassButtonAttachment(audioProcessor.apvts, "peak3Bypass", peak3BypassButton),
peak4BypassButtonAttachment(audioProcessor.apvts, "peak4Bypass", peak4BypassButton),
highShelfBypassButtonAttachment(audioProcessor.apvts, "highShelfBypass", highShelfBypassButton),
highCutBypassButtonAttachment(audioProcessor.apvts, "highCutBypass", highCutBypassButton),
analyzerEnabledButtonAttachment(audioProcessor.apvts, "analyzerEnabled", analyzerEnabledButton),
presetPanel(p.getPresetManager())
{
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.

  peakFreqSlider.labels.add({0.f, "20Hz"});  // add a label to the peak frequency slider
  peakFreqSlider.labels.add({1.f, "20kHz"}); // add a label to the peak frequency slider

  peakGainSlider.labels.add({0.f, "-24dB"}); // add a label to the peak gain slider
  peakGainSlider.labels.add({1.f, "24dB"});  // add a label to the peak gain slider

  peakQualitySlider.labels.add({0.f, "0.1"});  // add a label to the peak quality slider
  peakQualitySlider.labels.add({1.f, "10.0"}); // add a label to the peak quality slider

  peak2FreqSlider.labels.add({0.f, "20Hz"});  // add a label to the second peak frequency slider
  peak2FreqSlider.labels.add({1.f, "20kHz"}); // add a label to the second peak frequency slider

  peak2GainSlider.labels.add({0.f, "-24dB"}); // add a label to the second peak gain slider
  peak2GainSlider.labels.add({1.f, "24dB"});  // add a label to the second peak gain slider

  peak2QualitySlider.labels.add({0.f, "0.1"});  // add a label to the second peak quality slider
  peak2QualitySlider.labels.add({1.f, "10.0"}); // add a label to the second peak quality slider

  peak3FreqSlider.labels.add({0.f, "20Hz"});  // add a label to the third peak frequency slider
  peak3FreqSlider.labels.add({1.f, "20kHz"}); // add a label to the third peak frequency slider

  peak3GainSlider.labels.add({0.f, "-24dB"}); // add a label to the third peak gain slider
  peak3GainSlider.labels.add({1.f, "24dB"});  // add a label to the third peak gain slider

  peak3QualitySlider.labels.add({0.f, "0.1"});  // add a label to the third peak quality slider
  peak3QualitySlider.labels.add({1.f, "10.0"}); // add a label to the third peak quality slider

  peak4FreqSlider.labels.add({0.f, "20Hz"});  // add a label to the fourth peak frequency slider
  peak4FreqSlider.labels.add({1.f, "20kHz"}); // add a label to the fourth peak frequency slider

  peak4GainSlider.labels.add({0.f, "-24dB"}); // add a label to the fourth peak gain slider
  peak4GainSlider.labels.add({1.f, "24dB"});  // add a label to the fourth peak gain slider

  peak4QualitySlider.labels.add({0.f, "0.1"});  // add a label to the fourth peak quality slider
  peak4QualitySlider.labels.add({1.f, "10.0"}); // add a label to the fourth peak quality slider

  lowShelfFreqSlider.labels.add({0.f, "20Hz"});  // add a label to the low shelf frequency slider
  lowShelfFreqSlider.labels.add({1.f, "20kHz"}); // add a label to the low shelf frequency slider

  lowShelfGainSlider.labels.add({0.f, "-24dB"}); // add a label to the low shelf gain slider
  lowShelfGainSlider.labels.add({1.f, "24dB"});  // add a label to the low shelf gain slider

  lowShelfQSlider.labels.add({0.f, "0.1"});  // add a label to the low shelf quality slider
  lowShelfQSlider.labels.add({1.f, "10.0"}); // add a label to the low shelf quality slider

  highShelfFreqSlider.labels.add({0.f, "20Hz"});  // add a label to the high shelf frequency slider
  highShelfFreqSlider.labels.add({1.f, "20kHz"}); // add a label to the high shelf frequency slider

  highShelfGainSlider.labels.add({0.f, "-24dB"}); // add a label to the high shelf gain slider
  highShelfGainSlider.labels.add({1.f, "24dB"});  // add a label to the high shelf gain slider

  highShelfQSlider.labels.add({0.f, "0.1"});  // add a label to the high shelf quality slider
  highShelfQSlider.labels.add({1.f, "10.0"}); // add a label to the high shelf quality slider

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
  peak2BypassButton.setLookAndFeel(&lnf);     // set the look and feel of the second peak bypass button
  peak3BypassButton.setLookAndFeel(&lnf);     // set the look and feel of the third peak bypass button
  peak4BypassButton.setLookAndFeel(&lnf);     // set the look and feel of the fourth peak bypass button
  lowShelfBypassButton.setLookAndFeel(&lnf);  // set the look and feel of the low shelf bypass button
  highShelfBypassButton.setLookAndFeel(&lnf); // set the look and feel of the high shelf bypass button
  lowCutBypassButton.setLookAndFeel(&lnf);    // set the look and feel of the low cut bypass button
  highCutBypassButton.setLookAndFeel(&lnf);   // set the look and feel of the high cut bypass button
  analyzerEnabledButton.setLookAndFeel(&lnf); // set the look and feel of the analyzer enabled button

  auto safePtr = juce::Component::SafePointer<JBEqualizerAudioProcessorEditor>(this);
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

  peak2BypassButton.onClick = [safePtr]()
  {
    if (auto *comp = safePtr.getComponent())
    {
      auto bypassed = comp->peak2BypassButton.getToggleState();

      comp->peak2FreqSlider.setEnabled(!bypassed);
      comp->peak2GainSlider.setEnabled(!bypassed);
      comp->peak2QualitySlider.setEnabled(!bypassed);
    }
  };

  peak3BypassButton.onClick = [safePtr]()
  {
    if (auto *comp = safePtr.getComponent())
    {
      auto bypassed = comp->peak3BypassButton.getToggleState();

      comp->peak3FreqSlider.setEnabled(!bypassed);
      comp->peak3GainSlider.setEnabled(!bypassed);
      comp->peak3QualitySlider.setEnabled(!bypassed);
    }
  };

  peak4BypassButton.onClick = [safePtr]()
  {
    if (auto *comp = safePtr.getComponent())
    {
      auto bypassed = comp->peak4BypassButton.getToggleState();

      comp->peak4FreqSlider.setEnabled(!bypassed);
      comp->peak4GainSlider.setEnabled(!bypassed);
      comp->peak4QualitySlider.setEnabled(!bypassed);
    }
  };

  lowShelfBypassButton.onClick = [safePtr]()
  {
    if (auto *comp = safePtr.getComponent())
    {
      auto bypassed = comp->lowShelfBypassButton.getToggleState();

      comp->lowShelfFreqSlider.setEnabled(!bypassed);
      comp->lowShelfGainSlider.setEnabled(!bypassed);
      comp->lowShelfQSlider.setEnabled(!bypassed);
    }
  };

  highShelfBypassButton.onClick = [safePtr]()
  {
    if (auto *comp = safePtr.getComponent())
    {
      auto bypassed = comp->highShelfBypassButton.getToggleState();

      comp->highShelfFreqSlider.setEnabled(!bypassed);
      comp->highShelfGainSlider.setEnabled(!bypassed);
      comp->highShelfQSlider.setEnabled(!bypassed);
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

  addAndMakeVisible(presetPanel); // add the preset panel to the editor

  setResizable(true, true); // make the editor resizable

  setSize(900, 600); // size of the editor window
}

JBEqualizerAudioProcessorEditor::~JBEqualizerAudioProcessorEditor()
{
  // This will be deleted by the compiler automatically, but you can do it here
  // if you want to be explicit about it.
  peakBypassButton.setLookAndFeel(nullptr);      // set the look and feel of the peak bypass button to nullptr
  peak2BypassButton.setLookAndFeel(nullptr);     // set the look and feel of the second peak bypass button to nullptr
  peak3BypassButton.setLookAndFeel(nullptr);     // set the look and feel of the third peak bypass button to nullptr
  peak4BypassButton.setLookAndFeel(nullptr);     // set the look and feel of the fourth peak bypass button to nullptr
  lowShelfBypassButton.setLookAndFeel(nullptr);  // set the look and feel of the low shelf bypass button to nullptr
  highShelfBypassButton.setLookAndFeel(nullptr); // set the look and feel of the high shelf bypass button to nullptr
  lowCutBypassButton.setLookAndFeel(nullptr);    // set the look and feel of the low cut bypass button to nullptr
  highCutBypassButton.setLookAndFeel(nullptr);   // set the look and feel of the high cut bypass button to nullptr
  analyzerEnabledButton.setLookAndFeel(nullptr); // set the look and feel of the analyzer enabled button to nullptr
}

//==============================================================================
void JBEqualizerAudioProcessorEditor::paint(juce::Graphics &g)
{
  using namespace juce;

  g.fillAll(Colours::black); // fill the background with black

  g.setColour(Colours::white);
  g.setFont(14);
  g.drawFittedText("Low Pass", lowCutSlopeSlider.getBounds(), juce::Justification::centredBottom, 1);
  g.drawFittedText("Low Shelf", lowShelfQSlider.getBounds(), juce::Justification::centredBottom, 1);
  g.drawFittedText("Low Peak", peakQualitySlider.getBounds(), juce::Justification::centredBottom, 1);
  g.drawFittedText("Mid-Low Peak", peak2QualitySlider.getBounds(), juce::Justification::centredBottom, 1);
  g.drawFittedText("Mid-High Peak", peak3QualitySlider.getBounds(), juce::Justification::centredBottom, 1);
  g.drawFittedText("High Peak", peak4QualitySlider.getBounds(), juce::Justification::centredBottom, 1);
  g.drawFittedText("High Shelf", highShelfQSlider.getBounds(), juce::Justification::centredBottom, 1);
  g.drawFittedText("High Pass", highCutSlopeSlider.getBounds(), juce::Justification::centredBottom, 1);
}

void JBEqualizerAudioProcessorEditor::resized()
{
  auto bounds = getLocalBounds();
  presetPanel.setBounds(bounds.removeFromTop(proportionOfHeight(0.1f)));
  bounds.removeFromTop(4); // top spacing

  // Analyzer enabled controls at the top
  auto analyzerEnabledArea = bounds.removeFromTop(25);
  analyzerEnabledArea.setWidth(50);
  analyzerEnabledArea.setX(5);
  analyzerEnabledArea.removeFromTop(2);
  analyzerEnabledButton.setBounds(analyzerEnabledArea);

  // Allocate space for the response curve
  float hRatio = 25.f / 100.f;
  auto responseArea = bounds.removeFromTop(bounds.getHeight() * hRatio);
  responseCurveComponent.setBounds(responseArea);
  bounds.removeFromTop(5); // spacing

  // For the lower area, we split into three vertical sections:
  // - A left section for the low cut (remains unchanged)
  // - A right section for the high cut (remains unchanged)
  // - The middle section will now be split into three parts:
  //      [low shelf] [4 peak groups] [high shelf]
  auto areaForFilters = bounds;
  int totalWidth = areaForFilters.getWidth();

  // Calculate fixed widths for the low cut and high cut sections (25% each of the original filter area)
  int sideWidth = static_cast<int>(totalWidth * 0.25f);
  auto lowCutAreaOriginal = areaForFilters.removeFromLeft(sideWidth);
  auto highCutAreaOriginal = areaForFilters.removeFromRight(sideWidth);

  // The remaining area (for peaks and shelves)
  auto middleArea = areaForFilters;
  int middleWidth = middleArea.getWidth();
  // Allocate 15% of the middle area to each shelf:
  int shelfWidth = static_cast<int>(middleWidth * 0.15f);
  // The peak group area will be the remaining width:
  //int peakAreaWidth = middleWidth - (2 * shelfWidth);

  // Remove the shelf areas from the middle area:
  auto lowShelfArea = middleArea.removeFromLeft(shelfWidth);
  auto highShelfArea = middleArea.removeFromRight(shelfWidth);
  auto peakGroupArea = middleArea; // whatever remains is for the 4 peak groups

  // Layout low-cut section (unchanged)
  {
    auto lowCutArea = lowCutAreaOriginal;
    lowCutBypassButton.setBounds(lowCutArea.removeFromTop(25));
    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5));
    lowCutSlopeSlider.setBounds(lowCutArea);
  }

  // Layout high-cut section (unchanged)
  {
    auto highCutArea = highCutAreaOriginal;
    highCutBypassButton.setBounds(highCutArea.removeFromTop(25));
    highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    highCutSlopeSlider.setBounds(highCutArea);
  }

  // Layout low shelf section (new, between low cut and peak group)
  {
    lowShelfBypassButton.setBounds(lowShelfArea.removeFromTop(25));
    lowShelfFreqSlider.setBounds(lowShelfArea.removeFromTop(lowShelfArea.getHeight() * 0.33));
    lowShelfGainSlider.setBounds(lowShelfArea.removeFromTop(lowShelfArea.getHeight() * 0.5));
    lowShelfQSlider.setBounds(lowShelfArea);
  }

  // Layout high shelf section (new, between peak group and high cut)
  {
    highShelfBypassButton.setBounds(highShelfArea.removeFromTop(25));
    highShelfFreqSlider.setBounds(highShelfArea.removeFromTop(highShelfArea.getHeight() * 0.33));
    highShelfGainSlider.setBounds(highShelfArea.removeFromTop(highShelfArea.getHeight() * 0.5));
    highShelfQSlider.setBounds(highShelfArea);
  }

  // Layout peak group (center) section (4 peaks)
  {
    const int numPeaks = 4;
    const int gap = 5; // gap between peak groups
    const int totalGap = (numPeaks - 1) * gap;
    int sectionWidth = (peakGroupArea.getWidth() - totalGap) / numPeaks;

    auto peak1Area = peakGroupArea.removeFromLeft(sectionWidth);
    peakGroupArea.removeFromLeft(gap);
    auto peak2Area = peakGroupArea.removeFromLeft(sectionWidth);
    peakGroupArea.removeFromLeft(gap);
    auto peak3Area = peakGroupArea.removeFromLeft(sectionWidth);
    peakGroupArea.removeFromLeft(gap);
    auto peak4Area = peakGroupArea; // remaining area

    // Peak Group 1
    peakBypassButton.setBounds(peak1Area.removeFromTop(25));
    auto remaining1 = peak1Area;
    peakFreqSlider.setBounds(remaining1.removeFromTop(remaining1.getHeight() * 0.33));
    peakGainSlider.setBounds(remaining1.removeFromTop(remaining1.getHeight() * 0.5));
    peakQualitySlider.setBounds(remaining1);

    // Peak Group 2
    peak2BypassButton.setBounds(peak2Area.removeFromTop(25));
    auto remaining2 = peak2Area;
    peak2FreqSlider.setBounds(remaining2.removeFromTop(remaining2.getHeight() * 0.33));
    peak2GainSlider.setBounds(remaining2.removeFromTop(remaining2.getHeight() * 0.5));
    peak2QualitySlider.setBounds(remaining2);

    // Peak Group 3
    peak3BypassButton.setBounds(peak3Area.removeFromTop(25));
    auto remaining3 = peak3Area;
    peak3FreqSlider.setBounds(remaining3.removeFromTop(remaining3.getHeight() * 0.33));
    peak3GainSlider.setBounds(remaining3.removeFromTop(remaining3.getHeight() * 0.5));
    peak3QualitySlider.setBounds(remaining3);

    // Peak Group 4
    peak4BypassButton.setBounds(peak4Area.removeFromTop(25));
    auto remaining4 = peak4Area;
    peak4FreqSlider.setBounds(remaining4.removeFromTop(remaining4.getHeight() * 0.33));
    peak4GainSlider.setBounds(remaining4.removeFromTop(remaining4.getHeight() * 0.5));
    peak4QualitySlider.setBounds(remaining4);
  }
}

std::vector<juce::Component *> JBEqualizerAudioProcessorEditor::getComps()
{
  return {
      &peakFreqSlider,
      &peakGainSlider,
      &peakQualitySlider,
      &peak2FreqSlider,
      &peak2GainSlider,
      &peak2QualitySlider,
      &peak3FreqSlider,
      &peak3GainSlider,
      &peak3QualitySlider,
      &peak4FreqSlider,
      &peak4GainSlider,
      &peak4QualitySlider,
      &lowShelfFreqSlider,
      &lowShelfGainSlider,
      &lowShelfQSlider,
      &highShelfFreqSlider,
      &highShelfGainSlider,
      &highShelfQSlider,
      &lowCutFreqSlider,
      &highCutFreqSlider,
      &lowCutSlopeSlider,
      &highCutSlopeSlider,
      &responseCurveComponent,
      &lowCutBypassButton,
      &peakBypassButton,
      &peak2BypassButton,
      &peak3BypassButton,
      &peak4BypassButton,
      &lowShelfBypassButton,
      &highShelfBypassButton,
      &highCutBypassButton,
      &analyzerEnabledButton

  };
}
