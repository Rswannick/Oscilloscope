/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OscilliscopeAudioProcessorEditor::OscilliscopeAudioProcessorEditor (OscilliscopeAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p),
cGlobal(p.apvts, "GAIN", "ROTATION", "LINE THICKNESS", "MONO/STEREO"),
cCH1(p.apvts, "CH 1 V OFFSET", "CH 1 H OFFSET", "CH 1 VOLTS", "CH 1 SECONDS"),
cCH2(p.apvts, "CH 2 V OFFSET", "CH 2 H OFFSET", "CH 2 VOLTS", "CH 2 SECONDS"),
oScopeScreen(p),
cTopBar(p)
{
    addLookAndFeel();
    addComponents();
    addListeners();
    setGUISize();
}

OscilliscopeAudioProcessorEditor::~OscilliscopeAudioProcessorEditor()
{
    stopTimer();
    mBypass.setLookAndFeel(nullptr);
    mPolarity.setLookAndFeel(nullptr);
    mXYMode.setLookAndFeel(nullptr);
    mFullscreen.setLookAndFeel(nullptr);
    blackBox.setLookAndFeel(nullptr);
    
//    mFSAttachment.reset();
//    mXYAttachment.reset();
}

//==============================================================================
void OscilliscopeAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::black);
    g.fillAll();
}

void OscilliscopeAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    bounds = setiOSBounds(bounds);
    
    float scopeX = bounds.getX() + (bounds.getWidth() * (0.05));
    float scopeY = bounds.getY() + bounds.getHeight() / 6;
    float scopeW = bounds.getWidth() / 2.0;
    float scopeH = bounds.getHeight() / 1.585;
    
    float globalX = bounds.getX() +  bounds.getWidth() * .11;
    float globalY = bounds.getY() + bounds.getHeight() * .875;
    float globalW = bounds.getWidth() * .7915;
    float globalH = bounds.getHeight() * .0825;
    
    float chX =  bounds.getX() + bounds.getWidth() * .610;
    float ch1Y = bounds.getY() + bounds.getHeight() * .225;
    float ch2Y = bounds.getY() + bounds.getHeight() * .54;
    float chW = bounds.getWidth() * .325;
    float chH = bounds.getHeight() * .130;
    
    float buttonX =  bounds.getX() + bounds.getWidth() * .895;
    float buttonY = bounds.getY() + bounds.getHeight() * .74;
    float buttonW = bounds.getHeight() * .080;
    float buttonH = bounds.getHeight() * .080;
    
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

    //Visuals
    float backGroundW = bounds.getWidth();
    float backGroundH = bounds.getHeight();
    float backGroundX = bounds.getX();
    float backGroundY = bounds.getY();
    
    //Background
    cBackground.setBounds(backGroundX, backGroundY, backGroundW, backGroundH);

    if (audioProcessor.fullScreen)
    {
        blackBox.setBounds(backGroundX, backGroundY + (bounds.getHeight() * .099), backGroundW, backGroundH * .90);
        //Scope screen
        oScopeScreen.setBounds(backGroundX, backGroundY + (bounds.getHeight() * .099), backGroundW, backGroundH * (.90));
        //Top Bar
        cTopBar.setBounds(backGroundX, backGroundY, backGroundW, backGroundH);
    }
    else
    {
        //Top Bar
        cTopBar.setBounds(backGroundX, backGroundY, backGroundW, backGroundH);
        //Oscillator Screen
        oScopeScreen.setBounds(scopeX, scopeY, scopeW, scopeH);
        //Controls
        cGlobal.setBounds(globalX, globalY, globalW, globalH);
        cCH1.setBounds(chX, ch1Y, chW, chH);
        cCH2.setBounds(chX, ch2Y, chW, chH);
        //Power
        mBypass.setBounds(buttonX, buttonY, buttonW, buttonH);
        //Polarity
        mPolarity.setBounds(mBypass.getX() - (buttonW * 3.85), buttonY + (bounds.getHeight() * .001), switchW, switchH);
    }
    
    //Fullscreen XY Mode
    mFullscreen.setBounds(bounds.getX() + (bounds.getWidth() * .69), backGroundY + (bounds.getHeight() * .0335), buttonW * .5, buttonH * .5);
    mXYMode.setBounds(bounds.getX() + (bounds.getWidth() * .735), backGroundY + (bounds.getHeight() * .0395), buttonW * .86, buttonW * .36);
    
    //Variable Parameters
    audioProcessor.windowW = bounds.getWidth();
    audioProcessor.windowH = bounds.getHeight();
    //Top Bar Name Fix
    if (audioProcessor.pname.isNotEmpty()) { cTopBar.setPresetBarText(audioProcessor.pname); }
}

