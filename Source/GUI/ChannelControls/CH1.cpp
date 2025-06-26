/*
  ==============================================================================

    CH1.cpp
    Created: 10 Sep 2022 7:13:43pm
    Author:  Ryan Swannick

  ==============================================================================
*/

#include "CH1.h"

CHComponent::CHComponent(juce::AudioProcessorValueTreeState& apvts, juce::String vOffsetId, juce::String hOffsetId,juce::String voltsId, juce::String secondsId)
{
    setSlider(mHOffsetSlider, apvts, hOffsetId, mHOffsetSliderAttachment);
    setSlider(mVOffsetSlider, apvts, vOffsetId, mVOffsetSliderAttachment);
    setSlider(mVoltsSlider, apvts, voltsId, mVoltsSliderAttachment);
    setSlider(mSecondsSlider, apvts, secondsId, mSecondsSliderAttachment);
}

CHComponent::~CHComponent()
{
    mVoltsSlider.setLookAndFeel(nullptr);
    mVOffsetSlider.setLookAndFeel(nullptr);
    mSecondsSlider.setLookAndFeel(nullptr);
    mHOffsetSlider.setLookAndFeel(nullptr);
}

void CHComponent::paint (juce::Graphics& g)
{
}

void CHComponent::resized()
{
    auto bounds = getLocalBounds();
    
    float sliderX = 0;
    float sliderY = 0;
    float sliderW = bounds.getWidth() * .230;
    float sliderH = bounds.getWidth() * .230;
    
    mVOffsetSlider.setBounds(sliderX, sliderY, sliderW, sliderH);
    mVoltsSlider.setBounds(mVOffsetSlider.getRight() * 1.1, sliderY, sliderW, sliderH);
    mHOffsetSlider.setBounds(mVoltsSlider.getRight() * 1.1, sliderY, sliderW, sliderH);
    mSecondsSlider.setBounds(mHOffsetSlider.getRight(), sliderY , sliderW, sliderH);
}

void CHComponent::setSlider (juce::Slider& slider, juce::AudioProcessorValueTreeState& apvts, juce::String paramId, std::unique_ptr<Attachment>& attachment)
{
    slider.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, true, 50, 25);
    
    slider.setName("CH1Volts");
    slider.setLookAndFeel(&cLAF.lOSC);
    
    addAndMakeVisible (slider);
    
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramId, slider);

}


 
