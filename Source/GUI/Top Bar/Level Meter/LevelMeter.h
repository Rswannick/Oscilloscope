/*
  ==============================================================================

    LevelMeter.h
    Created: 23 Nov 2023 1:34:35am
    Author:  RyanSwannick

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../../../PluginProcessor.h"
//====================================================&==========================
/*
*/
class LevelMeterComponent  : public juce::Component, juce::Timer
{
public:
    LevelMeterComponent(OscilliscopeAudioProcessor&);
    ~LevelMeterComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
    float currentPeakL { 0 };
    float currentPeakR { 0 };

    juce::Colour vLevelMeterColor1 {juce::Colours::aquamarine}, vLevelMeterColor2 { juce::Colours::yellowgreen }; 
    
private:
    OscilliscopeAudioProcessor& audioProcessor;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeterComponent)
};
