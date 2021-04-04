/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================




MyFirstPluginAudioProcessor::MyFirstPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     //#if ! JucePlugin_IsMidiEffect
                      //#if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      //#endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     //#endif
                       ),
#endif
    parameters(*this,nullptr,juce::Identifier ("RTHumanizer"),
{
    std::make_unique<juce::AudioParameterInt> ("velocity",            // parameterID
                                                 "Velocity",            // parameter name
                                                 0,              // minimum value
                                                 100,              // maximum value
                                                 0),             // default value
    std::make_unique<juce::AudioParameterInt> ("timing",            // parameterID
                                                 "Timing",            // parameter name
                                                 0,              // minimum value
                                                 100,              // maximum value
                                                 0),             // default value
    std::make_unique<juce::AudioParameterInt> ("beatone",            // parameterID
                                                 "Beat One",            // parameter name
                                                 -50,              // minimum value
                                                 50,              // maximum value
                                                 0),             // default value
    std::make_unique<juce::AudioParameterInt> ("beattwo",            // parameterID
                                                 "Beat Two",            // parameter name
                                                 -50,              // minimum value
                                                 50,              // maximum value
                                                 0),             // default value
    std::make_unique<juce::AudioParameterInt> ("beatthree",            // parameterID
                                                 "Beat Three",            // parameter name
                                                 -50,              // minimum value
                                                 50,              // maximum value
                                                 0),             // default value
    std::make_unique<juce::AudioParameterInt> ("beatfour",            // parameterID
                                                 "Beat Four",            // parameter name
                                                 -50,              // minimum value
                                                 50,              // maximum value
                                                 0)             // default value
})
{
    velocityParameter = parameters.getRawParameterValue ("velocity");
    timingParameter  = parameters.getRawParameterValue ("timing");
    beatOneParameter  = parameters.getRawParameterValue ("beatone");
    beatTwoParameter  = parameters.getRawParameterValue ("beattwo");
    beatThreeParameter  = parameters.getRawParameterValue ("beatthree");
    beatFourParameter  = parameters.getRawParameterValue ("beatfour");
}




MyFirstPluginAudioProcessor::~MyFirstPluginAudioProcessor()
{
}

//==============================================================================
const juce::String MyFirstPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MyFirstPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MyFirstPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MyFirstPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MyFirstPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MyFirstPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MyFirstPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MyFirstPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MyFirstPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void MyFirstPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MyFirstPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    

    setLatencySamples(2500);

}

void MyFirstPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MyFirstPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void MyFirstPluginAudioProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    
    
    juce::MidiBuffer processedMidi;
    int time,time2,newTime;
    int randomVelocity, randomTiming;
    juce::MidiMessage m,n;
    float BPMInSamples= (60/currentPositionInfo.bpm)*getSampleRate();
    timing=*timingParameter;
    velocity=*velocityParameter;
    beatone=*beatOneParameter;
    beattwo=*beatTwoParameter;
    beatthree=*beatThreeParameter;
    beatfour=*beatFourParameter;
    
    //Transfer previous PositionInfo into storage
    previousPositionInfo=currentPositionInfo;
    
    //get current beat
    playHead = this->getPlayHead();
    playHead->getCurrentPosition (currentPositionInfo);
    
    
    //iterate through delayed notes from previous buffers.
    for(juce::MidiBuffer::Iterator j (backupBuffer1); j.getNextEvent(n,time2);)
    {
        if(time2<buffer.getNumSamples())
        {
            processedMidi.addEvent(n,time2);
        }
        
        else if(time2>buffer.getNumSamples())
        {
            backupBuffer2.addEvent(n, time2-buffer.getNumSamples());
        }
    }
    
    backupBuffer1.clear();
    backupBuffer1.swapWith(backupBuffer2);
    
    //If playhead has stopped moving, output an AllNotesOff MIDI message
    if(currentPositionInfo.ppqPosition==previousPositionInfo.ppqPosition)
    {
        processedMidi.addEvent(juce::MidiMessage::allNotesOff(n.getChannel()), 0);
    }
    
    //iterate through current buffer
    for (juce::MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {

        
        
        //add to ppqPosition to account for samples within buffer
        float ppqPositionWithinBuffer=time/BPMInSamples;
        float samplePPQPosition= currentPositionInfo.ppqPosition+ppqPositionWithinBuffer;
        int z=0;
        
        //determine beat in compound time
        if(currentPositionInfo.timeSigNumerator%3==0 && currentPositionInfo.timeSigNumerator!=3)
        {
            numOfBeats= currentPositionInfo.timeSigNumerator/3;
            
            if(currentPositionInfo.timeSigDenominator!=0)
            {
                //DBG(currentPositionInfo.ppqPosition);
                //DBG(samplePPQPosition);
                //DBG(currentPositionInfo.timeSigNumerator);
                //DBG(currentPositionInfo.timeSigDenominator);
                
            beat=(int((samplePPQPosition-currentPositionInfo.ppqPositionOfLastBarStart)/(12.0/currentPositionInfo.timeSigDenominator))%(currentPositionInfo.timeSigNumerator/3));
        
            }
            
        }
        
        //determine the beat in simple meter
        else if(currentPositionInfo.timeSigNumerator<=4)
        {
            numOfBeats=currentPositionInfo.timeSigNumerator;
            beat = (int((samplePPQPosition-currentPositionInfo.ppqPositionOfLastBarStart)/(4.0/currentPositionInfo.timeSigDenominator))%currentPositionInfo.timeSigNumerator);
        }
        
        if (m.isNoteOn())
        {
            DBG(beat);
            if(beat==0)
            {
                z=beatone;
            }
            else if(beat==1)
            {
                z=beattwo;
            }
            else if(beat==2)
            {
                z=beatthree;
            }
            else if(beat==3)
            {
                z=beatfour;
            }
            else
            {
                z=0;
            }
            
            //generate velocity randomization
            if(velocity>0)
            {
                randomVelocity= (rand()%(juce::uint8)velocity)-((juce::uint8)velocity/2);
            }
            
            else
            {
                randomVelocity=0;
            }
            
            //add beat emphasization (z) and randomization (x)
            juce::uint8 newVel = (juce::uint8)m.getVelocity() + randomVelocity + z;
            
            //avoid negative velocity values
            if (newVel<1)
            {
                newVel=1;
            }
            
            //create new MIDI note
            m = juce::MidiMessage::noteOn(m.getChannel(), m.getNoteNumber(), newVel);
            //z++;
            
            if(timing>0)
            {
            //randomize timing
            randomTiming=(((rand()%(juce::uint8)timing)-((juce::uint8)timing/2))*50);
            newTime=time+randomTiming+2500;
            }
            
            else
            {
                newTime=time+2500;
            }
            
            //if note is out of buffer range send it to outside buffer, if not output it.
            if(newTime>=buffer.getNumSamples())
            {
                backupBuffer1.addEvent(m,newTime-buffer.getNumSamples());
            }
         
            else
            {
            processedMidi.addEvent (m,newTime);
            }
        }
        else if (m.isNoteOff())
        {
            if((time+2500-(timing*25))>=buffer.getNumSamples())
            {
                //subtracts the necessary amount of samples to ensure noteOn never occurs before the previous noteOFF
                backupBuffer1.addEvent(m,time+2500-buffer.getNumSamples()-(timing*25));
            }
            
            else
            {
            processedMidi.addEvent (m, time+2500-(timing*25));
            }
        }
        else if (m.isAftertouch())
        {
            if((time+2500)>=buffer.getNumSamples())
            {
                backupBuffer1.addEvent(m,time+2500-buffer.getNumSamples());
            }
            
            else
            {
            processedMidi.addEvent (m, time+2500);
            }
        }
        else if (m.isPitchWheel())
        {
            if((time+2500)>=buffer.getNumSamples())
            {
                backupBuffer1.addEvent(m,time+2500-buffer.getNumSamples());
            }
            
            else
            {
            processedMidi.addEvent (m, time+2500);
            }
        }
        
        else
        {
            processedMidi.addEvent(m, time);
        }
        
    }

    //exchange the contents of the two MIDI Buffers
    midiMessages.swapWith (processedMidi);
}

//==============================================================================
bool MyFirstPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MyFirstPluginAudioProcessor::createEditor()
{
    return new MyFirstPluginAudioProcessorEditor (*this);
}

//==============================================================================
void MyFirstPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    auto state = parameters.copyState();
            std::unique_ptr<juce::XmlElement> xml (state.createXml());
            copyXmlToBinary (*xml, destData);
}

void MyFirstPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
     
            if (xmlState.get() != nullptr)
                if (xmlState->hasTagName (parameters.state.getType()))
                    parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyFirstPluginAudioProcessor();
}
