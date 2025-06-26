/*
  ==============================================================================

    CH2.h
    Created: 10 Sep 2022 7:14:03pm
    Author:  Ryan Swannick

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../../LookAndFeel/LAF.h"


class CH2Component  : public juce::Component
{
public:
    CH2Component();
    ~CH2Component() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    juce::Slider

    mHOffsetSlider,
    mVOffsetSlider,
    mVoltsSlider,
    mSecondsSlider;

private:
    LAFComponent cLAF;

    
    void setSlider (juce::Slider& slider);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CH2Component)
};



