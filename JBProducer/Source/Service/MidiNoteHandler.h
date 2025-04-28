#pragma once
#include <JuceHeader.h>
#include <cmath>
#include <limits>
#include <map>

namespace MidiNoteHandler
{
    /**
     * @brief Enum representing musical note names and octaves.
     */
    typedef enum
    {
        // Natural Notes
        A0,
        A1,
        A2,
        A3,
        A4,
        A5,
        A6,
        A7,
        A8,
        A9,
        B0,
        B1,
        B2,
        B3,
        B4,
        B5,
        B6,
        B7,
        B8,
        B9,
        C0,
        C1,
        C2,
        C3,
        C4,
        C5,
        C6,
        C7,
        C8,
        C9,
        D0,
        D1,
        D2,
        D3,
        D4,
        D5,
        D6,
        D7,
        D8,
        D9,
        E0,
        E1,
        E2,
        E3,
        E4,
        E5,
        E6,
        E7,
        E8,
        E9,
        F0,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        G0,
        G1,
        G2,
        G3,
        G4,
        G5,
        G6,
        G7,
        G8,
        G9,
        // Sharp/Flat Notes (using 's' suffix)
        A0s,
        A1s,
        A2s,
        A3s,
        A4s,
        A5s,
        A6s,
        A7s,
        A8s,
        A9s, // A# / Bb
        C1s,
        C2s,
        C3s,
        C4s,
        C5s,
        C6s,
        C7s,
        C8s,
        C9s, // C# / Db (No C0s)
        D0s,
        D1s,
        D2s,
        D3s,
        D4s,
        D5s,
        D6s,
        D7s,
        D8s,
        D9s, // D# / Eb
        E0s,
        E1s,
        E2s,
        E3s,
        E4s,
        E5s,
        E6s,
        E7s,
        E8s,
        E9s, // E# / F (Enharmonic to F)
        F0s,
        F1s,
        F2s,
        F3s,
        F4s,
        F5s,
        F6s,
        F7s,
        F8s,
        F9s, // F# / Gb
        G0s,
        G1s,
        G2s,
        G3s,
        G4s,
        G5s,
        G6s,
        G7s,
        G8s,
        G9s // G# / Ab
    } NoteName;

