/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

enum FFTOrder
{
  order2048 = 11,
  order4096 = 12,
  order8192 = 13
};

template <typename BlockType>
struct FFTDataGenerator
{
  // produces the FFT data from an audio buffer
  void produceFFTDataForRendering(const juce::AudioBuffer<float> &audioData, const float negativeInfinity)
  {
    const auto fftSize = getFFTSize(); // get the size of the FFT

    fftData.assign(fftData.size(), 0);
    auto *readIndex = audioData.getReadPointer(0);              // get the read pointer for the audio data
    std::copy(readIndex, readIndex + fftSize, fftData.begin()); // copy the audio data to the FFT data

    // first apply a windowing function to our data
    window->multiplyWithWindowingTable(fftData.data(), fftSize); // apply the windowing function to the FFT data [1]

    // then render our FFT data
    forwardFFT->performFrequencyOnlyForwardTransform(fftData.data()); // perform the FFT on the data [2]

    int numBins = (int)fftSize / 2; // number of bins in the FFT

    // normalize the FFT data
    for (int i = 0; i < numBins; ++i)
    {
      fftData[i] /= (float)numBins;
    }

    // convert the FFT data to decibels
    for (int i = 0; i < numBins; ++i)
    {
      fftData[i] = juce::Decibels::gainToDecibels(fftData[i], negativeInfinity);
    }

    fftDataFifo.push(fftData); // push the FFT data to the FIFO buffer
  }

  void changeOrder(FFTOrder newOrder)
  {
    // when you change order, recreate the window, forwardFFT, fftData
    // also reset the fifoIndex
    // things that need recreating should be created on the heap via std::make_unique<>

    order = newOrder;            // set the new order
    auto fftSize = getFFTSize(); // get the size of the FFT

    forwardFFT = std::make_unique<juce::dsp::FFT>(order);                                                                         // create the FFT object
    window = std::make_unique<juce::dsp::WindowingFunction<float>>(fftSize, juce::dsp::WindowingFunction<float>::blackmanHarris); // create the windowing function

    fftData.clear();                // clear the FFT data
    fftData.resize(fftSize * 2, 0); // resize the FFT data to the size of the FFT

    fftDataFifo.prepare(fftData.size()); // prepare the FIFO buffer for the FFT data
  }
  int getFFTSize() const { return 1 << order; }                                                // get the size of the FFT
  int getNumAvailableFFTDataBlocks() const { return fftDataFifo.getNumAvailableForReading(); } // get the number of available FFT data blocks
  bool getFFTData(BlockType &fftData) { return fftDataFifo.pull(fftData); }                    // get the FFT data from the FIFO buffer
private:
  FFTOrder order;                                              // order of the FFT
  BlockType fftData;                                           // FFT data
  std::unique_ptr<juce::dsp::FFT> forwardFFT;                  // FFT object
  std::unique_ptr<juce::dsp::WindowingFunction<float>> window; // windowing function

  Fifo<BlockType> fftDataFifo; // FIFO buffer for the FFT data
};

template <typename PathType>
struct AnalyzerPathGenerator
{
  // converts 'renderData[]' into a juce::Path
  void generatePath(const std::vector<float> &renderData,
                    juce::Rectangle<float> fftBounds,
                    int fftSize,
                    float binWidth,
                    float negativeInfinity)
  {
    auto top = fftBounds.getY();
    auto bottom = fftBounds.getBottom();
    auto width = fftBounds.getWidth();

    int numBins = (int)fftSize / 2; // number of bins in the FFT

    // create a path for the FFT data
    PathType p;
    p.preallocateSpace(3 * (int)fftBounds.getWidth()); // preallocate space for the path

    auto map = [bottom, top, negativeInfinity](float input)
    {
      // map the input value to the output range
      return juce::jmap(input, negativeInfinity, 0.f, (float)bottom, top);
    };

    auto y = map(renderData[0]); // map the first value of the FFT data to the y position

    jassert(!std::isnan(y) && !std::isinf(y)); // check if the value is valid

    p.startNewSubPath(0, y); // start the path at the first point

    const int pathResolution = 2; // you can draw line-to's every 'pathResolution' pixels

    for (int binNum = 1; binNum < numBins; binNum += pathResolution)
    {
      y = map(renderData[binNum]); // map the FFT data to the y position

      jassert(!std::isnan(y) && !std::isinf(y)); // check if the value is valid

      if (!std::isnan(y) && !std::isinf(y))
      {
        auto binFreq = binNum * binWidth;                                // get the frequency of the bin
        auto normalizedBinX = juce::mapFromLog10(binFreq, 1.f, 20000.f); // map the frequency to a value between 0 and 1
        int binX = std::floor(normalizedBinX * width);                   // get the x position of the bin
        p.lineTo(binX, y);                                               // add a line to the path for the bin
      }
    }
    pathFifo.push(p); // push the path to the FIFO buffer
  }

