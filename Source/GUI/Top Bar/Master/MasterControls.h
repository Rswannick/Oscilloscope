/*
  ==============================================================================

     MasterControls.h
    Created: 22 Aug 2023 12:52:45am
    Author:  Ryan Swannick

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>
#include "../../../LookAndFeel/LAF.h"
#include "../../../PluginProcessor.h"
#include "../Level Meter/LevelMeter.h"

class MasterControlComponent  : public juce::Component
{
public:
    MasterControlComponent (OscilliscopeAudioProcessor&, juce::AudioProcessorValueTreeState& apvts,
                            juce::String gVolumeId);
    ~MasterControlComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    juce::Slider mVolumeSlider;
    LevelMeterComponent cMeter;

private:
    LAFComponent cLAF;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<Attachment>

    mVolumeSliderAttachment;
    
    void setSlider (juce::Slider& slider, juce::AudioProcessorValueTreeState& apvts, juce::String paramId, std::unique_ptr<Attachment>& attachment);
    
    juce::String componentName {""};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MasterControlComponent)
};
