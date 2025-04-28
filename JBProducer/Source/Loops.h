#pragma once
#include <JuceHeader.h>
#include "Service/RhythmGenerator.h"

using namespace RhythmGenerator;

#define note(freq) {.noteType = Note, .velocity = 0.5, .frequency = freq}
#define cont(freq) {.noteType = Continuation, .velocity = 0.5, .frequency = freq}
#define rest(freq) {.noteType = Rest, .velocity = 0.5, .frequency = freq}

// initialize a loop with 8 notes
static Loop8 midiLoop8Init()
{
    Loop8 midi;
    const MusicNote mNote = rest(MidiNoteHandler::C3);
    for (int i = 0; i < 8; i++)
    {
        midi.bar_1.notes[i] = mNote;
        midi.bar_2.notes[i] = mNote;
        midi.bar_3.notes[i] = mNote;
        midi.bar_4.notes[i] = mNote;
    }
    return midi;
}

// initialize a loop with 16 notes
static Loop16 midiLoop16Init()
{
    Loop16 midi;
    const MusicNote mNote = rest(MidiNoteHandler::C3);
    for (int i = 0; i < 16; i++)
    {
        midi.bar_1.notes[i] = mNote;
        midi.bar_2.notes[i] = mNote;
        midi.bar_3.notes[i] = mNote;
        midi.bar_4.notes[i] = mNote;
    }
    return midi;
}

// repetitve 8 notes
Loop8 midiLoopHighHatBasic8()
{
    Loop8 midi = midiLoop8Init();
    const MusicNote hiHatNote = note(MidiNoteHandler::C3);
    for (int i = 0; i < 8; i++)
    {
        midi.bar_1.notes[i] = hiHatNote;
        midi.bar_2.notes[i] = hiHatNote;
        midi.bar_3.notes[i] = hiHatNote;
        midi.bar_4.notes[i] = hiHatNote;
    }
    return midi;
}

// repetitve 16th notes
Loop16 midiLoopHighHatBasic16()
{
    Loop16 midi = midiLoop16Init();
    const MusicNote hiHatNote = note(MidiNoteHandler::C3);
    for (int i = 0; i < 16; i++)
    {
        midi.bar_1.notes[i] = hiHatNote;
        midi.bar_2.notes[i] = hiHatNote;
        midi.bar_3.notes[i] = hiHatNote;
        midi.bar_4.notes[i] = hiHatNote;
    }
    return midi;
}

// repetitve hit on 2nd (3/8) and 4th (7/8) beat
Loop8 midiLoopSnareBasic()
{
    Loop8 midi = midiLoop8Init();
    const MusicNote snareNote = note(MidiNoteHandler::C3);
    //
    midi.bar_1.notes[2] = snareNote;
    midi.bar_1.notes[6] = snareNote;
    //
    midi.bar_2.notes[2] = snareNote;
    midi.bar_2.notes[6] = snareNote;
    //
    midi.bar_3.notes[2] = snareNote;
    midi.bar_3.notes[6] = snareNote;
    //
    midi.bar_4.notes[2] = snareNote;
    midi.bar_4.notes[6] = snareNote;
    //
    return midi;
}

// repetitve hit on 2nd (3/8) and 4th (7/8) beat
Loop16 midiLoopSnareBasic16()
{
    Loop16 midi = midiLoop16Init();
    const MusicNote snareNote = note(MidiNoteHandler::C3);
    //
    midi.bar_1.notes[4] = snareNote;
    midi.bar_1.notes[12] = snareNote;
    //
    midi.bar_2.notes[4] = snareNote;
    midi.bar_2.notes[12] = snareNote;
    //
    midi.bar_3.notes[4] = snareNote;
    midi.bar_3.notes[12] = snareNote;
    //
    midi.bar_4.notes[4] = snareNote;
    midi.bar_4.notes[12] = snareNote;
    //
    return midi;
}

