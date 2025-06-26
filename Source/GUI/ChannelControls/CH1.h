/*
  ==============================================================================

    CH1.h
    Created: 10 Sep 2022 7:13:43pm
    Author: Ryan Swannick
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#include "../../LookAndFeel/LAF.h"

class CHComponent  : public juce::Component
{
public:
    CHComponent(juce::AudioProcessorValueTreeState& apvts, juce::String vOffsetId, juce::String hOffsetId,juce::String voltsId, juce::String secondsId);
    ~CHComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    juce::Slider

    mHOffsetSlider,
    mVOffsetSlider,
    mVoltsSlider,
    mSecondsSlider;
    
private:
    LAFComponent cLAF;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    std::unique_ptr<Attachment>
    mHOffsetSliderAttachment,
    mVOffsetSliderAttachment,
    mVoltsSliderAttachment,
    mSecondsSliderAttachment;
    
    void setSlider (juce::Slider& slider, juce::AudioProcessorValueTreeState& apvts, juce::String paramId, std::unique_ptr<Attachment>& attachment);
           


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CHComponent)
};



