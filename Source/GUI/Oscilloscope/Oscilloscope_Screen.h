/*
  ==============================================================================

    ScopeComponent.h
    Created: 10 Sep 2022 8:37:38pm
    Author:  Ryan Swannick

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../../PluginProcessor.h"
#include "../../GUI/Oscilloscope/OScopeComponent.h"
#include "Oscilloscope_GUI.h"
#include "Oscilloscope_Overlay.h"

//
#ifndef JUCE_IOS
#define JUCE_IOS 0 // Define a default value if JUCE_IOS is not defined
#endif

class OscilloscoeScreen  : public juce::Component
{
public:
    OscilloscoeScreen(OscilliscopeAudioProcessor&);
    ~OscilloscoeScreen() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    bool shouldPaint { false };
    bool drawOverlay { true };
    
    float rmsValue = { 0 };
    float peakValue = { 0 };
    float dspVoltage = 1;
    float timeSeconds = 1;
    
    OScopeComponent cOscope, cOscope2;
    OscilloscopeOverlay cOverlay;
    
private:
    OscilliscopeAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscilloscoeScreen)
};