    /**
     * @brief Converts a NoteName enum value to its corresponding MIDI note number.
     * @param note The NoteName enum value.
     * @return The MIDI note number (0-127), or -1 if the NoteName corresponds
     *         to a note outside the standard MIDI range or is otherwise invalid.
     */
    inline constexpr int noteToMidiNote(NoteName note)
    {
        // Standard MIDI notes: C4 = 60, A4 = 69
        switch (note)
        {
            // A Notes (Octave 0 starts at MIDI 21)
        case A0:
            return 21;
        case A1:
            return 33;
        case A2:
            return 45;
        case A3:
            return 57;
        case A4:
            return 69;
        case A5:
            return 81;
        case A6:
            return 93;
        case A7:
            return 105;
        case A8:
            return 117;
        case A9:
            return -1; // 129 > 127

            // B Notes
        case B0:
            return 23;
        case B1:
            return 35;
        case B2:
            return 47;
        case B3:
            return 59;
        case B4:
            return 71;
        case B5:
            return 83;
        case B6:
            return 95;
        case B7:
            return 107;
        case B8:
            return 119;
        case B9:
            return -1; // 131 > 127

            // C Notes (Octave 0 starts at MIDI 12)
        case C0:
            return 12;
        case C1:
            return 24;
        case C2:
            return 36;
        case C3:
            return 48;
        case C4:
            return 60;
        case C5:
            return 72;
        case C6:
            return 84;
        case C7:
            return 96;
        case C8:
            return 108;
        case C9:
            return 120;

            // D Notes
        case D0:
            return 14;
        case D1:
            return 26;
        case D2:
            return 38;
        case D3:
            return 50;
        case D4:
            return 62;
        case D5:
            return 74;
        case D6:
            return 86;
        case D7:
            return 98;
        case D8:
            return 110;
        case D9:
            return 122;

            // E Notes
        case E0:
            return 16;
        case E1:
            return 28;
        case E2:
            return 40;
        case E3:
            return 52;
        case E4:
            return 64;
        case E5:
            return 76;
        case E6:
            return 88;
        case E7:
            return 100;
        case E8:
            return 112;
        case E9:
            return 124;

            // F Notes
        case F0:
            return 17;
        case F1:
            return 29;
        case F2:
            return 41;
        case F3:
            return 53;
        case F4:
            return 65;
        case F5:
            return 77;
        case F6:
            return 89;
        case F7:
            return 101;
        case F8:
            return 113;
        case F9:
            return 125;

            // G Notes
        case G0:
            return 19;
        case G1:
            return 31;
        case G2:
            return 43;
        case G3:
            return 55;
        case G4:
            return 67;
        case G5:
            return 79;
        case G6:
            return 91;
        case G7:
            return 103;
        case G8:
            return 115;
        case G9:
            return 127;

            // A# / Bb Notes
        case A0s:
            return 22;
        case A1s:
            return 34;
        case A2s:
            return 46;
        case A3s:
            return 58;
        case A4s:
            return 70;
        case A5s:
            return 82;
        case A6s:
            return 94;
        case A7s:
            return 106;
        case A8s:
            return 118;
        case A9s:
            return -1; // 130 > 127

            // C# / Db Notes
        case C1s:
            return 25;
        case C2s:
            return 37;
        case C3s:
            return 49;
        case C4s:
            return 61;
        case C5s:
            return 73;
        case C6s:
            return 85;
        case C7s:
            return 97;
        case C8s:
            return 109;
        case C9s:
            return 121;

            // D# / Eb Notes
        case D0s:
            return 15;
        case D1s:
            return 27;
        case D2s:
            return 39;
        case D3s:
            return 51;
        case D4s:
            return 63;
        case D5s:
            return 75;
        case D6s:
            return 87;
        case D7s:
            return 99;
        case D8s:
            return 111;
        case D9s:
            return 123;

            // E# / F Notes (Enharmonic to F)
        case E0s:
            return 17;
        case E1s:
            return 29;
        case E2s:
            return 41;
        case E3s:
            return 53;
        case E4s:
            return 65;
        case E5s:
            return 77;
        case E6s:
            return 89;
        case E7s:
            return 101;
        case E8s:
            return 113;
        case E9s:
            return 125;

            // F# / Gb Notes
        case F0s:
            return 18;
        case F1s:
            return 30;
        case F2s:
            return 42;
        case F3s:
            return 54;
        case F4s:
            return 66;
        case F5s:
            return 78;
        case F6s:
            return 90;
        case F7s:
            return 102;
        case F8s:
            return 114;
        case F9s:
            return 126;

            // G# / Ab Notes
        case G0s:
            return 20;
        case G1s:
            return 32;
        case G2s:
            return 44;
        case G3s:
            return 56;
        case G4s:
            return 68;
        case G5s:
            return 80;
        case G6s:
            return 92;
        case G7s:
            return 104;
        case G8s:
            return 116;
        case G9s:
            return -1; // 128 > 127

            // Default case for any potential gaps or future additions
        default:
            return -1;
        }
    }

    /**
     * @brief Converts a MIDI note number to its corresponding frequency in Hz.
     *        Uses the standard A4=440Hz tuning.
     * @param midiNote The MIDI note number (0-127).
     * @return The frequency in Hz, or 0.0f if the MIDI note is outside the valid range.
     */
    inline constexpr float midiNoteToFrequency(int midiNote)
    {
        if (midiNote < 0 || midiNote > 127)
        {
            return 0.0f; // Invalid MIDI note
        }
        // Formula: freq = 440 * 2^((midiNote - 69) / 12)
        return 440.0f * std::pow(2.0f, (static_cast<float>(midiNote) - 69.0f) / 12.0f);
    }

    /**
     * @brief Converts a NoteName enum value directly to its frequency in Hz.
     * @param note The NoteName enum value.
     * @return The frequency in Hz, or 0.0f if the note is invalid or out of MIDI range.
     */
    inline constexpr float noteToFrequency(NoteName note)
    {
        return midiNoteToFrequency(noteToMidiNote(note));
    }

