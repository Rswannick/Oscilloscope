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
                       ), apvts (*this, nullptr, "parameters" , createParams())
//, pitchMPM(2048)
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

    cOscilloscope.prepareToPlay(sampleRate, samplesPerBlock);
    
//    pitchMPM.setSampleRate(sampleRate);
//    pitchMPM.setBufferSize(samplesPerBlock);
            
    smoothedPeakL.reset(sampleRate, 0.001);
    smoothedPeakR.reset(sampleRate, 0.001);
    smoothedRMS.reset(sampleRate, 0.001);
    smoothedFreq.reset(sampleRate, 0.001);
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.numChannels = 1;
    spec.maximumBlockSize = samplesPerBlock;
    
    volumeL.reset();
    volumeR.reset();
    
    volumeL.prepare(spec);
    volumeR.prepare(spec);

    apvts.addParameterListener("QUALITY", this);
    apvts.addParameterListener("GAIN", this);
    apvts.addParameterListener("XY Mode", this);

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

    float mBypass = apvts.getRawParameterValue("POWER")->load();
    
    
    if (mBypass == 1 && buffer.getNumSamples() > 0)
    {
        cOscilloscope.processBlock(buffer);
        
        smoothedRMS.setTargetValue(juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples())));
        smoothedPeakL.setTargetValue(juce::Decibels::gainToDecibels(buffer.getMagnitude(0, 0, buffer.getNumSamples())));
        smoothedPeakR.setTargetValue(juce::Decibels::gainToDecibels(buffer.getMagnitude(1, 0, buffer.getNumSamples())));
//
//        if (pitchMPM.getPitch(buffer.getReadPointer(0)) < 20)
//        {
//            smoothedFreq.setTargetValue (20);
//        }
//        else
//        {
//            smoothedFreq.setTargetValue( pitchMPM.getPitch(buffer.getReadPointer(0)) );
//        }
//        
        PEAKL = smoothedPeakL.getNextValue();
        PEAKR = smoothedPeakR.getNextValue();
//        FREQ = smoothedFreq.getNextValue();

    }
    else
    {
        smoothedPeakL.setTargetValue(-99);
        smoothedPeakR.setTargetValue(-99);

        PEAKL = smoothedPeakL.getNextValue();
        PEAKR = smoothedPeakR.getNextValue();
    }
    
    juce::dsp::AudioBlock<float> block1 (buffer);

    auto leftBlock = block1.getSingleChannelBlock(0);
    auto rightBlock = block1.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext (leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext (rightBlock);

    volumeL.setGainDecibels(apvts.getRawParameterValue("VOLUME")->load());
    volumeR.setGainDecibels(apvts.getRawParameterValue("VOLUME")->load());
    volumeL.process(leftContext);
    volumeR.process(rightContext);
    
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
    juce::UndoManager undo;
    apvts.state.setProperty("PresetName", pname, &undo);
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    
    xml->setAttribute("WIDTH", windowW);
    xml->setAttribute("HEIGHT", windowH);
    xml->setAttribute("COLOR1", sSelectedColorOSC1);
    xml->setAttribute("COLOR2", sSelectedColorOSC2);
    
    copyXmlToBinary (*xml, destData);
}

void OscilliscopeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (apvts.state.getType())){ apvts.replaceState (juce::ValueTree::fromXml (*xmlState)); }
        juce::UndoManager undo;
        juce::String swapName = apvts.state.getPropertyAsValue("PresetName", &undo).toString();
        pname.swapWith(swapName);
        
        sSelectedColorOSC1 = xmlState->getStringAttribute("COLOR1");
        sSelectedColorOSC2 = xmlState->getStringAttribute("COLOR2");
        
        if (windowW != 0)
        {
            windowW = xmlState->getDoubleAttribute("WIDTH");
            windowH = xmlState->getDoubleAttribute("HEIGHT");
        }
    }}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OscilliscopeAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout OscilliscopeAudioProcessor::createParams()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat>( juce::ParameterID("CH 1 VOLTS", 1 ) , "CH 1 VOLTS", juce::NormalisableRange<float> {0, 2, .01, 1.f}, 1.0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>( juce::ParameterID("CH 1 SECONDS", 1 ) , "CH 1 SECONDS", juce::NormalisableRange<float> {0, 2, .01, 1.f}, 1.0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>( juce::ParameterID("CH 1 V OFFSET", 1 ) , "CH 1 V OFFSET", juce::NormalisableRange<float> {0, 2, .01, 1.f}, 1.0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>( juce::ParameterID("CH 1 H OFFSET", 1 ) , "CH 1 H OFFSET", juce::NormalisableRange<float> {0, 2, .01, 1.f}, 1.0));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>( juce::ParameterID("CH 2 VOLTS", 1 ) , "CH 2 VOLTS", juce::NormalisableRange<float> {0, 2, .01, 1.f}, 1.0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>( juce::ParameterID("CH 2 SECONDS", 1 ) , "CH 2 SECONDS", juce::NormalisableRange<float> {0, 2, .01, 1.f}, 1.0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>( juce::ParameterID("CH 2 V OFFSET", 1 ) , "CH 2 V OFFSET", juce::NormalisableRange<float> {0, 2, .01, 1.f}, 1.0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>( juce::ParameterID("CH 2 H OFFSET", 1 ) , "CH 2 H OFFSET", juce::NormalisableRange<float> {0, 2, .01, 1.f}, 1.0));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>( juce::ParameterID("GAIN", 1 ) , "GAIN", juce::NormalisableRange<float> {-24, 24, 0.01, 1.f}, 0.0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>( juce::ParameterID("ROTATION", 1 ) , "ROTATION", juce::NormalisableRange<float> {0.0, 3.15, .01, 1.f}, 0.0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>( juce::ParameterID("LINE THICKNESS", 1 ) , "LINE THICKNESS", juce::NormalisableRange<float> {1.0, 5.0, .01, 1.f}, 3.0));
    
    params.push_back (std::make_unique<juce::AudioParameterChoice>( juce::ParameterID( "MONO/STEREO" , 1 ), "MONO/STEREO", juce::StringArray { "MONO", "STEREO"}, 0));
    params.push_back (std::make_unique<juce::AudioParameterChoice>( juce::ParameterID( "POWER" , 1 ), "POWER", juce::StringArray { "Off", "On"}, 1));
    params.push_back (std::make_unique<juce::AudioParameterChoice>( juce::ParameterID( "PHASE" , 1 ), "PHASE", juce::StringArray { "0", "180"}, 0));
    params.push_back (std::make_unique<juce::AudioParameterChoice>( juce::ParameterID( "XY Mode" , 2 ), "XY Mode", juce::StringArray { "OFF", "ON"}, 0));
    params.push_back (std::make_unique<juce::AudioParameterChoice>( juce::ParameterID( "Fullscreen" , 2 ), "Fullscreen", juce::StringArray { "OFF", "ON"}, 0));

    
    params.push_back (std::make_unique<juce::AudioParameterChoice>( juce::ParameterID( "QUALITY" , 1 ), "QUALITY", juce::StringArray { "LOW HZ", "MED HZ", "HI HZ"}, 0));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>( juce::ParameterID("VOLUME", 2 ) , "VOLUME", juce::NormalisableRange<float> {-48.0, 6.0, 0.1, 1.f}, 0.0));


    return { params.begin(), params.end() };
}


void OscilliscopeAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
//    if (parameterID.contains("QUALITY"))
//    {
//        if (apvts.getRawParameterValue("QUALITY")->load() == 0)
//        { pitchMPM.setBufferSize(getBlockSize() * 2); }
//        else if (apvts.getRawParameterValue("QUALITY")->load() == 1)
//        { pitchMPM.setBufferSize(getBlockSize()); }
//        else { pitchMPM.setBufferSize(getBlockSize() * .5); }
//    }
    if (parameterID.contains("GAIN"))
    {
        cOscilloscope.gainValue = newValue;
    }
    
    if (parameterID.contains("XY Mode"))
    {
        xyMode = newValue;
    }
}