  int getNumPathsAvailable() const { return pathFifo.getNumAvailableForReading(); } // get the number of available paths
  bool getPath(PathType &path) { return pathFifo.pull(path); }                      // get the path from the FIFO buffer
private:
  Fifo<PathType> pathFifo; // FIFO buffer for the path
};

struct LookAndFeel : juce::LookAndFeel_V4
{
  void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                        float sliderPosProportional, float rotaryStartAngle,
                        float rotaryEndAngle, juce::Slider &slider) override;
};

struct RotarySliderWithLabels : juce::Slider
{
  RotarySliderWithLabels(juce::RangedAudioParameter &rap, const juce::String &unitSuffix) : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                                                                                         juce::Slider::TextEntryBoxPosition::NoTextBox),
                                                                                            param(&rap), suffix(unitSuffix)
  {
    setLookAndFeel(&lnf); // set the custom look and feel for the slider
  }

  ~RotarySliderWithLabels()
  {
    setLookAndFeel(nullptr); // reset the look and feel to the default
  }

  struct LabelPos
  {
    float pos;          // position of the label
    juce::String label; // label text
  };

  juce::Array<LabelPos> labels; // array of labels for the slider

  void paint(juce::Graphics &g) override;
  juce::Rectangle<int> getSliderBounds() const;
  int getTextHeight() const { return 14; } // height of the text label
  juce::String getDisplayString() const;

private:
  LookAndFeel lnf;                   // custom look and feel for the slider
  juce::RangedAudioParameter *param; // pointer to the parameter that the slider is controlling
  juce::String suffix;               // suffix for the parameter value
};

struct ResponseCurveComponent : public juce::Component, juce::AudioProcessorParameter::Listener, juce::Timer
{
  ResponseCurveComponent(SimpleEQAudioProcessor &);
  ~ResponseCurveComponent();
  void parameterValueChanged(int parameterIndex, float newValue) override;
  void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {};
  void timerCallback() override; // query atomic flag to decide if the chain needs updating and our components need repainting
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  SimpleEQAudioProcessor &audioProcessor;
  juce::Atomic<bool> parametersChanged{false}; // atomic flag to indicate if the parameters have changed
  MonoChain monoChain;                         // chain to hold the filter coefficients

  void updateChain(); // update the chain with the new parameters

  juce::Image background; // background image for the response curve component

  juce::Rectangle<int> getRenderArea(); // get the render area for the response curve component

  juce::Rectangle<int> getAnalysisArea(); // get the analysis area for the response curve component

  SingleChannelSampleFifo<SimpleEQAudioProcessor::BlockType> *leftChannelFifo;

  juce::AudioBuffer<float> monoBuffer;

  FFTDataGenerator<std::vector<float>> leftChannelFFTDataGenerator; // FFT data generator for the left channel

  AnalyzerPathGenerator<juce::Path> pathProducer; // path generator for the analyzer

  juce::Path leftChannelFFTPath; // path for the left channel FFT data
};

//==============================================================================
/**
 */
class SimpleEQAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
  SimpleEQAudioProcessorEditor(SimpleEQAudioProcessor &);
  ~SimpleEQAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  SimpleEQAudioProcessor &audioProcessor;

  RotarySliderWithLabels peakFreqSlider,
      peakGainSlider,
      peakQualitySlider,
      lowCutFreqSlider,
      highCutFreqSlider,
      lowCutSlopeSlider,
      highCutSlopeSlider;

  // typename alias to help with readability of the function
  using APVTS = juce::AudioProcessorValueTreeState;
  using Attachment = APVTS::SliderAttachment;

  // declare an attachment for each slider
  Attachment peakFreqSliderAttachment,
      peakGainSliderAttachment,
      peakQualitySliderAttachment,
      lowCutFreqSliderAttachment,
      highCutFreqSliderAttachment,
      lowCutSlopeSliderAttachment,
      highCutSlopeSliderAttachment;

  ResponseCurveComponent responseCurveComponent; // component to show the frequency response of the EQ

  std::vector<juce::Component *> getComps();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQAudioProcessorEditor)
};
