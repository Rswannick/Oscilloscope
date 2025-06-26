/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h"

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/Background/Background.h"
#include "GUI/ChannelControls/CH1.h"
#include "GUI/ChannelControls/CH2.h"
#include "GUI/GlobalControls/Global.h"
#include "GUI/Top Bar/TopBar.h"
#include "LookAndFeel/LAF.h"

#include "GUI/Oscilloscope/OScopeComponent.h"
#include "GUI/Oscilloscope/Oscilloscope_Screen.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef JUCE_IOS
#define JUCE_IOS 0 // Define a default value if JUCE_IOS is not defined
#endif


//==============================================================================
/**
*/

class OscilliscopeAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Button::Listener, juce::Slider::Listener, juce::Timer, juce::ComboBox::Listener
{
public:
    OscilliscopeAudioProcessorEditor (OscilliscopeAudioProcessor&);
    ~OscilliscopeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void comboBoxChanged(juce::ComboBox *box) override;
    void sliderValueChanged(juce::Slider *slider) override;
    void buttonStateChanged(juce::Button *button) override;
    void buttonClicked(juce::Button *button) override;
    void timerCallback() override;

    void setOSCColor1();
    void setOSCColor2();
    void setPresetColor1();
    void setPresetColor2();

    void addComponents();
    void addListeners();
    void addLookAndFeel();
    void setGUISize();
    
    juce::Rectangle<int> setiOSBounds(juce::Rectangle<int> iOSBounds);
    
    float ratio { 2436 / 1125 }, sizeX { 800 / ratio }, sizeY { 800  };
    
    //Open Settings Menu
    void openSettings();

private:
    OscilliscopeAudioProcessor& audioProcessor;
    //LookAndFeel
    LAFComponent cLAF;
    //Settings menu
    juce::ImageButton mSettings;
    
    BackgroundComponent cBackground;
    GlobalComponent cGlobal;
    CHComponent cCH1, cCH2;
    OscilloscoeScreen oScopeScreen;
    
    TopBarComponent cTopBar;
    juce::ToggleButton mBypass, mPolarity, mXYMode, mFullscreen;
    juce::TextButton blackBox;
    
    using Attachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    
    std::unique_ptr<Attachment> mBypassAttachment, mPolarityAttachment, mXYAttachment, mFSAttachment;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscilliscopeAudioProcessorEditor)
};
