/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct ChainSettings
{
    float LPFreq { 0 }, HPFreq { 0 };
    float drive{ 0 };
    /*float mix{0};*/
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

//==============================================================================
/**
*/
class UBHsat_eqAudioProcessor  : public juce::AudioProcessor
{
public:
    
    //==============================================================================
    UBHsat_eqAudioProcessor();
    ~UBHsat_eqAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    juce::AudioProcessorValueTreeState apvts{*this, nullptr, "Parameters", createParameterLayout()};

private:
    
    static float currentDrive;
    
    static float waveshape(float sample);
    
    using Filter = juce::dsp::IIR::Filter<float>;
    
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    
    using WaveShaper = juce::dsp::WaveShaper<float>;
    
    using MonoChain = juce::dsp::ProcessorChain<CutFilter,WaveShaper, CutFilter>;
    
    MonoChain leftChain, rightChain;
    
    static float waveshapeSignal(float sample, float drive) {
            // Arctan distortion
            constexpr float pi = juce::MathConstants<float>::pi;
            return (2.0f / pi) * std::atan(sample * (1.0f + drive));
        }
    
    enum ChainPositions{
        HP,
        drive,
        LP//,
        //Mix
    };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UBHsat_eqAudioProcessor)
};
