#include "../GUI/ResponseCurveComponent.h"

namespace GUI
{
    ResponseCurveComponent::ResponseCurveComponent(BasicAudioProcessor &p)
        : audioProcessor(p),
          leftPathProducer(audioProcessor.getLeftChannelFifo()),
          rightPathProducer(audioProcessor.getRightChannelFifo())
    {
        // Register this component as a listener for each parameter in the processor.
        for (auto *param : audioProcessor.getParameters())
            param->addListener(this);

        updateChain(); // Initialize the DSP chain

        startTimerHz(60); // Set the timer to refresh at 60 Hz
    }

    ResponseCurveComponent::~ResponseCurveComponent()
    {
        // Deregister this component as a listener from each parameter.
        for (auto *param : audioProcessor.getParameters())
            param->removeListener(this);
    }

    void ResponseCurveComponent::parameterValueChanged(int /*parameterIndex*/, float /*newValue*/)
    {
        // Mark that parameters have changed so the DSP chain can be updated in the timer callback.
        parametersChanged.set(true);
    }

    void ResponseCurveComponent::timerCallback()
    {
        if (shouldShowFFTAnalysis)
        {
            auto fftBounds = getAnalysisArea().toFloat();
            auto sampleRate = audioProcessor.getSampleRate();

            leftPathProducer.process(fftBounds, sampleRate);
            rightPathProducer.process(fftBounds, sampleRate);
        }

        // Update DSP chain if parameters have changed
        if (parametersChanged.compareAndSetBool(false, true))
            updateChain();

        repaint(); // Request a repaint to show updated response curve/analysis
    }

    void ResponseCurveComponent::updateChain()
    {
        // Retrieve current settings from the processor's APVTS
        auto chainSettings = getChainSettings(audioProcessor.getAPVTS());

        // Update bypass states for each filter in the DSP chain
        monoChain.setBypassed<ChainPositions::LowCut>(chainSettings.lowCutBypassed);
        monoChain.setBypassed<ChainPositions::LowShelf>(chainSettings.lowShelfBypassed);
        monoChain.setBypassed<ChainPositions::Peak1>(chainSettings.peakBypassed);
        monoChain.setBypassed<ChainPositions::Peak2>(chainSettings.peak2Bypassed);
        monoChain.setBypassed<ChainPositions::Peak3>(chainSettings.peak3Bypassed);
        monoChain.setBypassed<ChainPositions::Peak4>(chainSettings.peak4Bypassed);
        monoChain.setBypassed<ChainPositions::HighShelf>(chainSettings.highShelfBypassed);
        monoChain.setBypassed<ChainPositions::HighCut>(chainSettings.highCutBypassed);

        auto sampleRate = audioProcessor.getSampleRate();

        // Update the peak filter coefficients
        auto peakCoefficients = makePeakFilter(chainSettings.peakFreq, chainSettings.peakGainInDecibels, chainSettings.peakQuality, sampleRate);
        auto peak2Coefficients = makePeakFilter(chainSettings.peak2Freq, chainSettings.peak2GainInDecibels, chainSettings.peak2Quality, sampleRate);
        auto peak3Coefficients = makePeakFilter(chainSettings.peak3Freq, chainSettings.peak3GainInDecibels, chainSettings.peak3Quality, sampleRate);
        auto peak4Coefficients = makePeakFilter(chainSettings.peak4Freq, chainSettings.peak4GainInDecibels, chainSettings.peak4Quality, sampleRate);
        updateCoefficients(monoChain.get<ChainPositions::Peak1>().coefficients, peakCoefficients);
        updateCoefficients(monoChain.get<ChainPositions::Peak2>().coefficients, peak2Coefficients);
        updateCoefficients(monoChain.get<ChainPositions::Peak3>().coefficients, peak3Coefficients);
        updateCoefficients(monoChain.get<ChainPositions::Peak4>().coefficients, peak4Coefficients);

        // Update the low and high shelf filter coefficients
        auto lowShelfCoefficients = makeShelfFilter(chainSettings.lowShelfFreq, chainSettings.lowShelfGain, chainSettings.lowShelfQ, sampleRate, false);
        auto highShelfCoefficients = makeShelfFilter(chainSettings.highShelfFreq, chainSettings.highShelfGain, chainSettings.highShelfQ, sampleRate, true);
        updateCoefficients(monoChain.get<ChainPositions::LowShelf>().coefficients, lowShelfCoefficients);
        updateCoefficients(monoChain.get<ChainPositions::HighShelf>().coefficients, highShelfCoefficients);

        // Update the low and high cut filter coefficients
        auto lowCutCoefficients = makeCutFilter(chainSettings.lowCutFreq, chainSettings.lowCutSlope, sampleRate, false);
        auto highCutCoefficients = makeCutFilter(chainSettings.highCutFreq, chainSettings.highCutSlope, sampleRate, true);
        updateCutFilter(monoChain.get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCutSlope);
        updateCutFilter(monoChain.get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCutSlope);
    }