void OscilliscopeAudioProcessorEditor::buttonStateChanged(juce::Button *button)
{

    if ( mBypass.getToggleState() )
    {
        if (cGlobal.mMonoStereo.getToggleState())
        { oScopeScreen.cOscope.setVisible(true);  oScopeScreen.cOscope2.setVisible(true); }
        else { oScopeScreen.cOscope.setVisible(true); oScopeScreen.cOscope2.setVisible(false); }
    }
    else
    {
        if (cGlobal.mMonoStereo.getToggleState()) { oScopeScreen.cOscope.setVisible(false);  oScopeScreen.cOscope2.setVisible(false); }
        else { oScopeScreen.cOscope.setVisible(false); oScopeScreen.cOscope2.setVisible(false); }
    }
    
    if (button == &cGlobal.mMonoStereo)
    {
       if (cGlobal.mMonoStereo.getToggleState()) { oScopeScreen.cOscope.setVisible(true);  oScopeScreen.cOscope2.setVisible(true); }
       else { oScopeScreen.cOscope.setVisible(true); oScopeScreen.cOscope2.setVisible(false); }
    }
    
    if (button == &mPolarity)
    {
        oScopeScreen.cOscope.scopeComponent.mPhase = audioProcessor.apvts.getRawParameterValue("PHASE")->load();
        oScopeScreen.cOscope2.scopeComponent.mPhase =
        audioProcessor.apvts.getRawParameterValue("PHASE")->load();
    }
    
    if (button == &mXYMode)
    {
        if (mXYMode.getToggleState())
        {
            audioProcessor.xyMode = true;
        }
        else
        {
            audioProcessor.xyMode = false;
        }
    }
    
    if (button == &mFullscreen)
    {
        if (button->getToggleState())
        {
            audioProcessor.fullScreen = true;
            blackBox.setVisible(true);
            //Controls
            cGlobal.setVisible(false);
            cCH1.setVisible(false);
            cCH2.setVisible(false);
            //Power
            mBypass.setVisible(false);
            //Polarity
            mPolarity.setVisible(false);
        }
        else
        {
            audioProcessor.fullScreen = false;
            //
            blackBox.setVisible(false);
            //Controls
            cGlobal.setVisible(true);
            cCH1.setVisible(true);
            cCH2.setVisible(true);
            //Power
            mBypass.setVisible(true);
            //Polarity
            mPolarity.setVisible(true);
        }
        repaint();
        resized();
    }
}

void OscilliscopeAudioProcessorEditor::buttonClicked(juce::Button *button)
{
    //Necessary function for Button Listener
    if (button == &cTopBar.cSettings.mOSCColor1)
    {
        cTopBar.cSettings.selectColor();
        setOSCColor1();
    }
    
    if (button == &cTopBar.cSettings.mOSCColor2)
    {
        cTopBar.cSettings.selectColor2();
        setOSCColor2();
    }

}

