#pragma once
#include <JuceHeader.h>
#include "../Service/MidiNoteHandler.h"

namespace RhythmGenerator
{
    typedef enum
    {
        Rest = 0,
        Note = 1,
        Continuation = 2,
    } NoteType;

    typedef struct
    {
        NoteType noteType;
        float velocity;
        MidiNoteHandler::NoteName frequency;
    } MusicNote;

    // 8 notes per bar
    typedef struct
    {
        MusicNote notes[8];
    } Bar8;

    // 16 notes per bar
    typedef struct
    {
        MusicNote notes[16];
    } Bar16;

    typedef struct
    {
        Bar8 bar_1;
        Bar8 bar_2;
        Bar8 bar_3;
        Bar8 bar_4;
    } Loop8;

    typedef struct
    {
        Bar16 bar_1;
        Bar16 bar_2;
        Bar16 bar_3;
        Bar16 bar_4;
    } Loop16;

    /*
        Example using Twinkle Twinkle Little Star
        {
            bar_1: {
                note_1: { type: Note, velocity: 0.5, frequency: C3 },
                note_2: { type: Note, velocity: 0.5, frequency: C3 },
                note_3: { type: Note, velocity: 0.0, frequency: C3 },
                note_4: { type: Note, velocity: 0.5, frequency: C3 },
                note_5: { type: Note, velocity: 0.5, frequency: C3 }, // quarter note
                note_6: { type: Continuation, velocity: 0.5, frequency: C3 },
                note_7: { type: Note, velocity: 0.5, frequency: C3 }, // quarter note
                note_8: { type: Continuation, velocity: 0.5, frequency: C3 }
            },
            bar_2: {
                note_1: { type: Note, velocity: 0.5, frequency: G3 },
                note_2: { type: Note, velocity: 0.5, frequency: G3 },
                note_3: { type: Note, velocity: 0.5, frequency: G3 },
                note_4: { type: Note, velocity: 0.5, frequency: G3 },
                note_5: { type: Rest, velocity: 0.5, frequency: G3 }, // quarter note
                note_6: { type: Continuation, velocity: 0.5, frequency: G3 },
                note_7: { type: Note, velocity: 0.5, frequency: G3 }, // quarter note
                note_8: { type: Continuation, velocity: 0.5, frequency: G3 }
            },
            bar_3: {
                note_1: { type: Note, velocity: 0.5, frequency: A3 },
                note_2: { type: Note, velocity: 0.5, frequency: A3 },
                note_3: { type: Note, velocity: 0.5, frequency: A3 },
                note_4: { type: Note, velocity: 0.5, frequency: A3 },
                note_5: { type: Rest, velocity: 0.5, frequency: A3 }, // quarter note
                note_6: { type: Continuation, velocity: 0.5, frequency: A3 },
                note_7: { type: Note, velocity: 0.5, frequency: A3 }, // quarter note
                note_8: { type: Continuation, velocity: 0.5, frequency: A3 }
            },
            bar_4: {
                note_1: { type: Note, velocity: 0.5, frequency: G3 },
                note_2: { type: Note, velocity: 0.5, frequency: G3 },
                note_3: { type: Note, velocity: 0.5, frequency: G3 },
                note_4: { type: Note, velocity: 0.5, frequency: G3 },
                note_5: { type: Rest, velocity: 0.5, frequency: G3 }, // quarter note
                note_6: { type: Continuation, velocity: 0.5, frequency: G3 },
                note_7: { type: Note, velocity: 0.5, frequency: G3 }, // quarter note
                note_8: { type: Continuation, velocity: 0.5, frequency: G3 }
            },
            bar_5: {
                note_1: { type: Note, velocity: 0.5, frequency: F3 },
                note_2: { type: Note, velocity: 0.5, frequency: F3 },
                note_3: { type: Note, velocity: 0.5, frequency: F3 },
                note_4: { type: Note, velocity: 0.5, frequency: F3 },
                note_5: { type: Rest, velocity: 0.5, frequency: F3 }, // quarter note
                note_6: { type: Continuation, velocity: 0.5, frequency: F3 },
                note_7: { type: Note, velocity: 0.5, frequency: F3 }, // quarter note
                note_8: { type: Continuation, velocity: 0.5, frequency: F3 }
            },
            bar_6: {
                note_1: { type: Note, velocity: 0.5, frequency: E3 },
                note_2: { type: Note, velocity: 0.5, frequency: E3 },
                note_3: { type: Note, velocity: 0.5, frequency: E3 },
                note_4: { type: Note, velocity: 0.5, frequency: E3 },
                note_5: { type: Rest, velocity: 0.5, frequency: E3 }, // quarter note
                note_6: { type: Continuation, velocity: 0.5, frequency: E3 },
                note_7: { type: Note, velocity: 0.5, frequency: E3 }, // quarter note
                note_8: { type: Continuation, velocity: 0.5, frequency: E3 }
            },
            bar_7: {
                note_1: { type: Note, velocity: 0.5, frequency: D3 },
                note_2: { type: Note, velocity: 0.5, frequency: D3 },
                note_3: { type: Note, velocity: 0.5, frequency: D3 },
                note_4: { type: Note, velocity: 0.5, frequency: D3 },
                note_5: { type: Rest, velocity: 0.5, frequency: D3 }, // quarter note
                note_6: { type: Continuation, velocity: 0.5, frequency: D3 },
                note_7: { type: Note, velocity: 0.5, frequency: D3 }, // quarter note
                note_8: { type: Continuation, velocity: 0.5, frequency: D3 }
            },
            bar_8: {
                note_1: { type: Note, velocity: 0.5, frequency: C3 },
                note_2: { type: Note, velocity: 0.5, frequency: C3 },
                note_3: { type: Note, velocity: 0.5, frequency: C3 },
                note_4: { type: Note, velocity: 0.5, frequency: C3 },
                note_5: { type: Rest, velocity: 0.5, frequency: C3 }, // quarter note
                note_6: { type: Continuation, velocity: 0.5, frequency: C3 },
                note_7: { type: Note, velocity: 0.5, frequency: C3 }, // quarter note
                note_8: { type: Continuation, velocity: 0.5, frequency: C3 }
            },
        }
    */

