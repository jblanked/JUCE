/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void LookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                                   float sliderPosProportional, float rotaryStartAngle,
                                   float rotaryEndAngle, juce::Slider &slider)
{
  using namespace juce;

  auto bounds = Rectangle<float>(x, y, width, height); // create a rectangle for the slider
  g.setColour(Colours::darkgrey);                      // set the colour to dark grey
  g.fillEllipse(bounds);                               // fill the ellipse with the colour

  g.setColour(Colours::lightblue); // set the colour to light blue
  g.drawEllipse(bounds, 1.f);      // draw the ellipse with a width of 1 pixel

  if (auto *rswl = dynamic_cast<RotarySliderWithLabels *>(&slider))
  {
    auto center = bounds.getCentre(); // get the center of the ellipse

    Path p;                                                   // create a path for the slider knob
    Rectangle<float> r;                                       // create a rectangle for the slider knob
    r.setLeft(center.getX() - 2);                             // set the left edge of the rectangle to the center of the ellipse minus 2 pixels
    r.setRight(center.getX() + 2);                            // set the right edge of the rectangle to the center of the ellipse plus 2 pixels
    r.setTop(bounds.getY());                                  // set the top edge of the rectangle to the top of the ellipse
    r.setBottom(center.getY() - rswl->getTextHeight() * 1.5); // set the bottom edge of the rectangle to the center of the ellipse

    p.addRoundedRectangle(r, 2.f);              // add a rounded rectangle to the path for the slider knob
    jassert(rotaryStartAngle < rotaryEndAngle); // check that the start angle is less than the end angle

    auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle); // map the slider position to the angle of the knob

    p.applyTransform(AffineTransform::rotation(sliderAngRad, center.getX(), center.getY())); // rotate the path to the angle of the knob

    g.fillPath(p); // fill the path with the colour

    g.setFont(rswl->getTextHeight());                        // set the font size to the height of the text label
    auto text = rswl->getDisplayString();                    // get the text label for the slider
    auto strWidth = g.getCurrentFont().getStringWidth(text); // get the width of the text label

    r.setSize(strWidth + 4, rswl->getTextHeight() + 2); // set the size of the rectangle to the width and height of the text label
    r.setCentre(bounds.getCentre());

    g.setColour(Colours::black); // set the colour to black
    g.fillRect(r);               // fill the rectangle with the colour

    g.setColour(Colours::white);                                         // set the colour to white
    g.drawFittedText(text, r.toNearestInt(), Justification::centred, 1); // draw the text label in the rectangle
  }
}

