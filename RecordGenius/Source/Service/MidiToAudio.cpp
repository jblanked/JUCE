#include "MidiToAudio.h"

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
        int numChannels,
        double sampleRate)
    {
        // 1) Generate raw MIDI file data from the rhythm
        juce::MemoryBlock midiFileData = RhythmGenerator::createMIDISequence(loop, bpm);

        // 2) Parse the MIDI file
        juce::MemoryInputStream midiStream(midiFileData, false);
        juce::MidiFile midiFile;
        midiFile.readFrom(midiStream);

        // JUCE's MidiFile uses getTimeFormat() to report ticks per quarter note
        int ticksPerQuarter = midiFile.getTimeFormat();

        auto *track = midiFile.getTrack(0);
        if (track == nullptr)
            return {};

        juce::MidiMessageSequence sequence = *track;
        sequence.updateMatchedPairs();

        // 3) Compute loop length in samples
        int samplesPerBeat = static_cast<int>(sampleRate * 60.0 / bpm);
        constexpr int beatsPerBar = 4;
        constexpr int numBars = 4; // Loop8 always has 4 bars
        int totalSamples = samplesPerBeat * beatsPerBar * numBars;

        // 4) Prepare output buffer
        juce::AudioBuffer<float> output(numChannels, totalSamples);
        output.clear();

        // Create a processed sequence to handle note events that should be ignored
        juce::MidiMessageSequence processedSequence;

        // 5) Preprocess the note events to filter out unwanted triggers
        for (int i = 0; i < sequence.getNumEvents(); ++i)
        {
            const auto &msgOn = sequence.getEventPointer(i)->message;
            if (!msgOn.isNoteOn())
            {
                processedSequence.addEvent(msgOn);
                continue;
            }

            // Find the matching note-off (key-up) event
            int offIndex = sequence.getIndexOfMatchingKeyUp(i);
            if (offIndex < 0)
                continue;

            const auto &msgOff = sequence.getEventPointer(offIndex)->message;

            // Timing: convert MIDI ticks to beats to check position
            double startTicks = msgOn.getTimeStamp();
            double startBeats = startTicks / ticksPerQuarter;

            // Check if this note falls exactly on an eighth-note boundary
            double eighthNote = 0.5; // half a beat
            bool isOnEighthBoundary = std::fmod(startBeats, eighthNote) < 0.01;

            if (isOnEighthBoundary)
            {
                // Add valid notes to our processed sequence
                processedSequence.addEvent(msgOn);
                processedSequence.addEvent(msgOff);
            }
        }

        processedSequence.updateMatchedPairs();

        // 6) Render each valid note by generating a sample snippet
        for (int i = 0; i < processedSequence.getNumEvents(); ++i)
        {
            const auto &msgOn = processedSequence.getEventPointer(i)->message;
            if (!msgOn.isNoteOn())
                continue;

            // Find the matching note-off (key-up) event
            int offIndex = processedSequence.getIndexOfMatchingKeyUp(i);
            if (offIndex < 0)
                continue;

            const auto &msgOff = processedSequence.getEventPointer(offIndex)->message;

            // Timing: convert MIDI ticks to sample indices
            double startTicks = msgOn.getTimeStamp();
            double lengthTicks = msgOff.getTimeStamp() - startTicks;
            double startBeats = startTicks / ticksPerQuarter;
            double lengthBeats = lengthTicks / ticksPerQuarter;

            int startSample = static_cast<int>(startBeats * samplesPerBeat);
            int lengthInSamples = static_cast<int>(lengthBeats * samplesPerBeat);

            // Limit maximum note duration to prevent overlapping hits
            int maxDuration = samplesPerBeat / 2; // Limit to eighth note duration
            lengthInSamples = std::min(lengthInSamples, maxDuration);

            // Determine playback frequency for the note
            float freq = MidiNoteHandler::midiNoteToFrequency(msgOn.getNoteNumber());

            // Generate a pitched snippet for this note
            auto noteBuf = SampleLoopGenerator::generateSampleLoopFromBinary(
                data, dataSize,
                freq,
                lengthInSamples,
                sampleRate,
                bpm,
                numChannels);

            // Apply a short fade-out to the end of the snippet to avoid abrupt artifacts
            constexpr double fadeDurationSec = 0.005; // 5ms fade
            int fadeSamples = juce::jmin(lengthInSamples, static_cast<int>(sampleRate * fadeDurationSec));
            for (int ch = 0; ch < numChannels; ++ch)
            {
                float *buf = noteBuf.getWritePointer(ch);
                for (int i = 0; i < fadeSamples; ++i)
                {
                    int idx = lengthInSamples - 1 - i;
                    float env = static_cast<float>(i) / fadeSamples;
                    buf[idx] *= env;
                }
            }

            // Mix the snippet into the output buffer at the correct offset
            for (int ch = 0; ch < numChannels; ++ch)
            {
                float *dest = output.getWritePointer(ch);
                const float *src = noteBuf.getReadPointer(ch);
                for (int n = 0; n < lengthInSamples; ++n)
                {
                    int idx = startSample + n;
                    if (idx < totalSamples)
                        dest[idx] += src[n];
                }
            }
        }

        return output;
    }

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
        int numChannels,
        double sampleRate)
    {
        // 1) Generate raw MIDI file data from the rhythm
        juce::MemoryBlock midiFileData = RhythmGenerator::createMIDISequence(loop, bpm);

        // 2) Parse the MIDI file
        juce::MemoryInputStream midiStream(midiFileData, false);
        juce::MidiFile midiFile;
        midiFile.readFrom(midiStream);

        // JUCE's MidiFile uses getTimeFormat() to report ticks per quarter note
        int ticksPerQuarter = midiFile.getTimeFormat();

        auto *track = midiFile.getTrack(0);
        if (track == nullptr)
            return {};

        juce::MidiMessageSequence sequence = *track;
        sequence.updateMatchedPairs();

        // 3) Compute loop length in samples
        int samplesPerBeat = static_cast<int>(sampleRate * 60.0 / bpm);
        constexpr int beatsPerBar = 4;
        constexpr int numBars = 4; // Loop16 still has 4 bars
        int totalSamples = samplesPerBeat * beatsPerBar * numBars;

        // 4) Prepare output buffer
        juce::AudioBuffer<float> output(numChannels, totalSamples);
        output.clear();

        // Create a processed sequence to handle note events that should be ignored
        juce::MidiMessageSequence processedSequence;

        // 5) Preprocess the note events to filter out unwanted triggers
        for (int i = 0; i < sequence.getNumEvents(); ++i)
        {
            const auto &msgOn = sequence.getEventPointer(i)->message;
            if (!msgOn.isNoteOn())
            {
                processedSequence.addEvent(msgOn);
                continue;
            }

            // Find the matching note-off (key-up) event
            int offIndex = sequence.getIndexOfMatchingKeyUp(i);
            if (offIndex < 0)
                continue;

            const auto &msgOff = sequence.getEventPointer(offIndex)->message;

            // Timing: convert MIDI ticks to beats to check position
            double startTicks = msgOn.getTimeStamp();
            double startBeats = startTicks / ticksPerQuarter;

            // For Loop16, we're dealing with 16th notes (0.25 of a beat)
            double sixteenthNote = 0.25; // quarter of a beat
            bool isOnSixteenthBoundary = std::fmod(startBeats, sixteenthNote) < 0.01;

            if (isOnSixteenthBoundary)
            {
                // Add valid notes to our processed sequence
                processedSequence.addEvent(msgOn);
                processedSequence.addEvent(msgOff);
            }
        }

        processedSequence.updateMatchedPairs();

        // 6) Render each valid note by generating a sample snippet
        for (int i = 0; i < processedSequence.getNumEvents(); ++i)
        {
            const auto &msgOn = processedSequence.getEventPointer(i)->message;
            if (!msgOn.isNoteOn())
                continue;

            // Find the matching note-off (key-up) event
            int offIndex = processedSequence.getIndexOfMatchingKeyUp(i);
            if (offIndex < 0)
                continue;

            const auto &msgOff = processedSequence.getEventPointer(offIndex)->message;

            // Timing: convert MIDI ticks to sample indices
            double startTicks = msgOn.getTimeStamp();
            double lengthTicks = msgOff.getTimeStamp() - startTicks;
            double startBeats = startTicks / ticksPerQuarter;
            double lengthBeats = lengthTicks / ticksPerQuarter;

            int startSample = static_cast<int>(startBeats * samplesPerBeat);
            int lengthInSamples = static_cast<int>(lengthBeats * samplesPerBeat);

            // Limit maximum note duration to prevent overlapping hits
            // For 16th notes, we use 1/4 of a beat to avoid collisions
            int maxDuration = samplesPerBeat / 4; // Limit to sixteenth note duration
            lengthInSamples = std::min(lengthInSamples, maxDuration);

            // Determine playback frequency for the note
            float freq = MidiNoteHandler::midiNoteToFrequency(msgOn.getNoteNumber());

            // Generate a pitched snippet for this note
            auto noteBuf = SampleLoopGenerator::generateSampleLoopFromBinary(
                data, dataSize,
                freq,
                lengthInSamples,
                sampleRate,
                bpm,
                numChannels);

            // Apply a short fade-out to the end of the snippet to avoid abrupt artifacts
            constexpr double fadeDurationSec = 0.005; // 5ms fade
            int fadeSamples = juce::jmin(lengthInSamples, static_cast<int>(sampleRate * fadeDurationSec));
            for (int ch = 0; ch < numChannels; ++ch)
            {
                float *buf = noteBuf.getWritePointer(ch);
                for (int i = 0; i < fadeSamples; ++i)
                {
                    int idx = lengthInSamples - 1 - i;
                    float env = static_cast<float>(i) / fadeSamples;
                    buf[idx] *= env;
                }
            }

            // Mix the snippet into the output buffer at the correct offset
            for (int ch = 0; ch < numChannels; ++ch)
            {
                float *dest = output.getWritePointer(ch);
                const float *src = noteBuf.getReadPointer(ch);
                for (int n = 0; n < lengthInSamples; ++n)
                {
                    int idx = startSample + n;
                    if (idx < totalSamples)
                        dest[idx] += src[n];
                }
            }
        }

        return output;
    }

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
        int numChannels,
        double sampleRate)
    {
        juce::AudioBuffer<float> audioLoop = convert(
            loop, data, dataSize, bpm, numChannels, sampleRate);
        return createWavFile(audioLoop, sampleRate);
    }

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
        int numChannels,
        double sampleRate)
    {
        juce::AudioBuffer<float> audioLoop = convert(
            loop, data, dataSize, bpm, numChannels, sampleRate);
        return createWavFile(audioLoop, sampleRate);
    }

    /**
     * Creates a WAV file from an audio buffer.
     *
     * @param buffer      The audio buffer to write to the WAV file.
     * @param sampleRate  The sample rate of the audio buffer.
     * @return           A MemoryBlock containing the WAV file data.
     */
    juce::MemoryBlock createWavFile(
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

} // namespace MidiToAudio
