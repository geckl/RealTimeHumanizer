/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyFirstPluginAudioProcessorEditor::MyFirstPluginAudioProcessorEditor (MyFirstPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    //initializes timer for greying out unused sliders
    startTimer(100);
    
    // these define the parameters of the velocity randomizer
    midiVolume.setSliderStyle (juce::Slider::LinearBarVertical);
    midiVolume.setRange (1.0, 100.0, 1.0);
    midiVolume.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    midiVolume.setPopupDisplayEnabled (true, false, this);
    midiVolume.setTextValueSuffix (" Volume");
    midiVolume.setValue(1.0);
 
    // this function adds the slider to the editor
    addAndMakeVisible (&midiVolume);
    
    //this function connects the slider to the automation parameter
    velocityAttachment= new juce::AudioProcessorValueTreeState::SliderAttachment(p.parameters, "velocity",midiVolume);
  
    // add the listener to the slider (not needed anymore)
    midiVolume.addListener (this);
    
    // these define the parameters of the velocity randomizer
    midiTiming.setSliderStyle (juce::Slider::LinearBarVertical);
    midiTiming.setRange (1.0f, 100.0, 1.0);
    midiTiming.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    midiTiming.setPopupDisplayEnabled (true, false, this);
    midiTiming.setTextValueSuffix (" Timing");
    midiTiming.setValue(1.0);
 
    // this function adds the slider to the editor
    addAndMakeVisible (&midiTiming);
    
    //this function connects the slider to the automation parameter
    timingAttachment= new juce::AudioProcessorValueTreeState::SliderAttachment(p.parameters, "timing",midiTiming);
    
    // add the listener to the slider (not needed anymore))
    midiTiming.addListener (this);
    
    // these define the parameters of the beat-based velocity changer
    beatOne.setSliderStyle (juce::Slider::LinearBarVertical);
    beatOne.setRange (-50.0, 50.0, 1.0);
    beatOne.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    beatOne.setPopupDisplayEnabled (true, false, this);
    beatOne.setTextValueSuffix (" Beat One");
    beatOne.setValue(1.0);
    beatOne.setColour(juce::Slider::trackColourId,juce::Colours::black);
    
    addAndMakeVisible (&beatOne);
    beatOneAttachment= new juce::AudioProcessorValueTreeState::SliderAttachment(p.parameters, "beatone",beatOne);
    
    beatOne.addListener (this);
    Sliders.add(&beatOne);
    
    beatTwo.setSliderStyle (juce::Slider::LinearBarVertical);
    beatTwo.setRange (-50.0, 50.0, 1.0);
    beatTwo.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    beatTwo.setPopupDisplayEnabled (true, false, this);
    beatTwo.setTextValueSuffix (" Beat Two");
    beatTwo.setValue(1.0);
    beatTwo.setColour(juce::Slider::trackColourId,juce::Colours::black);
    
    addAndMakeVisible (&beatTwo);
    beatTwoAttachment= new juce::AudioProcessorValueTreeState::SliderAttachment(p.parameters, "beattwo",beatTwo);
    
    beatTwo.addListener (this);
    Sliders.add(&beatTwo);
    
    beatThree.setSliderStyle (juce::Slider::LinearBarVertical);
    beatThree.setRange (-50.0, 50.0, 1.0);
    beatThree.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    beatThree.setPopupDisplayEnabled (true, false, this);
    beatThree.setTextValueSuffix (" Beat Three");
    beatThree.setValue(1.0);
    beatThree.setColour(juce::Slider::trackColourId,juce::Colours::black);
    
    addAndMakeVisible (&beatThree);
    beatThreeAttachment= new juce::AudioProcessorValueTreeState::SliderAttachment(p.parameters, "beatthree",beatThree);
    
    beatThree.addListener (this);
    Sliders.add(&beatThree);
    
    beatFour.setSliderStyle (juce::Slider::LinearBarVertical);
    beatFour.setRange (-50.0, 50.0, 1.0);
    beatFour.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    beatFour.setPopupDisplayEnabled (true, false, this);
    beatFour.setTextValueSuffix (" Beat Four");
    beatFour.setValue(1.0);
    beatFour.setColour(juce::Slider::trackColourId,juce::Colours::black);
    
    addAndMakeVisible (&beatFour);
    beatFourAttachment= new juce::AudioProcessorValueTreeState::SliderAttachment(p.parameters, "beatfour",beatFour);
    
    beatFour.addListener (this);
    Sliders.add(&beatFour);
}

