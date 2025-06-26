/*
  ==============================================================================

    Oscilloscope_DSP.h
    Created: 10 Dec 2023 3:30:19pm
    Author:  Ryan Swannick

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

template <typename SampleType>
class AudioBufferQueue
{
public:
    //==============================================================================
    static constexpr size_t order = 9;
    static constexpr size_t bufferSize = 1U << order;
    static constexpr size_t numBuffers = 5;

    //==============================================================================
    void push (const SampleType* dataToPush, size_t numSamples)
    {
        jassert (numSamples <= bufferSize);

        int start1, size1, start2, size2;
        abstractFifo.prepareToWrite (1, start1, size1, start2, size2);

        jassert (size1 <= 1);
        jassert (size2 == 0);

        if (size1 > 0)
            juce::FloatVectorOperations::copy (buffers[(size_t) start1].data(), dataToPush, (int) juce::jmin (bufferSize, numSamples));

        abstractFifo.finishedWrite (size1);
    }

    //==============================================================================
    void pop (SampleType* outputBuffer)
    {
        int start1, size1, start2, size2;
        abstractFifo.prepareToRead (1, start1, size1, start2, size2);

        jassert (size1 <= 1);
        jassert (size2 == 0);

        if (size1 > 0)
            juce::FloatVectorOperations::copy (outputBuffer, buffers[(size_t) start1].data(), (int) bufferSize);

        abstractFifo.finishedRead (size1);
    }

    
private:
    //==============================================================================
    juce::AbstractFifo abstractFifo { numBuffers };
    std::array<std::array<SampleType, bufferSize>, numBuffers> buffers;
};



template <typename SampleType>
class ScopeDataCollector
{
public:
    //==============================================================================
    ScopeDataCollector (AudioBufferQueue<SampleType>& queueToUse)
        : audioBufferQueue (queueToUse)
    {}

    //==============================================================================
    void process (const SampleType* data, size_t numSamples)
    {
        size_t index = 0;

        if (state == State::waitingForTrigger)
        {
            while (index++ < numSamples)
            {
                auto currentSample = *data++;

                if (currentSample >= triggerLevel && prevSample < triggerLevel)
                {
                    numCollected = 0;
                    state = State::collecting;
                    break;
                }

                prevSample = currentSample;
            }
        }

        if (state == State::collecting)
        {
            while (index++ < numSamples)
            {
                buffer[numCollected++] = *data++;

                if (numCollected == buffer.size())
                {
                    audioBufferQueue.push (buffer.data(), buffer.size());
                    state = State::waitingForTrigger;
                    prevSample = SampleType (100);
                    break;
                }
            }
        }
    }


private:
    //==============================================================================
    AudioBufferQueue<SampleType>& audioBufferQueue;
    std::array<SampleType, AudioBufferQueue<SampleType>::bufferSize> buffer;
    size_t numCollected;
    SampleType prevSample = SampleType (100);
 
    static constexpr auto triggerLevel = SampleType (0.0);

    enum class State { waitingForTrigger, collecting } state { State::waitingForTrigger };
};



//==============================================================================
/**
*/
class OscilloscopeDSP
{
public:
    //==============================================================================
    OscilloscopeDSP();
    ~OscilloscopeDSP();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void processBlock (juce::AudioBuffer<float>& buffer);
    
    //=====================
    AudioBufferQueue<float>& getAudioBufferQueue() noexcept { return audioBufferQueue; }
    AudioBufferQueue<float>& getAudioBufferQueue2() noexcept { return audioBufferQueue2; }
    
    float mBypass { 0 };
    float rmsValue { 0 };
    float peakValue { 0 };
    float gainValue { 0 };
    float rmsValue2 { 0 };
    float peakValue2 { 0 };
    float gainValue2 { 0 };
    
    bool xyModePower = true;

private:
    AudioBufferQueue<float> audioBufferQueue, audioBufferQueue2;
    ScopeDataCollector<float> scopeDataCollector { audioBufferQueue };
    ScopeDataCollector<float> scopeDataCollector2 { audioBufferQueue2 };
    juce::AudioSourceChannelInfo bufferToFill;
    juce::dsp::Gain<float> gainL, gainR;
    juce::SmoothedValue<float> smoothedPeak, smoothedRMS;
    double sampleRate;

    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscilloscopeDSP)
};
