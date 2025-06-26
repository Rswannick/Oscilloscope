/*
  ==============================================================================

    TopBar.cpp
    Created: 1 Feb 2024 11:07:33pm
    Author:  Ryan Swannick

  ==============================================================================
*/
#include <JuceHeader.h>
#include "TopBar.h"

TopBarComponent::TopBarComponent(OscilliscopeAudioProcessor& p): cSettings(p.apvts),
cMasterSection(p, p.apvts, "VOLUME"),
cPresetManager(p),
audioProcessor (p)
{
    setInterceptsMouseClicks(false, true);
    //Settings Menu
    mSettings.setLookAndFeel(&cLAF.lOSC);
    mVolume.setLookAndFeel(&cLAF.lOSC);
    mInfo.setLookAndFeel(&cLAF.lOSC);
    mSettings.setName("SETTINGS");
    mVolume.setName("VOLUME");
    mInfo.setName("INFO");
    mSettings.setClickingTogglesState(true);
    mVolume.setClickingTogglesState(true);
    mInfo.setClickingTogglesState(true);
    mInfo.onClick = [this] { openInfo(); };
    mVolume.onClick = [this] { openVolume(); };
    mSettings.onClick = [this] { openSettings(); };
    //Preset Manager
    cPresetManager.setInterceptsMouseClicks(false, true);
    cMasterSection.setInterceptsMouseClicks(false, true);
    //Master Section
    addAndMakeVisible(cPresetManager);
    addAndMakeVisible(cMasterSection);
    addAndMakeVisible(mVolume);
    addChildComponent(cSettings);
    addChildComponent(cInfo);
    addAndMakeVisible(mInfo);
    addAndMakeVisible(mSettings);
    cSettings.mClose.onClick = [this] {  mSettings.setToggleState(false, juce::dontSendNotification); cSettings.setVisible(false);};
    cInfo.mClose.onClick = [this] {  mInfo.setToggleState(false, juce::dontSendNotification); cInfo.setVisible(false);};
    cInfo.setInterceptsMouseClicks(false, true);
    //Settings
    restoreGUISize();
    //Handle iOS Preset Create
    createPresetDirectory();
}

TopBarComponent::~TopBarComponent()
{
    shutdown();
    mSettings.setLookAndFeel(nullptr);
    mInfo.setLookAndFeel(nullptr);
    mVolume.setLookAndFeel(nullptr);
}

void TopBarComponent::paint (juce::Graphics& g)
{

}

void TopBarComponent::resized()
{
    auto bounds = getLocalBounds();
    float buttonWidth = bounds.getHeight() * .075;
    float buttonHeight = bounds.getHeight() * .075;
    
    //Settings Buttons
    mSettings.setBounds(bounds.getRight() - (bounds.getWidth() * .105), bounds.getY() + (bounds.getHeight() * .033), buttonWidth * .475, buttonHeight * .475);
    mInfo.setBounds(bounds.getRight() - (bounds.getWidth() * .15), mSettings.getY(), buttonWidth * .475, buttonHeight * .475);
    mVolume.setBounds(bounds.getRight() - (bounds.getWidth() * .20), mSettings.getY() , buttonWidth * .475, buttonHeight * .475);
    
    //Settings Windows
    cSettings.setBounds(bounds.getX(), bounds.getY(), (bounds.getWidth()), bounds.getHeight());
    cInfo.setBounds(bounds.getX() + (bounds.getWidth() * .20), bounds.getY() + (bounds.getHeight() * .13), (bounds.getWidth() * .70), bounds.getHeight() * .75);
    
    //Preset Bar (Top Bar)
    cPresetManager.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight() * .10);
    cMasterSection.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
}

void TopBarComponent::openSettings()
{
    
    if (!cSettings.isVisible())
    {
        cSettings.setVisible(true); mSettings.setToggleState(true, juce::sendNotification);
        cSettings.toFront(true);
        mVolume.setToggleState(false, juce::dontSendNotification); cMasterSection.mVolumeSlider.setVisible(false);
        mInfo.setToggleState(false, juce::dontSendNotification); cInfo.setVisible(false);
        cSettings.selectColor();
        cSettings.selectColor2();
        cSettings.mOSCColor1.setToggleState(false, juce::dontSendNotification);
        cSettings.mOSCColor2.setToggleState(false, juce::dontSendNotification);
        cSettings.mColorSelector.setVisible(false);
        cSettings.mColorSelector2.setVisible(false);
    }
    else
    {
        cSettings.setVisible(false); mSettings.setToggleState(false, juce::sendNotification);
        mInfo.setToggleState(false, juce::dontSendNotification); cInfo.setVisible(false);
        mVolume.setToggleState(false, juce::dontSendNotification); cMasterSection.mVolumeSlider.setVisible(false);
        cSettings.selectColor();
        cSettings.selectColor2();
        cSettings.mOSCColor1.setToggleState(false, juce::dontSendNotification);
        cSettings.mOSCColor2.setToggleState(false, juce::dontSendNotification);
        cSettings.mColorSelector.setVisible(false);
        cSettings.mColorSelector2.setVisible(false);
    }

}

