/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class UBHsat_eqAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Slider::Listener
{
public:
    UBHsat_eqAudioProcessorEditor (UBHsat_eqAudioProcessor&);
    ~UBHsat_eqAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    UBHsat_eqAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UBHsat_eqAudioProcessorEditor)
};