// hit on beats: 1/16, 8/16, 11/16
Loop16 midiLoopKickBasic16()
{
    Loop16 midi = midiLoop16Init();
    const MusicNote kickNote = note(MidiNoteHandler::C3);
    //
    midi.bar_1.notes[0] = kickNote;
    midi.bar_1.notes[7] = kickNote;
    midi.bar_1.notes[10] = kickNote;
    //
    midi.bar_2.notes[0] = kickNote;
    midi.bar_2.notes[7] = kickNote;
    midi.bar_2.notes[10] = kickNote;
    //
    midi.bar_3.notes[0] = kickNote;
    midi.bar_3.notes[7] = kickNote;
    midi.bar_3.notes[10] = kickNote;
    //
    midi.bar_4.notes[0] = kickNote;
    midi.bar_4.notes[7] = kickNote;
    midi.bar_4.notes[10] = kickNote;
    //
    return midi;
}

// hit on beats: 1/8, 4/8 for first, second, and third bar
// hit on beats: 1/8, 4/8, 6/8 for the last bar
Loop8 midiLoopKickBasic()
{
    Loop8 midi = midiLoop8Init();
    const MusicNote kickNote = note(MidiNoteHandler::C3);
    //
    midi.bar_1.notes[0] = kickNote;
    midi.bar_1.notes[3] = kickNote;
    //
    midi.bar_2.notes[0] = kickNote;
    midi.bar_2.notes[3] = kickNote;
    //
    midi.bar_3.notes[0] = kickNote;
    midi.bar_3.notes[3] = kickNote;
    //
    midi.bar_4.notes[0] = kickNote;
    midi.bar_4.notes[3] = kickNote;
    midi.bar_4.notes[5] = kickNote;
    //
    return midi;
}

// hit on beats: 1/16, 8/16 for first, second, and third bar
// hit on beats: 1/16, 4/16, 8/16 for the last bar
Loop16 midiLoopKickBasic2()
{
    Loop16 midi = midiLoop16Init();
    const MusicNote kickNote = note(MidiNoteHandler::C3);
    //
    midi.bar_1.notes[0] = kickNote;
    midi.bar_1.notes[7] = kickNote;
    //
    midi.bar_2.notes[0] = kickNote;
    midi.bar_2.notes[7] = kickNote;
    //
    midi.bar_3.notes[0] = kickNote;
    midi.bar_3.notes[7] = kickNote;
    //
    midi.bar_4.notes[0] = kickNote;
    midi.bar_4.notes[3] = kickNote;
    midi.bar_4.notes[7] = kickNote;
    //
    return midi;
}

// hit on beats: 1/16, 8/16, 11/16, 14/16
Loop16 midiLoopKickExtra16()
{
    Loop16 midi = midiLoop16Init();
    const MusicNote kickNote = note(MidiNoteHandler::C3);
    //
    midi.bar_1.notes[0] = kickNote;
    midi.bar_1.notes[7] = kickNote;
    midi.bar_1.notes[10] = kickNote;
    midi.bar_1.notes[13] = kickNote;
    //
    midi.bar_2.notes[0] = kickNote;
    midi.bar_2.notes[7] = kickNote;
    midi.bar_2.notes[10] = kickNote;
    midi.bar_2.notes[13] = kickNote;
    //
    midi.bar_3.notes[0] = kickNote;
    midi.bar_3.notes[7] = kickNote;
    midi.bar_3.notes[10] = kickNote;
    midi.bar_3.notes[13] = kickNote;
    //
    midi.bar_4.notes[0] = kickNote;
    midi.bar_4.notes[7] = kickNote;
    midi.bar_4.notes[10] = kickNote;
    midi.bar_4.notes[13] = kickNote;
    //
    return midi;
}

