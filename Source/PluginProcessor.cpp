/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OscilliscopeAudioProcessor::OscilliscopeAudioProcessor( )
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

OscilliscopeAudioProcessor::~OscilliscopeAudioProcessor()
{
}

//==============================================================================
const juce::String OscilliscopeAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OscilliscopeAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OscilliscopeAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OscilliscopeAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OscilliscopeAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OscilliscopeAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OscilliscopeAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OscilliscopeAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OscilliscopeAudioProcessor::getProgramName (int index)
{
    return {};
}

void OscilliscopeAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void OscilliscopeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumInputChannels();
    spec.maximumBlockSize = samplesPerBlock;
    
    gainL.reset();
    gainR.reset();
    
    gainL.prepare(spec);
    gainR.prepare(spec);
    
    
    
    
}

void OscilliscopeAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OscilliscopeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void OscilliscopeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    juce::dsp::AudioBlock<float> block1 (buffer);

    auto leftBlock = block1.getSingleChannelBlock(0);
    auto rightBlock = block1.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext (leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext (rightBlock);
    
    gainR.setGainDecibels(gainValue);
    gainL.setGainDecibels(gainValue);
    
    if (mBypass == 0)
    {
        if (buffer.getNumChannels() > 0)
        {
            double rmsV = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
            double peakV = juce::Decibels::gainToDecibels(buffer.getMagnitude(0, 0, buffer.getNumSamples()));
            rmsValue = rmsV;
            peakValue = peakV;
            gainL.process(leftContext);
            scopeDataCollector.process (buffer.getReadPointer (0), (size_t) buffer.getNumSamples());
        }
        if (buffer.getNumChannels() > 1)
        {
            double rmsV2 = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
            double peakV2 = juce::Decibels::gainToDecibels(buffer.getMagnitude(1, 0, buffer.getNumSamples()));
            rmsValue2 = rmsV2;
            peakValue2 = peakV2;
            gainR.process(rightContext);
            scopeDataCollector2.process (buffer.getReadPointer (1), (size_t) buffer.getNumSamples());
        }
    }
}

//==============================================================================
bool OscilliscopeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OscilliscopeAudioProcessor::createEditor()
{
    return new OscilliscopeAudioProcessorEditor (*this);
}

//==============================================================================
void OscilliscopeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void OscilliscopeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OscilliscopeAudioProcessor();
}