void TopBarComponent::openInfo()
{
    if (!cInfo.isVisible())
    {
        cInfo.setVisible(true); mInfo.setState(juce::Button::ButtonState::buttonDown);
        mSettings.setToggleState(false, juce::sendNotification); cSettings.setVisible(false);
        mVolume.setToggleState(false, juce::dontSendNotification); cMasterSection.mVolumeSlider.setVisible(false);
        cSettings.mOSCColor1.setToggleState(false, juce::dontSendNotification);
        cSettings.mOSCColor2.setToggleState(false, juce::dontSendNotification);
        cSettings.mColorSelector.setVisible(false);
        cSettings.mColorSelector2.setVisible(false);
        cSettings.selectColor();
        cSettings.selectColor2();
    }
    else
    {
        mInfo.setState(juce::Button::ButtonState::buttonNormal); cInfo.setVisible(false);
        mSettings.setToggleState(false, juce::sendNotification); cSettings.setVisible(false);
        mVolume.setToggleState(false, juce::dontSendNotification); cMasterSection.mVolumeSlider.setVisible(false);
        cSettings.mOSCColor1.setToggleState(false, juce::dontSendNotification);
        cSettings.mOSCColor2.setToggleState(false, juce::dontSendNotification);
        cSettings.mColorSelector.setVisible(false);
        cSettings.mColorSelector2.setVisible(false);
        cSettings.selectColor();
        cSettings.selectColor2();
    }
}

void TopBarComponent::openVolume()
{
    
    if (!cMasterSection.mVolumeSlider.isVisible())
    {
        mVolume.setToggleState(true, juce::sendNotification); cMasterSection.mVolumeSlider.setVisible(true);
        mInfo.setToggleState(false, juce::dontSendNotification); cInfo.setVisible(false);
        mSettings.setToggleState(false, juce::sendNotification); cSettings.setVisible(false);
    }
    else
    {
        mVolume.setToggleState(false, juce::sendNotification); cMasterSection.mVolumeSlider.setVisible(false);
        mInfo.setToggleState(false, juce::dontSendNotification); cInfo.setVisible(false);
        mSettings.setToggleState(false, juce::sendNotification); cSettings.setVisible(false);
        cSettings.mOSCColor1.setToggleState(false, juce::dontSendNotification);
        cSettings.mOSCColor2.setToggleState(false, juce::dontSendNotification);
        cSettings.mColorSelector.setVisible(false);
        cSettings.mColorSelector2.setVisible(false);
    }
}

void TopBarComponent::createPresetDirectory()
{
    juce::String defaultPresetPath = (JUCE_IOS ? "/Presets/Default" : "/OSC Audio/" + appName + "/Presets/Default") + presetSuffix;
    juce::File defaultPresetFile(findDocumentsFolder(JUCE_IOS, "") + defaultPresetPath);
    if (!defaultPresetFile.existsAsFile())
        defaultPresetFile.create();

}


