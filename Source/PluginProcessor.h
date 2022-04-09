//PluginProcessor.h

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class RTHumanizerAudioProcessor  : public juce::AudioProcessor
{
public:
    
     float velocity=0;
     float timing=0;
    

     int numOfBeats=99;
     float beatone,beattwo,beatthree,beatfour;
     juce::MidiBuffer backupBuffer1;
     juce::MidiBuffer backupBuffer2;
    
    //==============================================================================
    RTHumanizerAudioProcessor();
    ~RTHumanizerAudioProcessor() override;
    

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
    
    juce::AudioProcessorValueTreeState parameters;
    
    std::atomic<float>* velocityParameter = nullptr;
    std::atomic<float>* timingParameter  = nullptr;
    std::atomic<float>* beatOneParameter = nullptr;
    std::atomic<float>* beatTwoParameter = nullptr;
    std::atomic<float>* beatThreeParameter = nullptr;
    std::atomic<float>* beatFourParameter = nullptr;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RTHumanizerAudioProcessor)
    
    juce::AudioPlayHead* playHead;
    juce::AudioPlayHead::CurrentPositionInfo currentPositionInfo;
    juce::AudioPlayHead::CurrentPositionInfo previousPositionInfo;
    int beat;

};
