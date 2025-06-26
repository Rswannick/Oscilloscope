/*
  ==============================================================================

    TopBar.h
    Created: 1 Feb 2024 11:07:33pm
 Author: Ryan Swannick!

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#include "../../PluginProcessor.h"
#include "../EnterKeyListener.h"
#include "../../LookAndFeel/LAF.h"

#include "Settings/InfoPage.h"
#include "Settings/SettingsPage.h"
#include "Preset Manager/PresetManager.h"
#include "Master/MasterControls.h"

class TopBarComponent  : public juce::Component,  juce::Timer
{
public:
    TopBarComponent(OscilliscopeAudioProcessor&);
    ~TopBarComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void shutdown();
    void restoreGUISize();
    
    void openSettings();
    void openInfo();
    void openVolume();
    void setPresetBarText(juce::String presetName);

    void timerCallback() override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    
    void openAlertBoxForSliderValue(juce::Slider* slider);

    void createPresetDirectory();
    juce::String appName { "pocketOscilloscope" };
    juce::String presetSuffix { ".pocketOscilloscope" };

    juce::String findDocumentsFolder(bool isIOS, juce::String documentsPath);
    SettingsPageComponent cSettings;
    
    //Master Section
    MasterControlComponent cMasterSection;
    
    //Preset Manager
    PresetManagerComponent cPresetManager;
    
private:
    OscilliscopeAudioProcessor& audioProcessor;
    LAFComponent cLAF;
    //Settings & Info menu
    InfoPageComponent cInfo;
    juce::ToggleButton mInfo, mSettings, mVolume;
    
    bool sliderMouseDown {false}, sliderMouseUp{false}, sliderMouseDragging { false };
    int mouseDownTime { 0 }, mouseUpTime { 0 };
    juce::Slider* currentlyHeldSlider;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TopBarComponent)
};