void TopBarComponent::shutdown()
{
    if (juce::JUCEApplication::isStandaloneApp())
    {
        juce::String documentsPath = findDocumentsFolder(JUCE_IOS, "");
        
        juce::String guiSettingsLocation;
        juce::String audioSettingsLocation;
        
        if (JUCE_IOS)
        {
            guiSettingsLocation = "/Settings/guisettings.recall";
            audioSettingsLocation = "/Settings/audiosettings.recall";
        }
        else
        {
            guiSettingsLocation = "/OSC Audio/" + appName + "/guisettings.recall";
            audioSettingsLocation = "/OSC Audio/" + appName +"/audiosettings.recall";
        }
        
        juce::File guiSettingsFile (documentsPath + guiSettingsLocation);
        juce::File audioSettingsFile (documentsPath + audioSettingsLocation);
        
        juce::ScopedPointer<juce::XmlElement> xml = new juce::XmlElement("PROPERTIES");
        juce::XmlElement* element1 = xml->createNewChildElement("GUI");
        
        element1->setAttribute("Width", getLocalBounds().getWidth());
        element1->setAttribute("Height", getLocalBounds().getHeight());
        
        juce::FileOutputStream guiPropertiesStream (guiSettingsFile);
        
        if (guiSettingsFile.create())
        { guiPropertiesStream.writeString(xml->toString()); }
        else if (guiSettingsFile.existsAsFile())
        { guiSettingsFile.replaceWithText(xml->toString()); }
        
        if (audioSettingsFile.create()) {
            
            if (cSettings.audioSettings->deviceManager.createStateXml() != nullptr)
            { cSettings.audioSettings->deviceManager.createStateXml()->writeTo(audioSettingsFile); }
            
        }
    }
}

void TopBarComponent::restoreGUISize()
{
    if (juce::JUCEApplication::isStandaloneApp())
    {
        juce::String documentsPath = findDocumentsFolder(JUCE_IOS, "");
        
        juce::String guiSettingsLocation;
        juce::String audioSettingsLocation;
        
        if (JUCE_IOS)
        {
            guiSettingsLocation = "/Settings/guisettings.recall";
            audioSettingsLocation = "/Settings/audiosettings.recall";
        }
        else
        {
            guiSettingsLocation = "/OSC Audio/" + appName + "/guisettings.recall";
            audioSettingsLocation = "/OSC Audio/" + appName +"/audiosettings.recall";
        }
        
        juce::File guiSettingsFile (documentsPath + guiSettingsLocation);
        juce::File audioSettingsFile (documentsPath + audioSettingsLocation);
        
        if (guiSettingsFile.existsAsFile())
        {
            juce::XmlDocument xmlDoc(guiSettingsFile);
            
            std::unique_ptr<juce::XmlElement> propertiesElement(xmlDoc.getDocumentElement()); // Store in unique_ptr
            
            if (propertiesElement != nullptr && propertiesElement->getTagName() == "PROPERTIES")
            {
                if (auto* guiElement = propertiesElement->getChildByName("GUI"))
                {
                    // Get the width and height attributes as strings
                    juce::String widthString = guiElement->getStringAttribute("Width");
                    juce::String heightString = guiElement->getStringAttribute("Height");
                    
                    // Convert the string attributes to float values
                    float width = widthString.getFloatValue();
                    float height = heightString.getFloatValue();
                    
                    // Now you have the float values of width and height
                    // You can use them as needed
                    audioProcessor.windowW = width;
                    audioProcessor.windowH = height;
                }
            }
        }
        
        if (audioSettingsFile.existsAsFile())
        {
            std::unique_ptr<juce::XmlElement> oldDeviceManagerXML (parseXML(audioSettingsFile));
            const juce::XmlElement* oldDeviceManagerXMLElement (oldDeviceManagerXML.get());
            cSettings.audioSettings->deviceManager.initialise(0, 2, oldDeviceManagerXMLElement, true);
        }
    }
}


juce::String TopBarComponent::findDocumentsFolder(bool isIOS, juce::String documentsPath)
{
    documentsPath = (JUCE_IOS ? juce::File::getSpecialLocation(juce::File::userDocumentsDirectory) : juce::File::getSpecialLocation(juce::File::commonDocumentsDirectory)).getFullPathName();
    return documentsPath;
}


void TopBarComponent::mouseDrag(const juce::MouseEvent &event)
{
    juce::Slider* sliders[] = {
        &cMasterSection.mVolumeSlider
    };
    
    if (juce::Slider* slider = dynamic_cast<juce::Slider*>(event.eventComponent))
    {
        stopTimer();

        // If the right mouse button is pressed, execute the following block
        for (juce::Slider* s : sliders)
        {
            if (s == slider)
            {
                sliderMouseUp = false;
                sliderMouseDragging = true;
                break;
            }
        }
    }
}

void TopBarComponent::mouseUp(const juce::MouseEvent &event)
{
    juce::Slider* sliders[] = {
        &cMasterSection.mVolumeSlider
    };
    
    if (juce::Slider* slider = dynamic_cast<juce::Slider*>(event.eventComponent))
    {
        stopTimer();

        // If the right mouse button is pressed, execute the following block
        for (juce::Slider* s : sliders)
        {
            if (s == slider)
            {
                sliderMouseUp = true;
                sliderMouseDown = false;
                sliderMouseDragging = false;
                break;
            }
        }
    }
}