    /**
     * @brief Converts a frequency in Hz to the nearest MIDI note number.
     * @param frequency The frequency in Hz. Must be positive.
     * @return The nearest MIDI note number (0-127), or -1 if the frequency is non-positive.
     */
    inline int frequencyToMidiNote(float frequency)
    {
        if (frequency <= 0.0f)
        {
            return -1; // Invalid frequency
        }
        // Formula: midiNote = 12 * log2(freq / 440) + 69
        // Use std::round for nearest integer MIDI note.
        double midiNoteDouble = 12.0 * std::log2(static_cast<double>(frequency) / 440.0) + 69.0;
        int midiNote = static_cast<int>(std::round(midiNoteDouble));

        // Clamp to valid MIDI range [0, 127]
        if (midiNote < 0)
            return 0;
        if (midiNote > 127)
            return 127;
        return midiNote;
    }

    /**
     * @brief Converts a frequency in Hz to the nearest NoteName enum value.
     *
     * @warning This function finds the nearest MIDI note and then searches the NoteName
     *          enum for a value that maps to that MIDI note. Due to the enum's structure
     *          (e.g., E#=F enharmonics, gaps, out-of-range notes), this mapping might
     *          not be unique or might return an unexpected NoteName. It prioritizes
     *          the first match found in the enum order for a given MIDI note.
     *          Consider using `frequencyToMidiNote` if only the MIDI number is needed.
     *
     * @param frequency The frequency in Hz.
     * @return The NoteName enum value corresponding to the nearest MIDI note.
     *         Returns A4 as a default if the frequency is invalid or no matching
     *         NoteName is found within the standard MIDI range represented by the enum.
     */
    inline NoteName frequencyToNote(float frequency)
    {
        int targetMidiNote = frequencyToMidiNote(frequency);

        if (targetMidiNote < 0)
        {
            return A4; // Handle invalid frequency by returning a default
        }

        // Build a reverse map (MIDI note -> NoteName) for efficient lookup.
        // Static initialization ensures this is built only once.
        static const std::map<int, NoteName> midiToNoteMap = []
        {
            std::map<int, NoteName> map;
            // Iterate through a representative range of the enum.
            // Assuming NoteName values might not be perfectly contiguous.
            // Need to manually list notes or iterate carefully if enum values have large gaps.
            // Let's iterate based on known structure C0 to G9s range approximately.
            NoteName allNotes[] = {
                A0, A1, A2, A3, A4, A5, A6, A7, A8, /*A9,*/
                B0, B1, B2, B3, B4, B5, B6, B7, B8, /*B9,*/
                C0, C1, C2, C3, C4, C5, C6, C7, C8, C9,
                D0, D1, D2, D3, D4, D5, D6, D7, D8, D9,
                E0, E1, E2, E3, E4, E5, E6, E7, E8, E9,
                F0, F1, F2, F3, F4, F5, F6, F7, F8, F9,
                G0, G1, G2, G3, G4, G5, G6, G7, G8, G9,
                A0s, A1s, A2s, A3s, A4s, A5s, A6s, A7s, A8s, /*A9s,*/
                C1s, C2s, C3s, C4s, C5s, C6s, C7s, C8s, C9s,
                D0s, D1s, D2s, D3s, D4s, D5s, D6s, D7s, D8s, D9s,
                E0s, E1s, E2s, E3s, E4s, E5s, E6s, E7s, E8s, E9s,
                F0s, F1s, F2s, F3s, F4s, F5s, F6s, F7s, F8s, F9s,
                G0s, G1s, G2s, G3s, G4s, G5s, G6s, G7s, G8s /*G9s*/
            };

            for (NoteName note : allNotes)
            {
                int midiNote = noteToMidiNote(note);
                if (midiNote >= 0)
                {
                    // If MIDI note not already in map, add it.
                    // This prioritizes the first NoteName encountered for a given MIDI note
                    // (e.g., F4 might be added before E4s).
                    if (map.find(midiNote) == map.end())
                    {
                        map[midiNote] = note;
                    }
                }
            }
            return map;
        }();

        auto it = midiToNoteMap.find(targetMidiNote);
        if (it != midiToNoteMap.end())
        {
            return it->second; // Found a matching NoteName
        }

        // Fallback if no NoteName in our map corresponds to the target MIDI note
        return A4;
    }
}