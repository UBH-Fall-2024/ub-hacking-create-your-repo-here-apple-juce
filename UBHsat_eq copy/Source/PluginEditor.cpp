/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
UBHsat_eqAudioProcessorEditor::UBHsat_eqAudioProcessorEditor (UBHsat_eqAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

UBHsat_eqAudioProcessorEditor::~UBHsat_eqAudioProcessorEditor()
{
}

//==============================================================================
void UBHsat_eqAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //fills the whole window with white
    g.fillAll (juce::Colours::white);
    //sets the drawing color to black
    g.setColour (juce::Colours::black);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Saturator", 0, 0, getWidth(),30,juce::Justification::centred,1);
}

void UBHsat_eqAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // (x,y,width,height) in px
}
