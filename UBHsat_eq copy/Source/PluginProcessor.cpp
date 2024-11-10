/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
UBHsat_eqAudioProcessor::UBHsat_eqAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

UBHsat_eqAudioProcessor::~UBHsat_eqAudioProcessor()
{
}

//==============================================================================
const juce::String UBHsat_eqAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool UBHsat_eqAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool UBHsat_eqAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool UBHsat_eqAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double UBHsat_eqAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int UBHsat_eqAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int UBHsat_eqAudioProcessor::getCurrentProgram()
{
    return 0;
}

void UBHsat_eqAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String UBHsat_eqAudioProcessor::getProgramName (int index)
{
    return {};
}

void UBHsat_eqAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

float UBHsat_eqAudioProcessor::currentDrive = 0.0f;

float UBHsat_eqAudioProcessor::waveshape(float sample){
    return (2.0f/juce::MathConstants<float>::pi) * std::atan(sample * (1.0f + currentDrive));
}

//==============================================================================
void UBHsat_eqAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    juce::dsp::ProcessSpec spec;
    
    spec.maximumBlockSize = samplesPerBlock;
    
    spec.numChannels = 1;
    
    spec.sampleRate = sampleRate;
    
    leftChain.prepare(spec);
    rightChain.prepare(spec);
    
    auto chainSettings = getChainSettings(apvts);
    
    auto HPCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.HPFreq,sampleRate,8);
    
    auto& leftHP = leftChain.get<ChainPositions::HP>();
    
    *leftHP.get<0>().coefficients = *HPCoefficients[0];
    leftHP.setBypassed<0>(false);
    *leftHP.get<1>().coefficients = *HPCoefficients[1];
    leftHP.setBypassed<1>(false);
    *leftHP.get<2>().coefficients = *HPCoefficients[2];
    leftHP.setBypassed<2>(false);
    *leftHP.get<3>().coefficients = *HPCoefficients[3];
    leftHP.setBypassed<3>(false);
    
    auto& rightHP = rightChain.get<ChainPositions::HP>();
    
    *rightHP.get<0>().coefficients = *HPCoefficients[0];
    rightHP.setBypassed<0>(false);
    *rightHP.get<1>().coefficients = *HPCoefficients[1];
    rightHP.setBypassed<1>(false);
    *rightHP.get<2>().coefficients = *HPCoefficients[2];
    rightHP.setBypassed<2>(false);
    *rightHP.get<3>().coefficients = *HPCoefficients[3];
    rightHP.setBypassed<3>(false);
    
    auto LPCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.LPFreq,sampleRate,8);
    
    auto& leftLP = leftChain.get<ChainPositions::LP>();
    
    *leftLP.get<0>().coefficients = *LPCoefficients[0];
    leftLP.setBypassed<0>(false);
    *leftLP.get<1>().coefficients = *LPCoefficients[1];
    leftLP.setBypassed<1>(false);
    *leftLP.get<2>().coefficients = *LPCoefficients[2];
    leftLP.setBypassed<2>(false);
    *leftLP.get<3>().coefficients = *LPCoefficients[3];
    leftLP.setBypassed<3>(false);
    
    auto& rightLP = rightChain.get<ChainPositions::LP>();
    
    *rightLP.get<0>().coefficients = *LPCoefficients[0];
    rightLP.setBypassed<0>(false);
    *rightLP.get<1>().coefficients = *LPCoefficients[1];
    rightLP.setBypassed<1>(false);
    *rightLP.get<2>().coefficients = *LPCoefficients[2];
    rightLP.setBypassed<2>(false);
    *rightLP.get<3>().coefficients = *LPCoefficients[3];
    rightLP.setBypassed<3>(false);
    
    auto& leftDrive = leftChain.get<ChainPositions::drive>();
    auto& rightDrive = rightChain.get<ChainPositions::drive>();
    
    leftDrive.functionToUse = waveshape;
    rightDrive.functionToUse = waveshape;
}