void RotarySliderWithLabels::paint(juce::Graphics &g)
{
  using namespace juce;

  auto startAng = degreesToRadians(180.f + 45.f);                              // start angle of the rotary slider
  auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<double>::twoPi; // end angle of the rotary slider

  auto range = getRange(); // get the range of the slider

  auto sliderBounds = getSliderBounds(); // get the bounds of the slider

  // g.setColour(Colours::lightblue); // set the colour to light blue
  // g.drawRect(getLocalBounds());    // draw the rectangle around the slider
  // g.setColour(Colours::yellow);    // set the colour to yellow
  // g.drawRect(sliderBounds);        // draw the rectangle around the slider bounds

  getLookAndFeel().drawRotarySlider(g, sliderBounds.getX(), sliderBounds.getY(), sliderBounds.getWidth(), sliderBounds.getHeight(),
                                    jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0), startAng, endAng, *this); // draw the rotary slider

  auto center = sliderBounds.toFloat().getCentre(); // get the center of the slider bounds
  auto radius = sliderBounds.getWidth() * 0.5f;     // get the radius of the slider

  g.setColour(Colours::lightblue); // set the colour to light blue
  g.setFont(getTextHeight());      // set the font size to the height of the text label

  auto numChoices = labels.size();     // get the number of labels for the slider
  for (int i = 0; i < numChoices; ++i) // iterate through the labels
  {
    auto pos = labels[i].pos;          // get the position of the label
    jassert(0.f <= pos && pos <= 1.f); // check that the position is between 0 and 1

    auto ang = jmap(pos, 0.f, 1.f, static_cast<float>(startAng), static_cast<float>(endAng)); // map the position to the angle of the label

    auto c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f + 1, ang); // get the point on the circumference of the circle for the label

    Rectangle<float> r;                                                 // create a rectangle for the label
    auto str = labels[i].label;                                         // get the label text
    r.setSize(g.getCurrentFont().getStringWidth(str), getTextHeight()); // set the size of the rectangle to the width and height of the text label
    r.setCentre(c);                                                     // set the center of the rectangle to the point on the circumference
    r.setY(r.getY() + getTextHeight());                                 // set the y position of the rectangle to the height of the text label

    g.drawFittedText(str, r.toNearestInt(), Justification::centred, 1); // draw the text label in the rectangle
  }
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
  auto bounds = getLocalBounds();                                // get the bounds of the slider
  auto size = juce::jmin(bounds.getWidth(), bounds.getHeight()); // get the minimum size of the slider
  size -= getTextHeight() * 2;                                   // subtract the height of the text label from the size
  juce::Rectangle<int> r;                                        // create a rectangle for the slider bounds
  r.setSize(size, size);                                         // set the size of the rectangle to the size of the slider
  r.setCentre(bounds.getCentreX(), 0);                           // set the center of the rectangle to the center of the slider
  r.setY(2);                                                     // set the y position of the rectangle to 2 pixels from the top of the slider
  return r;                                                      // return the rectangle
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
  if (auto choiceParam = dynamic_cast<juce::AudioParameterChoice *>(param))
  {
    return choiceParam->getCurrentChoiceName(); // get the current choice name for the parameter
  }

  juce::String str;  // create a string for the parameter value
  bool addK = false; // flag to indicate if the suffix should be added

  if (auto *floatParam = dynamic_cast<juce::AudioParameterFloat *>(param))
  {
    float val = getValue(); // get the value of the parameter

    if (val > 999.f)
    {
      val /= 1000.f; // divide the value by 1000 if it is greater than 999
      addK = true;   // set the flag to true to indicate that the suffix should be added
    }

    str = juce::String(val, (addK ? 2 : 0)); // convert the value to a string with 2 decimal places if the suffix is added, otherwise 0 decimal places
  }
  else
  {
    jassertfalse; // this shouldnt happen
  }

  if (suffix.isNotEmpty())
  {
    str << " "; // add a space to the string
    if (addK)
      str << "k"; // add the suffix "k" if the flag is set

    str << suffix; // add the suffix to the string
  }
  return str; // return the string
}

ResponseCurveComponent::ResponseCurveComponent(SimpleEQAudioProcessor &p) : audioProcessor(p)
{
  // listen for parameter changes
  const auto &params = audioProcessor.getParameters(); // get the parameters from the audio processor
  for (auto param : params)
  {
    param->addListener(this); // add the editor as a listener to each parameter
  }
  updateChain(); // update the chain with the initial parameters

  startTimerHz(60); // start the timer at 60 hertz refresh rate
}

void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue)
{
  parametersChanged.set(true); // set the atomic flag to true to indicate that the parameters have changed
}

void ResponseCurveComponent::timerCallback()
{
  // check if the parameters have changed
  if (parametersChanged.compareAndSetBool(false, true))
  {
    updateChain(); // update the chain with the new parameters
    repaint();     // repaint the editor window to show the new frequency response
  }
}

void ResponseCurveComponent::updateChain()
{
  // update the chain with the new parameters
  auto chainSettings = getChainSettings(audioProcessor.apvts);                              // get the chain settings from the apvts
  auto peakCoefficients = makePeakFilter(chainSettings, audioProcessor.getSampleRate());    // create the peak filter coefficients
  updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients); // set the coefficients for the peak filter

  auto lowCutCoefficients = makeLowCutFilter(chainSettings, audioProcessor.getSampleRate());   // create the low cut filter coefficients
  auto highCutCoefficients = makeHighCutFilter(chainSettings, audioProcessor.getSampleRate()); // create the high cut filter coefficients

  updateCutFilter(monoChain.get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCutSlope);    // set the coefficients for the low cut filter
  updateCutFilter(monoChain.get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCutSlope); // set the coefficients for the high cut filter
}

