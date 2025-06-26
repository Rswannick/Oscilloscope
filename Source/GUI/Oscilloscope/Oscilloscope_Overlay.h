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

//
#ifndef JUCE_IOS
#define JUCE_IOS 0 // Define a default value if JUCE_IOS is not defined
#endif

class OscilloscopeOverlay  : public juce::Component
{
public:
    OscilloscopeOverlay(OscilliscopeAudioProcessor&);
    ~OscilloscopeOverlay() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    bool shouldPaint { false };
    bool drawOverlay { true };
    
    float rmsValue = { 0 };
    float peakValue = { 0 };
    float dspVoltage = 1;
    float timeSeconds = 1;

private:
    OscilliscopeAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscilloscopeOverlay)
};
