/*
  ==============================================================================

    Background.cpp
    Created: 10 Sep 2022 7:16:16pm
    Author:  Ryan Swannick

  ==============================================================================
*/

#include "Background.h"

BackgroundComponent::BackgroundComponent()
{
    auto tapImage = juce::ImageCache::getFromMemory (BinaryData::Background_png, BinaryData::Background_pngSize);
    
    if (tapImage.isValid()) backGround.setImage (tapImage, juce::RectanglePlacement::stretchToFit);
    else jassertfalse;
     
    addAndMakeVisible(backGround);

}

BackgroundComponent::~BackgroundComponent()
{

}

void BackgroundComponent::paint (juce::Graphics& g)
{
    
}

void BackgroundComponent::resized()
{
    
    backGround.setBounds(0, 0, getWidth(), getHeight());

}


