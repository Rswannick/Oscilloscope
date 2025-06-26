/*
  ==============================================================================

    Background.cpp
    Created: 10 Sep 2022 7:16:16pm
    Author:  Ryan Swannick

  ==============================================================================
*/

#include "Oscilloscope_Overlay.h"

OscilloscopeOverlay::OscilloscopeOverlay(OscilliscopeAudioProcessor& p): audioProcessor (p)
{

}

OscilloscopeOverlay::~OscilloscopeOverlay()
{

}

void OscilloscopeOverlay::paint (juce::Graphics& g)
{
    auto area = getLocalBounds(); // Decrease the size of the drawing area by 2%

    // Center Lines
    g.setColour(juce::Colour::fromRGB(100, 100, 100));

    // Voltage axis (Y-axis)
    juce::Array<float> voltagePercentages
    { -0.75f, -0.5f, -0.25f, 0.0f, 0.25f, 0.5f, 0.75f };

    for (auto voltagePercentage : voltagePercentages)
    {
        auto y = juce::jmap(voltagePercentage, -1.0f, 1.0f, float(area.getBottom()), float(area.getY()));
        g.setColour(juce::Colours::darkgrey);
        g.drawHorizontalLine(y, area.getX(), area.getRight());
        
        // Draw text labels based on voltage percentage
        g.setColour(juce::Colours::darkgrey);
        if (voltagePercentage == 0.0f)
        {
            g.drawText("Time", area.getX() - 45, y - 10, 50, 20, juce::Justification::right);
        }
        else
        {
           g.setColour(juce::Colours::grey);
           g.setFont(area.getHeight() * .060);
           g.drawText(juce::String(voltagePercentage * 100.0f, 0) + "%", area.getX(), y - 10, area.getWidth() * .10, 20, juce::Justification::right);
        }
    }

    // Time axis (X-axis)
    juce::Array<float> timePercentages
    {
        0.2f, 0.4f, 0.6f, 0.8f, 1.0f
    };

    for (auto timePercentage : timePercentages)
    {
        auto x = juce::jmap(timePercentage, 0.0f, 1.0f, float(area.getX()), float(area.getRight()));
        g.setColour(juce::Colours::darkgrey);
        g.drawVerticalLine(x, area.getY(), area.getBottom());
        
        // Draw text labels based on time percentage
        if (timePercentage != 1.0f)
        {
            g.setFont(area.getHeight() * .06);
            g.setColour(juce::Colours::grey);
            g.drawFittedText(juce::String(timePercentage * 100.0f, 0) + "%", x - 20, area.getY(), 50, 20, juce::Justification::horizontallyCentred, 1);
            g.setColour(juce::Colours::darkgrey);
        }
    }


    // Overlay
    if (drawOverlay)
    {
        g.setColour(juce::Colours::grey);
        juce::Rectangle<int> overlay;
        overlay.setBounds(area.getX(), area.getY() + area.getHeight() * .10, area.getWidth(), area.getHeight() * .10);
        g.fillRect(overlay);
        
        juce::String rmsString = std::to_string(audioProcessor.smoothedRMS.getNextValue()).substr(0, 3);
        juce::String peakString = (std::to_string(audioProcessor.PEAKL)).substr(0, 3);
        juce::String freqString = (std::to_string(audioProcessor.FREQ));
        
        if (audioProcessor.FREQ <= 999) { freqString = freqString.substring(0, 3); }
        else if (audioProcessor.FREQ <= 9999) { freqString = freqString.substring(0, 4); }
        else if (audioProcessor.FREQ <= 99) { freqString = freqString.substring(0, 2); }
        else { freqString = freqString.substring(0, 5); }
        
        g.setFont(overlay.getHeight() * .7);
        if (peakValue > 0) { g.setColour(juce::Colours::red); }
        else { g.setColour(juce::Colours::white); }
        g.drawFittedText("RMS: " + rmsString + " dB", overlay.getX() + overlay.getWidth() * .0155, overlay.getY(), overlay.getWidth(), overlay.getHeight(), juce::Justification::left, 1);
        g.drawFittedText("PEAK: " + peakString + " dB", overlay.getX() + overlay.getWidth() * .30, overlay.getY(), overlay.getWidth(), overlay.getHeight(), juce::Justification::left, 1);
        
//        g.setColour(juce::Colours::white);
//        g.drawFittedText("FREQ: " + freqString + " Hz", overlay.getX() + overlay.getWidth() * .60, overlay.getY(), overlay.getWidth(), overlay.getHeight(), juce::Justification::left, 1);
    }

    if (shouldPaint)
    {
        g.fillAll(juce::Colour::fromRGB(40,40,40).withAlpha(.45f));
        g.drawText("DEVICE INPUT NOT SELECTED", area.getX() + (area.getWidth() * .185), area.getY() + (area.getHeight() * .65), area.getWidth() * .70, area.getHeight() * .30, juce::Justification::centred);
    }


}

void OscilloscopeOverlay::resized()
{

}
