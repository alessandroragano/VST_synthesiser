/*
  ==============================================================================

    Author:  Copyright © 2017 Alessandro Ragano, Luis Germain Arango
    
    VST PlugIn Technology by Steinberg Media Technologies
  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Oscillator.cpp"
#include "ADSR.cpp"

//==============================================================================
/**
*/
class SynthesizerAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    SynthesizerAudioProcessor();
    ~SynthesizerAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void setAdsr(double attackSeconds, double decaySeconds, double sustainLevel, double releaseSeconds, bool active);
    void createVoicesOsc1(float depthVibrato, float rateVibrato, float depthTremolo, float rateTremolo, PlayVoices::TypeVoice playVoice, PlayVoices::TypeVoice modulationVibrato, PlayVoices::TypeVoice moduationTremolo, bool vibrato, bool tremolo);
    void createVoicesOsc2(float depthVibrato, float rateVibrato, float depthTremolo, float rateTremolo, PlayVoices::TypeVoice playVoice, PlayVoices::TypeVoice modulationVibrato, PlayVoices::TypeVoice moduationTremolo, bool vibrato, bool tremolo);
    void setPlayVoice1(PlayVoices::TypeVoice);
    void setPlayVoice2(PlayVoices::TypeVoice);
    vector<PlayVoices*> getVoicesOsc1();
    vector<PlayVoices*> getVoicesOsc2();
    void setNumberOfVoices(int value);
    int getNumberOfVoices();
    Synthesiser getKeyboardState();
    void setSuffix(String value);
    bool checkSameVoices(PlayVoices::TypeVoice osc1,PlayVoices::TypeVoice osc2);
    void setEditorIsCreated(bool value);
    bool getIsPressed();
    int getNumSamplesBuffer();
    void setVolume(double value);
    void setAttack(int value);
    void setDecay(int value);
    void setSustain(int value);
    void setRelease(int value);
    
    
	MidiKeyboardState keyboardState;
private:
    // VOICES AND SOUNDS
    Synthesiser synthesiser1;
    Synthesiser synthesiser2;
    vector<PlayVoices*> voicesOsc1;
    vector<PlayVoices*> voicesOsc2;
    PlayVoices::TypeVoice playVoice1;
    PlayVoices::TypeVoice playVoice2;
    int numberOfVoices;
    VoicesEffectsSound* sound;
    bool editorIsCreated;
    
    // EFFECTS
    PlayVoices::TypeVoice modulationVibrato1;
    PlayVoices::TypeVoice modulationTremolo1;
    PlayVoices::TypeVoice modulationVibrato2;
    PlayVoices::TypeVoice modulationTremolo2;
    float depthVibrato1, rateVibrato1;
    float depthTremolo1, rateTremolo1;
    float depthVibrato2, rateVibrato2;
    float depthTremolo2, rateTremolo2;
    
    // ADSR
    Adsr* adsr;
    bool adsrIsActive;
    int attack;
    int decay;
    double sustain;
    int release;
    String suffix;
    int attackKnob;
    int decayKnob;
    int sustainKnob;
    int releaseKnob;
    
    // int midiNumber;
    bool isPressed;
    int numSamplesBuffer;
    
    // VOLUME
    double volume;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthesizerAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