void TopBarComponent::mouseDown(const juce::MouseEvent &event)
{
    juce::Slider* sliders[] = {
        &cMasterSection.mVolumeSlider
    };
    
    if (juce::Slider* slider = dynamic_cast<juce::Slider*>(event.eventComponent))
    {
        sliderMouseDragging = false;
        sliderMouseUp = false;
        sliderMouseDown = true;
        currentlyHeldSlider = slider;
        startTimer(30);
                
        // If eventComponent is a Slider, check if the right mouse button is pressed
        if (event.mods.isRightButtonDown())
        {
            // If the right mouse button is pressed, execute the following block
            for (juce::Slider* s : sliders)
            {
                if (s == slider)
                {
                    openAlertBoxForSliderValue(slider);
                    break;
                }
            }
        }
    }
}

void TopBarComponent::openAlertBoxForSliderValue(juce::Slider* slider)
{

    double currentValue = slider->getValue();
    auto* parentWindow = getTopLevelComponent();
    auto* p_alert = new juce::AlertWindow("", "", juce::AlertWindow::NoIcon, this);
    EnterKeyListener* enterKeyListener = new EnterKeyListener(p_alert);

    p_alert->setTitle("INSERT VALUE");
    p_alert->addTextEditor("Value", juce::String(currentValue));
    p_alert->getTextEditor("Value")->addKeyListener(enterKeyListener);
    p_alert->getTextEditor("Value")->selectAll();
    p_alert->addButton("CANCEL", 0); p_alert->addButton("DONE", 1);
    p_alert->getButton(0)->setName("ALERTBOX"); p_alert->getButton(1)->setName("ALERTBOX");
    p_alert->setLookAndFeel(&cLAF.lOSC);
    p_alert->setDropShadowEnabled(true);
    p_alert->setEscapeKeyCancels(true);
    p_alert->toFront(true);

    auto localBounds = getLocalBounds();
    float alertX = localBounds.getX() + (localBounds.getWidth() * 0.15);
    float alertY = parentWindow->getLocalBounds().getY() + (localBounds.getHeight() * 0.10);
    float alertW = localBounds.getWidth() * 0.70;
    float alertH = localBounds.getHeight() * 0.40;
    p_alert->setBounds(alertX, alertY, alertW, alertH);

    auto* valueEditor = p_alert->getTextEditor("Value");
    valueEditor->setFont(valueEditor->getHeight() * 0.65);
    valueEditor->setJustification(juce::Justification::centred);
    valueEditor->setBounds(alertX - (alertW * 0.075), alertY + (alertH * 0.10), alertW * 0.75, alertH * 0.25);

    p_alert->getButton(0)->setBounds(valueEditor->getX() + (valueEditor->getWidth() * 0.30), valueEditor->getBottom(), alertW * 0.10, valueEditor->getHeight() * 0.80);
    p_alert->getButton(1)->setBounds(p_alert->getButton(0)->getRight(), valueEditor->getBottom(), alertW * 0.20, valueEditor->getHeight() * 0.80);

    addChildComponent(p_alert);
    p_alert->toFront(true);
    

    p_alert->enterModalState(true, juce::ModalCallbackFunction::create([=](int result) {
        valueEditor->grabKeyboardFocus();

        if (result == 1 && valueEditor != nullptr) {
            double newValue = valueEditor->getText().getDoubleValue();
            slider->setValue(newValue, juce::sendNotification);
            valueEditor->grabKeyboardFocus();

        }
        p_alert->exitModalState(result);
        p_alert->setLookAndFeel(nullptr);
        delete p_alert;
    }));


}


void TopBarComponent::timerCallback()
{

    if (sliderMouseDown)
    {
        if (!sliderMouseUp && !sliderMouseDragging) { mouseDownTime = mouseDownTime + 1; }
        else { mouseDownTime = 0; }
        
        if (mouseDownTime > 10)
        {
            openAlertBoxForSliderValue(currentlyHeldSlider);
            mouseDownTime = 0;
            sliderMouseDown = false;
        }
    }
    else {  mouseDownTime = 0; }

}


void TopBarComponent::setPresetBarText(juce::String presetName)
{
    cPresetManager.setPresetBarText(presetName);
}
