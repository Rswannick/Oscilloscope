/*
  ==============================================================================

    MasterControls.cpp
    Created: 22 Aug 2023 12:52:45am
    Author:  Ryan Swannick

  ==============================================================================
*/
#include "MasterControls.h"

MasterControlComponent::MasterControlComponent(OscilliscopeAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts,
                                               juce::String gVolumeId): cMeter(p)
{
    setInterceptsMouseClicks(true, true);
    setSlider(mVolumeSlider, apvts, gVolumeId, mVolumeSliderAttachment);
    mVolumeSlider.setName("Output");
    mVolumeSlider.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(cMeter);
}

MasterControlComponent::~MasterControlComponent()
{
    juce::Component* componentsToSetLookAndFeel[] = { &mVolumeSlider, &cMeter};
    for (juce::Component* component : componentsToSetLookAndFeel)
    { component->setLookAndFeel(nullptr); }
}

void MasterControlComponent::paint (juce::Graphics& g)
{

}

void MasterControlComponent::resized()
{
    //Global Variables
    auto bounds = getLocalBounds();
    float sliderWidth = bounds.getWidth() * .05;
    float sliderHeight = bounds.getHeight() * .290;
    float sliderY = bounds.getY() + bounds.getHeight() * .115;
    float sliderX = bounds.getX() + (bounds.getWidth() * .78);
    mVolumeSlider.setBounds(sliderX, sliderY, sliderWidth, sliderHeight);
    cMeter.setBounds(bounds.getX() + (bounds.getWidth() * .175), bounds.getY() + (bounds.getHeight() * .0315), bounds.getWidth() * .175, bounds.getHeight() * .28);
}

using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

void MasterControlComponent::setSlider (juce::Slider& slider, juce::AudioProcessorValueTreeState& apvts, juce::String paramId, std::unique_ptr<Attachment>& attachment)
{
    //Slider
    slider.setMouseClickGrabsKeyboardFocus(false);
    slider.setSliderStyle (juce::Slider::SliderStyle::LinearVertical);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, true, 50, 25);
    slider.setPopupDisplayEnabled(true, true, this);
    slider.setTextValueSuffix(" dB");
    slider.setName("MASTER");
    slider.setLookAndFeel(&cLAF.lOSC);
    addChildComponent(slider);
    //Attach Slider to APVTS
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramId, slider);
}