void ResponseCurveComponent::paint(juce::Graphics &g)
{
  using namespace juce;

  g.fillAll(Colours::black); // fill the background with black

  g.drawImage(background, getLocalBounds().toFloat()); // draw the background image

  // get dimensions
  auto responseArea = getAnalysisArea(); // get the render area for the response curve component
  auto w = responseArea.getWidth();      // get the width of the response area

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

  g.setColour(Colours::orange);                                // set the colour to orange
  g.drawRoundedRectangle(getRenderArea().toFloat(), 4.f, 1.f); // draw a rounded rectangle around the response area

  g.setColour(Colours::white);                      // set the colour to white
  g.strokePath(responseCurve, PathStrokeType(2.f)); // stroke the path with a width of 2 pixels
}

void ResponseCurveComponent::resized()
{
  // make new backgroubnd image based on width/height of component
  using namespace juce;
  background = Image(Image::PixelFormat::ARGB, getWidth(), getHeight(), true); // create a new image for the background

  Graphics g(background); // create a graphics context for the image

  // draw frequency lines
  Array<float> freqs{
      20, 50, 100,
      200, 500, 1000,
      2000, 5000, 10000,
      20000};

  auto renderArea = getRenderArea();    // get the render area for the response curve component
  auto left = renderArea.getX();        // get the left edge of the render area
  auto right = renderArea.getRight();   // get the right edge of the render area
  auto top = renderArea.getY();         // get the top edge of the render area
  auto bottom = renderArea.getBottom(); // get the bottom edge of the render area
  auto width = renderArea.getWidth();   // get the width of the render area

  Array<float> xs;        // array of x positions for the frequency lines
  for (auto freq : freqs) // iterate through the frequencies
  {
    auto normX = mapFromLog10(freq, 20.f, 20000.f); // map the frequency to a value between 0 and 1
    xs.add(left + width * normX);                   // add the x position to the array
  }

  g.setColour(Colours::dimgrey); // set the colour to dimgrey
  for (auto x : xs)              // iterate through the x positions
  {
    g.drawVerticalLine(x, top, bottom); // draw a vertical line at the x position
  }

  // draw gain lines
  Array<float> gains{
      -24, -12, 0, 12, 24}; // array of gain values

  for (auto gDb : gains) // iterate through the gain values
  {
    auto y = jmap(gDb, -24.f, 24.f, (float)bottom, (float)top);      // map the gain value to a y position
    g.setColour(gDb == 0.f ? Colours::lightblue : Colours::dimgrey); // set the colour to lightblue if the gain value is 0, otherwise dimgrey
    g.drawHorizontalLine(y, left, right);                            // draw a horizontal line at the y position
  }

  g.setColour(Colours::lightgrey); // set the colour to light grey
  const int fontHeight = 10;       // height of the font
  g.setFont(fontHeight);           // set the font size to the height of the font

  // loop through xs and draw the frequency labels
  for (int i = 0; i < xs.size(); ++i)
  {
    auto x = xs[i];       // get the x position
    auto freq = freqs[i]; // get the frequency

    bool addK = false; // flag to indicate if the suffix should be added
    String str;
    if (freq > 999.f) // check if the frequency is greater than 999
    {
      freq /= 1000.f; // divide the frequency by 1000
      addK = true;    // set the flag to true to indicate that the suffix should be added
    }
    str << freq; // add the frequency to the string
    if (addK)
      str << "k"; // add the suffix "k" if the flag is set
    str << "Hz";  // add the suffix "Hz" to the string

    auto strWidth = g.getCurrentFont().getStringWidth(str); // get the width of the string
    Rectangle<int> r;                                       // create a rectangle for the string
    r.setSize(strWidth, fontHeight);                        // set the size of the rectangle to the width and height of the string
    r.setCentre(x, 0);                                      // set the center of the rectangle to the x position and 0 y position
    r.setY(1);                                              // set the y position of the rectangle to 1 pixel from the top of the response area

    g.drawFittedText(str, r, Justification::centred, 1); // draw the string in the rectangle
  }

  // loop through gains and draw the gain labels
  for (auto gDb : gains) // iterate through the gain values
  {
    auto y = jmap(gDb, -24.f, 24.f, (float)bottom, (float)top); // map the gain value to a y position
    String str;
    if (gDb > 0)  // check if the gain value is greater than 0
      str << "+"; // add a plus sign to the string

    str << gDb; // add the gain value to the string

    auto textWidth = g.getCurrentFont().getStringWidth(str); // get the width of the string

    Rectangle<int> r;                 // create a rectangle for the string
    r.setSize(textWidth, fontHeight); // set the size of the rectangle to the width and height of the string
    r.setX(getWidth() - textWidth);   // set the x position of the rectangle to the right edge of the response area
    r.setCentre(r.getCentreX(), y);   // set the center of the rectangle to the x position and y position

    g.setColour(gDb == 0.f ? Colours::lightblue : Colours::lightgrey); // set the colour to lightblue if the gain value is 0, otherwise lightgrey

    g.drawFittedText(str, r, Justification::centred, 1); // draw the string in the rectangle
  }
}

