#include "PluginProcessor.h"
#include "PluginEditor.h"

DraggableLoopComponent::DraggableLoopComponent(const juce::String &name, const juce::Colour &color)
    : name(name), color(color)
{
  setMouseCursor(juce::MouseCursor::DraggingHandCursor);
}

DraggableLoopComponent::~DraggableLoopComponent()
{
}

void DraggableLoopComponent::paint(juce::Graphics &g)
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

void DraggableLoopComponent::mouseDown(const juce::MouseEvent &e)
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
void DraggableLoopComponent::mouseDrag(const juce::MouseEvent &e)
{
  // Already handled by mouseDown
}

void DraggableLoopComponent::setLoopData(const juce::MemoryBlock &data, const juce::String &format)
{
  loopData = data;
  fileFormat = format;
}

LoopGeneratorAudioProcessorEditor::LoopGeneratorAudioProcessorEditor(LoopGeneratorAudioProcessor &p,
                                                                     juce::AudioProcessorValueTreeState &params)
    : AudioProcessorEditor(&p), audioProcessor(p), parameters(params)
{
  midiLoopComponent.reset(new DraggableLoopComponent("MIDI Loop", juce::Colours::blue.withAlpha(0.6f)));
  audioLoopComponent.reset(new DraggableLoopComponent("Audio Loop", juce::Colours::green.withAlpha(0.6f)));
  addAndMakeVisible(midiLoopComponent.get());
  addAndMakeVisible(audioLoopComponent.get());

  setupSliders();
  addAndMakeVisible(generateMidiButton);
  addAndMakeVisible(generateAudioButton);
  generateMidiButton.onClick = [this]
  { generateMidiLoop(); };
  generateAudioButton.onClick = [this]
  { generateAudioLoop(); };

  setResizeLimits(400, 400, 2000, 2000);
  setSize(500, 400);
  setInterceptsMouseClicks(false, true);
}

LoopGeneratorAudioProcessorEditor::~LoopGeneratorAudioProcessorEditor()
{
}

void LoopGeneratorAudioProcessorEditor::setupSliders()
{
  // Set up sliders and attachments
  addAndMakeVisible(numBarsSlider);
  addAndMakeVisible(beatsPerBarSlider);
  addAndMakeVisible(bpmSlider);
  addAndMakeVisible(waveformSelector);
  addAndMakeVisible(frequencySlider);

  // Configure sliders
  numBarsSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  numBarsSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);

  beatsPerBarSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  beatsPerBarSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);

  bpmSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  bpmSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);

  frequencySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
  frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);

  // Configure waveform selector
  waveformSelector.addItem("Sine", 1);
  waveformSelector.addItem("Square", 2);
  waveformSelector.addItem("Saw", 3);
  waveformSelector.addItem("Triangle", 4);

  // Add labels
  addAndMakeVisible(numBarsLabel);
  addAndMakeVisible(beatsPerBarLabel);
  addAndMakeVisible(bpmLabel);
  addAndMakeVisible(waveformLabel);
  addAndMakeVisible(frequencyLabel);

  // Create attachments for parameters
  numBarsAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
      parameters, "numBars", numBarsSlider));

  beatsPerBarAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
      parameters, "beatsPerBar", beatsPerBarSlider));

  bpmAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
      parameters, "bpm", bpmSlider));

  waveformAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
      parameters, "waveform", waveformSelector));

  frequencyAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
      parameters, "frequency", frequencySlider));
}

void LoopGeneratorAudioProcessorEditor::paint(juce::Graphics &g)
{
  // Fill the background
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  // Draw the title
  g.setColour(juce::Colours::white);
  g.setFont(18.0f);
  g.drawText("Loop Generator", getLocalBounds().removeFromTop(30), juce::Justification::centred);
}

