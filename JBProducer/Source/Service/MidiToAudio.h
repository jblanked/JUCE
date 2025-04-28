#pragma once

#include <JuceHeader.h>
#include "RhythmGenerator.h"
#include "SampleLoopGenerator.h"
#include "MidiNoteHandler.h"

namespace MidiToAudio
{
    /**
     * Converts a rhythmic Loop8 structure and sample data into an audio buffer by
     * rendering each MIDI note with the provided sample.
     *
     * @param loop        The RhythmGenerator::Loop8 struct defining the rhythm.
     * @param data        Pointer to the binary WAV data for the sample.
     * @param dataSize    Size of the binary data in bytes.
     * @param bpm         Tempo in beats per minute.
     * @param numChannels Number of audio channels (default = 2).
     * @param sampleRate  Sample rate in Hz (default = 44100.0).
     * @return            An AudioBuffer<float> containing the rendered audio.
     */
    juce::AudioBuffer<float> convert(
        const RhythmGenerator::Loop8 &loop,
        const void *data,
        size_t dataSize,
        float bpm,
        int numChannels = 2,
        double sampleRate = 44100.0);

    /**
     * Converts a rhythmic Loop16 structure and sample data into an audio buffer by
     * rendering each MIDI note with the provided sample. Handles 16th note resolution.
     *
     * @param loop        The RhythmGenerator::Loop16 struct defining the rhythm.
     * @param data        Pointer to the binary WAV data for the sample.
     * @param dataSize    Size of the binary data in bytes.
     * @param bpm         Tempo in beats per minute.
     * @param numChannels Number of audio channels (default = 2).
     * @param sampleRate  Sample rate in Hz (default = 44100.0).
     * @return            An AudioBuffer<float> containing the rendered audio.
     */
    juce::AudioBuffer<float> convert(
        const RhythmGenerator::Loop16 &loop,
        const void *data,
        size_t dataSize,
        float bpm,
        int numChannels = 2,
        double sampleRate = 44100.0);

    /**
     * Converts a rhythmic Loop8 structure and sample data into an audio buffer by
     * rendering each MIDI note with the provided sample.
     *
     * @param loop        The RhythmGenerator::Loop8 struct defining the rhythm.
     * @param data        Pointer to the binary WAV data for the sample.
     * @param dataSize    Size of the binary data in bytes.
     * @param bpm         Tempo in beats per minute.
     * @param numChannels Number of audio channels (default = 2).
     * @param sampleRate  Sample rate in Hz (default = 44100.0).
     * @return            A MemoryBlock containing the WAV file data.
     */
    juce::MemoryBlock convertToWavFile(
        const RhythmGenerator::Loop8 &loop,
        const void *data,
        size_t dataSize,
        float bpm,
        int numChannels = 2,
        double sampleRate = 44100.0);

    /**
     * Converts a rhythmic Loop16 structure and sample data into an audio buffer by
     * rendering each MIDI note with the provided sample. Handles 16th note resolution.
     *
     * @param loop        The RhythmGenerator::Loop16 struct defining the rhythm.
     * @param data        Pointer to the binary WAV data for the sample.
     * @param dataSize    Size of the binary data in bytes.
     * @param bpm         Tempo in beats per minute.
     * @param numChannels Number of audio channels (default = 2).
     * @param sampleRate  Sample rate in Hz (default = 44100.0).
     * @return            A MemoryBlock containing the WAV file data.
     */
    juce::MemoryBlock convertToWavFile(
        const RhythmGenerator::Loop16 &loop,
        const void *data,
        size_t dataSize,
        float bpm,
        int numChannels = 2,
        double sampleRate = 44100.0);

    /**
     * Creates a WAV file from an audio buffer.
     *
     * @param buffer      The audio buffer to write to the WAV file.
     * @param sampleRate  The sample rate of the audio buffer.
     * @return           A MemoryBlock containing the WAV file data.
     */
    juce::MemoryBlock createWavFile(
        const juce::AudioBuffer<float> &buffer,
        double sampleRate);

} // namespace MidiToAudio