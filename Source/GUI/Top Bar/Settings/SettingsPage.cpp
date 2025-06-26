/*
  ==============================================================================

    SettingsPage.cpp
    Created: 15 Jan 2024 9:25:59pm
    Author:  Ryan Swannick

  ==============================================================================
*/

#include "SettingsPage.h"

SettingsPageComponent::SettingsPageComponent(juce::AudioProcessorValueTreeState& apvts)
{
    
    qualityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "QUALITY", mQuality);
    
    setBackground();
    addComponents();
}

SettingsPageComponent::~SettingsPageComponent()
{
    mOSCColor2.setLookAndFeel(nullptr);
    mOSCColor1.setLookAndFeel(nullptr);
    mOSCColor1Label.setLookAndFeel(nullptr);
    mOSCColor2Label.setLookAndFeel(nullptr);
    mColorSelector.setLookAndFeel(nullptr);
    mColorSelector2.setLookAndFeel(nullptr);
    mClose.setLookAndFeel(nullptr);
//    if (juce::JUCEApplication::isStandaloneApp())
//    audioSettings->setLookAndFeel(nullptr);
//    qualityAttachment.reset();
    mQuality.setLookAndFeel(nullptr);
    mQualityLabel.setLookAndFeel(nullptr);
//    mSettingsViewPort.deleteAllChildren();
    mSettingsViewPort.setLookAndFeel(nullptr);
//    audioSettings.get()->setLookAndFeel(nullptr);
    mSettingsViewPort.removeAllChildren();
    
    audioSettings.reset();
}

void SettingsPageComponent::paint (juce::Graphics& g)
{

}

void SettingsPageComponent::resized()
{
    auto bounds = getLocalBounds();
    
    float switchH;
    float switchW;
    
    float aspectRatio = 82.0f / 108.0f; // Width divided by height
    float availableWidth = bounds.getWidth() * 0.10f; // Considering 95% of available width
    float availableHeight = bounds.getHeight() * 0.10f; // Considering 95% of available height

    // Calculate button width and height based on the aspect ratio
    if (availableWidth / availableHeight > aspectRatio) {
        // Height is limiting factor
       switchH = availableHeight;
       switchW = switchH * aspectRatio;
    } else {
        // Width is limiting factor
        switchW = availableWidth;
        switchH = switchW / aspectRatio;
    }
    
    mBackground.setBounds(bounds);

    if (juce::JUCEApplication::isStandaloneApp())
    {
        mSettingsViewPort.setBounds(bounds.getX() + (bounds.getWidth() * .135), bounds.getY() + (bounds.getHeight() * .15), bounds.getWidth() * .65, bounds.getHeight() * .60 );
        
        audioSettings->setItemHeight(bounds.getHeight() * .070);
//        audioSettings->setBounds(bounds.getX() + (bounds.getWidth() * .10), bounds.getY() + (bounds.getHeight() * .095), bounds.getWidth() * .70, bounds.getHeight() * .70);
        audioSettings->setBounds(mSettingsViewPort.getX(), mSettingsViewPort.getY(), bounds.getWidth() * .60, bounds.getHeight() * .70);

    }

    
    if (JUCE_IOS)
    { mClose.setBounds(bounds.getX() + (bounds.getWidth() * .775), bounds.getY() + (bounds.getHeight() * .0425), bounds.getWidth() * .055, bounds.getHeight() * .085); }
    else
    { mClose.setBounds(bounds.getX() + (bounds.getWidth() * .785), bounds.getY() + (bounds.getHeight() * .0425), bounds.getWidth() * .050, bounds.getHeight() * .085); }
    
    mOSCColor1.setBounds(bounds.getX() + (bounds.getWidth() * .25), bounds.getY() + (bounds.getHeight() * .825), switchW, switchH);
   
    mColorSelector.setBounds(bounds.getX() + (bounds.getWidth() * .15), bounds.getY() + (bounds.getHeight() * .125), bounds.getWidth() * .70, bounds.getHeight() * .70);
    
    mOSCColor2.setBounds(bounds.getX() + (bounds.getWidth() * .725), bounds.getY() + (bounds.getHeight() * .825), switchW, switchH);
    mColorSelector2.setBounds(bounds.getX() + (bounds.getWidth() * .15), bounds.getY() + (bounds.getHeight() * .125), bounds.getWidth() * .70, bounds.getHeight() * .70);
    
    mOSCColor1Label.setBounds(mOSCColor1.getRight() * .70, bounds.getY() + (bounds.getHeight() * .770), bounds.getWidth() * .35, bounds.getHeight() * .20);
    mOSCColor2Label.setBounds(mOSCColor2.getX() * .65, bounds.getY() + (bounds.getHeight() * .770), bounds.getWidth() * .35, bounds.getHeight() * .20);

//    mQuality.setBounds(bounds.getX() + (bounds.getWidth() * .40), bounds.getY() + (bounds.getHeight() * .75), switchW, switchH);
//    mQualityLabel.setBounds(mQuality.getRight() * .80, bounds.getY() + (bounds.getHeight() * .70), bounds.getWidth() * .35, bounds.getHeight() * .20);
    
}