void LoopGeneratorAudioProcessorEditor::resized()
{
  // if our children haven’t been created yet, bail out
  if (!midiLoopComponent || !audioLoopComponent)
    return;

  constexpr int minSize = 400;
  if (getWidth() < minSize || getHeight() < minSize)
    return;

  auto safeReduced = [](juce::Rectangle<int> r, int mx, int my)
  {
    int w = juce::jmax(0, r.getWidth() - 2 * mx);
    int h = juce::jmax(0, r.getHeight() - 2 * my);
    return juce::Rectangle<int>(r.getX() + mx,
                                r.getY() + my,
                                w, h);
  };

  auto area = getLocalBounds().reduced(10);

  area.removeFromTop(30);

  // Controls section (180px)
  auto controlsArea = area.removeFromTop(180);
  auto rhythmControls = controlsArea.removeFromLeft(250);
  auto audioControls = controlsArea;

  const int sliderW = 70;
  const int labelH = 20;

  // Rhythm: Number of Bars
  numBarsLabel.setBounds(rhythmControls.removeFromTop(labelH));
  {
    auto sliderArea = rhythmControls.removeFromTop(80);
    int x = sliderArea.getX() + (sliderArea.getWidth() - sliderW) / 2;
    numBarsSlider.setBounds(x, sliderArea.getY(), sliderW, 80);
  }
  rhythmControls.removeFromTop(10); // spacing

  // Rhythm: Beats per Bar
  beatsPerBarLabel.setBounds(rhythmControls.removeFromTop(labelH));
  {
    auto sliderArea = rhythmControls.removeFromTop(80);
    int x = sliderArea.getX() + (sliderArea.getWidth() - sliderW) / 2;
    beatsPerBarSlider.setBounds(x, sliderArea.getY(), sliderW, 80);
  }

  // Audio: Waveform chooser
  waveformLabel.setBounds(audioControls.removeFromTop(labelH));
  waveformSelector.setBounds(audioControls.removeFromTop(25).reduced(0, 5));
  audioControls.removeFromTop(10);

  // Audio: Frequency slider
  frequencyLabel.setBounds(audioControls.removeFromTop(labelH));
  {
    auto sliderArea = audioControls.removeFromTop(80);
    int x = sliderArea.getX() + (sliderArea.getWidth() - sliderW) / 2;
    frequencySlider.setBounds(x, sliderArea.getY(), sliderW, 80);
  }

  // BPM in the centre
  {
    auto bpmArea = area.removeFromTop(60);
    bpmLabel.setBounds(bpmArea.removeFromTop(labelH));
    int x = bpmArea.getX() + (bpmArea.getWidth() - sliderW) / 2;
    bpmSlider.setBounds(x, bpmArea.getY(), sliderW, 40);
  }

  // Buttons
  {
    auto buttonArea = area.removeFromTop(40);
    auto midiButtonArea = buttonArea.removeFromLeft(buttonArea.getWidth() / 2);
    generateMidiButton.setBounds(midiButtonArea.reduced(10, 5));
    generateAudioButton.setBounds(buttonArea.reduced(10, 5));
  }

  // Draggable Loop Components
  {
    auto dragArea = area.removeFromTop(80);
    auto midiLoopArea = dragArea.removeFromLeft(dragArea.getWidth() / 2);
    auto audioLoopArea = dragArea; // whatever’s left

    midiLoopComponent->setBounds(safeReduced(midiLoopArea, 10, 5));
    audioLoopComponent->setBounds(safeReduced(audioLoopArea, 10, 5));
  }
}

void LoopGeneratorAudioProcessorEditor::generateMidiLoop()
{
  // Get parameters
  int numBars = static_cast<int>(*parameters.getRawParameterValue("numBars"));
  int beatsPerBar = static_cast<int>(*parameters.getRawParameterValue("beatsPerBar"));
  float bpm = *parameters.getRawParameterValue("bpm");

  // Generate MIDI loop
  juce::MidiMessageSequence midiLoop = audioProcessor.generateMidiLoop(numBars, beatsPerBar, bpm);

  // Create MIDI file
  juce::MemoryBlock midiFileData = createMidiFile(midiLoop);

  // Update draggable component
  midiLoopComponent->setLoopData(midiFileData, "mid");
}

void LoopGeneratorAudioProcessorEditor::generateAudioLoop()
{
  // Get parameters
  float bpm = *parameters.getRawParameterValue("bpm");
  int numBars = static_cast<int>(*parameters.getRawParameterValue("numBars"));
  int beatsPerBar = static_cast<int>(*parameters.getRawParameterValue("beatsPerBar"));

  // Calculate the length of the audio in samples
  double sampleRate = audioProcessor.getSampleRate();
  if (sampleRate < 8000) // If we're not in a DAW, use a default
    sampleRate = 44100.0;

  double secondsPerBeat = 60.0 / bpm;
  double loopLengthInSeconds = numBars * beatsPerBar * secondsPerBeat;
  int lengthInSamples = static_cast<int>(loopLengthInSeconds * sampleRate);

  // Generate audio loop
  juce::AudioBuffer<float> audioLoop = audioProcessor.generateAudioLoop(lengthInSamples, sampleRate);

  // Create WAV file
  juce::MemoryBlock wavFileData = createWavFile(audioLoop, sampleRate);

  // Update draggable component
  audioLoopComponent->setLoopData(wavFileData, "wav");
}

juce::MemoryBlock LoopGeneratorAudioProcessorEditor::createMidiFile(const juce::MidiMessageSequence &midiSequence)
{
  juce::MemoryBlock result;

  juce::MidiFile midiFile;
  midiFile.setTicksPerQuarterNote(960);
  midiFile.addTrack(midiSequence);

  // Write to memory
  juce::MemoryOutputStream stream(result, false);
  midiFile.writeTo(stream);

  return result;
}

juce::MemoryBlock LoopGeneratorAudioProcessorEditor::createWavFile(const juce::AudioBuffer<float> &audioBuffer, double sampleRate)
{
  juce::MemoryBlock result;

  // Set up an output stream
  juce::WavAudioFormat wavFormat;
  std::unique_ptr<juce::AudioFormatWriter> writer(wavFormat.createWriterFor(
      new juce::MemoryOutputStream(result, false),
      sampleRate,
      audioBuffer.getNumChannels(),
      16, // bit depth
      {}, 0));

  if (writer != nullptr)
  {
    writer->writeFromAudioSampleBuffer(audioBuffer, 0, audioBuffer.getNumSamples());
  }

  return result;
}
