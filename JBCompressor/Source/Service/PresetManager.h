#pragma once

#include <JuceHeader.h>
#include <map>

namespace Service
{
	using namespace juce;

	struct DefaultPreset
	{
		juce::String name;
		std::map<juce::String, float> parameters;
	};

	class PresetManager : public ValueTree::Listener
	{
	public:
		static const File defaultDirectory;
		static const String extension;
		static const String presetNameProperty;

		PresetManager(AudioProcessorValueTreeState &apvts,
					  const DefaultPreset *defaultPresets = nullptr,
					  int numDefaultPresets = 0);

		void savePreset(const String &presetName);
		void deletePreset(const String &presetName);
		void loadPreset(const String &presetName);
		int loadNextPreset();
		int loadPreviousPreset();
		StringArray getAllPresets() const;
		String getCurrentPreset() const;

	private:
		void valueTreeRedirected(ValueTree &treeWhichHasBeenChanged) override;

		AudioProcessorValueTreeState &valueTreeState;
		Value currentPreset;
	};
}
