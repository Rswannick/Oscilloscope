/*
  ==============================================================================

    LevelMeter.cpp
    Created: 23 Nov 2023 1:34:35am
 Author:  Ryan Swannick

  ==============================================================================
*/
#include <JuceHeader.h>
#include "LevelMeter.h"

LevelMeterComponent::LevelMeterComponent(OscilliscopeAudioProcessor& p): audioProcessor (p)
{
    setInterceptsMouseClicks(false, false);
    startTimerHz(120);
}

LevelMeterComponent::~LevelMeterComponent()
{
    stopTimer();
}

void LevelMeterComponent::paint (juce::Graphics& g)
{
    //Set bounds for meters
    auto bounds = getLocalBounds();
    juce::Rectangle<float> meterRectL, meterRectR;
    meterRectL.setBounds(bounds.getX()  + (bounds.getWidth() * .075), bounds.getY(), bounds.getWidth() * .855, bounds.getHeight() * .075);
    meterRectR.setBounds(bounds.getX()  + (bounds.getWidth() * .075), meterRectL.getBottom() * 1.15, bounds.getWidth() * .855, bounds.getHeight() * .075);

    //Background fill for meters
    g.setColour(juce::Colour::fromRGB(25, 25, 25));
    g.fillRect(meterRectL); g.fillRect(meterRectR);
    
    //Convert Level to Width
    float meterLevelL = juce::jmap(float(currentPeakL), -100.0f, 12.0f, 0.0f, float(bounds.getWidth()));
    float meterLevelR = juce::jmap(float(currentPeakR), -100.0f, 12.0f, 0.0f, float(bounds.getWidth()));

    //Normal colors
    auto color1L = vLevelMeterColor1;
    auto color1R = vLevelMeterColor1;
    auto color2L = vLevelMeterColor2;
    auto color2R = vLevelMeterColor2;

    //Clipping colors
    if (currentPeakL > 0.01) { color1L = juce::Colours::orange; color2L = juce::Colours::red; }
    if (currentPeakR > 0.01) { color1R = juce::Colours::orange; color2R = juce::Colours::red; }

    //Set Gradient
    auto levelGradient = juce::ColourGradient(color1L, 0.0f, meterRectL.getHeight(), color2L, meterRectL.getWidth() * .75, meterRectL.getHeight(), true);
    
    //Limit Meter Fill to Meter Bounds
    meterLevelL = juce::jlimit(0.0f, float(meterRectL.getWidth() ), meterLevelL);
    meterRectL.setWidth(meterLevelL);
    
    meterLevelR = juce::jlimit(0.0f, float(meterRectL.getWidth() ), meterLevelR);
    meterRectR.setWidth(meterLevelR);
    
    //FIll Meters
    g.setGradientFill(levelGradient);
    g.fillRect(meterRectL); g.fillRect(meterRectR);

}

void LevelMeterComponent::resized()
{
   
}

void LevelMeterComponent::timerCallback()
{
    repaint();
}
