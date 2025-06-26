/*
  ==============================================================================

    Oscilloscope_GUI.h
    Created: 23 Mar 2024 4:35:48pm
    Author:  IK Multimedia

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../PluginProcessor.h"

#ifndef JUCE_IOS
#define JUCE_IOS 0 // Define a default value if JUCE_IOS is not defined
#endif

template <typename SampleType>

class ScopeComponent  : public juce::Component,
                        private juce::Timer
{
    
public:
    using Queue = AudioBufferQueue<SampleType>;

    //==============================================================================
    ScopeComponent (OscilliscopeAudioProcessor& p, Queue& queueToUse, Queue& queueToUse2)
    :  audioProcessor(p), audioBufferQueue (queueToUse), audioBufferQueue2 (queueToUse2)
    {
        sampleData.fill (SampleType (0));
        setFramesPerSecond (120);
    }

    //==============================================================================
    void setFramesPerSecond (int framesPerSecond)
    {
        jassert (framesPerSecond > 0 && framesPerSecond < 1000);
        startTimerHz (framesPerSecond);
    }

    //==============================================================================
    void paint (juce::Graphics& g) override
    {
        auto area = getLocalBounds();
        auto h = (SampleType) area.getHeight();
        auto w = (SampleType) area.getWidth();

        if (audioProcessor.apvts.getRawParameterValue("XY Mode")->load()) // Check if X-Y mode is enabled
        {
            // X-Y mode - compare the two signals (left vs. right)
            auto scopeRect = juce::Rectangle<SampleType> { SampleType(0), SampleType(0), w, h / 2 };

            // Assuming 'sampleData' holds your left and right channels' data
            plotXYMode(sampleData.data(), sampleData.data() + sampleData.size() / 2, sampleData.size() / 2, g, scopeRect);
        }
        else
        {
            // Regular oscilloscope mode (time-domain)
            auto scopeRect = juce::Rectangle<SampleType> { SampleType(0), SampleType(0), w, h / 2 };
            plot(sampleData.data(), sampleData.size(), g, scopeRect, SampleType(1), h / 4);
        }
    }
    
    juce::Colour selectColor (juce::Colour color) {
        vOSCColor1 = color;
        return color;
    }

    //==============================================================================
    void resized() override {}
    
    float mPhase = { 1.0 };
    float mThick = { 1.0 };
    float mFreq = { 1.0 };
    float mRotation = { 0.0 };
    
    float mVOffset = { 1.0 };
    float mHOffset = { 1.0 };
    float mVolts = { 1.0 };
    float mSeconds = { 1.0 };
    
    float rmsValue = { 0 };
    float peakValue = { 0 };
    
    bool shouldPaint { false };
    bool drawOverlay { true };
    
    juce::Colour vOSCColor1 { juce::Colours::yellowgreen };
    
private:
    OscilliscopeAudioProcessor& audioProcessor;

    //==============================================================================
    Queue& audioBufferQueue;
    Queue& audioBufferQueue2;
    std::array<SampleType, Queue::bufferSize> sampleData;
    std::array<SampleType, Queue::bufferSize> sampleData2;

    juce::dsp::FFT fft { Queue::order };
    using WindowFun = juce::dsp::WindowingFunction<SampleType>;
    WindowFun windowFun { (size_t) fft.getSize(), WindowFun::hann };
    std::array<SampleType, 2 * Queue::bufferSize> spectrumData;

    //==============================================================================
    void timerCallback() override
    {
        if (audioProcessor.apvts.getRawParameterValue("XY Mode")->load()) // Check if X-Y mode is enabled
         {
             // Populate the data for X-Y mode
             audioBufferQueue.pop(sampleData.data());  // For left channel
             audioBufferQueue2.pop(sampleData.data() + sampleData.size() / 2);  // For right channel

             juce::FloatVectorOperations::copy(spectrumData.data(), sampleData.data(), (int) sampleData.size());

             auto fftSize = (size_t) fft.getSize();
             jassert(spectrumData.size() == 2 * fftSize);

             windowFun.multiplyWithWindowingTable(spectrumData.data(), fftSize);
             fft.performFrequencyOnlyForwardTransform(spectrumData.data());

             static constexpr auto mindB = SampleType(-160);
             static constexpr auto maxdB = SampleType(0);

             for (auto& s : spectrumData)
                 s = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(s) - juce::Decibels::gainToDecibels(SampleType(fftSize))),
                                mindB, maxdB, SampleType(0), SampleType(1));
         }
         else
         {
             // Normal mode logic (if any)
             audioBufferQueue.pop(sampleData.data());  // Regular processing for one buffer
             juce::FloatVectorOperations::copy(spectrumData.data(), sampleData.data(), (int) sampleData.size());

             auto fftSize = (size_t) fft.getSize();
             jassert(spectrumData.size() == 2 * fftSize);

             windowFun.multiplyWithWindowingTable(spectrumData.data(), fftSize);
             fft.performFrequencyOnlyForwardTransform(spectrumData.data());

             static constexpr auto mindB = SampleType(-160);
             static constexpr auto maxdB = SampleType(0);

             for (auto& s : spectrumData)
                 s = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(s) - juce::Decibels::gainToDecibels(SampleType(fftSize))),
                                mindB, maxdB, SampleType(0), SampleType(1));
         }

         repaint();
    }

    void plot(const SampleType* data,
              size_t numSamples,
              juce::Graphics& g,
              juce::Rectangle<SampleType> rect,
              SampleType scaler = SampleType(1),
              SampleType offset = SampleType(0))
    {
        if (mPhase == 0) { mPhase = 1.0; }
        else { mPhase = -1.0; }

        float w = rect.getWidth();

        auto h = rect.getHeight();
        auto right = rect.getRight() * mHOffset;
        auto center = rect.getBottom() * mVOffset;
        auto gain = ((h * scaler) * mVolts) * mPhase;

        for (size_t i = 1; i < numSamples; ++i)
        {
            auto startX = juce::jmap(SampleType(i - 1), SampleType(0), SampleType(numSamples - 1), SampleType(right - w * mSeconds * 5), SampleType(right));
            auto endX = juce::jmap(SampleType(i), SampleType(0), SampleType(numSamples - 1), SampleType(right - w * mSeconds * 5), SampleType(right));

            auto startY = center - gain * data[i - 1];
            auto endY = center - gain * data[i];

            juce::Path p, pd;

            juce::DropShadow shadow1;
            shadow1.colour = juce::Colours::white;

            juce::Line<float> line1;
            line1.setStart(startX, startY);
            line1.setEnd(endX, endY);

            pd.addLineSegment(line1, mThick * 1.10);
            pd.applyTransform(juce::AffineTransform::rotation(mRotation, getWidth() * .50, getHeight() * .50));

            if (!JUCE_IOS) { shadow1.drawForPath(g, pd); }

            p.addLineSegment(line1, mThick);
            p.applyTransform(juce::AffineTransform::rotation(mRotation, getWidth() * .50, getHeight() * .50));
            g.setColour(vOSCColor1);

            g.fillPath(p);
        }
    }
    
    void plotXYMode(const SampleType* xData, const SampleType* yData, size_t numSamples, juce::Graphics& g, juce::Rectangle<SampleType> rect)
    {
        auto w = rect.getWidth();
        auto h = rect.getHeight();
        auto right = rect.getRight() * mHOffset;
        auto center = rect.getBottom() * mVOffset;

        // Adjust scaling and drawing based on phase
        for (size_t i = 1; i < numSamples; ++i)
        {
            // Map sample index to screen space for X and Y
            auto startXCoord = juce::jmap(SampleType(i - 1), SampleType(0), SampleType(numSamples - 1), SampleType(right - w), SampleType(right));
            auto endXCoord = juce::jmap(SampleType(i), SampleType(0), SampleType(numSamples - 1), SampleType(right - w), SampleType(right));

            // Map X and Y data values to screen space (scale the signals accordingly)
            auto startY = center - (yData[i - 1] * h * mVolts);
            auto endY = center - (yData[i] * h * mVolts);

            auto startX = right + (xData[i - 1] * w * mVolts);
            auto endX = right + (xData[i] * w * mVolts);

            juce::Path p;
            juce::Line<float> line1;
            line1.setStart(startX, startY);
            line1.setEnd(endX, endY);

            p.addLineSegment(line1, mThick);
            g.setColour(vOSCColor1);

            g.fillPath(p);
        }
    }




};
