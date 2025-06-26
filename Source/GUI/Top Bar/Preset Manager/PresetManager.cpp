
/*
  ==============================================================================

    PresetManager.cpp
    Created: 17 Sep 2023 1:30:57pm
    Author:  Ryan Swannick

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PresetManager.h"

//==============================================================================
PresetManagerComponent::PresetManagerComponent(OscilliscopeAudioProcessor& p)
    : audioProcessor(p)
{
    addNames();
    addLookAndFeel();
    addListeners();
    addMoreOptions();
    addComponents();
    addFunctions();
    copyFactoryPresets();
//    copySAPresetsAU();
    addPresetDropDown();
}

PresetManagerComponent::~PresetManagerComponent()
{
    mLoadPresetDropDown.removeListener(this);
    mMoreOptionsDropDown.removeListener(this);
    
    juce::Component* componentsToSetLookAndFeel[] = { &mNextPreset, &mMoreOptionsDropDown, &mPrevPreset, &mLoadPresetDropDown };
    for (juce::Component* component : componentsToSetLookAndFeel)
    { component->setLookAndFeel(nullptr); }
}

void PresetManagerComponent::paint(juce::Graphics &g)
{

}

void PresetManagerComponent::resized()
{
    auto bounds = getLocalBounds();
    
    float loadDropDownX = bounds.getCentreX() - (bounds.getWidth() * .097);
    float loadDropDownY = bounds.getY() + (bounds.getHeight() * .245);
    float loadDropDownW = bounds.getWidth() * .20;
    float loadDropDownH = bounds.getHeight() * .540;

    mLoadPresetDropDown.setBounds(loadDropDownX, loadDropDownY, loadDropDownW, loadDropDownH);
    
    float arrowL = mLoadPresetDropDown.getRight() * .975;
    float arrowR = mLoadPresetDropDown.getX() - (bounds.getWidth() * .005);
    float arrowY = bounds.getY() + (bounds.getHeight() * 0.20);
    float arrowW = bounds.getWidth() * .030;
    float arrowH = bounds.getHeight() * .80;
    
    mNextPreset.setBounds(arrowL, arrowY, arrowW, arrowH);
    mPrevPreset.setBounds(arrowR, arrowY, arrowW, arrowH);
    
    float moreOptionsX = mLoadPresetDropDown.getRight() + (mLoadPresetDropDown.getWidth() * .19);
    float moreOptionsY = mLoadPresetDropDown.getY();
    float moreOptionsW = mLoadPresetDropDown.getWidth() * .176;
    float moreOptionsH = mLoadPresetDropDown.getHeight();
    
    mMoreOptionsDropDown.setBounds(moreOptionsX, moreOptionsY, moreOptionsW, moreOptionsH);
}

void PresetManagerComponent::addComponents()
{
    mLoadPresetDropDown.setMouseClickGrabsKeyboardFocus(false);
    mMoreOptionsDropDown.setMouseClickGrabsKeyboardFocus(false);
    mNextPreset.setMouseClickGrabsKeyboardFocus(false); mPrevPreset.setMouseClickGrabsKeyboardFocus(false);
    
    mLoadPresetDropDown.setScrollWheelEnabled(true);
    
    addAndMakeVisible(mLoadPresetDropDown);
    addAndMakeVisible(mMoreOptionsDropDown);
    
    addAndMakeVisible(mNextPreset);
    addAndMakeVisible(mPrevPreset);
    
    mLoadPresetDropDown.setTextWhenNothingSelected("DEFAULT");


}

void PresetManagerComponent::addListeners()
{
    mLoadPresetDropDown.addListener(this);
    mMoreOptionsDropDown.addListener(this);
}

void PresetManagerComponent::addNames()
{
    mNextPreset.setName("HIDE");
    mPrevPreset.setName("HIDE");
    mLoadPresetDropDown.setName("PresetDropDown");
    mMoreOptionsDropDown.setName("MoreOptions");
}

void PresetManagerComponent::addLookAndFeel()
{
    mNextPreset.setLookAndFeel(&cLAF.lOSC);
    mPrevPreset.setLookAndFeel(&cLAF.lOSC);
    mLoadPresetDropDown.setLookAndFeel(&cLAF.lOSC);
    mMoreOptionsDropDown.setLookAndFeel(&cLAF.lOSC);
}

void PresetManagerComponent::addFunctions()
{
    mNextPreset.onClick = [this] {  nextPreset();  };
    mPrevPreset.onClick = [this] {  prevPreset();  };
}

void PresetManagerComponent::comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged)
{
    audioProcessor.suspendProcessing(true);

    if (comboBoxThatHasChanged == &mMoreOptionsDropDown)
    {
        switch (int(mMoreOptionsDropDown.getSelectedItemIndex())) {
            case 0: presetSave(); break;
            case 1: presetSaveAs(); break;
            case 2: presetRename(); break;
            case 3: presetDelete(); break;
            default: break;
        }
    }

    //NEW
    if (comboBoxThatHasChanged == &mLoadPresetDropDown)
    {
        juce::File presetsDir (getDocumentsPath() + getPresetsFolderPath());
        if (JUCE_IOS) { presetsDir = container.getFullPathName(); }
        
        if (!presetsDir.exists() || !presetsDir.isDirectory()) { presetsDir.createDirectory(); }
        // Get the selected item name from the ComboBox (loadSlotDropDown)
        juce::String selectedFileName = mLoadPresetDropDown.getItemText(mLoadPresetDropDown.getSelectedItemIndex());
        // Find the file in the array based on its name
        juce::File myFile;
        juce::Array<juce::File> childFiles;
        presetsDir.findChildFiles(childFiles, juce::File::findFiles, true); // Retrieve all child files
        
        for (const juce::File& file : childFiles) { if (file.getFileNameWithoutExtension() == selectedFileName) { myFile = file; break; } }
        juce::Identifier idtype = audioProcessor.apvts.state.getType();
        juce::String fileName = myFile.getFileNameWithoutExtension();
        
        std::unique_ptr<juce::XmlElement> xmlState (   juce::XmlDocument::parse( myFile )  ) ;
        if (xmlState != nullptr)
        {
            juce::String xmlData = xmlState->toString();
            juce::ValueTree newTree (juce::ValueTree::fromXml(xmlData));
            audioProcessor.apvts.replaceState(newTree);
            audioProcessor.sSelectedColorOSC1 = xmlState->getStringAttribute("COLOR1");
            audioProcessor.sSelectedColorOSC2 = xmlState->getStringAttribute("COLOR2");
        }
    
        if (JUCE_IOS)
        {
            if (myFile.exists())
            {
                juce::URL juceURL(myFile);
                std::unique_ptr<juce::InputStream> wi(myFile.createInputStream());
                if (wi != nullptr)
                {
                    const juce::String s = wi->readEntireStreamAsString(); if (s.length() > 0)
                    { std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument::parse(s));
                        if (xml != nullptr) { audioProcessor.apvts.state = juce::ValueTree::fromXml(*xml); } }
                }
            }
        }
        
        if (fileName.isNotEmpty())
        {
            audioProcessor.pname = fileName;
        }
    }
    
    audioProcessor.suspendProcessing(false);
//    resized();
//    repaint();

//    copySAPresetsAU();
}

void  PresetManagerComponent::addMoreOptions()
{
    mMoreOptionsDropDown.addItem("SAVE", 1);
    mMoreOptionsDropDown.addItem("SAVE AS", 2);
    mMoreOptionsDropDown.addItem("RENAME", 4);
    mMoreOptionsDropDown.addItem("DELETE", 5);
}

void  PresetManagerComponent::addPresetDropDown()
{

    mLoadPresetDropDown.clear();

    juce::String presetPath = getDocumentsPath() + getPresetsFolderPath();

    if (JUCE_IOS) { presetPath = container.getFullPathName(); }
    juce::File presetsDir(presetPath);

    if (!presetsDir.exists() || !presetsDir.isDirectory()) { presetsDir.createDirectory(); }

    juce::SortedSet<juce::String> addedEntries; // Keep track of entries already added

    // Add items from the top-level directory
    juce::DirectoryIterator iter(presetPath, true, "*", juce::File::TypesOfFileToFind::findFiles);

    juce::StringArray filesToAdd; // Temporary array to store files to be added

    while (iter.next())
    {
        juce::String fileName = iter.getFile().getFileNameWithoutExtension();
        if (fileName != ".DS_Store" && !fileName.containsIgnoreCase("com.apple") && !fileName.containsIgnoreCase("Insert") && !fileName.containsIgnoreCase("Send") && !fileName.containsIgnoreCase("DeletedFiles"))
        {
            // Check if the entry has already been added
            if (!addedEntries.contains(fileName))
            {
                filesToAdd.add(fileName); // Add the file name to the temporary array
                addedEntries.add(fileName); // Add the entry to the set
            }
        }
    }

    // Sort the filesToAdd array alphabetically
    filesToAdd.sort(true);

    // Add sorted files to the ComboBox
    mLoadPresetDropDown.addSeparator();
    mLoadPresetDropDown.addSectionHeading("USER");

    for (const auto& fileName : filesToAdd)
    {
        mLoadPresetDropDown.addItem(fileName, mLoadPresetDropDown.getNumItems() + 1);
    }

    // Add items from subdirectories
    juce::DirectoryIterator subfolderIter(presetPath, true, "*", juce::File::TypesOfFileToFind::findDirectories);

    juce::Array<juce::String> subHeadings;

    while (subfolderIter.next())
    {
        juce::String subfolderName = subfolderIter.getFile().getFileName();

        if ((subfolderName == "Inserts" || subfolderName == "Sends") && (subfolderName != ".DS_Store" && subfolderName != "Library" && subfolderName != "Caches" && !subfolderName.contains("PRESETS")))
        {
            bool alreadyExists = false;

            // Check if subfolderName already exists in subHeadings
            for (int i = 0; i < subHeadings.size(); i++) {
                if (subHeadings.getReference(i) == subfolderName) {
                    alreadyExists = true;
                    break; // No need to continue if found
                }
            }

            if (!alreadyExists) {
                subHeadings.add(subfolderName); // Add subfolderName to subHeadings
                mLoadPresetDropDown.addSeparator(); // Add separator
                mLoadPresetDropDown.addSectionHeading(subfolderName); // Add section heading
            }

            
            juce::DirectoryIterator presetIter(subfolderIter.getFile(), false, "*", juce::File::TypesOfFileToFind::findFiles);

            // Reset filesToAdd for each subdirectory
            filesToAdd.clear();

            while (presetIter.next())
            {
                juce::String presetName = presetIter.getFile().getFileNameWithoutExtension();
                if (presetName != ".DS_Store" && !presetName.containsIgnoreCase("METADATA"))
                {
                    // Check if the entry has already been added
                    if (!addedEntries.contains(presetName))
                    {
                        filesToAdd.add(presetName); // Add the file name to the temporary array
                        addedEntries.add(presetName); // Add the entry to the set
                    }
                }
            }

            // Sort the filesToAdd array alphabetically
            filesToAdd.sort(true);

            // Add sorted files to the ComboBox
            for (const auto& fileName : filesToAdd)
            {
                mLoadPresetDropDown.addItem(fileName, mLoadPresetDropDown.getNumItems() + 1);
            }
        }
    }

//    copySAPresetsAU();

}

void  PresetManagerComponent::presetSaveAs()
{
    juce::String currentValue = mLoadPresetDropDown.getText();

    auto *p_alert = new juce::AlertWindow("", "", juce::AlertWindow::AlertIconType::NoIcon, this);
    
    p_alert->setTitle("SAVE PRESET");
    p_alert->addTextEditor("Value", currentValue);
    p_alert->addButton("CANCEL", 0);
    p_alert->addButton("SAVE", 1);
    p_alert->getButton(0)->setName("ALERTBOX");
    p_alert->getButton(1)->setName("ALERTBOX");
    p_alert->getButton(0)->setLookAndFeel(&cLAF.lOSC);
    p_alert->getButton(1)->setLookAndFeel(&cLAF.lOSC);
    p_alert->setLookAndFeel(&cLAF.lOSC);


    p_alert->setDropShadowEnabled(true);
    p_alert->toFront(true);

    auto localBounds = getParentComponent()->getLocalBounds();
    auto topLevelBounds = getTopLevelComponent()->getLocalBounds();
    
    float alertW = localBounds.getWidth() * .70;
    float alertH = localBounds.getHeight() * .40;
    float alertX = localBounds.getX() + (localBounds.getWidth() * .15);
    float alertY = topLevelBounds.getY() + localBounds.getHeight() * .25;
    
    if (JUCE_IOS)
    {
        alertW = this->getWidth() * .70;
        alertH = this->getHeight() * 4;
        alertX = this->getX() + (this->getWidth() * .15);
        alertY = this->getY() + this->getHeight() * .15;
    }
    
    p_alert->setBounds(alertX, alertY, alertW, alertH);
    
    auto alertBounds = p_alert->getBounds();
    
    float textH = alertBounds.getHeight() * .25;
    float textW = alertBounds.getWidth() * .75;
    
    float textY = alertBounds.getCentreY();
    float textX = alertBounds.getCentreX() - (textW * .75);

    p_alert->getTextEditor("Value")->setFont(p_alert->getTextEditor("Value")->getHeight() * .65);
    p_alert->getTextEditor("Value")->setJustification(juce::Justification::centred);
    
    p_alert->getTextEditor("Value")->setBounds(textX,
                                               textY,
                                               textW,
                                               textH);
    
    getParentComponent()->addChildComponent(p_alert);

    auto* valueEditor = p_alert->getTextEditor("Value");
    
    p_alert->enterModalState(true, juce::ModalCallbackFunction::create([=](int result)
    {

        if (result == 1)
        {
            if (valueEditor != nullptr)
            {
                mLoadPresetDropDown.setText(  p_alert->getTextEditor("Value")->getText()   );
                juce::String fileName = p_alert->getTextEditor("Value")->getText();
                //Make Preset
                juce::File docs = getDocumentsPath();
                juce::String path = docs.getFullPathName();
                juce::String filePath = getPresetsFolderPath();
                juce::String documents = path + filePath + fileName + presetExtension;
                
                juce::String presetPath = getDocumentsPath() + getPresetsFolderPath();
                
                juce::File presetsDir (documents);
                if (JUCE_IOS) { presetsDir = container; }
                
                juce::File sourceDirectory = presetsDir.getFullPathName() + "/" +  fileName + presetExtension;
                juce::File destinationDirectory = presetPath + "/" +  fileName + presetExtension;
                
                DBG ("File Saved to: " + destinationDirectory.getFullPathName());
                DBG ("File Saved (AUv3) to: " + sourceDirectory.getFullPathName());
                
                juce::File myFile = destinationDirectory;
                juce::File auFile (sourceDirectory);

                bool skip = false;
                
                if (!myFile.exists())
                {  myFile.create();}
                else
                {
                    juce::String currentValue = mLoadPresetDropDown.getText();
                    
                    auto *p_alertb = new juce::AlertWindow("", "", juce::AlertWindow::AlertIconType::NoIcon, this);
                        
                    p_alertb->setTitle("OVERWRITE PRESET");

                    p_alertb->addButton("CANCEL", 0);
                    p_alertb->addButton("OVERWRITE", 1);
                    p_alert->getButton(0)->setName("ALERTBOX");
                    p_alert->getButton(1)->setName("ALERTBOX");
                    p_alertb->setLookAndFeel(&cLAF.lOSC);
                    
                    p_alertb->toFront(true);
                    
                    auto localBounds = getParentComponent()->getLocalBounds();
                    auto topLevelBounds = getTopLevelComponent()->getLocalBounds();
                    float alertX = localBounds.getX() + (localBounds.getWidth() * .15);
                    float alertY = topLevelBounds.getY() + localBounds.getHeight() * .25;
                    
                    if (JUCE_IOS) { alertY = topLevelBounds.getY(); }

                    float alertW = localBounds.getWidth() * .70;
                    float alertH = localBounds.getHeight() * .30;
                    
                    p_alertb->setBounds(alertX, alertY, alertW, alertH);
                    
                    auto alertBounds = p_alert->getBounds();
                    float textY = alertBounds.getY() + (alertBounds.getHeight() * .35);
                    float textX = alertBounds.getX() - (alertBounds.getWidth() * .015);
                    float textH = alertBounds.getHeight() * .175;
                    float textW = alertBounds.getWidth() * .25;

                    p_alertb->getButton(0)->setBounds(textX,
                                                     textY,
                                                     textW,
                                                     textH);
                    
                    p_alertb->getButton(1)->setBounds(p_alert->getButton(0)->getRight() * 1.25,
                                                     textY,
                                                     textW,
                                                     textH);
                    
                    getParentComponent()->addChildComponent(p_alertb);
                    
                    p_alertb->enterModalState(true, juce::ModalCallbackFunction::create([=, this](int result) mutable
                    {
                        if (result == 1) { skip = false; }
                        else { skip = true; }
                        
                        // Exit the modal state after handling the result
                        p_alertb->exitModalState(result);
                        p_alertb->setLookAndFeel(nullptr);
                        delete p_alertb;
                    }));
                    
                    }

                if (JUCE_IOS) { if (!auFile.existsAsFile()){ auFile.create(); }}

                juce::UndoManager undo;

                audioProcessor.apvts.state.setProperty("PresetName", fileName, &undo);

                auto state = audioProcessor.apvts.copyState();
                juce::Identifier idtype = audioProcessor.apvts.state.getType();

                if (!skip)
                {
                    
                    if(myFile.existsAsFile())
                    {
                        std::unique_ptr<juce::XmlElement> xml (state.createXml());
                        xml->setTagName(idtype);
                        xml->writeTo(myFile);
                        
                        if (JUCE_IOS)
                        { xml->writeTo(auFile); }
                        mLoadPresetDropDown.setText(fileName);
                    }
                    else { mLoadPresetDropDown.setText("LOAD PRESET"); }
                    
                    if (JUCE_IOS)
                    {
                        auto baseURL = myFile.getFullPathName();
                        juce::URL url = baseURL;
                        
                        std::unique_ptr<juce::OutputStream> wo(url.createOutputStream());
                        if (wo != nullptr)
                        {
                            juce::XmlElement xml("tpb");
                            wo->writeString(state.toXmlString());
                            wo->writeString(xml.toString());
                            mLoadPresetDropDown.setText(fileName);
                        }
                        
                        juce::URL urlAU (auFile);
                        std::unique_ptr<juce::OutputStream> woAU(urlAU.createOutputStream());
                        if (wo != nullptr)
                        {
                            juce::XmlElement xml("tpb");
                            woAU->writeString(state.toXmlString());
                            woAU->writeString(xml.toString());
                            mLoadPresetDropDown.setText(fileName);
                        }
                        
                        DBG (std::to_string(auFile.existsAsFile()));
                        
                        copySAPresetsAU();
                    }
                    
                    audioProcessor.pname = fileName;
                    addPresetDropDown();

                    mLoadPresetDropDown.setText(  p_alert->getTextEditor("Value")->getText()   );
                }
        }
    }
    // Exit the modal state after handling the result
    p_alert->exitModalState(result);
    p_alert->setLookAndFeel(nullptr);
    delete p_alert;
    }));
    
    mMoreOptionsDropDown.setSelectedId(0);

}

void  PresetManagerComponent::presetSave()
{
    using namespace juce;
    
    juce::File presetsDir (getDocumentsPath() + getPresetsFolderPath());
    if (JUCE_IOS) { presetsDir = container.getFullPathName(); }

    if (!presetsDir.exists() || !presetsDir.isDirectory()) {    presetsDir.createDirectory(); }
    
    juce::String currentFilePath = mLoadPresetDropDown.getText();
    juce::String presetFileName = mLoadPresetDropDown.getText() + presetExtension;
    
    juce::File myFile (presetsDir.getFullPathName() + "/" + (mLoadPresetDropDown.getText() + presetExtension));
    if (!myFile.existsAsFile()) { myFile.create();  }
    
    juce::UndoManager undo;
    
    String fileName = myFile.getFileNameWithoutExtension();
    audioProcessor.apvts.state.setProperty("PresetName", fileName, &undo);
    auto state = audioProcessor.apvts.copyState();
    Identifier idtype = audioProcessor.apvts.state.getType();
        
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    xml->setTagName(idtype);
    xml->writeTo(myFile);
    
    if (JUCE_IOS)
    {
        auto url = juce::URL(myFile);
        std::unique_ptr<OutputStream> wo(url.createOutputStream());
        
        if (wo != nullptr)
        {
            XmlElement xml("tpb");
            wo->writeString(state.toXmlString());
            wo->writeString(xml.toString());
        }
        
    }
    
    if (myFile.existsAsFile()) { myFile.replaceWithText(xml->toString()); }
    
    audioProcessor.pname = fileName;
    mLoadPresetDropDown.setText(fileName);
    mMoreOptionsDropDown.setSelectedId(0);
}

void  PresetManagerComponent::presetDelete()
{
    using namespace juce;
        
        juce::String currentValue = mLoadPresetDropDown.getText();
        
        auto *p_alert = new juce::AlertWindow("", "", juce::AlertWindow::AlertIconType::NoIcon, this);
            
        p_alert->setTitle("DELETE PRESET");

        p_alert->setColour(juce::AlertWindow::ColourIds::backgroundColourId, juce::Colour::fromRGB(10, 10, 10).withAlpha(0.85f));
        p_alert->addButton("CANCEL", 0);
        p_alert->addButton("DELETE", 1);
        p_alert->getButton(0)->setName("ALERTBOX");
        p_alert->getButton(1)->setName("ALERTBOX");
        p_alert->setLookAndFeel(&cLAF.lOSC);
        p_alert->toFront(true);
        
        auto localBounds = getParentComponent()->getLocalBounds();
        auto topLevelBounds = getTopLevelComponent()->getLocalBounds();
        float alertX = localBounds.getX() + (localBounds.getWidth() * .15);
        float alertY = topLevelBounds.getY() + localBounds.getHeight() * .25;
        float alertW = localBounds.getWidth() * .70;
        float alertH = localBounds.getHeight() * .30;
        
        if (JUCE_IOS)
        {
            alertW = this->getWidth() * .70;
            alertH = this->getHeight() * 4;
            alertX = this->getX() + (this->getWidth() * .15);
            alertY = this->getY() + this->getHeight() * .15;
        }
        

        p_alert->setBounds(alertX, alertY, alertW, alertH);
        
        auto alertBounds = p_alert->getBounds();
        float textY = alertBounds.getY() + (alertBounds.getHeight() * .35);
        float textX = alertBounds.getX() - (alertBounds.getWidth() * .015);
        float textH = alertBounds.getHeight() * .25;
        float textW = alertBounds.getWidth() * .25;

        p_alert->getButton(0)->setBounds(textX,
                                         textY,
                                         textW,
                                         textH);
        
        p_alert->getButton(1)->setBounds(p_alert->getButton(0)->getRight() * 1.25,
                                         textY,
                                         textW,
                                         textH);
        
        getParentComponent()->addChildComponent(p_alert);
        
        p_alert->enterModalState(true, juce::ModalCallbackFunction::create([=](int result)
                                                                           {
            if (result == 1)
            {
                juce::String presetPath = getDocumentsPath() + getPresetsFolderPath();
                juce::File presetsDir (presetPath);
                juce::File sourceDirectory = presetsDir.getFullPathName();
                
                juce::File destinationDirectory = container;

                if (!presetsDir.exists() || !presetsDir.isDirectory())
                {    presetsDir.createDirectory(); }
                
                juce::String currentFilePath = mLoadPresetDropDown.getText();
                juce::String presetFileName = mLoadPresetDropDown.getText() + presetExtension;
                
                juce::File docs = getDocumentsPath();
                
                if (!presetsDir.exists() || !presetsDir.isDirectory())
                    presetsDir.createDirectory();
                
                juce::DirectoryIterator iter(presetsDir, true, "*", juce::File::TypesOfFileToFind::findFiles);
                
                while (iter.next())
                {
                    juce::File file = iter.getFile();
                    juce::String fileName = file.getFileNameWithoutExtension();
                                     
                    DBG ("File Found: " << file.getFullPathName());

                    if (fileName.contains(currentFilePath))
                    {   file.deleteFile();
                        DBG ("File Deleted:  " << file.getFullPathName());
                        break;
                    }
                }
                
                
                juce::DirectoryIterator iterAU(destinationDirectory, true, "*", juce::File::TypesOfFileToFind::findFiles);
                
                while (iterAU.next())
                {
                    juce::File auFile = iterAU.getFile();
                    juce::String auFileName = auFile.getFileNameWithoutExtension();
                            
                    DBG ("File Found AUv3: " << auFile.getFullPathName());

                    if (auFileName.contains(currentFilePath))
                    {
                        DBG ("Deleted File (AUv3):" << auFile.getFullPathName());
                        deletedFileList.add(auFile);
                        auFile.deleteFile();
                    }
                }
                
                
                if (!deletedFileList.isEmpty())
                {

                    juce::XmlElement rootElement("deletedFiles");

                    for (int i = 0; i < deletedFileList.size(); ++i)
                    {
                        juce::XmlElement* fileElement = new juce::XmlElement("File");
                        fileElement->setAttribute("Path", deletedFileList.getReference(i).getParentDirectory().getFullPathName() + "/Presets/" + deletedFileList.getReference(i).getFileName());
                        rootElement.addChildElement(fileElement);
                    }
                    
//                    DBG ("To Delete: " << deletedFileList.getReference(0).getFullPathName());

                    juce::File xmlFile(container.getFullPathName() + "/deletedFiles.xml");
                    if (xmlFile.create()) {xmlFile.replaceWithText(rootElement.toString());}
                    
//                    DBG ("Deleted Files XML is here: " << xmlFile.getFullPathName());
                }
                
//                mLoadPresetDropDown.clear();
                addPresetDropDown();
            }
            
            // Exit the modal state after handling the result
            p_alert->exitModalState(result);
            p_alert->setLookAndFeel(nullptr);
            
            delete p_alert;
        }));
        
        mMoreOptionsDropDown.setSelectedId(0);
}


void  PresetManagerComponent::presetRename()
{
    juce::String currentValue = mLoadPresetDropDown.getText();

    auto *p_alert = new juce::AlertWindow("", "", juce::AlertWindow::AlertIconType::NoIcon, this);
    p_alert->setTitle("RENAME PRESET");
    p_alert->addTextEditor("Value", currentValue);
    
    p_alert->addButton("CANCEL", 0);
    p_alert->addButton("RENAME", 1);
    p_alert->getButton(0)->setName("ALERTBOX");
    p_alert->getButton(1)->setName("ALERTBOX");
    p_alert->setLookAndFeel(&cLAF.lOSC);

    p_alert->setDropShadowEnabled(true);
    p_alert->toFront(true);

    auto localBounds = getParentComponent()->getLocalBounds();
    auto topLevelBounds = getTopLevelComponent()->getLocalBounds();
    float alertX = localBounds.getX() + (localBounds.getWidth() * .15);
    float alertY = topLevelBounds.getY() + localBounds.getHeight() * .25;
    float alertW = localBounds.getWidth() * .70;
    float alertH = localBounds.getHeight() * .40;

    if (JUCE_IOS)
    {
        alertW = this->getWidth() * .70;
        alertH = this->getHeight() * 4;
        alertX = this->getX() + (this->getWidth() * .15);
        alertY = this->getY() + this->getHeight() * .15;
    }

    p_alert->setBounds(alertX, alertY, alertW, alertH);
    
    auto alertBounds = p_alert->getBounds();
    float textY = alertBounds.getY() + (alertBounds.getHeight() * .20);
    float textX = alertBounds.getX() - (alertBounds.getWidth() * .075);
    float textH = alertBounds.getHeight() * .25;
    float textW = alertBounds.getWidth() * .75;

    p_alert->getTextEditor("Value")->setFont(p_alert->getTextEditor("Value")->getHeight() * .65);
    p_alert->getTextEditor("Value")->setJustification(juce::Justification::centred);
    
    p_alert->getTextEditor("Value")->setBounds(textX,
                                               textY,
                                               textW,
                                               textH);
        
    p_alert->getButton(0)->setBounds(textX + (p_alert->getTextEditor("Value")->getWidth() * .20),
                                     p_alert->getTextEditor("Value")->getBottom() * 1.25,
                                     p_alert->getBounds().getWidth() * .20, p_alert->getTextEditor("Value")->getHeight() * .80);
    
    p_alert->getButton(1)->setBounds(p_alert->getButton(0)->getRight(),
                                     p_alert->getTextEditor("Value")->getBottom() * 1.25,
                                     p_alert->getBounds().getWidth() * .20,
                                     p_alert->getTextEditor("Value")->getHeight() * .80);
    
    getParentComponent()->setInterceptsMouseClicks(false, false);
    getParentComponent()->addChildComponent(p_alert);
    p_alert->getTextEditor("Value")->setClicksOutsideDismissVirtualKeyboard(false);

    auto* valueEditor = p_alert->getTextEditor("Value");
    
    juce::String oldFile = mLoadPresetDropDown.getText();
    
    p_alert->enterModalState(true, juce::ModalCallbackFunction::create([=](int result)
    {
        if (result == 1)
        {
            if (valueEditor != nullptr)
            {
                juce::String presetPath = getDocumentsPath() + getPresetsFolderPath();
                juce::File presetsDir (presetPath);
                juce::File sourceDirectory = presetsDir.getFullPathName();
                
                juce::File destinationDirectory = sourceDirectory;

                if (JUCE_IOS) {   destinationDirectory = container;     }

                if (!presetsDir.exists() || !presetsDir.isDirectory())
                {    presetsDir.createDirectory(); }
                
                juce::String currentFilePath = mLoadPresetDropDown.getText();
                juce::String presetFileName = mLoadPresetDropDown.getText() + presetExtension;
                
                juce::File docs = getDocumentsPath();
                
                if (!presetsDir.exists() || !presetsDir.isDirectory())
                    presetsDir.createDirectory();
                
                juce::DirectoryIterator iterA(presetsDir, true, "*", juce::File::TypesOfFileToFind::findFiles);
                
                while (iterA.next())
                {
                    juce::File file = iterA.getFile();
                    juce::String fileName = file.getFileNameWithoutExtension();
                                     
                    DBG ("File Found: " << file.getFullPathName());

                    if (fileName.contains(currentFilePath))
                    {   file.deleteFile();
                        DBG ("File Deleted:  " << file.getFullPathName());
                        break;
                    }
                }
                
                
                juce::DirectoryIterator iterB(destinationDirectory, true, "*", juce::File::TypesOfFileToFind::findFiles);
                
                while (iterB.next())
                {
                    juce::File auFile = iterB.getFile();
                    juce::String auFileName = auFile.getFileNameWithoutExtension();
                            
                    DBG ("File Found AUv3: " << auFile.getFullPathName());

                    if (auFileName.contains(currentFilePath))
                    {
                        DBG ("Deleted File (AUv3):" << auFile.getFullPathName());
                        auFile.deleteFile();
                    }
                }
                
                mLoadPresetDropDown.setText(  valueEditor->getText()   );
                
                //Make Preset
                presetsDir = (getDocumentsPath() + getPresetsFolderPath());
                                
                DBG ("File Saved to: " + presetsDir.getFullPathName()); DBG ("File Saved (AUv3) to: " + presetsDir.getFullPathName());
                   
                juce::DirectoryIterator iter(presetsDir, true, "*", juce::File::TypesOfFileToFind::findFiles);
                
                while (iter.next())
                {
                    juce::File file = iter.getFile();
                    juce::String fileName = file.getFileNameWithoutExtension();
                    
                    if (fileName.containsIgnoreCase(oldFile))
                    {
                        file.deleteFile();
                        break;
                    }
                }
                
                if (JUCE_IOS) { presetsDir = container.getFullPathName(); }

                juce::DirectoryIterator iterAU(presetsDir, true, "*", juce::File::TypesOfFileToFind::findFiles);
                
                while (iterAU.next())
                {
                    juce::File auFile = iterAU.getFile();
                    juce::String auFileName = auFile.getFileNameWithoutExtension();
                    
                    DBG ("File Found: " << auFile.getFullPathName());
                    DBG ("File Looked For:  " << oldFile);
                    
                    if (auFileName.containsIgnoreCase(oldFile))
                    {
                        DBG ("Deleted File:" << auFile.getFullPathName());
                        auFile.deleteFile();
                        break;
                    }
                }
                
                presetSave();
                addPresetDropDown();
                mLoadPresetDropDown.setText(   valueEditor->getText()   );
            }
        }
        // Exit the modal state after handling the result
        p_alert->exitModalState(result);
        p_alert->setLookAndFeel(nullptr);
        
        valueEditor->grabKeyboardFocus();
        valueEditor->selectAll();
        valueEditor->setSelectAllWhenFocused(true);
        valueEditor->setClicksOutsideDismissVirtualKeyboard(false);
        valueEditor->setMouseClickGrabsKeyboardFocus(true);

        
        EnterKeyListener* enterKeyListener = new EnterKeyListener(p_alert);
        valueEditor->addKeyListener(enterKeyListener);
        
        audioProcessor.pname = valueEditor->getText();

        delete p_alert;
        
    }));

    getParentComponent()->setInterceptsMouseClicks(false, true);
    mMoreOptionsDropDown.setSelectedId(0);
}


void  PresetManagerComponent::nextPreset()
{
    mLoadPresetDropDown.setSelectedId((mLoadPresetDropDown.getSelectedId() % mLoadPresetDropDown.getNumItems()) + 1);
}

void  PresetManagerComponent::prevPreset()
{
    mLoadPresetDropDown.setSelectedId(mLoadPresetDropDown.getSelectedId() == 1 ? mLoadPresetDropDown.getNumItems() : mLoadPresetDropDown.getSelectedId() - 1);
}

void  PresetManagerComponent::importURL (juce::URL url)
{
    
}

void PresetManagerComponent::copySAPresetsAU() {
  
if (JUCE_IOS)
{
    juce::File destinationDirectory = container;
    juce::File presetsDir(getDocumentsPath());
    juce::File sourceDirectory = presetsDir.getChildFile("Presets");

    if (!sourceDirectory.isDirectory() || !destinationDirectory.isDirectory()) { juce::Logger::writeToLog("Source and/or destination directories are not valid."); return; }

    if (juce::JUCEApplication::isStandaloneApp())
    {
        
    juce::File xmlFile(container.getFullPathName() + "/deletedFiles.xml");
    // Load the XML document
    std::unique_ptr<juce::XmlElement> rootElement(juce::XmlDocument::parse(xmlFile));
        
        if (rootElement != nullptr && rootElement->getTagName() == "deletedFiles")
        {
            // Iterate through child elements
            for (int i = 0; i < rootElement->getNumChildElements(); i++)
            {
                juce::XmlElement* childElement = rootElement->getChildElement(i);
                if (childElement->getTagName() == "File")
                {
                    // Get the "path" attribute value
                    juce::String filePath = childElement->getStringAttribute("Path");
                    juce::File fileToDelete (filePath);
                    
                    juce::File secondFileToDelete ( sourceDirectory.getFullPathName() + "/" + fileToDelete.getFileName() );
                    
                    DBG ("File Found in XML: " << fileToDelete.getFullPathName());
                    DBG ("Second File Found in XML: " << secondFileToDelete.getFullPathName());

                    if (  fileToDelete.exists()  )
                    {  fileToDelete.deleteFile(); }
                    if (  secondFileToDelete.exists()  )
                    {  secondFileToDelete.deleteFile(); }
                }
            }
        }
        xmlFile.deleteFile();
    }
    
    // Delete recursively the presets folder to ensure no duplicate files exist
    juce::File presetsFolder = presetsDir.getChildFile("Presets");
//    presetsFolder.deleteRecursively();

    // Iterate through the source directory files
    juce::DirectoryIterator iter(sourceDirectory, true, "*", juce::File::TypesOfFileToFind::findFiles);
    while (iter.next()) {
        juce::File currentFile = iter.getFile();
        if (currentFile.getFullPathName().contains(appName)) {
            juce::String fileName = currentFile.getFileNameWithoutExtension();
            juce::File destinationSubDirectory;

            if (fileName.containsIgnoreCase("Insert")) { destinationSubDirectory = destinationDirectory.getChildFile("Inserts"); }
            else if (fileName.containsIgnoreCase("Send")) { destinationSubDirectory = destinationDirectory.getChildFile("Sends"); }
            else { destinationSubDirectory = destinationDirectory; }

            destinationSubDirectory.createDirectory(); // Ensure the subdirectory exists
            juce::File destinationFile = destinationSubDirectory.getChildFile(currentFile.getFileName());

            if (currentFile.copyFileTo(destinationFile)) {
                juce::Logger::writeToLog("File copied to " + destinationFile.getFullPathName());
                juce::Logger::writeToLog("File copied from " + currentFile.getFullPathName()); }
            else {
                juce::Logger::writeToLog("Failed to copy to " + destinationFile.getFullPathName());
                juce::Logger::writeToLog("Failed to copy from " + currentFile.getFullPathName()); }
        }
    }

    // Iterate through the source directory files
    juce::DirectoryIterator iterb(destinationDirectory, true, "*", juce::File::TypesOfFileToFind::findFiles);
    while (iterb.next()) {
        juce::File currentFile = iterb.getFile();
        if (currentFile.getFullPathName().contains(appName)) {
            juce::String fileName = currentFile.getFileNameWithoutExtension();
            juce::File destinationSubDirectory;

            if (fileName.containsIgnoreCase("Insert")) { destinationSubDirectory = sourceDirectory.getChildFile("Inserts"); }
            else if (fileName.containsIgnoreCase("Send")) { destinationSubDirectory = sourceDirectory.getChildFile("Sends"); }
            else { destinationSubDirectory = sourceDirectory; }

            destinationSubDirectory.createDirectory(); // Ensure the subdirectory exists
            juce::File destinationFile = destinationSubDirectory.getChildFile(currentFile.getFileName());

            if (currentFile.copyFileTo(destinationFile)) {
                juce::Logger::writeToLog("File copied to " + destinationFile.getFullPathName());
                juce::Logger::writeToLog("File copied from " + currentFile.getFullPathName()); }
            else {
                juce::Logger::writeToLog("Failed to copy to " + destinationFile.getFullPathName());
                juce::Logger::writeToLog("Failed to copy from " + currentFile.getFullPathName()); }
        }
    }
    
    juce::String oldName = mLoadPresetDropDown.getText();
    addPresetDropDown();
    mLoadPresetDropDown.setText (oldName);
    
}
}


juce::String PresetManagerComponent::getDocumentsPath()
{
    return juce::File::getSpecialLocation(JUCE_IOS ? juce::File::userDocumentsDirectory : juce::File::commonDocumentsDirectory).getFullPathName();
}

juce::String PresetManagerComponent::getPresetsFolderPath()
{
    return JUCE_IOS ? "/Presets/" : "/OSC Audio/" + appName + "/Presets/";
}

void PresetManagerComponent::copyFactoryPresets()
{
    
    bool documentsExists = false;
    
    if (JUCE_IOS)
    { documentsExists = juce::File (container).getChildFile("Presets/Bigscreen.pocketOscilloscope").existsAsFile(); }
    else
    { documentsExists = juce::File ( getDocumentsPath() + getPresetsFolderPath() + "/Bigscreen.pocketOscilloscope" ).existsAsFile() ; }
    
    if ( !documentsExists )
    {
    
        DBG ("Let's make some presets!");
        
    for (int i = 0; i < BinaryData::namedResourceListSize; i++)
    {
        juce::File docs;
        juce::String path, slotPresetPath, chainPresetPath, slotPresetName, defaultSlotPresetPath, presetName;
        const char * fileName = BinaryData::namedResourceList[i];
        int fileSize = sizeof(BinaryData::namedResourceList[i]);
        auto data = BinaryData::getNamedResource(fileName, fileSize);
        
        std::unique_ptr<juce::XmlElement> xml;
        
        if ( juce::String(fileName).containsWholeWord(appName) )
        {
            if (data != nullptr)
            {
                if (!juce::String(fileName).containsWholeWord("wav")&& !juce::String(fileName).containsWholeWord("aif") && !juce::String(fileName).containsWholeWord("TTF") && !juce::String(fileName).containsWholeWord("ttf") && !juce::String(fileName).containsWholeWord("png") )
                {
                    xml = juce::parseXML(  data  );
                }
            }
        }
        
        if (xml != nullptr)
        {
            
            
            presetName = juce::String(fileName).dropLastCharacters(19);
            presetName = presetName.replaceCharacters("_", " ");
            
            if (juce::String(fileName).containsWholeWord(appName))
            {
                //Set paths for each OS
                if (JUCE_IOS)
                {
                    docs = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
                    path = docs.getFullPathName();
                    
                    slotPresetPath = "/Presets/";
                    juce::String subFolder;
                    
                    if (presetName.contains("Send")) { subFolder = "/Sends/"; }
                    if (presetName.contains("Insert")) { subFolder = "/Inserts/"; }
                    
                    defaultSlotPresetPath = container.getFullPathName() + "/Presets/" + subFolder + presetName + presetExtension;
                    
                }
                else
                {
                    docs = juce::File::getSpecialLocation(juce::File::commonDocumentsDirectory);
                    path = docs.getFullPathName();
                    slotPresetPath = "/OSC Audio/"+ appName + "/Presets/";
                    
                    juce::File subFolderCreator;
                    
                    juce::String subFolder;
                    
                    if (presetName.contains("Send")) { subFolder = "/Sends/"; subFolderCreator = getDocumentsPath() + "/OSC Audio/"+ appName +"/Presets/" + subFolder; subFolderCreator.createDirectory();}
                    if (presetName.contains("Insert")) { subFolder = "/Inserts/"; subFolderCreator = getDocumentsPath() + "/OSC Audio/"+ appName +"/Presets/" + subFolder; subFolderCreator.createDirectory();}
                    
                    defaultSlotPresetPath = "/OSC Audio/"+ appName +"/Presets/" + subFolder + presetName + presetExtension;
                    
                    juce::File macPCFile = getDocumentsPath() + defaultSlotPresetPath;
                    // Check if the file exists
                    if (macPCFile.exists()) {
                        DBG("Old file exists. Deleting...");
                        // Attempt to delete the old file
                        if (macPCFile.deleteFile()) {
                            DBG("Old file deleted successfully.");
                        } else {
                            DBG("Failed to delete old file.");
                            // Handle error, e.g., return or throw an exception
                        }

                        // Check if the file still exists after deletion attempt
                        if (macPCFile.exists()) {
                            DBG("Old file still exists after deletion attempt.");
                        } else {
                            DBG("Old file deleted successfully.");
                        }
                    } else {
                        DBG("No old file found.");
                    }

                    if (macPCFile.exists())
                    {
                        // Replace the file with the new content
                        if (macPCFile.replaceWithText(xml->toString())) {
                            DBG("File replaced with text successfully.");
                            // Write the XML content to the file
                            if (xml->writeTo(macPCFile)) {
                                DBG("XML content written to file successfully.");
                            } else {
                                DBG("Failed to write XML content to file.");
                                // Handle error, e.g., return or throw an exception
                            }
                        } else {
                            DBG("Failed to replace file with text.");
                            // Handle error, e.g., return or throw an exception
                        }
                    }
                    if (!macPCFile.exists())
                    {
                        macPCFile.create();
                        xml->writeTo(macPCFile);
                    }


                }
                
                //Save iOS Presets
                juce::String documents = path + defaultSlotPresetPath;
                juce::File presetsDir = documents;
                
                if (JUCE_IOS) { presetsDir = container.getChildFile(documents);  }
                
                presetsDir = defaultSlotPresetPath;
                
                auto resultB = presetsDir.getFullPathName();
                
                juce::File propertiesFile = resultB;
                
                if (!propertiesFile.existsAsFile()) { propertiesFile.create(); }
                
                if (xml != nullptr)
                {
                    xml->writeTo(propertiesFile);
                    juce::FileOutputStream presetStreamB (propertiesFile);
                    
                    presetStreamB.writeString(xml->toString());
                    
                    if (propertiesFile.existsAsFile())
                    {  propertiesFile.replaceWithText(xml->toString()); }
                }
            }
        }
    }
    copySAPresetsAU();
    addPresetDropDown();
}
}

void PresetManagerComponent::setPresetBarText(juce::String presetName)
{
    if (presetName.isNotEmpty())
    {
        if (!mLoadPresetDropDown.getText().contains(presetName))
        { mLoadPresetDropDown.setText(presetName << " *"); }
    }
}
