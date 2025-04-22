#include "PresetManager.h"

namespace Service
{
	using namespace juce;

	const juce::File PresetManager::defaultDirectory{
		juce::File::getSpecialLocation(juce::File::SpecialLocationType::commonDocumentsDirectory)
			.getChildFile(ProjectInfo::companyName)
			.getChildFile(ProjectInfo::projectName)};
	const juce::String PresetManager::extension{"preset"};
	const juce::String PresetManager::presetNameProperty{"presetName"};

	PresetManager::PresetManager(AudioProcessorValueTreeState &apvts,
								 const DefaultPreset *defaults,
								 int numDefaults)
		: valueTreeState(apvts)
	{
		// Ensure the preset directory exists.
		if (!defaultDirectory.exists())
		{
			const auto result = defaultDirectory.createDirectory();
			if (result.failed())
			{
				DBG("Could not create preset directory: " + result.getErrorMessage());
				jassertfalse;
			}
		}

		// Add ourselves as a listener to the APVTS state.
		valueTreeState.state.addListener(this);
		currentPreset.referTo(valueTreeState.state.getPropertyAsValue(presetNameProperty, nullptr));

		// If default presets are provided, write them to disk.
		if (defaults != nullptr && getAllPresets().isEmpty())
		{
			for (int i = 0; i < numDefaults; ++i)
			{
				const auto &preset = defaults[i];

				// Start by copying the complete state from the APVTS.
				// This ensures we have the correct type and any structure defined by the APVTS.
				juce::ValueTree state = valueTreeState.copyState();

				// Update the preset name on the root node.
				state.setProperty(presetNameProperty, preset.name, nullptr);

				// Update all parameter attributes on the root.
				for (const auto &parameter : preset.parameters)
				{
					state.setProperty(parameter.first, parameter.second, nullptr);
				}

				// Remove any pre-existing child nodes.
				state.removeAllChildren(nullptr);

				// Add a child <PARAM> node for each parameter.
				// This will produce XML like:
				//   <PARAM id="lowCutFrequency" value="..."/>
				for (const auto &parameter : preset.parameters)
				{
					juce::ValueTree paramNode("PARAM");
					paramNode.setProperty("id", parameter.first, nullptr);
					// Set the "value" attribute – if you prefer to omit it for some parameters,
					// you can add logic here to only set it for specific parameters.
					paramNode.setProperty("value", parameter.second, nullptr);
					state.addChild(paramNode, -1, nullptr);
				}

				// Create the XML from the updated state and write it to a file.
				std::unique_ptr<juce::XmlElement> xml(state.createXml());
				auto presetFile = defaultDirectory.getChildFile(preset.name + "." + extension);
				if (!xml->writeTo(presetFile))
				{
					DBG("Could not write preset file for: " + preset.name);
					jassertfalse;
				}
			}
		}
	}

	void PresetManager::savePreset(const String &presetName)
	{
		if (presetName.isEmpty())
			return;

		currentPreset.setValue(presetName);
		const auto xml = valueTreeState.copyState().createXml();
		const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
		if (!xml->writeTo(presetFile))
		{
			DBG("Could not create preset file: " + presetFile.getFullPathName());
			jassertfalse;
		}
	}

	void PresetManager::deletePreset(const String &presetName)
	{
		if (presetName.isEmpty())
			return;

		const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
		if (!presetFile.existsAsFile())
		{
			DBG("Preset file " + presetFile.getFullPathName() + " does not exist");
			jassertfalse;
			return;
		}
		if (!presetFile.deleteFile())
		{
			DBG("Preset file " + presetFile.getFullPathName() + " could not be deleted");
			jassertfalse;
			return;
		}
		currentPreset.setValue("");
	}

	// Modified loadPreset: If the name matches a factory default, then reapply the in–code values.
	void PresetManager::loadPreset(const String &presetName)
	{
		if (presetName.isEmpty())
			return;

		const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
		if (!presetFile.existsAsFile())
		{
			DBG("Preset file " + presetFile.getFullPathName() + " does not exist");
			jassertfalse;
			return;
		}
		XmlDocument xmlDocument{presetFile};
		const auto valueTreeToLoad = ValueTree::fromXml(*xmlDocument.getDocumentElement());
		valueTreeState.replaceState(valueTreeToLoad);
		currentPreset.setValue(presetName);
	}

	int PresetManager::loadNextPreset()
	{
		const auto allPresets = getAllPresets();
		if (allPresets.isEmpty())
			return -1;
		const auto currentIndex = allPresets.indexOf(currentPreset.toString());
		const auto nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;
		loadPreset(allPresets.getReference(nextIndex));
		return nextIndex;
	}

	int PresetManager::loadPreviousPreset()
	{
		const auto allPresets = getAllPresets();
		if (allPresets.isEmpty())
			return -1;
		const auto currentIndex = allPresets.indexOf(currentPreset.toString());
		const auto previousIndex = currentIndex - 1 < 0 ? allPresets.size() - 1 : currentIndex - 1;
		loadPreset(allPresets.getReference(previousIndex));
		return previousIndex;
	}

	StringArray PresetManager::getAllPresets() const
	{
		StringArray presets;
		const auto fileArray = defaultDirectory.findChildFiles(
			File::TypesOfFileToFind::findFiles, false, "*." + extension);
		for (const auto &file : fileArray)
		{
			presets.add(file.getFileNameWithoutExtension());
		}
		return presets;
	}

	String PresetManager::getCurrentPreset() const
	{
		return currentPreset.toString();
	}

	void PresetManager::valueTreeRedirected(ValueTree &treeWhichHasBeenChanged)
	{
		currentPreset.referTo(treeWhichHasBeenChanged.getPropertyAsValue(presetNameProperty, nullptr));
	}
}
