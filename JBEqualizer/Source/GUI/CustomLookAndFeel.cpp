#include "../GUI/CustomLookAndFeel.h"
#include "../GUI/RotarySliderWithLabels.h"
namespace GUI
{
    void CustomLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                                             float sliderPosProportional, float rotaryStartAngle,
                                             float rotaryEndAngle, juce::Slider &slider)
    {
        using namespace juce;

        auto bounds = Rectangle<float>(x, y, width, height); // create a rectangle for the slider

        auto enabled = slider.isEnabled(); // check if the slider is enabled

        g.setColour(enabled ? Colours::grey : Colours::darkgrey); // set the colour based on the enabled state
        g.fillEllipse(bounds);                                    // fill the ellipse with the colour

        g.setColour(enabled ? Colours::lightblue : Colours::darkgrey); // set the colour based on the enabled state
        g.drawEllipse(bounds, 1.f);                                    // draw the ellipse with a width of 1 pixel

        if (auto *rswl = dynamic_cast<RotarySliderWithLabels *>(&slider))
        {
            auto center = bounds.getCentre(); // get the center of the ellipse

            Path p;                                                   // create a path for the slider knob
            Rectangle<float> r;                                       // create a rectangle for the slider knob
            r.setLeft(center.getX() - 2);                             // set the left edge of the rectangle to the center of the ellipse minus 2 pixels
            r.setRight(center.getX() + 2);                            // set the right edge of the rectangle to the center of the ellipse plus 2 pixels
            r.setTop(bounds.getY());                                  // set the top edge of the rectangle to the top of the ellipse
            r.setBottom(center.getY() - rswl->getTextHeight() * 1.5); // set the bottom edge of the rectangle to the center of the ellipse

            p.addRoundedRectangle(r, 2.f);              // add a rounded rectangle to the path for the slider knob
            jassert(rotaryStartAngle < rotaryEndAngle); // check that the start angle is less than the end angle

            auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle); // map the slider position to the angle of the knob

            p.applyTransform(AffineTransform::rotation(sliderAngRad, center.getX(), center.getY())); // rotate the path to the angle of the knob

            g.fillPath(p); // fill the path with the colour

            g.setFont(rswl->getTextHeight());                        // set the font size to the height of the text label
            auto text = rswl->getDisplayString();                    // get the text label for the slider
            auto strWidth = g.getCurrentFont().getStringWidth(text); // get the width of the text label

            r.setSize(strWidth + 4, rswl->getTextHeight() + 2); // set the size of the rectangle to the width and height of the text label
            r.setCentre(bounds.getCentre());

            g.setColour(enabled ? Colours::grey : Colours::darkgrey);
            g.fillRect(r); // fill the rectangle with the colour

            g.setColour(enabled ? Colours::lightblue : Colours::darkgrey);       // set the colour to white
            g.drawFittedText(text, r.toNearestInt(), Justification::centred, 1); // draw the text label in the rectangle
        }
    }

    void CustomLookAndFeel::drawToggleButton(juce::Graphics &g,
                                             juce::ToggleButton &toggleButton,
                                             bool shouldDrawButtonAsHighlighted,
                                             bool shouldDrawButtonAsDown)
    {
        using namespace juce;

        if (auto *pb = dynamic_cast<PowerButton *>(&toggleButton)) // check if the button is a power button
        {
            Path powerButton; // create a path for the power button

            auto bounds = toggleButton.getLocalBounds(); // get the bounds of the button

            auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 6; // get the size of the button
            auto r = bounds.withSizeKeepingCentre(size, size).toFloat(); // create a rectangle for the button

            float ang = 30.f; // 30.f;

            size -= 6;

            powerButton.addCentredArc(r.getCentreX(),
                                      r.getCentreY(),
                                      size * 0.5,
                                      size * 0.5,
                                      0.f,
                                      degreesToRadians(ang),
                                      degreesToRadians(360.f - ang),
                                      true); // create an arc for the button

            powerButton.startNewSubPath(r.getCentreX(), r.getY()); // start a new subpath at the top of the button
            powerButton.lineTo(r.getCentre());                     // draw a line to the center of the button

            PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved); // set the stroke type for the path

            auto color = toggleButton.getToggleState() ? Colours::dimgrey : Colours::lightblue;

            g.setColour(color);
            g.strokePath(powerButton, pst);
            g.drawEllipse(r, 2);
        }
        else if (auto *analyzerButton = dynamic_cast<AnalyzerButton *>(&toggleButton)) // check if the button is an analyzer button
        {
            auto color = !toggleButton.getToggleState() ? Colours::dimgrey : Colours::lightblue;

            g.setColour(color);

            auto bounds = toggleButton.getLocalBounds(); // get the bounds of the button
            g.drawRect(bounds);

            g.strokePath(analyzerButton->randomPath, PathStrokeType(1.f)); // draw the path for the button
        }
    }
}