void OscilliscopeAudioProcessorEditor::timerCallback()
{
    if (juce::JUCEApplication::isStandaloneApp())
    {
        oScopeScreen.cOscope.scopeComponent.shouldPaint = cTopBar.cSettings.audioDeviceNotSelected(false);
    }
    
    oScopeScreen.cOscope.scopeComponent.mFreq = audioProcessor.FREQ;
    
    oScopeScreen.rmsValue = audioProcessor.smoothedRMS.getNextValue();
    
    oScopeScreen.cOscope.scopeComponent.rmsValue = audioProcessor.smoothedRMS.getNextValue();
    
    oScopeScreen.cOscope.scopeComponent.peakValue = audioProcessor.PEAKL;
    
    cTopBar.cMasterSection.cMeter.currentPeakL = audioProcessor.PEAKL;
    cTopBar.cMasterSection.cMeter.currentPeakR = audioProcessor.PEAKR;
    
    oScopeScreen.cOverlay.repaint();
}

void OscilliscopeAudioProcessorEditor::openSettings()
{
    auto pluginHolder = juce::StandalonePluginHolder::getInstance();
    pluginHolder->showAudioSettingsDialog();
    pluginHolder->shouldMuteInput = false;
}

void OscilliscopeAudioProcessorEditor::addComponents()
{
    mXYAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "XY Mode", mXYMode);
    mFSAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "Fullscreen", mFullscreen);
    mBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "POWER", mBypass);
    mPolarityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "PHASE", mPolarity);
    cGlobal.mMonoStereo.setToggleState(false, juce::NotificationType::dontSendNotification);
    //Add Visual subcomponents
    addAndMakeVisible(cBackground);
    addChildComponent(blackBox);
    addAndMakeVisible(oScopeScreen);
    oScopeScreen.cOscope2.setVisible(false);
    //Add Control Subcomponents
    addAndMakeVisible(cGlobal);
    addAndMakeVisible(cCH1);
    addAndMakeVisible(cCH2);
    //
    cGlobal.mThicknessSlider.addListener(this);
    cGlobal.mRotationSlider.addListener(this);
    cTopBar.cSettings.mOSCColor1.addListener(this);
    cTopBar.cSettings.mOSCColor2.addListener(this);
    cTopBar.cPresetManager.mLoadPresetDropDown.addListener(this);
    mPolarity.addListener(this);
    //
    cCH1.mVoltsSlider.addListener(this);
    cCH1.mSecondsSlider.addListener(this);
    cCH1.mVOffsetSlider.addListener(this);
    cCH1.mHOffsetSlider.addListener(this);
    //
    cCH2.mVoltsSlider.addListener(this);
    cCH2.mSecondsSlider.addListener(this);
    cCH2.mVOffsetSlider.addListener(this);
    cCH2.mHOffsetSlider.addListener(this);
    mFullscreen.addListener(this);
    mXYMode.addListener(this);
    //
    //Bypass Polarity
    addAndMakeVisible(mBypass);
    addAndMakeVisible(mPolarity);
    addAndMakeVisible(mXYMode);
    addAndMakeVisible(mFullscreen);
    addAndMakeVisible(cTopBar);
    //
}

void OscilliscopeAudioProcessorEditor::addListeners()
{
    //Listeners
    cGlobal.mMonoStereo.addListener(this);
    mBypass.addListener(this);
}

void OscilliscopeAudioProcessorEditor::addLookAndFeel()
{
    juce::LookAndFeel::getDefaultLookAndFeel().setDefaultLookAndFeel(&cLAF.lOSC);
    
    mBypass.setName("POWER"); mBypass.setLookAndFeel(&cLAF.lOSC);
    mPolarity.setName("PHASE"); mPolarity.setLookAndFeel(&cLAF.lOSC);
    
    mXYMode.setName("XY"); mXYMode.setLookAndFeel(&cLAF.lOSC);
    mFullscreen.setName("Fullscreen"); mFullscreen.setLookAndFeel(&cLAF.lOSC);
    
    blackBox.setName("BLACK");
    blackBox.setLookAndFeel(&cLAF.lOSC);


}

