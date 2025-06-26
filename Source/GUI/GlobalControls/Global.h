/*
  ==============================================================================

    Global.h
    Created: 10 Sep 2022 7:13:26pm
    Author:  Ryan Swannick

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#include "../../LookAndFeel/LAF.h"

class GlobalComponent  : public juce::Component
{
public:
    GlobalComponent(juce::AudioProcessorValueTreeState& apvts, juce::String gainID, juce::String rotationId, juce::String thicknessId, juce::String monoId);
    ~GlobalComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;


    juce::Slider

    mGainSlider,
    mRotationSlider,
    mThicknessSlider;
    
    juce::ToggleButton mMonoStereo;
    
private:
    LAFComponent cLAF;

    using buttonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    
    std::unique_ptr<buttonAttachment>
    
    monoAttachment;
    
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    std::unique_ptr<Attachment>
    mGainSliderAttachment,
    mRotationSliderAttachment,
    mThicknessSliderAttachment;

    
    void setSlider (juce::Slider& slider, juce::AudioProcessorValueTreeState& apvts, juce::String paramId, std::unique_ptr<Attachment>& attachment);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlobalComponent)
};



