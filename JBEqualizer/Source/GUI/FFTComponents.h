#pragma once

#include <JuceHeader.h>
#include "../DSP/Fifo.h"
#include "../DSP/BasicAudioProcessor.h"

namespace GUI
{

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
                    auto binFreq = binNum * binWidth;                                 // get the frequency of the bin
                    auto normalizedBinX = juce::mapFromLog10(binFreq, 20.f, 20000.f); // map the frequency to a value between 0 and 1
                    int binX = std::floor(normalizedBinX * width);                    // get the x position of the bin
                    p.lineTo(binX, y);                                                // add a line to the path for the bin
                }
            }
            pathFifo.push(p); // push the path to the FIFO buffer
        }

        int getNumPathsAvailable() const { return pathFifo.getNumAvailableForReading(); } // get the number of available paths
        bool getPath(PathType &path) { return pathFifo.pull(path); }                      // get the path from the FIFO buffer
    private:
        Fifo<PathType> pathFifo; // FIFO buffer for the path
    };

    struct PathProducer
    {
        PathProducer(SingleChannelSampleFifo<juce::AudioBuffer<float>> &scsf)
            : leftChannelFifo(&scsf)
        {
            // split audio spectrum from 20Hz to 20kHz into FFTOrder bins, which store the magnitude level of a range of frequencies
            leftChannelFFTDataGenerator.changeOrder(FFTOrder::order2048);    // change the order of the FFT data generator to 2048
            monoBuffer.setSize(1, leftChannelFFTDataGenerator.getFFTSize()); // set the size of the mono buffer to the size of the FFT data generator
        }

        void process(juce::Rectangle<float> fftBounds, double sampleRate);
        juce::Path getPath() const { return leftChannelFFTPath; } // get the path for the left channel FFT data
    private:
        SingleChannelSampleFifo<juce::AudioBuffer<float>> *leftChannelFifo;

        juce::AudioBuffer<float> monoBuffer;

        FFTDataGenerator<std::vector<float>> leftChannelFFTDataGenerator; // FFT data generator for the left channel

        AnalyzerPathGenerator<juce::Path> pathProducer; // path generator for the analyzer

        juce::Path leftChannelFFTPath; // path for the left channel FFT data
    };
}