    /**
     * @brief Generates a MIDI sequence based on the provided notes and BPM.
     * @param bar struct containing notes
     * @param bpm Beats per minute for the MIDI sequence.
     * @return A MemoryBlock containing the generated MIDI sequence.
     */
    inline juce::MemoryBlock createMIDISequence(
        const Bar8 bar,
        int numNotes,
        float bpm)
    {
        juce::MidiMessageSequence sequence;
        sequence.clear();
        int ticksPerQuarterNote = 960; // Standard MIDI ticks per quarter note

        // For an 8-note bar, each note is 1/8th of a bar duration (half a quarter note)
        double ticksPerNote = ticksPerQuarterNote / 2.0;
        double noteOffsetInTicks = 0;

        // Track active notes to pair with note-offs
        struct ActiveNote
        {
            int midiNote;
            double onTime;
            double duration;
        };
        std::vector<ActiveNote> activeNotes;

        // Process each note in the bar
        for (int i = 0; i < std::min(numNotes, 8); ++i)
        {
            const MusicNote &note = bar.notes[i];

            // Calculate timing
            double time = noteOffsetInTicks;
            noteOffsetInTicks += ticksPerNote;

            // Convert NoteName to MIDI note number
            int midiNote = MidiNoteHandler::noteToMidiNote(note.frequency);

            switch (note.noteType)
            {
            case Note:
                // Add note-on message if this is a note (not a rest)
                if (midiNote >= 0)
                {
                    float v = juce::jlimit(0.0f, 1.0f, note.velocity);
                    auto noteOn = juce::MidiMessage::noteOn(1, midiNote, v);
                    noteOn.setTimeStamp(time);
                    sequence.addEvent(noteOn);

                    // Add to active notes to track for note-off later
                    activeNotes.push_back({midiNote, time, ticksPerNote});
                }
                break;

            case Continuation:
                // For continuation, we extend the duration of the previous note
                if (!activeNotes.empty())
                {
                    ActiveNote &lastNote = activeNotes.back();
                    lastNote.duration += ticksPerNote; // Extend duration
                }
                break;

            case Rest:
                // No MIDI events for rests
                break;
            }

            // Add note-off for completed notes (non-continued)
            for (auto it = activeNotes.begin(); it != activeNotes.end();)
            {
                if (note.noteType != Continuation)
                {
                    auto noteOff = juce::MidiMessage::noteOff(1, it->midiNote);
                    noteOff.setTimeStamp(it->onTime + it->duration);
                    sequence.addEvent(noteOff);
                    it = activeNotes.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        // Add any remaining note-offs
        for (const auto &note : activeNotes)
        {
            auto noteOff = juce::MidiMessage::noteOff(1, note.midiNote);
            noteOff.setTimeStamp(note.onTime + note.duration);
            sequence.addEvent(noteOff);
        }

        // Update matched pairs (JUCE's way of associating note-ons with their note-offs)
        sequence.updateMatchedPairs();

        // Create the MIDI file
        juce::MidiFile midiFile;
        midiFile.setTicksPerQuarterNote(ticksPerQuarterNote);
        midiFile.addTrack(sequence);

        // Write to memory block
        juce::MemoryBlock block;
        juce::MemoryOutputStream stream(block, false);
        midiFile.writeTo(stream);

        return block;
    }

    /**
     * @brief Generates a MIDI sequence based on the provided notes and BPM.
     * @param bar struct containing notes
     * @param bpm Beats per minute for the MIDI sequence.
     * @return A MemoryBlock containing the generated MIDI sequence.
     */
    inline juce::MemoryBlock createMIDISequence(
        const Bar16 bar,
        int numNotes,
        float bpm)
    {
        juce::MidiMessageSequence sequence;
        sequence.clear();
        int ticksPerQuarterNote = 960; // Standard MIDI ticks per quarter note

        // For a 16-note bar, each note is 1/16th of a bar duration (quarter of a quarter note)
        double ticksPerNote = ticksPerQuarterNote / 4.0;
        double noteOffsetInTicks = 0;

        // Track active notes to pair with note-offs
        struct ActiveNote
        {
            int midiNote;
            double onTime;
            double duration;
        };
        std::vector<ActiveNote> activeNotes;

        // Process each note in the bar
        for (int i = 0; i < std::min(numNotes, 16); ++i)
        {
            const MusicNote &note = bar.notes[i];

            // Calculate timing
            double time = noteOffsetInTicks;
            noteOffsetInTicks += ticksPerNote;

            // Convert NoteName to MIDI note number
            int midiNote = MidiNoteHandler::noteToMidiNote(note.frequency);

            switch (note.noteType)
            {
            case Note:
                // Add note-on message if this is a note (not a rest)
                if (midiNote >= 0)
                {
                    float v = juce::jlimit(0.0f, 1.0f, note.velocity);
                    auto noteOn = juce::MidiMessage::noteOn(1, midiNote, v);
                    noteOn.setTimeStamp(time);
                    sequence.addEvent(noteOn);

                    // Add to active notes to track for note-off later
                    activeNotes.push_back({midiNote, time, ticksPerNote});
                }
                break;

            case Continuation:
                // For continuation, we extend the duration of the previous note
                if (!activeNotes.empty())
                {
                    ActiveNote &lastNote = activeNotes.back();
                    lastNote.duration += ticksPerNote; // Extend duration
                }
                break;

            case Rest:
                // No MIDI events for rests
                break;
            }

            // Add note-off for completed notes (non-continued)
            for (auto it = activeNotes.begin(); it != activeNotes.end();)
            {
                if (note.noteType != Continuation)
                {
                    auto noteOff = juce::MidiMessage::noteOff(1, it->midiNote);
                    noteOff.setTimeStamp(it->onTime + it->duration);
                    sequence.addEvent(noteOff);
                    it = activeNotes.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        // Add any remaining note-offs
        for (const auto &note : activeNotes)
        {
            auto noteOff = juce::MidiMessage::noteOff(1, note.midiNote);
            noteOff.setTimeStamp(note.onTime + note.duration);
            sequence.addEvent(noteOff);
        }

        // Update matched pairs (JUCE's way of associating note-ons with their note-offs)
        sequence.updateMatchedPairs();

        // Create the MIDI file
        juce::MidiFile midiFile;
        midiFile.setTicksPerQuarterNote(ticksPerQuarterNote);
        midiFile.addTrack(sequence);

        // Write to memory block
        juce::MemoryBlock block;
        juce::MemoryOutputStream stream(block, false);
        midiFile.writeTo(stream);

        return block;
    }

    /**
     * @brief Generates a MIDI sequence based on a Loop8 rhythm structure.
     * @param loop The Loop8 structure containing multiple bars of notes.
     * @param bpm Beats per minute for the MIDI sequence.
     * @return A MemoryBlock containing the generated MIDI sequence.
     */
    inline juce::MemoryBlock createMIDISequence(
        const Loop8 loop,
        float bpm)
    {
        juce::MidiMessageSequence sequence;
        sequence.clear();
        int ticksPerQuarterNote = 960; // Standard MIDI ticks per quarter note

        // For an 8-note bar, each note is 1/8th of a bar duration (half a quarter note)
        double ticksPerNote = ticksPerQuarterNote / 2.0;
        double noteOffsetInTicks = 0;

        // Track active notes for proper note-offs
        struct ActiveNote
        {
            int midiNote;
            double onTime;
            double duration;
        };
        std::vector<ActiveNote> activeNotes;

        // Process all bars in the loop
        const Bar8 *bars[] = {&loop.bar_1, &loop.bar_2, &loop.bar_3, &loop.bar_4};

        for (const Bar8 *bar : bars)
        {
            // Process each note in the bar
            for (int i = 0; i < 8; ++i)
            {
                const MusicNote &note = bar->notes[i];

                // Calculate timing
                double time = noteOffsetInTicks;
                noteOffsetInTicks += ticksPerNote;

                // Convert NoteName to MIDI note number
                int midiNote = MidiNoteHandler::noteToMidiNote(note.frequency);

                switch (note.noteType)
                {
                case Note:
                    // Add note-on message if this is a note (not a rest)
                    if (midiNote >= 0)
                    {
                        float v = juce::jlimit(0.0f, 1.0f, note.velocity);
                        auto noteOn = juce::MidiMessage::noteOn(1, midiNote, v);
                        noteOn.setTimeStamp(time);
                        sequence.addEvent(noteOn);

                        // Add to active notes to track for note-off later
                        activeNotes.push_back({midiNote, time, ticksPerNote});
                    }
                    break;

                case Continuation:
                    // For continuation, we extend the duration of the previous note
                    if (!activeNotes.empty())
                    {
                        ActiveNote &lastNote = activeNotes.back();
                        lastNote.duration += ticksPerNote; // Extend duration
                    }
                    break;

                case Rest:
                    // No MIDI events for rests
                    break;
                }

                // Handle note-offs for completed notes
                for (auto it = activeNotes.begin(); it != activeNotes.end();)
                {
                    if (note.noteType != Continuation)
                    {
                        auto noteOff = juce::MidiMessage::noteOff(1, it->midiNote);
                        noteOff.setTimeStamp(it->onTime + it->duration);
                        sequence.addEvent(noteOff);
                        it = activeNotes.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
        }

        // Add any remaining note-offs
        for (const auto &note : activeNotes)
        {
            auto noteOff = juce::MidiMessage::noteOff(1, note.midiNote);
            noteOff.setTimeStamp(note.onTime + note.duration);
            sequence.addEvent(noteOff);
        }

        // Update matched pairs (JUCE's way of associating note-ons with their note-offs)
        sequence.updateMatchedPairs();

        // Create the MIDI file
        juce::MidiFile midiFile;
        midiFile.setTicksPerQuarterNote(ticksPerQuarterNote);
        midiFile.addTrack(sequence);

        // Write to memory block
        juce::MemoryBlock block;
        juce::MemoryOutputStream stream(block, false);
        midiFile.writeTo(stream);

        return block;
    }

    /**
     * @brief Generates a MIDI sequence based on a Loop16 rhythm structure.
     * @param loop The Loop16 structure containing multiple bars of notes.
     * @param bpm Beats per minute for the MIDI sequence.
     * @return A MemoryBlock containing the generated MIDI sequence.
     */
    inline juce::MemoryBlock createMIDISequence(
        const Loop16 loop,
        float bpm)
    {
        juce::MidiMessageSequence sequence;
        sequence.clear();
        int ticksPerQuarterNote = 960; // Standard MIDI ticks per quarter note

        // For a 16-note bar, each note is 1/16th of a bar duration (quarter of a quarter note)
        double ticksPerNote = ticksPerQuarterNote / 4.0;
        double noteOffsetInTicks = 0;

        // Track active notes for proper note-offs
        struct ActiveNote
        {
            int midiNote;
            double onTime;
            double duration;
        };
        std::vector<ActiveNote> activeNotes;

        // Process all bars in the loop
        const Bar16 *bars[] = {&loop.bar_1, &loop.bar_2, &loop.bar_3, &loop.bar_4};

        for (const Bar16 *bar : bars)
        {
            // Process each note in the bar
            for (int i = 0; i < 16; ++i)
            {
                const MusicNote &note = bar->notes[i];

                // Calculate timing
                double time = noteOffsetInTicks;
                noteOffsetInTicks += ticksPerNote;

                // Convert NoteName to MIDI note number
                int midiNote = MidiNoteHandler::noteToMidiNote(note.frequency);

                switch (note.noteType)
                {
                case Note:
                    // Add note-on message if this is a note (not a rest)
                    if (midiNote >= 0)
                    {
                        float v = juce::jlimit(0.0f, 1.0f, note.velocity);
                        auto noteOn = juce::MidiMessage::noteOn(1, midiNote, v);
                        noteOn.setTimeStamp(time);
                        sequence.addEvent(noteOn);

                        // Add to active notes to track for note-off later
                        activeNotes.push_back({midiNote, time, ticksPerNote});
                    }
                    break;

                case Continuation:
                    // For continuation, we extend the duration of the previous note
                    if (!activeNotes.empty())
                    {
                        ActiveNote &lastNote = activeNotes.back();
                        lastNote.duration += ticksPerNote; // Extend duration
                    }
                    break;

                case Rest:
                    // No MIDI events for rests
                    break;
                }

                // Handle note-offs for completed notes
                for (auto it = activeNotes.begin(); it != activeNotes.end();)
                {
                    if (note.noteType != Continuation)
                    {
                        auto noteOff = juce::MidiMessage::noteOff(1, it->midiNote);
                        noteOff.setTimeStamp(it->onTime + it->duration);
                        sequence.addEvent(noteOff);
                        it = activeNotes.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
        }

        // Add any remaining note-offs
        for (const auto &note : activeNotes)
        {
            auto noteOff = juce::MidiMessage::noteOff(1, note.midiNote);
            noteOff.setTimeStamp(note.onTime + note.duration);
            sequence.addEvent(noteOff);
        }

        // Update matched pairs (JUCE's way of associating note-ons with their note-offs)
        sequence.updateMatchedPairs();

        // Create the MIDI file
        juce::MidiFile midiFile;
        midiFile.setTicksPerQuarterNote(ticksPerQuarterNote);
        midiFile.addTrack(sequence);

        // Write to memory block
        juce::MemoryBlock block;
        juce::MemoryOutputStream stream(block, false);
        midiFile.writeTo(stream);

        return block;
    }
}
