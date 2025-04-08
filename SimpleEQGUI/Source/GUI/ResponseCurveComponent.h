#pragma once

#include <JuceHeader.h>
#include "../GUI/FFTComponents.h"
#include "../DSP/DSPUtilities.h"
#include "../DSP/BasicAudioProcessor.h"

namespace GUI
{
    using namespace DSP;

    struct ResponseCurveComponent : public juce::Component,
                                    public juce::AudioProcessorParameter::Listener,
                                    public juce::Timer
    {
        // Constructor now takes a reference to the processor.
        // To decouple further, consider taking a void* or an abstract interface pointer.
        ResponseCurveComponent(BasicAudioProcessor &processor);
        ~ResponseCurveComponent() override;

        // Parameter listener methods
        void parameterValueChanged(int parameterIndex, float newValue) override;
        void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}

        // Timer callback for periodic updates
        void timerCallback() override;

        // Component overrides
        void paint(juce::Graphics &g) override;
        void resized() override;

        // Toggle FFT analyzer display on/off
        void toggleAnalyzerEnabled(bool enabled) { shouldShowFFTAnalysis = enabled; }

        // Helper drawing methods and data conversion routines
        void drawTextLabels(juce::Graphics &g);
        std::vector<float> getXs(const std::vector<float> &freqs, float left, float width);
        std::vector<float> getGains();
        std::vector<float> getFrequencies();

    private:
        // Reference to the processor; to decouple this further you could store a void pointer or pointer to an interface.
        BasicAudioProcessor &audioProcessor;

        // Atomic flag to track parameter changes
        juce::Atomic<bool> parametersChanged{false};

        // Mono DSP chain for the EQ (holds filter coefficients)
        MonoChain monoChain;

        // Updates the DSP chain based on the latest parameter values
        void updateChain();

        // Background image for the response curve component
        juce::Image background;

        // Helper methods to get the areas to render and analyze the response curve
        juce::Rectangle<int> getRenderArea();
        juce::Rectangle<int> getAnalysisArea();

        // Path producers for the left and right FFT analysis
        PathProducer leftPathProducer, rightPathProducer;

        // Flag to enable/disable FFT analysis overlay
        bool shouldShowFFTAnalysis = true;
    };
}
