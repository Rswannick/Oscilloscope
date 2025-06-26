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
#include "Oscilloscope_GUI.h"
//
#ifndef JUCE_IOS
#define JUCE_IOS 0 // Define a default value if JUCE_IOS is not defined
#endif

class OScopeComponent  : public juce::Component
{
public:
    OScopeComponent(OscilliscopeAudioProcessor&);
    ~OScopeComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    OscilliscopeAudioProcessor& audioProcessor;
    ScopeComponent<float> scopeComponent;

    bool shouldPaint { false };
    bool drawOverlay { true };
    
    float rmsValue = { 0 };
    float peakValue = { 0 };
    
private:
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OScopeComponent)
};
