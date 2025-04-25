#pragma once

#include <JuceHeader.h>
#include "BinaryData.h"

/**
    MidiPlayer: A Component with embedded preset UI and a circular output meter from -24 to +10 dB.
*/
class MidiPlayer : public juce::Component, private juce::Timer
{
public:
    struct Preset
    {
        juce::String name;
        double attack, release, maxSampleLength;
        const void *sampleData;
        size_t sampleDataSize;
        int rootMidiNote;
    };

    MidiPlayer()
    {
        formatManager.registerBasicFormats();
        for (int i = 0; i < numVoices; ++i)
            synth.addVoice(new juce::SamplerVoice());

        addAndMakeVisible(presetBox);
        presetBox.onChange = [this]
        { selectPreset(presetBox.getSelectedItemIndex()); };

        level.store(1.0f);
        startTimerHz(30);
    }

    ~MidiPlayer() override = default;

    void addPreset(const juce::String &name,
                   double attack, double release,
                   double maxSampleLength,
                   const void *sampleData,
                   size_t sampleDataSize,
                   int rootMidiNote = 60)
    {
        // Add to presets vector
        presets.push_back({name, attack, release, maxSampleLength,
                           sampleData, sampleDataSize, rootMidiNote});

        // Clear combo box and repopulate in alphabetical order
        presetBox.clear();

        // Sort the preset names alphabetically and add to combo box
        std::vector<juce::String> presetNames;
        for (const auto &preset : presets)
            presetNames.push_back(preset.name);

        std::sort(presetNames.begin(), presetNames.end(),
                  [](const juce::String &a, const juce::String &b)
                  { return a.compareIgnoreCase(b) < 0; });

        for (int i = 0; i < presetNames.size(); ++i)
            presetBox.addItem(presetNames[i], i + 1);

        if (presets.size() == 1)
            presetBox.setSelectedId(1);
        else
            presetBox.setText(presets.back().name);
    }

    void prepareToPlay(double sampleRate)
    {
        synth.setCurrentPlaybackSampleRate(sampleRate);
    }

    void processBlock(juce::AudioBuffer<float> &buffer,
                      juce::MidiBuffer &midiMessages)
    {
        buffer.clear();

        currentNotes.clear();
        for (const auto midiMetadata : midiMessages)
        {
            const auto message = midiMetadata.getMessage();
            if (message.isNoteOn())
                currentNotes.add(message.getNoteNumber());
            else if (message.isNoteOff())
                currentNotes.removeFirstMatchingValue(message.getNoteNumber());
        }

        synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

        float peak = 0.0f;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            peak = juce::jmax(peak, buffer.getMagnitude(ch, 0, buffer.getNumSamples()));
        level.store(peak);
    }

