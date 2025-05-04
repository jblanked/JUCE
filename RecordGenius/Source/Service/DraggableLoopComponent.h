#pragma once
#include <JuceHeader.h>

class DraggableLoopComponent : public juce::Component
{
public:
    DraggableLoopComponent(const juce::String &name, const juce::Colour &color)
        : name(name), color(color)
    {
        setMouseCursor(juce::MouseCursor::DraggingHandCursor);
    }

    ~DraggableLoopComponent()
    {
    }

    void paint(juce::Graphics &g)
    {
        // Create a rounded-rectangle background
        g.setColour(color);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);

        // Add text
        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        g.drawText(name, getLocalBounds(), juce::Justification::centred);

        // Add instruction text
        g.setFont(10.0f);
        g.drawText("Drag me to your DAW", getLocalBounds().removeFromBottom(15),
                   juce::Justification::centredBottom);
    }

    void mouseDown(const juce::MouseEvent &e)
    {
        // check if we have data
        if (loopData.getSize() == 0)
        {
            return;
        }

        // Create a temporary file in system temp directory
        juce::File tempDirectory = juce::File::getSpecialLocation(juce::File::tempDirectory);
        juce::String filename = name.replaceCharacter(' ', '_') + "_" +
                                juce::String(juce::Random::getSystemRandom().nextInt(10000)) +
                                "." + fileFormat;
        juce::File tempFile = tempDirectory.getChildFile(filename);

        // Make sure we can write to this location
        bool fileCreated = tempFile.create();

        if (fileCreated)
        {
            // Now write the data
            juce::FileOutputStream stream(tempFile);
            if (stream.openedOk())
            {
                bool writeSuccess = stream.write(loopData.getData(), loopData.getSize());
                stream.flush();

                if (writeSuccess)
                {
                    juce::StringArray files;
                    files.add(tempFile.getFullPathName());
                    juce::DragAndDropContainer *dragContainer = juce::DragAndDropContainer::findParentDragContainerFor(this);
                    if (dragContainer != nullptr)
                    {
                        dragContainer->performExternalDragDropOfFiles(files, true);
                    }
                    else
                    {
                        juce::Component *parent = getParentComponent();
                        if (parent != nullptr)
                        {
                            tempFile.revealToUser();
                        }
                    }
                }
            }
        }
    }

    void mouseDrag(const juce::MouseEvent &e)
    {
        // Already handled by mouseDown
    }

    void setLoopData(const juce::MemoryBlock &data, const juce::String &format)
    {
        loopData = data;
        fileFormat = format;
    }

private:
    juce::String name;
    juce::Colour color;
    juce::MemoryBlock loopData;
    juce::String fileFormat;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DraggableLoopComponent)
};
