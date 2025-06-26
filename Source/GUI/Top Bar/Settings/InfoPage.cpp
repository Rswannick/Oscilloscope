/*
  ==============================================================================

    InfoPage.cpp
    Created: 15 Jan 2024 9:25:38pm
    Author: Author:  Ryan Swannick

  ==============================================================================
*/
#include <JuceHeader.h>
#include "InfoPage.h"


InfoPageComponent::InfoPageComponent()
{
        
    backgroundImg = juce::ImageCache::getFromMemory (BinaryData::AboutWindow_png, BinaryData::AboutWindow_pngSize);
    mBackground.setImage(backgroundImg);
    addAndMakeVisible(mBackground);
    
    mWebsite.setName("Empty");
    mWebsite.setLookAndFeel(&cLAF.lOSC);
    addAndMakeVisible(mWebsite);
    mWebsite.onClick = [&] { openWebsite(); };
    
    mNewsLetter.setName("Empty");
    mNewsLetter.setLookAndFeel(&cLAF.lOSC);
    addAndMakeVisible(mNewsLetter);
    mNewsLetter.onClick = [&] { openNewsletter(); };

    mViewMoreApps.setName("Empty");
    mViewMoreApps.setLookAndFeel(&cLAF.lOSC);
    addAndMakeVisible(mViewMoreApps);
    mViewMoreApps.onClick = [&] { openGumroad(); };

    mContactUs.setName("Empty");
    mContactUs.setLookAndFeel(&cLAF.lOSC);
    addAndMakeVisible(mContactUs);
    mContactUs.onClick = [&] { openContact(); };

    mDiscord.setName("Empty");
    mDiscord.setLookAndFeel(&cLAF.lOSC);
    addAndMakeVisible(mDiscord);
    mDiscord.onClick = [&] { openDiscord(); };

    mFacebook.setName("Empty");
    mFacebook.setLookAndFeel(&cLAF.lOSC);
    addAndMakeVisible(mFacebook);
    mFacebook.onClick = [&] { openFB(); };

    mYouTube.setName("Empty");
    mYouTube.setLookAndFeel(&cLAF.lOSC);
    addAndMakeVisible(mYouTube);
    mYouTube.onClick = [&] { openYT(); };
    
    mInstagram.setName("Empty");
    mInstagram.setLookAndFeel(&cLAF.lOSC);
    addAndMakeVisible(mInstagram);
    mInstagram.onClick = [&] { openIG(); };
    
    mClose.setName("Empty");
    mClose.setLookAndFeel(&cLAF.lOSC);
    addAndMakeVisible(mClose);
    mClose.onClick = [&] { this->setVisible(false); };

    mVersionBuild.setName("Version");
    mVersionBuild.setButtonText(versionNumber);
    mVersionBuild.setLookAndFeel(&cLAF.lOSC);
    mVersionBuild.onClick = [&] { showBuildVersion(); };
    addAndMakeVisible(mVersionBuild);
}

InfoPageComponent::~InfoPageComponent()
{
    mVersionBuild.setLookAndFeel(nullptr);
    mClose.setLookAndFeel(nullptr);
    mInstagram.setLookAndFeel(nullptr);
    mWebsite.setLookAndFeel(nullptr);
    mFacebook.setLookAndFeel(nullptr);
    mYouTube.setLookAndFeel(nullptr);
    mDiscord.setLookAndFeel(nullptr);
    mNewsLetter.setLookAndFeel(nullptr);
    mViewMoreApps.setLookAndFeel(nullptr);
    mContactUs.setLookAndFeel(nullptr);
}

void InfoPageComponent::paint (juce::Graphics& g)
{

}

void InfoPageComponent::resized()
{
    auto bounds = getLocalBounds();
    mBackground.setBounds(bounds);
    
    //Row 1
    mWebsite.setBounds(bounds.getX() + (bounds.getWidth() * .305), bounds.getY() + (bounds.getHeight() * .465), bounds.getWidth() * .175, bounds.getHeight() * .085 );
    mNewsLetter.setBounds(bounds.getX() + (bounds.getWidth() * .525), mWebsite.getY(), mWebsite.getWidth(), mWebsite.getHeight());
    //Row 2
    mContactUs.setBounds(mWebsite.getX(), bounds.getY() + (bounds.getHeight() * .590), mWebsite.getWidth(), mWebsite.getHeight());
    mViewMoreApps.setBounds(mNewsLetter.getX(), mContactUs.getY(),  mWebsite.getWidth(), mWebsite.getHeight() );

    //Social Row
    mDiscord.setBounds(mWebsite.getX() + (bounds.getWidth() * .05), bounds.getY() + (bounds.getHeight() * .765), mWebsite.getWidth() * .35, mWebsite.getHeight());
    mFacebook.setBounds(mDiscord.getRight() * 1.05, mDiscord.getY(), mWebsite.getWidth() * .35, mWebsite.getHeight());
    mYouTube.setBounds(mFacebook.getRight() * 1.015, mDiscord.getY(), mWebsite.getWidth() * .35, mWebsite.getHeight());
    mInstagram.setBounds(mYouTube.getRight() * 1.025, mDiscord.getY(), mWebsite.getWidth() * .35, mWebsite.getHeight());

    
    mClose.setBounds(bounds.getX() + (bounds.getWidth() * .80), bounds.getY() + (bounds.getHeight() * .030), mWebsite.getWidth() * .40, mWebsite.getHeight() * 1.50);
    
    mVersionBuild.setBounds(mWebsite.getRight() * .875, mDiscord.getY() + (bounds.getHeight() * .115), mWebsite.getWidth(), mWebsite.getHeight() * 1.10);

}

void InfoPageComponent::openWebsite()
{
    juce::URL link(websiteLink);
    link.launchInDefaultBrowser();
}

void InfoPageComponent::openNewsletter()
{
    juce::URL link(newsletterLink);
    link.launchInDefaultBrowser();
}

void InfoPageComponent::openContact()
{
    juce::URL emailLink(contactLink);
    emailLink.launchInDefaultBrowser();
}

void InfoPageComponent::openGumroad()
{
    if (JUCE_IOS) {  gumroadLink = "https://apps.apple.com/hk/developer/osc-audio/id1693533681" ; }
    juce::URL link(gumroadLink);
    link.launchInDefaultBrowser();
}

void InfoPageComponent::openDiscord()
{
    juce::URL link(discordLink);
    link.launchInDefaultBrowser();
}

void InfoPageComponent::openFB()
{
    juce::URL link(facebookLink);
    link.launchInDefaultBrowser();
}

void InfoPageComponent::openIG()
{
    juce::URL link(instagramLink);
    link.launchInDefaultBrowser();
}

void InfoPageComponent::openYT()
{
    juce::URL link(youTubeLink);
    link.launchInDefaultBrowser();

}

void InfoPageComponent::showBuildVersion()
{
    
    if (mVersionBuild.getToggleState())
    {
        mVersionBuild.setButtonText(versionNumber);
    }
    else
    {
        mVersionBuild.setButtonText(buildNumber);
    }
    
}
