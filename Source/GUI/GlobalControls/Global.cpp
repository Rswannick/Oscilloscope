/*
  ==============================================================================

    Global.cpp
    Created: 10 Sep 2022 7:13:26pm
    Author:  Ryan Swannick

  ==============================================================================
*/

#include "Global.h"

GlobalComponent::GlobalComponent(juce::AudioProcessorValueTreeState& apvts, juce::String gainID, juce::String rotationId, juce::String thicknessId, juce::String monoId)
{
    
    addAndMakeVisible(mMonoStereo);
    
    setSlider(mGainSlider, apvts, gainID, mGainSliderAttachment);
    setSlider(mRotationSlider, apvts, rotationId, mRotationSliderAttachment);
    setSlider(mThicknessSlider, apvts, thicknessId, mThicknessSliderAttachment);
    
    mMonoStereo.setName("MONO");
    mMonoStereo.setLookAndFeel(&cLAF.lOSC);
    
    monoAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, monoId, mMonoStereo);

}

GlobalComponent::~GlobalComponent()
{
    
    mMonoStereo.setLookAndFeel(nullptr);
    mGainSlider.setLookAndFeel(nullptr);
    mRotationSlider.setLookAndFeel(nullptr);
    mThicknessSlider.setLookAndFeel(nullptr);
}

void GlobalComponent::paint (juce::Graphics& g)
{

//    g.fillAll();
}

void GlobalComponent::resized()
{
    //Set Local Bounds
    auto bounds = getLocalBounds();
    //Button Variables
    float buttonX = bounds.getX() - bounds.getWidth() * .01;
    float buttonY = bounds.getHeight() * .10;
    float buttonH ;
    float buttonW;
    
    float aspectRatio = 114.0f / 69.0f; // Width divided by height
    float availableWidth = bounds.getWidth() * 0.98f; // Considering 95% of available width
    float availableHeight = bounds.getHeight() * 0.98f; // Considering 95% of available height

    // Calculate button width and height based on the aspect ratio
    if (availableWidth / availableHeight > aspectRatio) {
        // Height is limiting factor
        buttonH = availableHeight;
        buttonW = buttonH * aspectRatio;
    } else {
        // Width is limiting factor
        buttonW = availableWidth;
        buttonH = buttonW / aspectRatio;
    }

    //Slider Variables
    float sliderX = getWidth() * .320;
    float sliderY = bounds.getHeight() * .05;
    float sliderW = bounds.getHeight() * 1.0;
    float sliderH = bounds.getHeight() * 1.0;
    //Set Component Bounds in order from left to right
    mMonoStereo.setBounds(buttonX, buttonY, buttonW, buttonH);
    mGainSlider.setBounds(sliderX, sliderY, sliderW, sliderH);
    mRotationSlider.setBounds(mGainSlider.getRight() * 1.3, sliderY, sliderW, sliderH);
    mThicknessSlider.setBounds(mRotationSlider.getRight() * 1.315, sliderY, sliderW, sliderH);

}

void GlobalComponent::setSlider (juce::Slider& slider, juce::AudioProcessorValueTreeState& apvts, juce::String paramId, std::unique_ptr<Attachment>& attachment)
{
    slider.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, true, 50, 25);
    
    slider.setName("CH1Volts");
    slider.setLookAndFeel(&cLAF.lOSC);
    
    addAndMakeVisible (slider);
    
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramId, slider);

}


