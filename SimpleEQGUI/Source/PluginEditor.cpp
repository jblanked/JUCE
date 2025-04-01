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
      peakFreqSliderAttachment(audioProcessor.apvts, "peakFrequency", peakFreqSlider),
      peakGainSliderAttachment(audioProcessor.apvts, "peakGain", peakGainSlider),
      peakQualitySliderAttachment(audioProcessor.apvts, "peakQuality", peakQualitySlider),
      lowCutFreqSliderAttachment(audioProcessor.apvts, "lowCutFrequency", lowCutFreqSlider),
      highCutFreqSliderAttachment(audioProcessor.apvts, "highCutFrequency", highCutFreqSlider),
      lowCutSlopeSliderAttachment(audioProcessor.apvts, "lowCutSlope", lowCutSlopeSlider),
      highCutSlopeSliderAttachment(audioProcessor.apvts, "highCutSlope", highCutSlopeSlider)
{

  for (auto *comp : getComps())
  {
    addAndMakeVisible(comp); // add the components to the editor
  }

  // listen for parameter changes
  const auto &params = audioProcessor.getParameters(); // get the parameters from the audio processor
  for (auto param : params)
  {
    param->addListener(this); // add the editor as a listener to each parameter
  }

  startTimerHz(60); // start the timer at 60 hertz refresh rate

  setSize(600, 400); // size of the editor window
}

SimpleEQAudioProcessorEditor::~SimpleEQAudioProcessorEditor()
{
  // de-register as a listener
  const auto &params = audioProcessor.getParameters(); // get the parameters from the audio processor
  for (auto param : params)
  {
    param->removeListener(this); // remove the editor as a listener to each parameter
  }
}

//==============================================================================
void SimpleEQAudioProcessorEditor::paint(juce::Graphics &g)
{
  using namespace juce;

  g.fillAll(Colours::black); // fill the background with black

  // get dimensions
  auto bounds = getLocalBounds();                                      // get the bounds of the editor window
  auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33); // remove the top 1/3 of the display for showing the frequency response
  auto w = responseArea.getWidth();                                    // get the width of the response area

  // individual chain elements
  auto &lowcut = monoChain.get<ChainPositions::LowCut>();   // get the low cut filter from the chain
  auto &peak = monoChain.get<ChainPositions::Peak>();       // get the peak filter from the chain
  auto &highcut = monoChain.get<ChainPositions::HighCut>(); // get the high cut filter from the chain

  // get the frequency response of the EQ
  auto sampleRate = audioProcessor.getSampleRate(); // get the sample rate of the audio processor

  std::vector<double> mags; // one magnitude value for each pixel in the response area
  mags.resize(w);           // resize the vector to the width of the response area

  // iterate through each pixel and compute the magnitude of that frequency, which is expressed in gain units
  for (int i = 0; i < w; ++i)
  {
    double mag = 1.f;                                             // starting gain of 1
    auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0); // map the value to a frequency between 20 and 20kHz (human-hearable frequency range)

    // check if band is bypassed
    if (!monoChain.isBypassed<ChainPositions::Peak>())
      mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the peak filter for that frequency

    if (!lowcut.isBypassed<0>())
      mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the low cut filter for that frequency
    if (!lowcut.isBypassed<1>())
      mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the low cut filter for that frequency
    if (!lowcut.isBypassed<2>())
      mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the low cut filter for that frequency
    if (!lowcut.isBypassed<3>())
      mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the low cut filter for that frequency

    if (!highcut.isBypassed<0>())
      mag *= highcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the high cut filter for that frequency
    if (!highcut.isBypassed<1>())
      mag *= highcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the high cut filter for that frequency
    if (!highcut.isBypassed<2>())
      mag *= highcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the high cut filter for that frequency
    if (!highcut.isBypassed<3>())
      mag *= highcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the high cut filter for that frequency

    mags[i] = Decibels::gainToDecibels(mag); // convert the magnitude to decibels and store it in the vector
  }

  // convert vector of magnitudes to a path, then draw it
  Path responseCurve; // path to draw the frequency response curve

  const double outputMin = responseArea.getBottom(); // minimum output value (bottom of the response area)
  const double outputMax = responseArea.getY();      // maximum output value (top of the response area)
  auto map = [outputMin, outputMax](double input)
  {
    // peak control can go from -24 to 24 dB
    return jmap(input, -24.0, 24.0, outputMin, outputMax); // map the input value to the output range
  };

  // new subpath with the first magnitude
  responseCurve.startNewSubPath(responseArea.getX(), map(mags.front())); // start the path at the left edge of the response area

  for (size_t i = 1; i < mags.size(); ++i) // iterate through the magnitudes
  {
    // add a line to the path for each magnitude value
    responseCurve.lineTo(responseArea.getX() + i, map(mags[i])); // add a line to the path for each magnitude value
  }

  g.setColour(Colours::orange);                             // set the colour to orange
  g.drawRoundedRectangle(responseArea.toFloat(), 4.f, 1.f); // draw a rounded rectangle around the response area

  g.setColour(Colours::white);                      // set the colour to white
  g.strokePath(responseCurve, PathStrokeType(2.f)); // stroke the path with a width of 2 pixels
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

void SimpleEQAudioProcessorEditor::parameterValueChanged(int parameterIndex, float newValue)
{
  parametersChanged.set(true); // set the atomic flag to true to indicate that the parameters have changed
}

void SimpleEQAudioProcessorEditor::timerCallback()
{
  // check if the parameters have changed
  if (parametersChanged.compareAndSetBool(false, true))
  {
    // update the chain with the new parameters
    auto chainSettings = getChainSettings(audioProcessor.apvts);                              // get the chain settings from the apvts
    auto peakCoefficients = makePeakFilter(chainSettings, audioProcessor.getSampleRate());    // create the peak filter coefficients
    updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients); // set the coefficients for the peak filter

    repaint(); // repaint the editor window to show the new frequency response
  }
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