void UBHsat_eqAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UBHsat_eqAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void UBHsat_eqAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
     juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i){
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    auto chainSettings = getChainSettings(apvts);
    
    currentDrive = chainSettings.drive;
    
    auto passCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.HPFreq,getSampleRate(),8);
    
    auto& leftHP = leftChain.get<ChainPositions::HP>();
    
    *leftHP.get<0>().coefficients = *passCoefficients[0];
    leftHP.setBypassed<0>(false);
    *leftHP.get<1>().coefficients = *passCoefficients[1];
    leftHP.setBypassed<1>(false);
    *leftHP.get<2>().coefficients = *passCoefficients[2];
    leftHP.setBypassed<2>(false);
    *leftHP.get<3>().coefficients = *passCoefficients[3];
    leftHP.setBypassed<3>(false);
    
    auto& rightHP = rightChain.get<ChainPositions::HP>();
    
    *rightHP.get<0>().coefficients = *passCoefficients[0];
    rightHP.setBypassed<0>(false);
    *rightHP.get<1>().coefficients = *passCoefficients[1];
    rightHP.setBypassed<1>(false);
    *rightHP.get<2>().coefficients = *passCoefficients[2];
    rightHP.setBypassed<2>(false);
    *rightHP.get<3>().coefficients = *passCoefficients[3];
    rightHP.setBypassed<3>(false);
    
    auto LPCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.LPFreq,getSampleRate(),8);
    
    auto& leftLP = leftChain.get<ChainPositions::LP>();
    
    *leftLP.get<0>().coefficients = *LPCoefficients[0];
    leftLP.setBypassed<0>(false);
    *leftLP.get<1>().coefficients = *LPCoefficients[1];
    leftLP.setBypassed<1>(false);
    *leftLP.get<2>().coefficients = *LPCoefficients[2];
    leftLP.setBypassed<2>(false);
    *leftLP.get<3>().coefficients = *LPCoefficients[3];
    leftLP.setBypassed<3>(false);
    
    auto& rightLP = rightChain.get<ChainPositions::LP>();
    
    *rightLP.get<0>().coefficients = *LPCoefficients[0];
    rightLP.setBypassed<0>(false);
    *rightLP.get<1>().coefficients = *LPCoefficients[1];
    rightLP.setBypassed<1>(false);
    *rightLP.get<2>().coefficients = *LPCoefficients[2];
    rightLP.setBypassed<2>(false);
    *rightLP.get<3>().coefficients = *LPCoefficients[3];
    rightLP.setBypassed<3>(false);
    
    auto& leftDrive = leftChain.get<ChainPositions::drive>();
    auto& rightDrive = rightChain.get<ChainPositions::drive>();
    
    leftDrive.functionToUse = waveshape;
    rightDrive.functionToUse = waveshape;
    
    
    
    juce::dsp::AudioBlock<float> block(buffer);
    
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);
    
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

//==============================================================================
bool UBHsat_eqAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* UBHsat_eqAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void UBHsat_eqAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void UBHsat_eqAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts){
    ChainSettings settings;
    
    settings.HPFreq = apvts.getRawParameterValue("HPFreq")->load();
    settings.LPFreq = apvts.getRawParameterValue("LPFreq")->load();
    settings.drive = apvts.getRawParameterValue("drive")->load();
    //settings.mix = apvts.getRawParameterValue("mix")->load();
    
    return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout
    UBHsat_eqAudioProcessor::createParameterLayout(){
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        
        //1000 Hz default cutoff value, can go from 20 to 20k
        layout.add(std::make_unique<juce::AudioParameterFloat>("HPFreq", "HPFreq", juce::NormalisableRange<float>(20.f,20000.f,1.f,.25f),1000.f));
        
        layout.add(std::make_unique<juce::AudioParameterFloat>("LPFreq", "LPFreq", juce::NormalisableRange<float>(20.f,20000.f,1.f,.25f),5000.f));
        
        layout.add(std::make_unique<juce::AudioParameterFloat>("drive", "Drive", juce::NormalisableRange<float>(0.f,24.f,.25f,1.f),0.f));
        
        /*
         layout.add(std::make_unique<juce::AudioParameterFloat>("mix", "Wet/Dry Mix", juce::NormalisableRange<float>(0.f,100.f,.25f,1.f),50.f));
         */
        return layout;
    }
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UBHsat_eqAudioProcessor();
}
