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
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    parameters(*this,nullptr,juce::Identifier ("RTHumanizer"),
{
    std::make_unique<juce::AudioParameterInt> ("velocity",            // parameterID
                                                 "Velocity",            // parameter name
                                                 1,              // minimum value
                                                 100,              // maximum value
                                                 1),             // default value
    std::make_unique<juce::AudioParameterInt> ("timing",            // parameterID
                                                 "Timing",            // parameter name
                                                 1,              // minimum value
                                                 100,              // maximum value
                                                 1),             // default value
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
    juce::MidiMessage m,n;
    float BPMInSamples= (60/currentPositionInfo.bpm)*getSampleRate();
    timing=*timingParameter;
    velocity=*velocityParameter;
    beatone=*beatOneParameter;
    beattwo=*beatTwoParameter;
    beatthree=*beatThreeParameter;
    beatfour=*beatFourParameter;
    
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
    
    //iterate through current buffer
    for (juce::MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        //get current beat
        playHead = this->getPlayHead();
        playHead->getCurrentPosition (currentPositionInfo);
        
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
            
            /*if(currentPositionInfo.timeSigDenominator==8)
            {
            beat=(int((samplePPQPosition-currentPositionInfo.ppqPositionOfLastBarStart)/1.5))%(currentPositionInfo.timeSigNumerator/3);
            }
            
            if(currentPositionInfo.timeSigDenominator==4)
            {
            beat=(int((samplePPQPosition-currentPositionInfo.ppqPositionOfLastBarStart)/3))%(currentPositionInfo.timeSigNumerator/3);
            }
            
            if(currentPositionInfo.timeSigDenominator==16)
            {
                beat=(int((samplePPQPosition-currentPositionInfo.ppqPositionOfLastBarStart)/.75))%(currentPositionInfo.timeSigNumerator/3);
            }*/
            

        }
        
        //determine the beat in simple meter
        else if(currentPositionInfo.timeSigNumerator<=4)
        {
            numOfBeats=currentPositionInfo.timeSigNumerator;
            beat = (int((samplePPQPosition-currentPositionInfo.ppqPositionOfLastBarStart)/(4.0/currentPositionInfo.timeSigDenominator))%currentPositionInfo.timeSigNumerator);
        }


        
        /*else
        {
            z=0;
        }*/
        
        if (m.isNoteOn())
        {
            
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
            int x= (rand()%(juce::uint8)velocity)-((juce::uint8)velocity/2);
            
            //add beat emphasization (z) and randomization (x)
            juce::uint8 newVel = (juce::uint8)m.getVelocity() + x + z;
            
            //avoid negative velocity values
            if (newVel<1)
            {
                newVel=1;
            }
            
            //create new MIDI note
            m = juce::MidiMessage::noteOn(m.getChannel(), m.getNoteNumber(), newVel);
            //z++;
            
            //randomize timing
            int y=(((rand()%(juce::uint8)timing)-((juce::uint8)timing/2))*50)+2500;
            newTime=time+y;
            
            if(newTime>=buffer.getNumSamples())
            {
                backupBuffer1.addEvent(m,newTime-buffer.getNumSamples());
            }
            
            /*else if(newTime<1)
            {
                processedMidi.addEvent (m,1);
            }*/
         
            else
            {
            processedMidi.addEvent (m,newTime);
            }
            //DBG(newTime);
            /*else if (newTime>buffer.getNumSamples())
            {
                newTime=buffer.getNumSamples()-1;
            }*/
        }
        else if (m.isNoteOff())
        {
            processedMidi.addEvent (m, time);
        }
        else if (m.isAftertouch())
        {
            processedMidi.addEvent (m, time);
        }
        else if (m.isPitchWheel())
        {
            processedMidi.addEvent (m, time);
        }
        
        
        
 
        //play MIDI note
        //processedMidi.addEvent (m, time);
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
