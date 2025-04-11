#include "../GUI/FFTComponents.h"

namespace GUI
{
    void PathProducer::process(juce::Rectangle<float> fftBounds, double sampleRate)
    {
        // while there are buffers to pull, if we can pull a buffer, send to FFT data generator
        juce::AudioBuffer<float> tempIncomingBuffer;

        while (leftChannelFifo->getNumCompleteBuffersAvailable() > 0)
        {
            if (leftChannelFifo->getAudioBuffer(tempIncomingBuffer))
            {
                auto size = tempIncomingBuffer.getNumSamples(); // get the size of the buffer

                // shift data
                juce::FloatVectorOperations::copy(
                    monoBuffer.getWritePointer(0, 0),   // copy to 0 index
                    monoBuffer.getReadPointer(0, size), // read from 1 index
                    monoBuffer.getNumSamples() - size); // get the number of samples in that buffer

                // copy data
                juce::FloatVectorOperations::copy(
                    monoBuffer.getWritePointer(0, monoBuffer.getNumSamples() - size), // copy to the end of the buffer
                    tempIncomingBuffer.getReadPointer(0, 0),                          // read from the incoming buffer
                    size);                                                            // get the size of the buffer

                // send buffers to generator
                leftChannelFFTDataGenerator.produceFFTDataForRendering(
                    monoBuffer, // send the mono buffer to the generator
                    -48.f);     // set the negative infinity value to -48 dB
            }
        }

        // while there are FFT data buffers to pull, if we can pull a buffer,generate the path
        const auto fftSize = leftChannelFFTDataGenerator.getFFTSize(); // get the size of the FFT data generator
        const auto binWidth = sampleRate / (double)fftSize;            // get the bin width of the FFT data generator

        while (leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0)
        {
            std::vector<float> fftData; // create a vector for the FFT data
            if (leftChannelFFTDataGenerator.getFFTData(fftData))
            {
                pathProducer.generatePath(fftData, fftBounds, fftSize, binWidth, -48.f); // generate the path for the FFT data
            }
        }
        // while there are paths to pull, pull as many as we can, display the most recent path
        while (pathProducer.getNumPathsAvailable() > 0)
        {
            pathProducer.getPath(leftChannelFFTPath); // get the path for the left channel FFT data
        }
    }
}
