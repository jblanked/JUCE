#pragma once
#include <variant>
#include <array>
#include <JuceHeader.h>
#include "LoopData.h"
#include "Service/RhythmGenerator.h"
#include "Service/MidiToAudio.h"

namespace LoopDataHelpers
{
    // unify both Loop8 and Loop16 into a single variant
    using AnyLoopData = std::variant<LoopData<Loop8>, LoopData<Loop16>>;

    // pack them into a constexpr array
    static constexpr int kNumLoops = LOOP_DATA_COUNT;
    static const std::array<AnyLoopData, kNumLoops> allLoops = {
        loopData1,
        loopData2,
        loopData3,
        loopData4,
        loopData5,
        loopData6,
        loopData7,
        loopData8,
        loopData9,
        loopData10,
        loopData11,
        loopData12,
        loopData13,
        loopData14,
        loopData15,
        loopData16};

    // lookup by index
    inline const AnyLoopData &getLoopById(int id)
    {
        for (auto &ld : allLoops)
            if (std::visit([&](auto &d)
                           { return d.index == id; }, ld))
                return ld;

        return allLoops.front(); // fallback
    }

    // names for filling a ComboBox dynamically
    inline juce::StringArray getLoopNames()
    {
        juce::StringArray names;
        for (auto &ld : allLoops)
            names.add(std::visit([](auto &d)
                                 { return d.name; }, ld));
        return names;
    }

    // generate a MemoryBlock containing the MIDI file
    inline juce::MemoryBlock makeMidiBlock(int id, float bpm)
    {
        auto &ld = getLoopById(id);
        return std::visit([&](auto &d)
                          { return RhythmGenerator::createMIDISequence(d.loop, bpm); }, ld);
    }

    // generate a MemoryBlock containing the WAV file
    inline juce::MemoryBlock makeAudioBlock(int id,
                                            float bpm,
                                            int numChannels,
                                            double sampleRate)
    {
        auto &ld = getLoopById(id);
        return std::visit([&](auto &d)
                          { return MidiToAudio::convertToWavFile(d.loop,
                                                                 d.sample,
                                                                 d.sampleSize,
                                                                 bpm,
                                                                 numChannels,
                                                                 sampleRate); }, ld);
    }
}
