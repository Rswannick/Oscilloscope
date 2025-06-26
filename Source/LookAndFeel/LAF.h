/*
  ==============================================================================

    InfoWindow.h
    Created: 16 Jan 2022 3:53:43pm
    Author:  Ryan Swannick
==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#ifndef JUCE_IOS
#define JUCE_IOS 0 // Define a default value if JUCE_IOS is not defined
#endif

#ifndef JUCE_MAC
#define JUCE_MAC 0 // Define a default value if JUCE_IOS is not defined
#endif

class oscLAF : public juce::LookAndFeel_V4
{
public:

     void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
     {
         
         juce::Array<juce::Image> imageArray;
         
         for (int i = 0; i < BinaryData::namedResourceListSize; ++i)
         {
             const char* fileName = BinaryData::namedResourceList[i];
             int fileSize = 0; // Initialize to 0, it will be updated by getNamedResource
             
             auto data = BinaryData::getNamedResource(fileName, fileSize);
             auto knob = juce::ImageCache::getFromMemory(data, fileSize);
             
             if (juce::String::fromUTF8(fileName).contains(slider.getName()) &&
                 (!(juce::String::fromUTF8(fileName).contains("wav") ||
                    juce::String::fromUTF8(fileName).contains("aif") ||
                    juce::String::fromUTF8(fileName).contains("pocketOscilloscope") ||
                    juce::String::fromUTF8(fileName).contains("ttf") ||
                    juce::String::fromUTF8(fileName).contains("TTF"))))
              {
                  if (knob.isValid()) {
                      imageArray.add(knob);
                      // Use the 'knob' image as needed
                  } else {
                      // Handle the case where the image loading fails
                      DBG("Failed to load image: " + juce::String::fromUTF8(fileName));
                  }
              }
          }
          
         int mTD {0};
          
          mTD = juce::jmap (sliderPos, 0.0f, 1.0f, 0.f, 10.f);
          
          auto bounds = slider.getLocalBounds();
          juce::Rectangle<float> rect;
          rect.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
          
         if (imageArray.size() > 0)
         {
             if (mTD > 0 && mTD <= 10)
             {
                 juce::Image img = imageArray.getReference(mTD);
                 auto newImage = gin::applyResize(img, int(rect.getWidth()), int(rect.getHeight()));
                 if (!JUCE_IOS && !JUCE_MAC) { g.drawImage(newImage, rect); }
                 else { g.drawImage(img, rect); }
                 
             }
             
             else if (mTD > 10)
             {
                 juce::Image img = imageArray.getReference(60);
                 auto newImage = gin::applyResize(img, int(rect.getWidth()), int(rect.getHeight()));
                 if (!JUCE_IOS && !JUCE_MAC) { g.drawImage(newImage, rect); }
                 else { g.drawImage(img, rect); }
                 
             }
             
             else{
                 juce::Image img = imageArray.getReference(0);
                 auto newImage = gin::applyResize(img, int(rect.getWidth()), int(rect.getHeight()));
                 if (!JUCE_IOS && !JUCE_MAC) { g.drawImage(newImage, rect); }
                 else { g.drawImage(img, rect); }
             }
         }
         
     }

     void drawBubble(juce::Graphics &g, juce::BubbleComponent &bubble, const juce::Point<float> &tip, const juce::Rectangle<float> &body) override
     {
         bubble.setAllowedPlacement(juce::BubbleComponent::below);
         bubble.setColour(juce::BubbleComponent::ColourIds::backgroundColourId, juce::Colours::black);
         g.fillRect(body);
     }

    
        void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                              float sliderPos,
                              float minSliderPos,
                              float maxSliderPos,
                              const juce::Slider::SliderStyle style, juce::Slider& slider) override
        {

            using namespace juce;
            
            g.setColour(juce::Colour::fromRGB(0,0,0));
            g.setOpacity(0.40f);
            g.fillAll();

            auto trackWidth = jmin (6.0f, slider.isHorizontal() ? (float) height * 0.25f : (float) width * 0.25f);

            Point<float> startPoint (slider.isHorizontal() ? (float) x : (float) x + (float) width * 0.5f,
                                     slider.isHorizontal() ? (float) y + (float) height * 0.5f : (float) (height + y));

            Point<float> endPoint (slider.isHorizontal() ? (float) (width + x) : startPoint.x,
                                   slider.isHorizontal() ? startPoint.y : (float) y);

            Path backgroundTrack;
            backgroundTrack.startNewSubPath (startPoint);
            backgroundTrack.lineTo (endPoint);
            g.setColour (juce::Colours::black);
            g.strokePath (backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

            Path valueTrack;
            Point<float> minPoint, maxPoint, thumbPoint;

            auto kx = slider.isHorizontal() ? sliderPos : ((float) x + (float) width * 0.5f);
            auto ky = slider.isHorizontal() ? ((float) y + (float) height * 0.5f) : sliderPos;

            minPoint = startPoint;
            maxPoint = { kx, ky };

            auto thumbWidth = getSliderThumbRadius (slider);

            valueTrack.startNewSubPath (minPoint);
            g.setColour (juce::Colours::white);
            g.strokePath (valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

            g.setColour (juce::Colours::white);
            g.fillEllipse (Rectangle<float> (static_cast<float> (thumbWidth), static_cast<float>(thumbWidth)).withCentre(maxPoint));
        }

        void drawButtonBackground(juce::Graphics &g, juce::Button &button, const juce::Colour &backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
        {
            if (button.getName().contains("BLACK"))
            {
                g.fillAll();
            }
        }
        
        void drawButtonText(juce::Graphics &g, juce::TextButton &button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)override
        {
            
            if (button.getButtonText().contains("Test") || button.getName().contains("ALERTBOX"))
            {
                auto bounds = button.getLocalBounds();
                juce::Rectangle<int> rect;
                rect.setBounds(bounds.getX(),bounds.getY(),bounds.getWidth(),bounds.getHeight());
                
                if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
                { g.setColour(juce::Colour::fromRGB(0,250,250)); }
                else
                { g.setColour(juce::Colours::white); }

                g.setFont(bounds.getHeight() * .4);
                g.drawFittedText(button.getButtonText().toUpperCase(), rect, juce::Justification::centred, 1);
            }
            

        }
        
        void drawToggleButton(juce::Graphics &g, juce::ToggleButton & toggleButton, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {

        if (toggleButton.getName().contains("Version"))
        {
            auto bounds = toggleButton.getLocalBounds();
            juce::Rectangle<int> rect;
            rect.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
            
            g.setColour(juce::Colours::lightgrey);
            g.setFont(getWackyFont());
            g.setFont(bounds.getHeight() * .35);
            g.drawFittedText(toggleButton.getButtonText(), rect, juce::Justification::centred, 1);
        }
        else if (toggleButton.getName().contains("Empty"))
        {
            //Intentionally Empty
            //g.fillAll();
        }
        else if (toggleButton.getName().contains("PHASE"))
        {
            auto bounds = toggleButton.getLocalBounds();
            juce::Rectangle<float> rect;
            rect.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
            
            auto off = juce::ImageCache::getFromMemory(BinaryData::PHASE_ON_png, BinaryData::PHASE_ON_pngSize);
            auto on = juce::ImageCache::getFromMemory(BinaryData::PHASE_OFF_png, BinaryData::PHASE_OFF_pngSize);
            
            if (!toggleButton.getToggleState()) { g.drawImage(off, rect); }
            else { g.drawImage(on, rect); }
        }
        else if (toggleButton.getName().contains("POWER"))
        {
            auto bounds = toggleButton.getLocalBounds();
            juce::Rectangle<float> rect;
            rect.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
            
            auto on = juce::ImageCache::getFromMemory(BinaryData::POWER_ON_png, BinaryData::POWER_ON_pngSize);
            auto off = juce::ImageCache::getFromMemory(BinaryData::POWER_OFF_png, BinaryData::POWER_OFF_pngSize);
            
            if (!toggleButton.getToggleState()) { g.drawImage(off, rect); }
            else { g.drawImage(on, rect); }
        }
        else if (toggleButton.getName().contains("MONO"))
        {
            auto bounds = toggleButton.getLocalBounds();
            juce::Rectangle<float> rect;
            rect.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
            
            auto off = juce::ImageCache::getFromMemory(BinaryData::MONO_ON_png, BinaryData::MONO_ON_pngSize);
            auto on = juce::ImageCache::getFromMemory(BinaryData::MONO_OFF_png, BinaryData::MONO_OFF_pngSize);
            
            if (!toggleButton.getToggleState()) { g.drawImage(off, rect); }
            else { g.drawImage(on, rect); }
        }
        else if (toggleButton.getName().contains("SETTINGS"))
        {
            auto bounds = toggleButton.getLocalBounds();
            juce::Rectangle<float> rect;
            rect.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
            
            auto off = juce::ImageCache::getFromMemory(BinaryData::Settings_button_36x36_png, BinaryData::Settings_button_36x36_pngSize);
            auto on = juce::ImageCache::getFromMemory(BinaryData::Settings_Button_Selected_36x36_png, BinaryData::Settings_Button_Selected_36x36_pngSize);
            if (!toggleButton.getToggleState()) { g.drawImage(off, rect); }
            else { g.drawImage(on, rect); }
        }
        else if (toggleButton.getName().contains("INFO"))
        {
            auto bounds = toggleButton.getLocalBounds();
            juce::Rectangle<float> rect;
            rect.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
            auto off = juce::ImageCache::getFromMemory(BinaryData::Info_Icon_36x36_png, BinaryData::Info_Icon_36x36_pngSize);
            auto on = juce::ImageCache::getFromMemory(BinaryData::Info_Icon_Selected_36x36_png, BinaryData::Info_Icon_Selected_36x36_pngSize);
            if (!toggleButton.getToggleState()) { g.drawImage(off, rect); }
            else { g.drawImage(on, rect); }
        }
        else if (toggleButton.getName().contains("VOLUME"))
        {
            auto bounds = toggleButton.getLocalBounds();
            juce::Rectangle<float> rect;
            rect.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
            
            auto off = juce::ImageCache::getFromMemory(BinaryData::Volume_Icon_36x36_png, BinaryData::Volume_Icon_36x36_pngSize);
            auto on = juce::ImageCache::getFromMemory(BinaryData::Volume_Icon_Selected_36x36_png, BinaryData::Volume_Icon_Selected_36x36_pngSize);
            if (!toggleButton.getToggleState()) { g.drawImage(off, rect); }
            else { g.drawImage(on, rect); }
        }
        else if (toggleButton.getName().contains("XY"))
        {
            auto bounds = toggleButton.getLocalBounds();
            juce::Rectangle<float> rect;
            rect.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
            
            auto off = juce::ImageCache::getFromMemory(BinaryData::XY_Off_png, BinaryData::XY_Off_pngSize);
            auto on = juce::ImageCache::getFromMemory(BinaryData::XY_On_png, BinaryData::XY_On_pngSize);
            if (!toggleButton.getToggleState()) { g.drawImage(off, rect); }
            else { g.drawImage(on, rect); }
        }
        else if (toggleButton.getName().contains("Fullscreen"))
        {
            auto bounds = toggleButton.getLocalBounds();
            juce::Rectangle<float> rect;
            rect.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
            
            auto off = juce::ImageCache::getFromMemory(BinaryData::Fullscreen_Off_png, BinaryData::Fullscreen_Off_pngSize);
            auto on = juce::ImageCache::getFromMemory(BinaryData::Fullscreen_On_png, BinaryData::Fullscreen_On_pngSize);
            if (!toggleButton.getToggleState()) { g.drawImage(off, rect); }
            else { g.drawImage(on, rect); }
        }
        else if (toggleButton.getName().contains("ALERTBOX"))
        {
            auto bounds = toggleButton.getLocalBounds();

            juce::Rectangle<int> rect;
            rect.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
                        
            g.setColour(juce::Colours::lightgrey);
            g.setFont(getWackyFont());
            g.setFont(bounds.getHeight() * .35);
            g.drawFittedText(toggleButton.getButtonText(), rect, juce::Justification::centred, 1);

        }
        else
        {
            //Intentionally expty for CLOSE buttons in Settings/Info - Test Fill Below
            //g.fillAll();
        }
    }
    
    
    void drawComboBoxTextWhenNothingSelected(juce::Graphics &g, juce::ComboBox &box, juce::Label &label)override
    {
        box.setText("DEFAULT");
        
        g.setColour(juce::Colours::white);
        g.fillAll();
    }

    void positionComboBoxText(juce::ComboBox &box, juce::Label &label) override
    {
        label.setSize(box.getWidth(), box.getHeight());
        label.setJustificationType(juce::Justification::centred);
        label.setSize(0, 0);
        
        box.setJustificationType(juce::Justification::centred);
         
        if (JUCE_IOS)
        {
            getOptionsForComboBoxPopupMenu(box, label);
        }
    }
    
    void drawPopupMenuItem(juce::Graphics &g, const juce::Rectangle<int> &area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String &text, const juce::String &shortcutKeyText, const juce::Drawable *icon, const juce::Colour *textColour) override
    {
        
        g.setColour(juce::Colour::fromRGB(80, 80, 80));
        if (isTicked == true)
        { g.setColour(juce::Colour::fromRGB(250, 250, 250)); }
        else if (isHighlighted == true)
        { g.setColour(juce::Colour::fromRGB(250, 250, 250)); }
        else
        { g.setColour(juce::Colour::fromRGB(80, 80, 80)); }
        
        g.drawFittedText(text.toUpperCase(), area, juce::Justification::centred, 1);
    }
    
    void drawComboBox(juce::Graphics &g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox &box) override
    {
        box.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colour::fromRGB(16, 16, 16));
        
        if (box.isEnabled())
        {
            auto bounds = box.getLocalBounds();
            juce::Rectangle<float> rect;
            rect.setBounds(0, 0, bounds.getWidth(), bounds.getHeight());
            g.setColour(juce::Colour::fromRGB(0, 0, 0));
            //Size
            g.setFont(bounds.getHeight() * .20);
            g.setFont(getWackyFont().withExtraKerningFactor(.115));
                        
            if (box.getName().isEmpty())
            {
                g.setFont(bounds.getHeight() * .40);
            }
            
            g.setColour(juce::Colour::fromRGB(195, 195, 195));

            if (box.getName().contains("PresetDropDown")) { g.setFont(bounds.getHeight() * .35); g.setColour(juce::Colour::fromRGB(195, 195, 195));}
            
            if (!box.getName().contains("MoreOptions"))
            {
                g.drawFittedText(box.getText().toUpperCase(), bounds.getX(), bounds.getY() + (bounds.getHeight() * .0) , bounds.getWidth(), bounds.getHeight() ,juce::Justification::centred, 1);
            }
        }
        
//        g.setColour(juce::Colours::white);
//        g.fillAll();
    }
    
    juce::Component* getParentComponentForMenuOptions (const juce::PopupMenu::Options& options) override
    {
            
        
        #if JUCE_IOS
            if (juce::PluginHostType::getPluginLoadedAs() == juce::AudioProcessor::wrapperType_AudioUnitv3)
            {
                 if (options.getParentComponent() == nullptr && options.getTargetComponent() != nullptr)
                     return options.getTargetComponent()->getTopLevelComponent();
            }
        #endif
        
           return LookAndFeel_V4::getParentComponentForMenuOptions (options);
    }

    void drawPopupMenuBackground(juce::Graphics &g, int width, int height) override
    {
        g.setColour(juce::Colour::fromRGB(16,16,16));
        g.fillAll();

        g.setColour(juce::Colours::black);
        juce::Rectangle<float> rect;
        rect.setBounds(0, 0, width, height);
        g.drawRect(rect);
    }
    
    void drawAlertBox(juce::Graphics& g, juce::AlertWindow &box, const juce::Rectangle<int> &area, juce::TextLayout &textBox) override
       {
           box.setColour(juce::AlertWindow::backgroundColourId, juce::Colour::fromRGBA(25, 25, 25, .4f));
           box.setColour(juce::AlertWindow::textColourId, juce::Colours::white);
           box.setColour(juce::AlertWindow::outlineColourId, juce::Colours::white);
           
           if (box.getNumButtons() > 0) { box.getButton(0)->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(40, 40, 40)); }

           if (box.getNumButtons() > 0) {
           box.getButton(1)->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(40, 40, 40));
           }
           
           if ( box.getTextEditor("Value") != nullptr)
           { box.getTextEditor("Value")->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black); }
           
           juce::Rectangle<float> rect;
           rect.setBounds(0, 0, box.getBounds().getWidth(), box.getBounds().getHeight());
           g.setColour(juce::Colour::fromRGB(25,25,25));
           g.fillAll();

           juce::Rectangle<float> rectB (0, 0, box.getBounds().getWidth(), box.getBounds().getHeight());
           g.setColour(juce::Colour::fromRGB(50,50,50));
           g.drawRoundedRectangle(rectB, 5.0f, 5.0f);

            g.setColour(juce::Colour::fromRGB(250,250,250));
           

           auto* valueEditor = box.getTextEditor("Value");

            if (valueEditor != nullptr)
            {
                g.setFont(area.getHeight() * .09);
                
                if (!JUCE_IOS)
                {
                    g.drawText(box.getTitle(),
                               rect.getCentreX() - (rect.getWidth() * .425),
                               rect.getY() + (rect.getHeight() * .045),
                               rect.getWidth() * .85,
                               rect.getHeight() * .15,
                               juce::Justification::centred);
                    
                    box.getTextEditor("Value")->setBounds( rect.getX() + (rect.getWidth() * .15),
                                                          rect.getY() + (rect.getHeight() * .25),
                                                          rect.getWidth() * .70,
                                                          rect.getHeight() * .25);
                    
                    float textX = box.getTextEditor("Value")->getX();
                    
                    box.getButton(0)->setBounds(textX + (box.getTextEditor("Value")->getWidth() * .20),
                                                box.getTextEditor("Value")->getBottom() * 1.10,
                                                rect.getWidth() * .20,
                                                box.getTextEditor("Value")->getHeight() * .80);
                    
                    box.getButton(1)->setBounds(box.getButton(0)->getRight() * 1.05,
                                                box.getButton(0)->getY(),
                                                box.getButton(0)->getWidth(),
                                                box.getTextEditor("Value")->getHeight() * .80);
                }
                else if (JUCE_IOS && juce::JUCEApplication::isStandaloneApp())
                {

                    g.drawText(box.getTitle(),
                               rect.getCentreX() - (rect.getWidth() * .425),
                               rect.getY() + (rect.getHeight() * .045),
                               rect.getWidth() * .85,
                               rect.getHeight() * .15,
                               juce::Justification::centred);
                    
                    box.getTextEditor("Value")->setBounds( rect.getX() + (rect.getWidth() * .15),
                                                          (rect.getHeight() * .25),
                                                          rect.getWidth() * .70,
                                                          rect.getHeight() * .25);
                    
                    float textX = box.getTextEditor("Value")->getX();
                    
                    box.getButton(0)->setBounds(textX + (box.getTextEditor("Value")->getWidth() * .20),
                                                box.getTextEditor("Value")->getBottom() * 1.10,
                                                rect.getWidth() * .20,
                                                box.getTextEditor("Value")->getHeight() * .80);
                    
                    box.getButton(1)->setBounds(box.getButton(0)->getRight() * 1.05,
                                                box.getButton(0)->getY(),
                                                box.getButton(0)->getWidth(),
                                                box.getButton(0)->getHeight());
                    
                }
                else if (JUCE_IOS && !juce::JUCEApplication::isStandaloneApp())
                {
                    
                    g.drawText(box.getTitle(),
                               rect.getCentreX() - (rect.getWidth() * .425),
                               rect.getY() + (rect.getHeight() * .045),
                               rect.getWidth() * .85,
                               rect.getHeight() * .15,
                               juce::Justification::centred);
                    
                    box.getTextEditor("Value")->setBounds( rect.getX() + (rect.getWidth() * .15),
                                                          rect.getY() + (rect.getHeight() * .25),
                                                          rect.getWidth() * .70,
                                                          rect.getHeight() * .25);
                    
                    float textX = box.getTextEditor("Value")->getX();
                    
                    box.getButton(0)->setBounds(textX + (box.getTextEditor("Value")->getWidth() * .20),
                                                box.getTextEditor("Value")->getBottom() * 1.10,
                                                rect.getWidth() * .20,
                                                box.getTextEditor("Value")->getHeight() * .80);
                    
                    box.getButton(1)->setBounds(box.getButton(0)->getRight() * 1.05,
                                                box.getButton(0)->getY(),
                                                box.getButton(0)->getWidth(),
                                                box.getTextEditor("Value")->getHeight() * .80);
                }
                
            }
            else
            {
                
                    if (!JUCE_IOS)
                    {
                        g.setFont(area.getHeight() * .165);
                        
                        g.drawText(box.getTitle(),
                                   rect.getCentreX() - (rect.getWidth() * .425),
                                   rect.getY() + (rect.getHeight() * .065),
                                   rect.getWidth() * .85,
                                   rect.getHeight() * .15,
                                   juce::Justification::centred);
                        
                        box.getButton(0)->setBounds(rect.getX() + (rect.getWidth() * .25),
                                                    rect.getY() + (rect.getHeight() * .415),
                                                    rect.getWidth() * .25,
                                                    rect.getHeight() * .20);
                        
                        box.getButton(1)->setBounds(box.getButton(0)->getRight() * 1.05,
                                                    box.getButton(0)->getY(),
                                                    box.getButton(0)->getWidth(),
                                                    box.getButton(0)->getHeight());
                    }
                    else if (JUCE_IOS && juce::JUCEApplication::isStandaloneApp())
                    {
                        g.setFont(area.getHeight() * .165);
                        
                        g.drawText(box.getTitle(),
                                   rect.getCentreX() - (rect.getWidth() * .425),
                                   rect.getY() + (rect.getHeight() * .065),
                                   rect.getWidth() * .85,
                                   rect.getHeight() * .15,
                                   juce::Justification::centred);
                        
                        box.getButton(0)->setBounds(rect.getX() + (rect.getWidth() * .25),
                                                    rect.getY() + (rect.getHeight() * .415),
                                                    rect.getWidth() * .25,
                                                    rect.getHeight() * .20);
                        
                        box.getButton(1)->setBounds(box.getButton(0)->getRight() * 1.05,
                                                    box.getButton(0)->getY(),
                                                    box.getButton(0)->getWidth(),
                                                    box.getButton(0)->getHeight());
                        
                    }
                    else if (JUCE_IOS && !juce::JUCEApplication::isStandaloneApp())
                    {
                        
                        g.setFont(area.getHeight() * .165);
                        
                        g.drawText(box.getTitle(),
                                   rect.getCentreX() - (rect.getWidth() * .425),
                                   rect.getY() + (rect.getHeight() * .065),
                                   rect.getWidth() * .85,
                                   rect.getHeight() * .15,
                                   juce::Justification::centred);
                        
                        box.getButton(0)->setBounds(rect.getX() + (rect.getWidth() * .25),
                                                    rect.getY() + (rect.getHeight() * .415),
                                                    rect.getWidth() * .25,
                                                    rect.getHeight() * .20);
                        
                        box.getButton(1)->setBounds(box.getButton(0)->getRight() * 1.05,
                                                    box.getButton(0)->getY(),
                                                    box.getButton(0)->getWidth(),
                                                    box.getButton(0)->getHeight());
                    }
                }
       }
    
    void drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollbar, int x, int y, int width, int height, bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown) override
    {
        // Define colors for the scrollbar
        juce::Colour thumbColor(0xff808080); // Grey color
        juce::Colour backgroundColour(0xff000000); // Black background

        // Fill the background
        g.setColour(backgroundColour);
        g.fillRect(x, y, width, height);

        // Draw the thumb (the actual scrollbar)
        g.setColour(thumbColor);
        if (isScrollbarVertical)
            g.fillRect(x, thumbStartPosition, width, thumbSize);
        else
            g.fillRect(thumbStartPosition, y, thumbSize, height);
    }
       
    void drawTickBox(juce::Graphics &g, juce::Component &c, float x, float y, float w, float h, bool ticked, bool isEnabled, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        c.setColour(juce::ListBox::ColourIds::backgroundColourId, juce::Colours::black);
        g.setColour(juce::Colour::fromRGB(18, 18, 18));
        g.fillAll();
        
        // Draw the outline rectangle
        g.setColour(juce::Colours::white);
        g.drawRect(x, y, w, h, 1.0f);
        g.drawRoundedRectangle(x, y, w, h, 5.0f, 0.5f);


        // Fill the rectangle if ticked
        if (ticked)
        {
            g.setColour(juce::Colours::skyblue);
            g.fillRect(x + 2, y + 2, w - 4, h - 4);
        }
    }
    
    void drawLabel(juce::Graphics &g, juce::Label &label) override
    {
        auto bounds = label.getLocalBounds();
        
        juce::Rectangle<int> rect;
        rect.setBounds(bounds.getX(),bounds.getY(),bounds.getWidth(),bounds.getHeight());
        g.setFont(getWackyFont().withExtraKerningFactor(.01));
        
        if (JUCE_IOS)
        { g.setFont(bounds.getHeight() * .275);  }
        else
        { g.setFont(bounds.getHeight() * .19);  }
        
        if (label.getText().containsWholeWord("Input signal is muted to avoid a feedback loop"))
        {
            g.setColour(juce::Colours::white);
            g.setFont(bounds.getHeight() * .40);
        }
        else if (
                 label.getText().containsWholeWord("Output:") ||
                 label.getText().containsWholeWord("Input:") ||
                 label.getText().containsWholeWord("Active output channels:") ||
                 label.getText().containsWholeWord("Active MIDI inputs:") ||
                 label.getText().containsWholeWord("Active input channels:") ||
                 label.getText().containsWholeWord("Sample rate:")||
                 label.getText().containsWholeWord("Audio buffer size:")  )
        {
            g.setColour(juce::Colours::grey);
            
            if ( label.getText().containsWholeWord("Active MIDI inputs:") )
            {
                g.setFont(bounds.getHeight() * .25);
                g.drawFittedText("MIDI DEVICES: ", rect, juce::Justification::right, 1);
            }
            else if (label.getText().containsWholeWord("Active output channels:") )
            {
                g.setFont(bounds.getHeight() * .40);
                g.drawFittedText("OUTPUTS: ", rect.getX() - (rect.getWidth() * .10), rect.getY(), rect.getWidth(), rect.getHeight(), juce::Justification::right, 1);
            }
            else if (label.getText().containsWholeWord("Active input channels:") )
            {
                g.setFont(bounds.getHeight() * .40);
                g.drawFittedText("INPUTS: ", rect.getX() - (rect.getWidth() * .10), rect.getY(), rect.getWidth(), rect.getHeight(), juce::Justification::right, 1);
            }
            else if (label.getText().containsWholeWord("Output:") )
            {
                g.setFont(bounds.getHeight() * .40);
                g.drawFittedText("DEVICE:", rect, juce::Justification::right, 1);
            }
            else if (label.getText().containsWholeWord("Audio buffer size:") )
            {
                g.setFont(bounds.getHeight() * .40);
                g.drawFittedText("BUFFER SIZE:", rect, juce::Justification::right, 1);
            }
            else
            {
                g.setFont(bounds.getHeight() * .40);
                g.drawFittedText(label.getText().toUpperCase().toStdString(), rect, juce::Justification::right, 1);
            }
            
        }
        else
        {
            if (JUCE_IOS)
            { g.setFont(bounds.getHeight() * .275); }
            else
            { g.setFont(bounds.getHeight() * .19);  }
            
            g.setColour(juce::Colours::grey);
            
            g.drawFittedText(label.getText().toUpperCase().toStdString(), rect, juce::Justification::centred, 1);
        }
    }

 private:
     static const juce::Font& getWackyFont()
     {
     static juce::Font wacky (juce::Font (juce::Typeface::createSystemTypefaceFor (BinaryData::RobotoBlack_ttf, BinaryData::RobotoBlack_ttfSize))); return wacky;
     }
 };

//==============================================================================
/*
*/

class LAFComponent  : public juce::Component
{
public:
    LAFComponent();
    ~LAFComponent() override;
    
    oscLAF lOSC;
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LAFComponent)
};







