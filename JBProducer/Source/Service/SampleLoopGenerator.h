#pragma once
#include <JuceHeader.h>

namespace SampleLoopGenerator
{
    /**
     * Core generator: reads from any InputStream (e.g., MemoryInputStream or FileInputStream)
     * and tiles the sample at each beat with optional pitch-shift.
     *
     * @param inStream          Pointer to an open InputStream containing WAV data.
     *                          The reader will delete this stream when done.
     * @param frequency         Target pitch in Hz (e.g. from your frequency slider).
     * @param lengthInSamples   Total loop length in samples.
     * @param sampleRate        DAW sample rate (Hz).
     * @param bpm               Tempo (beats per minute).
     * @param numChannels       Number of output channels.
     * @param originalFreq      Frequency of the original sample (default C3 = 130.81278 Hz).
     * @param gain              Multiplier applied to each sample hit (default 0.7f).
     */
    inline juce::AudioBuffer<float> generateSampleLoopFromStream(
        juce::InputStream *inStream,
        float frequency,
        int lengthInSamples,
        double sampleRate,
        float bpm,
        int numChannels,
        float originalFreq = 130.81278f,
        float gain = 0.7f)
    {
        std::unique_ptr<juce::AudioFormatReader> reader(
            juce::WavAudioFormat().createReaderFor(inStream, /*deleteWhenDone*/ true));
        jassert(reader != nullptr);

        // Decode the entire sample into a buffer
        juce::AudioBuffer<float> sampleBuf(
            (int)reader->numChannels,
            (int)reader->lengthInSamples);
        reader->read(&sampleBuf,
                     0,
                     sampleBuf.getNumSamples(),
                     0,
                     true,
                     true);

        // Calculate samples per beat and pitch ratio
        int samplesPerBeat = int(sampleRate * (60.0 / bpm));
        float pitchRatio = originalFreq / frequency;

        // Prepare output and tile sample
        juce::AudioBuffer<float> out(numChannels, lengthInSamples);
        out.clear();

        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto *dest = out.getWritePointer(ch);
            auto *src = sampleBuf.getReadPointer(juce::jmin(ch, sampleBuf.getNumChannels() - 1));
            int srcLen = sampleBuf.getNumSamples();

            for (int start = 0; start < lengthInSamples; start += samplesPerBeat)
            {
                for (int i = 0; i < srcLen && start + i < lengthInSamples; ++i)
                {
                    double idx = i / pitchRatio;
                    if (idx >= srcLen - 1)
                        break;

                    int i0 = (int)idx;
                    float frac = (float)(idx - i0);
                    float v = src[i0] * (1.0f - frac) + src[i0 + 1] * frac;

                    dest[start + i] += v * gain;
                }
            }
        }

        return out;
    }

    /**
     * Convenience function to generate a sample loop from binary data.
     *
     * @param data             Pointer to the binary data.
     * @param dataSize         Size of the binary data in bytes.
     * @param frequency        Target pitch in Hz (e.g. from your frequency slider).
     * @param lengthInSamples  Total loop length in samples.
     * @param sampleRate       DAW sample rate (Hz).
     * @param bpm              Tempo (beats per minute).
     * @param numChannels      Number of output channels.
     * @param originalFreq     Frequency of the original sample (default C3 = 130.81278 Hz).
     * @param gain             Multiplier applied to each sample hit (default 0.7f).
     */
    inline juce::AudioBuffer<float> generateSampleLoopFromBinary(
        const void *data,
        size_t dataSize,
        float frequency,
        int lengthInSamples,
        double sampleRate,
        float bpm,
        int numChannels,
        float originalFreq = 130.81278f,
        float gain = 0.7f)
    {
        auto *inStream = new juce::MemoryInputStream(data, dataSize, false);
        return generateSampleLoopFromStream(
            inStream,
            frequency,
            lengthInSamples,
            sampleRate,
            bpm,
            numChannels,
            originalFreq,
            gain);
    }

    /**
     * Creates a WAV file from an audio buffer.
     *
     * @param buffer      The audio buffer to write to the WAV file.
     * @param sampleRate  The sample rate of the audio buffer.
     * @return           A MemoryBlock containing the WAV file data.
     */
    inline juce::MemoryBlock createWavFile(
        const juce::AudioBuffer<float> &buffer,
        double sampleRate)
    {
        juce::MemoryBlock result;
        auto writer = std::unique_ptr<juce::AudioFormatWriter>(
            juce::WavAudioFormat().createWriterFor(
                new juce::MemoryOutputStream(result, false),
                sampleRate,
                buffer.getNumChannels(),
                16,
                {},
                0));

        if (writer)
            writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());

        return result;
    }
}
