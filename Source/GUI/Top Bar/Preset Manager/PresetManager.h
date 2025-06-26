/*
  ==============================================================================

    PresetManager.h
    Created: 17 Sep 2023 1:30:57pm
    Author:  Ryan Swannick

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../../../PluginProcessor.h"
#include "../../../LookAndFeel/LAF.h"
#include "../../EnterKeyListener.h"

//More Definitions for Windows/PC
#ifndef JUCE_IOS
#define JUCE_IOS 0 // Define a default value if JUCE_IOS is not defined
#endif


//==============================================================================
/**
*/
class PresetManagerComponent : public juce::Component, juce::ComboBox::Listener
{
public:
    PresetManagerComponent (OscilliscopeAudioProcessor&);
    ~ PresetManagerComponent() override;

    //==============================================================================
    void paint(juce::Graphics &g) override;
    void resized() override;
    //All Platform
    void addComponents();
    void addListeners();
    void addNames();
    void addFunctions();
    void addLookAndFeel();
    void comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged)override;
    void addPresetDropDown();
    void addMoreOptions();
    void presetSaveAs();
    void presetSave();
    void presetDelete();
    void presetRename();
    void nextPreset();
    void prevPreset();
    void copyFactoryPresets();
    void setPresetBarText (juce::String presetName);

    juce::String getDocumentsPath ();
    juce::String getPresetsFolderPath ();
    juce::String presetsPath { "/OSC Audio/pocketOscilloscope/Presets/" };
    juce::String appGroupIdentifier = "group.oscaudio.pocketOscilloscope";
    juce::String presetExtension { ".pocketOscilloscope" };
    juce::String appName { "pocketOscilloscope" };
    juce::String documentsPath;
    //IOS
    void importURL (juce::URL url);
    void copySAPresetsAU();
    bool recalled { false };
    
    juce::Array<juce::File> deletedFileList;
    
    #if JUCE_IOS || JUCE_MAC
    juce::File container = juce::File::getContainerForSecurityApplicationGroupIdentifier("group.oscaudio.pocketOscilloscope");
    #else
    juce::File container = juce::File;
    #endif
    
    juce::ComboBox mLoadPresetDropDown {"LOAD PRESET"};


private:
    //Audio Processor Reference
    OscilliscopeAudioProcessor& audioProcessor;
    

    LAFComponent cLAF;
    
    juce::ComboBox mMoreOptionsDropDown { "" };

    juce::TextButton
    mNextPreset {">"},
    mPrevPreset {"<"};
    
    std::unique_ptr<juce::FileChooser> fc;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManagerComponent)
};