MyFirstPluginAudioProcessorEditor::~MyFirstPluginAudioProcessorEditor()
{
}

//==============================================================================
void MyFirstPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::ghostwhite);

    g.setColour (juce::Colours::black);
    g.setFont (15.0f);
    g.drawFittedText ("Velocity", 0, 0, 100, 30, juce::Justification::centred, 1);
    g.drawFittedText ("Randomization", 0, 15, 100, 30, juce::Justification::centred, 1);
    g.drawFittedText ("Timing", 100, 0, 100, 30, juce::Justification::centred, 1);
    g.drawFittedText ("Randomization", 100, 15, 100, 30, juce::Justification::centred, 1);
    g.drawFittedText ("Beat Emphasization", 210, 0, 120, 30, juce::Justification::centred, 1);
    g.drawFittedText ("1", 210, 15, 30, 30, juce::Justification::centred, 1);
    g.drawFittedText ("2", 240, 15, 30, 30, juce::Justification::centred, 1);
    g.drawFittedText ("3", 270, 15, 30, 30, juce::Justification::centred, 1);
    g.drawFittedText ("4", 300, 15, 30, 30, juce::Justification::centred, 1);
    
    //Disable beat sliders that aren't needed for current time signature
    for(int n=4; n>0;n--)
    {
        char b='0'+n;
        
        if(audioProcessor.numOfBeats<n)
        {
            Sliders[n-1]->setColour(juce::Slider::trackColourId,juce::Colours::grey);
            Sliders[n-1]->setTextValueSuffix (" Beat not used");
            Sliders[n-1]->setEnabled(0);
        }
        
        else if(audioProcessor.numOfBeats>=n)
        {
            std::string beatz=" Beat ";
            beatz+=b;
            Sliders[n-1]->setTextValueSuffix (beatz);
            Sliders[n-1]->setColour(juce::Slider::trackColourId,juce::Colours::black);
            Sliders[n-1]->setEnabled(1);
        }
    }
    

}

void MyFirstPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // sets the position and size of the slider with arguments (x, y, width, height)
    midiVolume.setBounds (40, 40, 20, getHeight() - 60);
    midiTiming.setBounds (140, 40, 20, getHeight() - 60);
    beatOne.setBounds (215, 40, 20, getHeight() - 60);
    beatTwo.setBounds (245, 40, 20, getHeight() - 60);
    beatThree.setBounds (275, 40, 20, getHeight() - 60);
    beatFour.setBounds (305, 40, 20, getHeight() - 60);
}

void MyFirstPluginAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &midiVolume)
    {
        audioProcessor.velocity = midiVolume.getValue();
    }
    
    if (slider == &midiTiming)
    {
        audioProcessor.timing = midiTiming.getValue();
    }
    
    else if (slider == &beatOne)
    {
        audioProcessor.beatone = beatOne.getValue();
    }
    
    else if (slider == &beatTwo)
    {
        audioProcessor.beattwo = beatTwo.getValue();
    }
    
    else if (slider == &beatThree)
    {
        audioProcessor.beatthree = beatThree.getValue();
    }

    else if (slider == &beatFour)
    {
        audioProcessor.beatfour = beatFour.getValue();
    }


}

void MyFirstPluginAudioProcessorEditor::timerCallback()
{
    void repaint();
};

/*juce::Timer::~Timer()
{
    
}*/
