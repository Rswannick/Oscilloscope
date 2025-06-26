/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP/Oscilloscope_DSP.h"

//==============================================================================
/**
*/
class OscilliscopeAudioProcessor  : public juce::AudioProcessor, juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    OscilliscopeAudioProcessor();
    ~OscilliscopeAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void parameterChanged(const juce::String &parameterID, float newValue) override;
    
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples);
  
    float ratio { 2436 / 1125 };
    float windowW { 800 / ratio };
    float windowH { 800 };
    bool xyMode { false };
    bool fullScreen { false };
    //Preset Management
    juce::String pname { "DEFAULT" };

    //Audio Value Tree State
    juce::AudioProcessorValueTreeState apvts;

    OscilloscopeDSP cOscilloscope;

    //Level Meter
    juce::SmoothedValue<float> smoothedPeakL, smoothedPeakR, smoothedRMS, smoothedFreq;
    float PEAKL { 0 }, PEAKR { 0 }, FREQ { 0 };
    
    juce::String sSelectedColorOSC1 { juce::Colours::yellowgreen.toString() }, sSelectedColorOSC2 { juce::Colours::yellow.toString()};
    
    

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParams();
    
    juce::dsp::Gain<float> volumeL, volumeR; 
    
//    adamski::PitchMPM pitchMPM;
//    adamski::PitchYIN pitchYIN { 1024 };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscilliscopeAudioProcessor)
};