void OscilliscopeAudioProcessorEditor::setGUISize()
{
    
    sizeX = audioProcessor.windowW;
    sizeY = audioProcessor.windowH;
    
    
        
    if (JUCE_IOS)
    {
        auto screenSize = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->userArea;
        float width = screenSize.getWidth();
        float height = screenSize.getHeight();
        sizeX = width;
        sizeY = height;
        startTimer(60);
        setSize(sizeX, sizeY);
    }
    else
    {
        setResizable(true, true);
        setResizeLimits(400, 400/ratio, 1200, 1200/ratio);
        getConstrainer()->setFixedAspectRatio(ratio);
        startTimer(30);
        
        if (sizeX > 1200 && sizeX < 800)
        { setSize(sizeX, sizeY); }
        else { setSize(800, 800 /ratio); }
        
        
    }
    
}

juce::Rectangle<int>  OscilliscopeAudioProcessorEditor::setiOSBounds(juce::Rectangle<int> iOSBounds)
{
    if (JUCE_IOS)
    {
        const double targetAspectRatio = 6.0 / 3.0;
        double currentAspectRatio = static_cast<double>(iOSBounds.getWidth()) / iOSBounds.getHeight();

        if (currentAspectRatio > targetAspectRatio)
        {
            int newWidth = static_cast<int>(iOSBounds.getHeight() * targetAspectRatio);
            int xOffset = (iOSBounds.getWidth() - newWidth) / 2;
            iOSBounds.setX(iOSBounds.getX() + xOffset);
            iOSBounds.setWidth(newWidth);
        }
        else if (currentAspectRatio < targetAspectRatio)
        {
            int newHeight = static_cast<int>(iOSBounds.getWidth() / targetAspectRatio);
            int yOffset = (iOSBounds.getHeight() - newHeight) / 2;
            iOSBounds.setY(iOSBounds.getY() + yOffset);
            iOSBounds.setHeight(newHeight);
        }
    }
    
    return iOSBounds;
}

void OscilliscopeAudioProcessorEditor::setOSCColor1()
{
    audioProcessor.sSelectedColorOSC1 = cTopBar.cSettings.vSelectedColorOSC1.toString();
    setPresetColor1();
}
            
void OscilliscopeAudioProcessorEditor::setOSCColor2()
{
    audioProcessor.sSelectedColorOSC2 = cTopBar.cSettings.vSelectedColorOSC2.toString();
    setPresetColor2();
}

void OscilliscopeAudioProcessorEditor::setPresetColor1()
{
    oScopeScreen.cOscope.scopeComponent.selectColor(juce::Colour::fromString(audioProcessor.sSelectedColorOSC1));
    
    cTopBar.cMasterSection.cMeter.vLevelMeterColor1 = juce::Colour::fromString(audioProcessor.sSelectedColorOSC1);
    
    if (audioProcessor.sSelectedColorOSC1.isEmpty())
    {
        oScopeScreen.cOscope.scopeComponent.selectColor(juce::Colours::yellow);
        cTopBar.cMasterSection.cMeter.vLevelMeterColor1 = juce::Colours::yellow;
    }
    
}

void OscilliscopeAudioProcessorEditor::setPresetColor2()
{
    oScopeScreen.cOscope2.scopeComponent.selectColor(juce::Colour::fromString(audioProcessor.sSelectedColorOSC2));
    
    cTopBar.cMasterSection.cMeter.vLevelMeterColor2 = juce::Colour::fromString(audioProcessor.sSelectedColorOSC2);
    
    if (audioProcessor.sSelectedColorOSC2.isEmpty())
    {
        oScopeScreen.cOscope2.scopeComponent.selectColor(juce::Colours::green);
        cTopBar.cMasterSection.cMeter.vLevelMeterColor2 = juce::Colours::green;
    }
}

void OscilliscopeAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &cCH1.mVoltsSlider) { oScopeScreen.cOscope.scopeComponent.mVolts = audioProcessor.apvts.getRawParameterValue("CH 1 VOLTS")->load(); }
    if (slider == &cCH2.mVoltsSlider) { oScopeScreen.cOscope2.scopeComponent.mVolts = audioProcessor.apvts.getRawParameterValue("CH 2 VOLTS")->load(); }

    if (slider == &cCH1.mSecondsSlider) { oScopeScreen.cOscope.scopeComponent.mSeconds = audioProcessor.apvts.getRawParameterValue("CH 1 SECONDS")->load(); }
    if (slider == &cCH2.mSecondsSlider) { oScopeScreen.cOscope2.scopeComponent.mSeconds = audioProcessor.apvts.getRawParameterValue("CH 2 SECONDS")->load(); }

    if (slider == &cCH1.mVOffsetSlider) { oScopeScreen.cOscope.scopeComponent.mVOffset = audioProcessor.apvts.getRawParameterValue("CH 1 V OFFSET")->load(); }
    if (slider == &cCH2.mVOffsetSlider) { oScopeScreen.cOscope2.scopeComponent.mVOffset = audioProcessor.apvts.getRawParameterValue("CH 2 V OFFSET")->load(); }

    if (slider == &cCH1.mHOffsetSlider) { oScopeScreen.cOscope.scopeComponent.mHOffset = audioProcessor.apvts.getRawParameterValue("CH 1 H OFFSET")->load(); }
    if (slider == &cCH2.mHOffsetSlider) { oScopeScreen.cOscope2.scopeComponent.mHOffset = audioProcessor.apvts.getRawParameterValue("CH 2 H OFFSET")->load(); }

    if (slider == &cGlobal.mThicknessSlider) {  oScopeScreen.cOscope.scopeComponent.mThick = audioProcessor.apvts.getRawParameterValue("LINE THICKNESS")->load(); oScopeScreen.cOscope2.scopeComponent.mThick = audioProcessor.apvts.getRawParameterValue("LINE THICKNESS")->load(); }
    
    if (slider == &cGlobal.mGainSlider){
    audioProcessor.cOscilloscope.gainValue = audioProcessor.apvts.getRawParameterValue("GAIN")->load();    audioProcessor.cOscilloscope.gainValue2 = audioProcessor.apvts.getRawParameterValue("GAIN")->load(); }
    
    if (slider == &cGlobal.mRotationSlider) {
        oScopeScreen.cOscope.scopeComponent.mRotation = audioProcessor.apvts.getRawParameterValue("ROTATION")->load();
        oScopeScreen.cOscope2.scopeComponent.mRotation = audioProcessor.apvts.getRawParameterValue("ROTATION")->load(); }
}


void OscilliscopeAudioProcessorEditor::comboBoxChanged(juce::ComboBox *box)
{
    if (box == &cTopBar.cPresetManager.mLoadPresetDropDown)
    {
        setPresetColor1();
        setPresetColor2();
        
        sliderValueChanged(&cCH1.mVoltsSlider);
        sliderValueChanged(&cCH2.mVoltsSlider);

        sliderValueChanged(&cCH1.mSecondsSlider);
        sliderValueChanged(&cCH2.mSecondsSlider);
        
        sliderValueChanged(&cCH1.mVOffsetSlider);
        sliderValueChanged(&cCH2.mVOffsetSlider);

        sliderValueChanged(&cCH1.mHOffsetSlider);
        sliderValueChanged(&cCH2.mHOffsetSlider);
        
        sliderValueChanged(&cGlobal.mGainSlider);
        sliderValueChanged(&cGlobal.mThicknessSlider);
        sliderValueChanged(&cGlobal.mRotationSlider);
        
        buttonStateChanged(&cGlobal.mMonoStereo);
        buttonStateChanged(&mPolarity);
        
        buttonStateChanged(&mFullscreen);

        audioProcessor.xyMode = bool(audioProcessor.apvts.getRawParameterValue("XY Mode")->load());
        buttonStateChanged(&mXYMode);
                
        resized();
        repaint();
        
        
    }
}