// hit on beats: 1/16, 4/16, 8/16, 11/16, 14/16
Loop16 midiLoopKickDrake16()
{
    Loop16 midi = midiLoop16Init();
    const MusicNote kickNote = note(MidiNoteHandler::C3);
    //
    midi.bar_1.notes[0] = kickNote;
    midi.bar_1.notes[3] = kickNote;
    midi.bar_1.notes[7] = kickNote;
    midi.bar_1.notes[10] = kickNote;
    midi.bar_1.notes[13] = kickNote;
    //
    midi.bar_2.notes[0] = kickNote;
    midi.bar_2.notes[3] = kickNote;
    midi.bar_2.notes[7] = kickNote;
    midi.bar_2.notes[10] = kickNote;
    midi.bar_2.notes[13] = kickNote;
    //
    midi.bar_3.notes[0] = kickNote;
    midi.bar_3.notes[3] = kickNote;
    midi.bar_3.notes[7] = kickNote;
    midi.bar_3.notes[10] = kickNote;
    midi.bar_3.notes[13] = kickNote;
    //
    midi.bar_4.notes[0] = kickNote;
    midi.bar_4.notes[3] = kickNote;
    midi.bar_4.notes[7] = kickNote;
    midi.bar_4.notes[10] = kickNote;
    midi.bar_4.notes[13] = kickNote;
    //
    return midi;
}

// hit on beats: 1/16, 8/16, and 11/16 the first and third bar
// hit on beats: 1/16, 4/16, 8/16, 11/16, and 14/16 the second and 4th bar
Loop16 midiLoopKickTrap16()
{
    Loop16 midi = midiLoop16Init();
    const MusicNote kickNote = note(MidiNoteHandler::C3);
    //
    midi.bar_1.notes[0] = kickNote;
    midi.bar_1.notes[7] = kickNote;
    midi.bar_1.notes[10] = kickNote;
    //
    midi.bar_2.notes[0] = kickNote;
    midi.bar_2.notes[3] = kickNote;
    midi.bar_2.notes[7] = kickNote;
    midi.bar_2.notes[10] = kickNote;
    midi.bar_2.notes[13] = kickNote;
    //
    midi.bar_3.notes[0] = kickNote;
    midi.bar_3.notes[7] = kickNote;
    midi.bar_3.notes[10] = kickNote;
    //
    midi.bar_4.notes[0] = kickNote;
    midi.bar_4.notes[3] = kickNote;
    midi.bar_4.notes[7] = kickNote;
    midi.bar_4.notes[10] = kickNote;
    midi.bar_4.notes[13] = kickNote;
    //
    return midi;
}

// hit on beats 1/16, 2/16, and 12/16 on the first and third bar
// hit on beats 1/16, 2/16, 12/16, and 14/16 on the second and 4th bar
Loop16 midiLoopKickTrap2()
{
    Loop16 midi = midiLoop16Init();
    const MusicNote kickNote = note(MidiNoteHandler::C3);
    //
    midi.bar_1.notes[0] = kickNote;
    midi.bar_1.notes[1] = kickNote;
    midi.bar_1.notes[11] = kickNote;
    //
    midi.bar_2.notes[0] = kickNote;
    midi.bar_2.notes[1] = kickNote;
    midi.bar_2.notes[10] = kickNote;
    midi.bar_2.notes[13] = kickNote;
    //
    midi.bar_3.notes[0] = kickNote;
    midi.bar_3.notes[1] = kickNote;
    midi.bar_3.notes[11] = kickNote;
    //
    midi.bar_4.notes[0] = kickNote;
    midi.bar_4.notes[1] = kickNote;
    midi.bar_4.notes[10] = kickNote;
    midi.bar_4.notes[13] = kickNote;
    //
    return midi;
}

// hit on beats 1/16, 12/16, and 14/16 on the first and third bar
// hit on beats 1/16, 11/16, 12/16, and 14/16 on the second and 4th bar
Loop16 midiLoopKickTrap3()
{
    Loop16 midi = midiLoop16Init();
    const MusicNote kickNote = note(MidiNoteHandler::C3);
    //
    midi.bar_1.notes[0] = kickNote;
    midi.bar_1.notes[11] = kickNote;
    midi.bar_1.notes[13] = kickNote;
    //
    midi.bar_2.notes[0] = kickNote;
    midi.bar_2.notes[10] = kickNote;
    midi.bar_2.notes[11] = kickNote;
    midi.bar_2.notes[13] = kickNote;
    //
    midi.bar_3.notes[0] = kickNote;
    midi.bar_3.notes[11] = kickNote;
    midi.bar_3.notes[13] = kickNote;
    //
    midi.bar_4.notes[0] = kickNote;
    midi.bar_4.notes[10] = kickNote;
    midi.bar_4.notes[11] = kickNote;
    midi.bar_4.notes[13] = kickNote;
    //
    return midi;
}

