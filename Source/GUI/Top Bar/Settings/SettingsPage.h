/*
  ==============================================================================

    SettingsPage.h
    Created: 15 Jan 2024 9:25:59pm
 Author: Ryan Swannick!

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h"
#include "../../../LookAndFeel/LAF.h"

//====================================================&==========================

class SettingsPageComponent  : public juce::Component
{
public:
    SettingsPageComponent(juce::AudioProcessorValueTreeState& apvts);
    ~SettingsPageComponent() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setBackground();
    void addComponents();
    void updateDeviceManager();
    void selectColor();
    void selectColor2();
    bool audioDeviceNotSelected(bool deviceSelected);

    std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSettings;
    juce::AudioDeviceManager customDeviceManager;
    
    bool audioInputSelect { false };
    
    juce::Label
    
    mOSCColor1Label { "CH 1 COLOR", "CH 1 COLOR" },
    mOSCColor2Label { "CH 2 COLOR", "CH 2 COLOR" },
    mQualityLabel { "PITCH QUALITY", "PITCH QUALITY" };

    
    juce::ToggleButton
    mClose,
    mOSCColor1,
    mOSCColor2,
    mQuality;
    
    juce::Colour vSelectedColorOSC1, vSelectedColorOSC2;
    juce::ColourSelector mColorSelector, mColorSelector2;
    
private:
    LAFComponent cLAF;
    
    juce::Image backgroundImg;
    juce::ImageComponent mBackground;
    
    using buttonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    
    std::unique_ptr<buttonAttachment>
    
    qualityAttachment;
    
    juce::Viewport mSettingsViewPort; 
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsPageComponent)
};