juce::Rectangle<int> ResponseCurveComponent::getRenderArea()
{
  auto bounds = getLocalBounds(); // get the bounds of the response area
  // bounds.reduce(
  //     10, // JUCE_LIVE_CONSTANT(5),
  //     8   // JUCE_LIVE_CONSTANT(5)
  // );
  bounds.removeFromTop(12);   // remove the top 12 pixels from the bounds for spacing
  bounds.removeFromBottom(2); // remove the bottom 2 pixels from the bounds for spacing
  bounds.removeFromLeft(20);  // remove the left 20 pixels from the bounds for spacing
  bounds.removeFromRight(20); // remove the right 20 pixels from the bounds for spacing
  return bounds;              // return the bounds of the response area
}

juce::Rectangle<int> ResponseCurveComponent::getAnalysisArea()
{
  auto bounds = getRenderArea(); // get the bounds of the response area
  bounds.removeFromTop(4);       // remove the top 4 pixels from the bounds for spacing
  bounds.removeFromBottom(4);    // remove the bottom 4 pixels from the bounds for spacing
  return bounds;                 // return the bounds of the response area
}
ResponseCurveComponent::~ResponseCurveComponent()
{
  // de-register as a listener
  const auto &params = audioProcessor.getParameters(); // get the parameters from the audio processor
  for (auto param : params)
  {
    param->removeListener(this); // remove the editor as a listener to each parameter
  }
}

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
      peakFreqSliderAttachment(audioProcessor.apvts, "peakFrequency", peakFreqSlider),
      peakGainSliderAttachment(audioProcessor.apvts, "peakGain", peakGainSlider),
      peakQualitySliderAttachment(audioProcessor.apvts, "peakQuality", peakQualitySlider),
      lowCutFreqSliderAttachment(audioProcessor.apvts, "lowCutFrequency", lowCutFreqSlider),
      highCutFreqSliderAttachment(audioProcessor.apvts, "highCutFrequency", highCutFreqSlider),
      lowCutSlopeSliderAttachment(audioProcessor.apvts, "lowCutSlope", lowCutSlopeSlider),
      highCutSlopeSliderAttachment(audioProcessor.apvts, "highCutSlope", highCutSlopeSlider)
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

  setSize(600, 480); // size of the editor window
}

SimpleEQAudioProcessorEditor::~SimpleEQAudioProcessorEditor()
{
}

//==============================================================================
void SimpleEQAudioProcessorEditor::paint(juce::Graphics &g)
{
  using namespace juce;

  g.fillAll(Colours::black); // fill the background with black
}

void SimpleEQAudioProcessorEditor::resized()
{
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..

  // first 3rd of the display for showing the frequency response of the EQ
  auto bounds = getLocalBounds();                                        // bounding box of the editor window
  float hRatio = 25.f / 100.f;                                           // JUCE_LIVE_CONSTANT(33) / 100.f;                      // height ratio of the response area
  auto responseArea = bounds.removeFromTop(bounds.getHeight() * hRatio); // remove the top 1/3 of the display for showing the frequency response

  responseCurveComponent.setBounds(responseArea); // set the bounds of the response curve component

  bounds.removeFromTop(5); // remove the top 5 pixels from the bounds for spacing

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
      &responseCurveComponent,
  };
}
