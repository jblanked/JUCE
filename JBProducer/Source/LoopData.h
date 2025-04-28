#pragma once
#include <JuceHeader.h>
#include "Loops.h"
#define LOOP_DATA_COUNT 16

template <typename T>
struct LoopData
{
    juce::String name;
    int index;
    const void *sample;
    size_t sampleSize;
    T loop;
};

LoopData<Loop8> loopData1 = {
    .name = "Hi-Hat 8th Notes",
    .index = 1,
    .sample = BinaryData::Trap_HiHat_wav,
    .sampleSize = BinaryData::Trap_HiHat_wavSize,
    .loop = midiLoopHighHatBasic8()};

LoopData<Loop16> loopData2 = {
    .name = "Hi-Hat 16th Notes",
    .index = 2,
    .sample = BinaryData::Trap_HiHat_wav,
    .sampleSize = BinaryData::Trap_HiHat_wavSize,
    .loop = midiLoopHighHatBasic16()};

LoopData<Loop8> loopData3 = {
    .name = "Snare 8th Notes",
    .index = 3,
    .sample = BinaryData::Trap_Snare_wav,
    .sampleSize = BinaryData::Trap_Snare_wavSize,
    .loop = midiLoopSnareBasic()};

LoopData<Loop16> loopData4 = {
    .name = "Snare 16th Notes",
    .index = 4,
    .sample = BinaryData::Trap_Snare_wav,
    .sampleSize = BinaryData::Trap_Snare_wavSize,
    .loop = midiLoopSnareBasic16()};

LoopData<Loop16> loopData5 = {
    .name = "Kick Basic 16",
    .index = 5,
    .sample = BinaryData::Trap_Kick_wav,
    .sampleSize = BinaryData::Trap_Kick_wavSize,
    .loop = midiLoopKickBasic16()};

LoopData<Loop8> loopData6 = {
    .name = "Kick Basic 8",
    .index = 6,
    .sample = BinaryData::Trap_Kick_wav,
    .sampleSize = BinaryData::Trap_Kick_wavSize,
    .loop = midiLoopKickBasic()};

LoopData<Loop16> loopData7 = {
    .name = "Kick Basic 16 (2)",
    .index = 7,
    .sample = BinaryData::Trap_Kick_wav,
    .sampleSize = BinaryData::Trap_Kick_wavSize,
    .loop = midiLoopKickBasic2()};

LoopData<Loop16> loopData8 = {
    .name = "Kick Extra",
    .index = 8,
    .sample = BinaryData::Trap_Kick_wav,
    .sampleSize = BinaryData::Trap_Kick_wavSize,
    .loop = midiLoopKickExtra16()};

LoopData<Loop16> loopData9 = {
    .name = "Kick Drake",
    .index = 9,
    .sample = BinaryData::Trap_Kick_wav,
    .sampleSize = BinaryData::Trap_Kick_wavSize,
    .loop = midiLoopKickDrake16()};

LoopData<Loop16> loopData10 = {
    .name = "Kick Trap",
    .index = 10,
    .sample = BinaryData::Trap_Kick_wav,
    .sampleSize = BinaryData::Trap_Kick_wavSize,
    .loop = midiLoopKickTrap16()};

LoopData<Loop16> loopData11 = {
    .name = "Kick Trap (2)",
    .index = 11,
    .sample = BinaryData::Trap_Kick_wav,
    .sampleSize = BinaryData::Trap_Kick_wavSize,
    .loop = midiLoopKickTrap2()};

LoopData<Loop16> loopData12 = {
    .name = "Kick Trap (3)",
    .index = 12,
    .sample = BinaryData::Trap_Kick_wav,
    .sampleSize = BinaryData::Trap_Kick_wavSize,
    .loop = midiLoopKickTrap3()};

LoopData<Loop8> loopData13 = {
    .name = "Kick Trap (4)",
    .index = 13,
    .sample = BinaryData::Trap_Kick_wav,
    .sampleSize = BinaryData::Trap_Kick_wavSize,
    .loop = midiLoopKickTrap4()};

LoopData<Loop16> loopData14 = {
    .name = "Kick Trap (5)",
    .index = 14,
    .sample = BinaryData::Trap_Kick_wav,
    .sampleSize = BinaryData::Trap_Kick_wavSize,
    .loop = midiLoopKickTrap5()};

LoopData<Loop16> loopData15 = {
    .name = "Kick Trap (6)",
    .index = 15,
    .sample = BinaryData::Trap_Kick_wav,
    .sampleSize = BinaryData::Trap_Kick_wavSize,
    .loop = midiLoopKickTrap6()};

LoopData<Loop8> loopData16 = {
    .name = "Kick Boom Bap",
    .index = 16,
    .sample = BinaryData::Trap_Kick_wav,
    .sampleSize = BinaryData::Trap_Kick_wavSize,
    .loop = midiLoopKickBoomBap()};
