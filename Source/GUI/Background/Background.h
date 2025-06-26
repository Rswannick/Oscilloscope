/*
  ==============================================================================

    Background.h
    Created: 10 Sep 2022 7:16:16pm
    Author:  Ryan Swannick
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class BackgroundComponent  : public juce::Component
{
public:
    BackgroundComponent();
    ~BackgroundComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;


private:
    juce::ImageComponent backGround;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BackgroundComponent)
};