    void ResponseCurveComponent::paint(juce::Graphics &g)
    {
        using namespace juce;

        g.fillAll(Colours::black); // fill the background with black

        g.drawImage(background, getLocalBounds().toFloat()); // draw the background image

        // get dimensions
        auto responseArea = getAnalysisArea(); // get the render area for the response curve component
        auto w = responseArea.getWidth();      // get the width of the response area

        // individual chain elements
        auto &lowcut = monoChain.get<ChainPositions::LowCut>();       // get the low cut filter from the chain
        auto &lowshelf = monoChain.get<ChainPositions::LowShelf>();   // get the low shelf filter from the chain
        auto &peak = monoChain.get<ChainPositions::Peak1>();          // get the peak filter from the chain
        auto &peak2 = monoChain.get<ChainPositions::Peak2>();         // get the peak filter from the chain
        auto &peak3 = monoChain.get<ChainPositions::Peak3>();         // get the peak filter from the chain
        auto &peak4 = monoChain.get<ChainPositions::Peak4>();         // get the peak filter from the chain
        auto &highshelf = monoChain.get<ChainPositions::HighShelf>(); // get the high shelf filter from the chain
        auto &highcut = monoChain.get<ChainPositions::HighCut>();     // get the high cut filter from the chain

        // get the frequency response of the EQ
        auto sampleRate = audioProcessor.getSampleRate(); // get the sample rate of the audio processor

        std::vector<double> mags; // one magnitude value for each pixel in the response area
        mags.resize(w);           // resize the vector to the width of the response area

        // iterate through each pixel and compute the magnitude of that frequency, which is expressed in gain units
        for (int i = 0; i < w; ++i)
        {
            double mag = 1.f;                                             // starting gain of 1
            auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0); // map the value to a frequency between 20 and 20kHz (human-hearable frequency range)

            // check if band is bypassed
            if (!monoChain.isBypassed<ChainPositions::Peak1>())
                mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the peak filter for that frequency

            if (!monoChain.isBypassed<ChainPositions::Peak2>())
                mag *= peak2.coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the second peak filter for that frequency

            if (!monoChain.isBypassed<ChainPositions::Peak3>())
                mag *= peak3.coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the third peak filter for that frequency

            if (!monoChain.isBypassed<ChainPositions::Peak4>())
                mag *= peak4.coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the fourth peak filter for that frequency

            if (!monoChain.isBypassed<ChainPositions::LowShelf>())
                mag *= lowshelf.coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the low shelf filter for that frequency

            if (!monoChain.isBypassed<ChainPositions::HighShelf>())
                mag *= highshelf.coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the high shelf filter for that frequency

            if (!monoChain.isBypassed<ChainPositions::LowCut>())
            {
                if (!lowcut.isBypassed<0>())
                    mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the low cut filter for that frequency
                if (!lowcut.isBypassed<1>())
                    mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the low cut filter for that frequency
                if (!lowcut.isBypassed<2>())
                    mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the low cut filter for that frequency
                if (!lowcut.isBypassed<3>())
                    mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the low cut filter for that frequency
            }
            if (!monoChain.isBypassed<ChainPositions::HighCut>())
            {
                if (!highcut.isBypassed<0>())
                    mag *= highcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the high cut filter for that frequency
                if (!highcut.isBypassed<1>())
                    mag *= highcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the high cut filter for that frequency
                if (!highcut.isBypassed<2>())
                    mag *= highcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the high cut filter for that frequency
                if (!highcut.isBypassed<3>())
                    mag *= highcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate); // get the magnitude of the high cut filter for that frequency
            }
            mags[i] = Decibels::gainToDecibels(mag); // convert the magnitude to decibels and store it in the vector
        }

        // convert vector of magnitudes to a path, then draw it
        Path responseCurve; // path to draw the frequency response curve

        const double outputMin = responseArea.getBottom(); // minimum output value (bottom of the response area)
        const double outputMax = responseArea.getY();      // maximum output value (top of the response area)
        auto map = [outputMin, outputMax](double input)
        {
            // peak control can go from -24 to 24 dB
            return jmap(input, -24.0, 24.0, outputMin, outputMax); // map the input value to the output range
        };

        // new subpath with the first magnitude
        responseCurve.startNewSubPath(responseArea.getX(), map(mags.front())); // start the path at the left edge of the response area

        for (size_t i = 1; i < mags.size(); ++i) // iterate through the magnitudes
        {
            // add a line to the path for each magnitude value
            responseCurve.lineTo(responseArea.getX() + i, map(mags[i])); // add a line to the path for each magnitude value
        }

        if (shouldShowFFTAnalysis)
        {
            auto leftChannelFFTPath = leftPathProducer.getPath();                                                       // get the path for the left channel FFT data
            leftChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY())); // apply a translation to the path to move it to the response area

            // draw the path
            g.setColour(Colours::lightblue);                       // set the colour to lightblue
            g.strokePath(leftChannelFFTPath, PathStrokeType(1.f)); // stroke the path with a width of 1 pixel

            auto rightChannelFFTPath = rightPathProducer.getPath();                                                      // get the path for the right channel FFT data
            rightChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY())); // apply a translation to the path to move it to the response area

            // draw the path
            g.setColour(Colours::lightyellow);                      // set the colour to lightyellow
            g.strokePath(rightChannelFFTPath, PathStrokeType(1.f)); // stroke the path with a width of 1 pixel
        }
        g.setColour(Colours::white);                      // set the colour to white
        g.strokePath(responseCurve, PathStrokeType(2.f)); // stroke the path with a width of 2 pixels

        Path border;

        border.setUsingNonZeroWinding(false);

        border.addRoundedRectangle(getRenderArea(), 4);
        border.addRectangle(getLocalBounds());

        g.setColour(Colours::black);

        g.fillPath(border);

        drawTextLabels(g);

        g.setColour(Colours::orange);                                // set the colour to orange
        g.drawRoundedRectangle(getRenderArea().toFloat(), 4.f, 1.f); // draw a rounded rectangle around the response area
    }

    std::vector<float> ResponseCurveComponent::getFrequencies()
    {
        return {20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};
    }

    std::vector<float> ResponseCurveComponent::getGains()
    {
        return {-24, -12, 0, 12, 24};
    }

    std::vector<float> ResponseCurveComponent::getXs(const std::vector<float> &freqs, float left, float width)
    {
        std::vector<float> xs;
        for (auto f : freqs)
        {
            auto normX = juce::mapFromLog10(f, 20.f, 20000.f);
            xs.push_back(left + width * normX);
        }
        return xs;
    }

    void ResponseCurveComponent::drawTextLabels(juce::Graphics &g)
    {
        using namespace juce;
        g.setColour(Colours::lightgrey);
        const int fontHeight = 10;
        g.setFont(fontHeight);

        auto renderArea = getAnalysisArea();
        auto left = renderArea.getX();
        auto top = renderArea.getY();
        auto bottom = renderArea.getBottom();
        auto width = renderArea.getWidth();

        auto freqs = getFrequencies();
        auto xs = getXs(freqs, left, width);

        // Draw frequency labels
        for (size_t i = 0; i < freqs.size(); ++i)
        {
            float f = freqs[i];
            float x = xs[i];
            bool addK = f > 999.f;
            juce::String str;
            if (addK)
            {
                f /= 1000.f;
                str << f << "kHz";
            }
            else
            {
                str << f << "Hz";
            }

            auto textWidth{juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), str)};

            juce::Rectangle<int> r;
            r.setSize(textWidth, fontHeight);
            r.setCentre(x, 0);
            r.setY(1);
            g.drawFittedText(str, r, juce::Justification::centred, 1);
        }

        // Draw gain labels
        auto gains = getGains();
        for (auto gDb : gains)
        {
            auto y = juce::jmap(gDb, -24.f, 24.f, static_cast<float>(bottom), static_cast<float>(top));
            juce::String str;
            if (gDb > 0)
                str << "+";
            str << gDb;

            auto textWidth{juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), str)};

            juce::Rectangle<int> r;
            r.setSize(textWidth, fontHeight);
            r.setX(getWidth() - textWidth);
            r.setCentre(r.getCentreX(), y);
            g.setColour(gDb == 0.f ? juce::Colour(0u, 172u, 1u) : juce::Colours::lightgrey);
            g.drawFittedText(str, r, juce::Justification::centredLeft, 1);

            // Draw reference gain value on left side
            str.clear();
            str << (gDb - 24.f);
            r.setX(1);
            auto textWidth2{juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), str)};
            r.setSize(textWidth2, fontHeight);
            g.setColour(juce::Colours::lightgrey);
            g.drawFittedText(str, r, juce::Justification::centredLeft, 1);
        }
    }

    juce::Rectangle<int> ResponseCurveComponent::getRenderArea()
    {
        auto bounds = getLocalBounds();
        bounds.removeFromTop(14);
        bounds.removeFromBottom(4);
        bounds.removeFromLeft(20);
        bounds.removeFromRight(20);
        return bounds;
    }

    juce::Rectangle<int> ResponseCurveComponent::getAnalysisArea()
    {
        auto bounds = getRenderArea();
        bounds.removeFromTop(4);
        bounds.removeFromBottom(4);
        return bounds;
    }

    void ResponseCurveComponent::resized()
    {
        // Generate a new background image based on the component's size.
        background = juce::Image(juce::Image::PixelFormat::ARGB, getWidth(), getHeight(), true);
        juce::Graphics g(background);

        // Draw frequency grid lines
        juce::Array<float> freqs{20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};
        auto renderArea = getRenderArea();
        auto left = renderArea.getX();
        auto right = renderArea.getRight();
        auto top = renderArea.getY();
        auto bottom = renderArea.getBottom();
        auto width = renderArea.getWidth();

        juce::Array<float> xs;
        for (auto freq : freqs)
        {
            auto normX = juce::mapFromLog10(freq, 20.f, 20000.f);
            xs.add(left + width * normX);
        }

        g.setColour(juce::Colours::dimgrey);
        for (auto x : xs)
            g.drawVerticalLine(x, top, bottom);

        // Draw gain grid lines
        juce::Array<float> gains{-24, -12, 0, 12, 24};
        for (auto gDb : gains)
        {
            auto y = juce::jmap(gDb, -24.f, 24.f, static_cast<float>(bottom), static_cast<float>(top));
            g.setColour(gDb == 0.f ? juce::Colours::lightblue : juce::Colours::dimgrey);
            g.drawHorizontalLine(y, left, right);
        }

        // Draw frequency and gain labels
        g.setColour(juce::Colours::lightgrey);
        const int fontHeight = 10;
        g.setFont(fontHeight);

        for (int i = 0; i < xs.size(); ++i)
        {
            auto x = xs[i];
            auto freq = freqs[i];
            bool addK = freq > 999.f;
            juce::String str;
            if (addK)
            {
                freq /= 1000.f;
                str << freq << "kHz";
            }
            else
            {
                str << freq << "Hz";
            }
            auto textWidth{juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), str)};
            juce::Rectangle<int> r;
            r.setSize(textWidth, fontHeight);
            r.setCentre(x, 0);
            r.setY(1);
            g.drawFittedText(str, r, juce::Justification::centred, 1);
        }
    }
}
