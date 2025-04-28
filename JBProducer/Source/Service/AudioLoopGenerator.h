#pragma once
#include <JuceHeader.h>
#include <cmath>

/**
 * Namespace providing utilities for creating and exporting short audio loops.
 */
namespace AudioLoopGenerator
{
    /**
     * Generates an audio loop buffer of a specified waveform and rhythmic envelope.
     *
     * @param frequency           Base frequency (Hz) of the oscillator.
     * @param lengthInSamples     Total length of the loop in samples.
     * @param sampleRate          Sample rate to use (Hz).
     * @param bpm                 Tempo in beats per minute for envelope modulation.
     * @param waveformType        Waveform selector: 0=Sine, 1=Square, 2=Saw, 3=Triangle.
     * @param numChannels         Number of audio channels to generate.
     * @return                    An AudioBuffer<float> containing the multi-channel loop.
     */
    inline juce::AudioBuffer<float> generateAudioLoop(float frequency,
                                                      int lengthInSamples,
                                                      double sampleRate,
                                                      float bpm,
                                                      int waveformType,
                                                      int numChannels)
    {
        // Prepare multi-channel buffer
        juce::AudioBuffer<float> buffer(numChannels, lengthInSamples);
        buffer.clear();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            float *channelData = buffer.getWritePointer(channel);

            for (int sample = 0; sample < lengthInSamples; ++sample)
            {
                double time = sample / sampleRate;
                double phase = std::fmod(time * frequency, 1.0);

                // Waveform generation
                float value = 0.0f;
                switch (waveformType)
                {
                case 1: // Square
                    value = phase < 0.5 ? 1.0f : -1.0f;
                    break;

                case 2: // Saw
                    value = static_cast<float>(2.0 * phase - 1.0);
                    break;

                case 3: // Triangle
                    value = phase < 0.5
                                ? static_cast<float>(4.0 * phase - 1.0)
                                : static_cast<float>(3.0 - 4.0 * phase);
                    break;

                default: // Sine
                    value = std::sin(2.0 * juce::MathConstants<double>::pi * phase);
                    break;
                }

                // Envelope for rhythmic effect
                double beatsPerSecond = bpm / 60.0;
                double beatPosition = std::fmod(time * beatsPerSecond, 1.0);
                float envelope = 0.7f + 0.3f * std::exp(-beatPosition * 4.0);

                channelData[sample] = value * 0.5f * envelope;
            }
        }

        return buffer;
    }

    /**
     * Serializes an AudioBuffer<float> into an in-memory WAV file.
     *
     * @param buffer      The audio buffer to encode.
     * @param sampleRate  Sample rate to write into the WAV header.
     * @return            A MemoryBlock containing WAV file data.
     */
    inline juce::MemoryBlock createWavFile(const juce::AudioBuffer<float> &audioBuffer,
                                           double sampleRate)
    {
        juce::MemoryBlock result;

        // Set up an output stream
        juce::WavAudioFormat wavFormat;
        std::unique_ptr<juce::AudioFormatWriter> writer(wavFormat.createWriterFor(
            new juce::MemoryOutputStream(result, false),
            sampleRate,
            audioBuffer.getNumChannels(),
            16, // bit depth
            {}, 0));

        if (writer != nullptr)
        {
            writer->writeFromAudioSampleBuffer(audioBuffer, 0, audioBuffer.getNumSamples());
        }

        return result;
    }
}
