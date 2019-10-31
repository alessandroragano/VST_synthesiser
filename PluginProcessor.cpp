/*
  ==============================================================================

    Author:  Copyright © 2017 Alessandro Ragano, Luis Germain Arango
 
    VST PlugIn Technology by Steinberg Media Technologies
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================
SynthesizerAudioProcessor::SynthesizerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    editorIsCreated = false;
    sound = new VoicesEffectsSound();
}

SynthesizerAudioProcessor::~SynthesizerAudioProcessor()
{
}

//==============================================================================
const String SynthesizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SynthesizerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SynthesizerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double SynthesizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SynthesizerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SynthesizerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SynthesizerAudioProcessor::setCurrentProgram (int index)
{
}

const String SynthesizerAudioProcessor::getProgramName (int index)
{
    return String();
}

void SynthesizerAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SynthesizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synthesiser1.setCurrentPlaybackSampleRate(sampleRate);
    synthesiser2.setCurrentPlaybackSampleRate(sampleRate);
    keyboardState.reset();
}

void SynthesizerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    keyboardState.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SynthesizerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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


void SynthesizerAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    numSamplesBuffer = buffer.getNumSamples();
    float* channelLeft;
    float* channelRight;
    keyboardState.processNextMidiBuffer(midiMessages, 0, numSamplesBuffer, true);
    int midiNumber = 0;
    
    for(int i=0; i<16; i++){
        for (int k = 0; k< 128; k++){
            if(keyboardState.isNoteOnForChannels(i, k))
            {
                isPressed = true;
                break;
            }
            else
            {
                isPressed = false;
            }
        }
    }
    
    if(editorIsCreated)
    {
        if(checkSameVoices(getVoicesOsc1().at(0)->getCurrentVoice(), getVoicesOsc2().at(0)->getCurrentVoice()))
        {
            synthesiser1.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
            for(int i = 0; i<numberOfVoices; i++)
                getVoicesOsc1().at(i)->setMix(0.0);
            buffer.applyGain(volume);

        }
        
        else
        {
            synthesiser1.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
            synthesiser2.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
            buffer.applyGain(volume);
        }
        
    }
}

//==============================================================================
bool SynthesizerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SynthesizerAudioProcessor::createEditor()
{ 
    return new SynthesizerAudioProcessorEditor (*this);;
}

//==============================================================================
void SynthesizerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SynthesizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthesizerAudioProcessor();
}

void SynthesizerAudioProcessor::setAdsr(double attackSeconds, double decaySeconds, double sustainLevel, double releaseSeconds, bool active)
{
    int attackSamples = floor(attackSeconds*44100/1000);
    int decaySamples = decaySeconds*44100/1000;
    int releaseSamples = releaseSeconds*44100/1000;

    
    for (int i=0; i<voicesOsc1.size(); i++)
    {
        voicesOsc1.at(i)->getAdsr()->setNumberAttackSamples(attackSamples);
        voicesOsc1.at(i)->getAdsr()->setNumberDecaySamples(decaySamples);
        voicesOsc1.at(i)->getAdsr()->setLevelSustain(sustainLevel);
        voicesOsc1.at(i)->getAdsr()->setNumberReleaseSamples(releaseSamples);
    }
    for (int i=0; i<voicesOsc2.size(); i++)
    {
        voicesOsc2.at(i)->getAdsr()->setNumberAttackSamples(attackSamples);
        voicesOsc2.at(i)->getAdsr()->setNumberDecaySamples(decaySamples);
        voicesOsc2.at(i)->getAdsr()->setLevelSustain(sustainLevel);
        voicesOsc2.at(i)->getAdsr()->setNumberReleaseSamples(releaseSamples);
    }
}


void SynthesizerAudioProcessor::createVoicesOsc1(float depthVibrato, float rateVibrato, float depthTremolo, float rateTremolo, PlayVoices::TypeVoice playVoice, PlayVoices::TypeVoice modulationVibrato, PlayVoices::TypeVoice modulationTremolo, bool vibrato, bool tremolo)
{
    synthesiser1.clearVoices();
    synthesiser1.clearSounds();
    synthesiser1.addSound (sound);
    voicesOsc1.clear();
    
    for(int i=0; i<numberOfVoices; i++)
    {
        PlayVoices* voice = new PlayVoices(depthVibrato,rateVibrato,depthTremolo,rateTremolo,playVoice,modulationVibrato,modulationTremolo, vibrato, tremolo);
        voicesOsc1.push_back(voice);
        synthesiser1.addVoice(voice);
    }
}

void SynthesizerAudioProcessor::createVoicesOsc2(float depthVibrato, float rateVibrato, float depthTremolo, float rateTremolo, PlayVoices::TypeVoice playVoice, PlayVoices::TypeVoice modulationVibrato, PlayVoices::TypeVoice modulationTremolo, bool vibrato, bool tremolo)
{
    synthesiser2.clearVoices();
    synthesiser2.clearSounds();
    synthesiser2.addSound(sound);
    voicesOsc2.clear();
    
    for(int i=0; i<numberOfVoices; i++)
    {
        PlayVoices* voice = new PlayVoices(depthVibrato,rateVibrato,depthTremolo,rateTremolo,playVoice,modulationVibrato,modulationTremolo, vibrato, tremolo);
        voicesOsc2.push_back(voice);
        synthesiser2.addVoice(voice);
    }
}

void SynthesizerAudioProcessor::setPlayVoice1(PlayVoices::TypeVoice playVoice)
{
    playVoice1 = playVoice;
}
void SynthesizerAudioProcessor::setPlayVoice2(PlayVoices::TypeVoice playVoice)
{
    playVoice2 = playVoice;
}

vector<PlayVoices*> SynthesizerAudioProcessor::getVoicesOsc1()
{
    return voicesOsc1;
}
vector<PlayVoices*> SynthesizerAudioProcessor::getVoicesOsc2()
{
    return voicesOsc2;
}

void SynthesizerAudioProcessor::setNumberOfVoices(int value)
{
    numberOfVoices = value;
}

int SynthesizerAudioProcessor::getNumberOfVoices()
{
    return numberOfVoices;
}

bool SynthesizerAudioProcessor::checkSameVoices(PlayVoices::TypeVoice osc1,PlayVoices::TypeVoice osc2)
{
    if(osc1 == osc2)
        return true;
    else
        return false;
}

void SynthesizerAudioProcessor::setEditorIsCreated(bool value)
{
    editorIsCreated = value;
}

bool SynthesizerAudioProcessor::getIsPressed()
{
    return isPressed;
}

int SynthesizerAudioProcessor::getNumSamplesBuffer()
{
    return numSamplesBuffer;
}

void SynthesizerAudioProcessor::setVolume(double value)
{
    volume = value;
}

void SynthesizerAudioProcessor::setAttack(int value)
{
    attackKnob = value;
}

void SynthesizerAudioProcessor::setDecay(int value)
{
    decayKnob = value;
}

void SynthesizerAudioProcessor::setSustain(int value)
{
    sustainKnob = value;
}

void SynthesizerAudioProcessor::setRelease(int value)
{
    releaseKnob = value;
}