void SettingsPageComponent::setBackground()
{
    backgroundImg = juce::ImageCache::getFromMemory (BinaryData::SettingsWindow_png, BinaryData::SettingsWindow_pngSize);
    mBackground.setImage(backgroundImg);
    addAndMakeVisible(mBackground);

}
void SettingsPageComponent::addComponents()
{
    if (juce::JUCEApplication::isStandaloneApp())
    {
        updateDeviceManager();
        addAndMakeVisible(audioSettings.get());
        
        mSettingsViewPort.setScrollOnDragMode(juce::Viewport::ScrollOnDragMode::nonHover);
        mSettingsViewPort.setViewedComponent(audioSettings.get());
        mSettingsViewPort.setScrollBarPosition(true, false);
        mSettingsViewPort.setScrollBarsShown(true, true);
        mSettingsViewPort.addChildComponent(audioSettings.get());
        addAndMakeVisible(mSettingsViewPort);

    }
    addAndMakeVisible(mQualityLabel);
    //
    mQuality.setClickingTogglesState(true);
    //
    addAndMakeVisible(mOSCColor1Label);
    addAndMakeVisible(mOSCColor2Label);
    //
    mOSCColor1.setName("PHASE");
    mOSCColor1.setLookAndFeel(&cLAF.lOSC);
    addAndMakeVisible(mOSCColor1);
    //
    mQuality.setName("PHASE");
    mQuality.setLookAndFeel(&cLAF.lOSC);
    addAndMakeVisible(mQuality);
    //
    mOSCColor2.setName("PHASE");
    mOSCColor2.setLookAndFeel(&cLAF.lOSC);
    addAndMakeVisible(mOSCColor2);
    //
    mColorSelector.setColour(juce::ColourSelector::ColourIds::backgroundColourId, juce::Colour::fromRGB(40, 40, 40));
    mColorSelector2.setColour(juce::ColourSelector::ColourIds::backgroundColourId, juce::Colour::fromRGB(40, 40, 40));
    //
    addChildComponent(mColorSelector);
    addChildComponent(mColorSelector2);
    //
    mClose.setLookAndFeel(&cLAF.lOSC);
    addAndMakeVisible(mClose);
    mClose.onClick = [&] { this->setVisible(false); updateDeviceManager(); };
    
}

void SettingsPageComponent::updateDeviceManager()
{
    auto pluginHolder = juce::StandalonePluginHolder::getInstance();
    audioSettings.reset(new juce::AudioDeviceSelectorComponent(pluginHolder->deviceManager, 0, 2, 2, 2, false, false, false, false) );
    audioSettings->setLookAndFeel(&cLAF.lOSC);
    pluginHolder->shouldMuteInput = false;
    pluginHolder->deviceManager.setAudioDeviceSetup(audioSettings.get()->deviceManager.getAudioDeviceSetup(), true);
}

void SettingsPageComponent::selectColor()
{
    if (mOSCColor1.getToggleState())
    { mColorSelector.setVisible(true); mColorSelector2.setVisible(false); }
    else
    { mColorSelector.setVisible(false); }
    vSelectedColorOSC1 = mColorSelector.getCurrentColour();
}

void SettingsPageComponent::selectColor2()
{
    if (mOSCColor2.getToggleState())
    { mColorSelector.setVisible(false); mColorSelector2.setVisible(true); }
    else
    {  mColorSelector2.setVisible(false); }
    vSelectedColorOSC2 = mColorSelector2.getCurrentColour();
}

bool SettingsPageComponent::audioDeviceNotSelected(bool deviceSelected)
{
    juce::String deviceName;
    int channelsActive { 0 };
    
    float inputLevel = audioSettings->deviceManager.getInputLevelGetter()->getCurrentLevel();

    if (audioSettings->deviceManager.createStateXml() != nullptr)
    {
        juce::String xmlString = audioSettings->deviceManager.createStateXml().get()->toString().fromFirstOccurrenceOf("audioInputDeviceName=\"", false, true).upToFirstOccurrenceOf("\"", false, true);
        
        juce::String channelsNumEngaged { audioSettings->deviceManager.createStateXml().get()->toString().fromFirstOccurrenceOf("audioDeviceInChans=\"", false, true).upToFirstOccurrenceOf("\"", false, true) };
                
        if ( channelsNumEngaged.isNotEmpty() )
        { channelsActive = std::stoi(channelsNumEngaged.toStdString()); }
        
        if (xmlString.toStdString().size() > 3)
        {
            deviceName.swapWith(xmlString);
            if ( channelsNumEngaged.isNotEmpty() )
            { channelsActive = std::stoi(channelsNumEngaged.toStdString());}
        }
        else
        {
            deviceName.clear();
        }
    }
        
    if ((deviceName.isEmpty()) && inputLevel == 0) {  audioInputSelect = true;  }
    else if ((deviceName.isNotEmpty() || channelsActive > 1) ||  inputLevel > 0)  {  audioInputSelect = false;  }
        
    return  audioInputSelect;
}
