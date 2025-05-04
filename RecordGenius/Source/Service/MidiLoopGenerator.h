#pragma once
#include <JuceHeader.h>
#include <cmath>

/**
 * Namespace providing utilities for creating and exporting short MIDI loops.
 */
namespace MidiLoopGenerator
{
    /**
     * Generates a sequence of quarter-note MIDI events based on specified parameters.
     * @param frequency   Base frequency (Hz) for calculating the MIDI note number (A4 = 440 Hz).
     * @param numBars     Number of bars in the loop.
     * @param beatsPerBar Number of beats per bar.
     * @param bpm         Tempo in beats per minute.
     * @return            A MidiMessageSequence containing matched Note On/Off events.
     */
    inline juce::MidiMessageSequence generateMidiLoop(float frequency,
                                                      int numBars,
                                                      int beatsPerBar,
                                                      float bpm)
    {
        juce::MidiMessageSequence sequence;
        sequence.clear();

        // Calculate MIDI note (69 = A4 = 440 Hz)
        int midiNote = static_cast<int>(std::round(69.0 + 12.0 * std::log2(frequency / 440.0)));
        const double ticksPerQuarter = 960.0;

        for (int bar = 0; bar < numBars; ++bar)
        {
            for (int beat = 0; beat < beatsPerBar; ++beat)
            {
                double time = (bar * beatsPerBar + beat) * ticksPerQuarter;

                auto noteOn = juce::MidiMessage::noteOn(1, midiNote, 1.0f);
                noteOn.setTimeStamp(time);
                sequence.addEvent(noteOn);

                auto noteOff = juce::MidiMessage::noteOff(1, midiNote);
                noteOff.setTimeStamp(time + ticksPerQuarter);
                sequence.addEvent(noteOff);
            }
        }

        sequence.updateMatchedPairs();
        return sequence;
    }

    /**
     * Converts a MidiMessageSequence into a standard MIDI file format stored in memory.
     * @param sequence The sequence of MIDI events to serialize.
     * @return         A MemoryBlock containing the raw MIDI file data.
     */
    inline juce::MemoryBlock createMidiFile(const juce::MidiMessageSequence &sequence)
    {
        juce::MidiFile midiFile;
        midiFile.setTicksPerQuarterNote(960);
        midiFile.addTrack(sequence);

        juce::MemoryBlock block;
        juce::MemoryOutputStream stream(block, false);
        midiFile.writeTo(stream);
        return block;
    }
}
