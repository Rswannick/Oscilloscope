/*
  ==============================================================================

    Background.cpp
    Created: 10 Sep 2022 7:16:16pm
    Author:  Ryan Swannick

  ==============================================================================
*/

#include "Oscilloscope_Screen.h"

OscilloscoeScreen::OscilloscoeScreen(OscilliscopeAudioProcessor& p): cOscope (p),
cOscope2(p), cOverlay(p), audioProcessor (p)
{
    addAndMakeVisible(cOscope);
    addAndMakeVisible(cOscope2);
    addAndMakeVisible(cOverlay);
}

OscilloscoeScreen::~OscilloscoeScreen()
{

}

void OscilloscoeScreen::paint (juce::Graphics& g)
{

}

void OscilloscoeScreen::resized()
{
    auto bounds = getLocalBounds();
    cOscope.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
    cOscope2.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
    cOverlay.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
}