// hit on beats 1/8, 4/8, 6/8
Loop8 midiLoopKickTrap4()
{
    Loop8 midi = midiLoop8Init();
    const MusicNote kickNote = note(MidiNoteHandler::C3);
    //
    midi.bar_1.notes[0] = kickNote;
    midi.bar_1.notes[3] = kickNote;
    midi.bar_1.notes[5] = kickNote;
    //
    midi.bar_2.notes[0] = kickNote;
    midi.bar_2.notes[3] = kickNote;
    midi.bar_2.notes[5] = kickNote;
    //
    midi.bar_3.notes[0] = kickNote;
    midi.bar_3.notes[3] = kickNote;
    midi.bar_3.notes[5] = kickNote;
    //
    midi.bar_4.notes[0] = kickNote;
    midi.bar_4.notes[3] = kickNote;
    midi.bar_4.notes[5] = kickNote;
    //
    return midi;
}

// hit on beats 1/16, 8/16, 12/16
Loop16 midiLoopKickTrap5()
{
    Loop16 midi = midiLoop16Init();
    const MusicNote kickNote = note(MidiNoteHandler::C3);
    //
    midi.bar_1.notes[0] = kickNote;
    midi.bar_1.notes[7] = kickNote;
    midi.bar_1.notes[11] = kickNote;
    //
    midi.bar_2.notes[0] = kickNote;
    midi.bar_2.notes[7] = kickNote;
    midi.bar_2.notes[11] = kickNote;
    //
    midi.bar_3.notes[0] = kickNote;
    midi.bar_3.notes[7] = kickNote;
    midi.bar_3.notes[11] = kickNote;
    //
    midi.bar_4.notes[0] = kickNote;
    midi.bar_4.notes[7] = kickNote;
    midi.bar_4.notes[11] = kickNote;
    //
    return midi;
}

// hit on beats 1/16, 8/16, and 12/16 on the first and third bar
// hit on beats 1/16, 8/16, 12/16, and 14/16 on the second and 4th bar
Loop16 midiLoopKickTrap6()
{
    Loop16 midi = midiLoop16Init();
    const MusicNote kickNote = note(MidiNoteHandler::C3);
    //
    midi.bar_1.notes[0] = kickNote;
    midi.bar_1.notes[7] = kickNote;
    midi.bar_1.notes[11] = kickNote;
    //
    midi.bar_2.notes[0] = kickNote;
    midi.bar_2.notes[7] = kickNote;
    midi.bar_2.notes[11] = kickNote;
    midi.bar_2.notes[13] = kickNote;
    //
    midi.bar_3.notes[0] = kickNote;
    midi.bar_3.notes[7] = kickNote;
    midi.bar_3.notes[11] = kickNote;
    //
    midi.bar_4.notes[0] = kickNote;
    midi.bar_4.notes[7] = kickNote;
    midi.bar_4.notes[11] = kickNote;
    midi.bar_4.notes[13] = kickNote;
    //
    return midi;
}

// hit on beats 1/8, 5/8, and 6/8 on the first and third bar
// hit on beats 1/8 and 6/8 on the second and 4th bar
Loop8 midiLoopKickBoomBap()
{
    Loop8 midi = midiLoop8Init();
    const MusicNote kickNote = note(MidiNoteHandler::C3);
    //
    midi.bar_1.notes[0] = kickNote;
    midi.bar_1.notes[4] = kickNote;
    midi.bar_1.notes[5] = kickNote;
    //
    midi.bar_2.notes[0] = kickNote;
    midi.bar_2.notes[5] = kickNote;
    //
    midi.bar_3.notes[0] = kickNote;
    midi.bar_3.notes[4] = kickNote;
    midi.bar_3.notes[5] = kickNote;
    //
    midi.bar_4.notes[0] = kickNote;
    midi.bar_4.notes[5] = kickNote;
    //
    return midi;
}
