/*
  ==============================================================================

    InfoPage.h
    Created: 15 Jan 2024 9:25:38pm
    Author: Ryan Swannick!

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../../LookAndFeel/LAF.h"
//====================================================&==========================
/*
*/
class InfoPageComponent  : public juce::Component
{
public:
    InfoPageComponent();
    ~InfoPageComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void openWebsite();
    void openNewsletter();
    void openContact();
    void openGumroad();
    
    void openDiscord();
    void openFB();
    void openIG();
    void openYT();
    
    void showBuildVersion();
    
    juce::ToggleButton mVersionBuild { "Version 1.2" };
    std::string versionNumber
    { "Version 1.2" }, buildNumber { "Build 25E13" };

    juce::String
    
    websiteLink {"https://www.oscaudio.com/"},
    newsletterLink  {"https://oscaudio.gumroad.com/posts"},
    contactLink  {"support@oscaudio.com"},
    gumroadLink  {"https://oscaudio.gumroad.com/"},
    youTubeLink   {"https://youtube.com/@oscaudio"},
    facebookLink  {"https://www.facebook.com/oscaudioofficial/"},
    instagramLink  {"https://www.instagram.com/osc_audio_official/"},
    discordLink {"https://discord.gg/S2mMtP4Y/"};
    
    juce::ToggleButton
    
    mWebsite,
    mNewsLetter,
    mViewMoreApps,
    mContactUs,
    mDiscord,
    mFacebook,
    mInstagram,
    mYouTube,
    mClose;
    
private:
    LAFComponent cLAF;
    
    juce::Image backgroundImg;
    juce::ImageComponent mBackground;
    


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoPageComponent)
};
