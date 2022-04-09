//PluginEditor.h

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
class RTHumanizerAudioProcessorEditor :public juce::AudioProcessorEditor,
                                         private juce::Timer,
                                         private juce::Slider::Listener
                                         
{
public:
    RTHumanizerAudioProcessorEditor (RTHumanizerAudioProcessor&);
    ~RTHumanizerAudioProcessorEditor();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:

    void sliderValueChanged (juce::Slider* slider) override;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RTHumanizerAudioProcessor& audioProcessor;

    juce::Slider midiVolume, midiTiming;
    juce::Slider beatOne,beatTwo,beatThree,beatFour;
    juce::Array<juce::Slider*> Sliders;
    juce::ImageComponent logoComponent;
    
    //juce::AudioProcessorValueTreeState& valueTreeState;
    
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> velocityAttachment;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> timingAttachment;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> beatOneAttachment,beatTwoAttachment,beatThreeAttachment,beatFourAttachment;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RTHumanizerAudioProcessorEditor)
};
