/*
  ==============================================================================

    EnterKeyListener.h
    Created: 16 Mar 2024 6:00:19pm
    Author:  Ryan Swannick

  ==============================================================================
*/

#pragma once

class EnterKeyListener : public juce::KeyListener {
public:
    EnterKeyListener(juce::AlertWindow* alert) : p_alert(alert) {}
    
    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override {
        
        if (originatingComponent->findKeyboardFocusContainer() != nullptr) {

            if (p_alert != nullptr && key == juce::KeyPress::returnKey) {
                // Check if the alert window's peer is valid before triggering the button click
                if (p_alert->getPeer() != nullptr) {
                    p_alert->triggerButtonClick("SAVE");
                    p_alert->triggerButtonClick("RENAME");
                    p_alert->triggerButtonClick("DELETE");
                    p_alert->triggerButtonClick("DONE");

                    return true;
                }
            }
        }
        return false;
    }

private:
    juce::AlertWindow* p_alert;
};