    void paintHeader(juce::Graphics &g)
    {
        g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));
        g.setColour(juce::Colours::white);
        g.setFont(15.0f);
        g.drawFittedText("Choose a preset, then play MIDI...",
                         getLocalBounds(), juce::Justification::centredTop, 1);
    }

    void paintMeterAndNotes(juce::Graphics &g, bool paintNotes = true)
    {
        auto area = getLocalBounds().reduced(10);
        area.removeFromTop(presetBox.getHeight() + 20);

        float circleFrac = 0.75f;
        int rawD = juce::jmin((int)(area.getWidth() * circleFrac),
                              (int)area.getHeight());
        int d = (int)(rawD * 0.85f);

        juce::Rectangle<float> meterArea(
            (getWidth() - d) * 0.5f,
            (float)(presetBox.getBottom() + (area.getHeight() - d) * 0.5f + 10.0f),
            (float)d, (float)d);

        float cx = meterArea.getCentreX();
        float cy = meterArea.getCentreY();
        float radius = d * 0.5f;

        struct Tick
        {
            const char *label;
            float angle;
        };
        const std::vector<Tick> ticks = {
            {"-24dB", juce::MathConstants<float>::halfPi},
            {"10dB", 11.0f * juce::MathConstants<float>::pi / 6.0f},
            {"0dB", 3.0f * juce::MathConstants<float>::halfPi}};

        float tickLen = 6.0f;
        g.setFont(12.0f);
        for (auto &t : ticks)
        {
            float ix = cx + std::cos(t.angle) * (radius - tickLen);
            float iy = cy + std::sin(t.angle) * (radius - tickLen);
            float ox = cx + std::cos(t.angle) * (radius + tickLen);
            float oy = cy + std::sin(t.angle) * (radius + tickLen);
            g.setColour(juce::Colours::white);
            g.drawLine(ix, iy, ox, oy, 1.0f);

            juce::String lbl(t.label);
            auto tw{juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), lbl)};
            float tx = cx + std::cos(t.angle) * (radius + tickLen + 10.0f) - tw * 0.5f;
            float ty = cy + std::sin(t.angle) * (radius + tickLen + 10.0f) - 6.0f;
            g.drawText(lbl,
                       juce::Rectangle<float>(tx, ty, tw, 12.0f),
                       juce::Justification::centred);
        }

        g.setColour(juce::Colours::grey);
        g.drawEllipse(meterArea, 2.0f);

        const float minDb = -24.0f, maxDb = 10.0f;
        float linear = level.load();
        float dBVal = juce::Decibels::gainToDecibels(linear, minDb);
        dBVal = juce::jlimit(minDb, maxDb, dBVal);

        const float angleMinus24 = ticks[0].angle;
        const float anglePlus10 = ticks[1].angle;
        const float angle0 = ticks[2].angle;

        float angLine;
        if (dBVal <= 0.0f)
        {
            float fraction = (dBVal - minDb) / (0.0f - minDb);
            angLine = angleMinus24 + fraction * (angle0 - angleMinus24);
        }
        else
        {
            float fraction = dBVal / maxDb;
            angLine = angle0 + fraction * (anglePlus10 - angle0);
        }

        float x2 = cx + std::cos(angLine) * radius;
        float y2 = cy + std::sin(angLine) * radius;
        g.setColour(juce::Colours::aqua);
        g.drawLine(cx, cy, x2, y2, 4.0f);

        if (paintNotes && currentNotes.size() > 0)
        {
            g.setColour(juce::Colours::white);
            g.setFont(18.0f);

            juce::String noteText;
            for (int i = 0; i < currentNotes.size() && i < 3; ++i)
            {
                if (i > 0)
                    noteText += " ";
                noteText += juce::MidiMessage::getMidiNoteName(currentNotes[i], true, true, 3);
            }

            if (currentNotes.size() > 3)
                noteText += "...";

            g.drawText(noteText,
                       juce::Rectangle<float>(cx - radius * 0.7f, cy - 30.0f, radius * 1.4f, 20.0f),
                       juce::Justification::centred);
        }
    }

    void paint(juce::Graphics &g) override
    {
        paintHeader(g);
        paintMeterAndNotes(g);
    }

    void resized() override
    {
        presetBox.setBounds(getLocalBounds().reduced(20).removeFromTop(30));
    }

private:
    void selectPreset(int comboBoxIndex)
    {
        // Get the selected preset name from the combo box
        juce::String selectedName = presetBox.getText();

        // Find the matching preset in the presets vector
        for (size_t i = 0; i < presets.size(); ++i)
        {
            if (presets[i].name == selectedName)
            {
                auto &p = presets[i];

                synth.clearSounds();

                auto stream = std::make_unique<juce::MemoryInputStream>(p.sampleData,
                                                                        p.sampleDataSize,
                                                                        false);

                std::unique_ptr<juce::AudioFormatReader> reader(
                    formatManager.createReaderFor(std::move(stream)));

                if (reader != nullptr)
                {
                    juce::BigInteger allNotes;
                    allNotes.setRange(0, 128, true);

                    synth.addSound(new juce::SamplerSound(p.name,
                                                          *reader,
                                                          allNotes,
                                                          p.rootMidiNote,
                                                          p.attack,
                                                          p.release,
                                                          p.maxSampleLength));
                }

                return;
            }
        }
    }

    void timerCallback() override { repaint(); }

    static constexpr int numVoices = 8;
    juce::Synthesiser synth;
    juce::AudioFormatManager formatManager;
    std::vector<Preset> presets;
    juce::ComboBox presetBox;
    std::atomic<float> level{1.0f};
    juce::Array<int> currentNotes;
};
