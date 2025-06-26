/*
  ==============================================================================

    CH2.cpp
    Created: 10 Sep 2022 7:14:03pm
    Author:  Ryan Swannick

  ==============================================================================
*/

#include "CH2.h"

CH2Component::CH2Component()
{
    setSlider(mHOffsetSlider);
    setSlider(mVOffsetSlider);
    setSlider(mVoltsSlider);
    setSlider(mSecondsSlider);
}

CH2Component::~CH2Component()
{
    mVOffsetSlider.setLookAndFeel(nullptr);
    mHOffsetSlider.setLookAndFeel(nullptr);
    mVoltsSlider.setLookAndFeel(nullptr);
    mSecondsSlider.setLookAndFeel(nullptr);
    
}

void CH2Component::paint (juce::Graphics& g)
{

}

void CH2Component::resized()
{
    auto bounds = getLocalBounds();
    float sliderX = 0;
    float sliderY = 0;
    float sliderW = bounds.getWidth() * .25;
    float sliderH = bounds.getHeight();
    
    mVOffsetSlider.setBounds(sliderX, sliderY, sliderW, sliderH);
    mVoltsSlider.setBounds(mVOffsetSlider.getRight(), sliderY, sliderW, sliderH);
    mHOffsetSlider.setBounds(mVoltsSlider.getRight(), sliderY, sliderW, sliderH);
    mSecondsSlider.setBounds(mHOffsetSlider.getRight(), sliderY , sliderW, sliderH);
}

void CH2Component::setSlider (juce::Slider& slider)
{
    slider.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, true, 50, 25);
    slider.setDoubleClickReturnValue(true, 1.f);
    slider.setRange(0.0f, 2.f, .01f);
    slider.setValue(1.f);
    
    slider.setName("CH1Volts");
    slider.setLookAndFeel(&cLAF.lOSC);

    addAndMakeVisible (slider);
}


