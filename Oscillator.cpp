/*
 ==============================================================================
 
 Oscillator.cpp
 Created: 28 Nov 2016 12:16:04am
 Author:  Copyright © 2017 Alessandro Ragano, Luis Germain Arango
 
 VST PlugIn Technology by Steinberg Media Technologies
 ==============================================================================
 */

#ifndef OSCILLATORS_H_INCLUDED
#define OSCILLATORS_H_INCLUDED

using namespace std;
#include "../JuceLibraryCode/JuceHeader.h"
#include "ADSR.cpp"
#include "NoiseGenerator.cpp"
#include "IIRFilters.cpp"

/**
 Base class for oscillators
 */

class Oscillator : public SynthesiserVoice
{
public:
    Oscillator()
    {
        amplitude.reset (44100, 0.1);
        phaseIncrement.reset (44100, 0.1);
    }
    
    virtual ~Oscillator()
    {
    }
    
    
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int) override
    {
        frequency = MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        
        phaseIncrement.setValue (((2.0 * double_Pi) * frequency) / sampleRate);
        phaseIncrement2.setValue((2.0 * double_Pi * 200)/getSampleRate());
        
        amplitude.setValue (velocity);
                // Store the initial note and work out the maximum frequency deviations for pitch bend
        initialNote = midiNoteNumber;
        maxFreq = MidiMessage::getMidiNoteInHertz (initialNote + 4) - frequency;
        minFreq = frequency - MidiMessage::getMidiNoteInHertz (initialNote - 4);
        

        // Set adsr variables
        adsr->setIsNoteReleased(false);
        startRelease = false;
        adsr->setReleaseIsOver(false);
        prev_sample = 0;
        adsr->setAngularCoefficient();
        adsr->setIndexAdsr(1);
        
    }
    
    void stopNote (float, bool) override
    {
        if (adsr->getIsActive())
        {
            startRelease = true;
            adsr->setIndexRelease(1);
            adsr->setIsNoteReleased(true);
        }
        else
        {
            clearCurrentNote();
            amplitude.setValue(0.0);
        }
        
    }
    
    void pitchWheelMoved (int newValue) override
    {
        // Change the phase increment based on pitch bend amount
        double frequencyOffset = ((newValue > 0 ? maxFreq : minFreq) * (newValue / 127.0));
        phaseIncrement.setValue (((2.0 * double_Pi) * (frequency + frequencyOffset)) / sampleRate);
    }
    
    void controllerMoved (int, int) override
    {
    }
    
    void channelPressureChanged (int newChannelPressureValue) override
    {
        // Set the amplitude based on pressure value
        amplitude.setValue (newChannelPressureValue / 127.0);
    }
    
    
    void renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        int index = 0;
        
        while(--numSamples >= 0)
        {
            double output;
            output = getSample()*amplitude.getNextValue()*0.125f;
            
            for (int i = outputBuffer.getNumChannels(); --i >= 0;)
            {
                outputBuffer.addSample (i, startSample, static_cast<float> (output));
            }
            
            ++startSample;
            index++;
        }
        currentMix = mix.getNextValue();
        currentFrequencyControl = frequencyControl;
    }
    
    /** Returns the next sample */
    double getSample()
    {
        LinearSmoothedValue<double> output;
        LinearSmoothedValue<double> envelope;
        LinearSmoothedValue<double> totalOut;
        double sample;
        double maxSample;
        double incrementMix = (mix.getNextValue()-currentMix)/numSamplesBuffer;
        // double incrementFrequencyControl = (frequencyControl - currentFrequencyControl)/numSamplesBuffer;
        
        currentMix += incrementMix;
        
        sample = renderWaveShape (phasePos,frequency);
        maxSample = max(fabs(prev_sample),fabs(sample));
        prev_sample = sample;
        
        if(adsr->getIsActive() && !adsr->getReleaseIsOver())
            envelope.setValue(adsr->getSample(maxSample).getNextValue());
        
        if(adsr->getReleaseIsOver())
        {
            clearCurrentNote();
            amplitude.setValue(0.0);
            adsr->setIsNoteReleased(false);
        }
        
        vibratoIsActive == true ?
        phasePos += phaseIncrement.getNextValue() + getSampleVibrato()*2.0*double_Pi/sampleRate:
        phasePos += phaseIncrement.getNextValue();
        
        if (phasePos > (2.0 * double_Pi))
            phasePos -= (2.0 * double_Pi);
        
        tremoloIsActive == true ?
        output.setValue(sample*getSampleTremolo()):
        output.setValue(sample);
        
        if(isFilterActivated)
        {
            if(filterType == FilterType::HPF)
            {
               filters->highPassFilter(frequencyControl);
            }
            else if(filterType == FilterType::BPF)
            {
                filters->bandPassFilter(frequencyControl, qFactorControl);
            }
            else if(filterType == FilterType::LPF)
            {
                filters->lowPassFilter(frequencyControl);
            }
            
            output.setValue(filters->getFilter()->processSingleSampleRaw((float)output.getNextValue()));
        }
        
        if(adsr->getIsActive())
            totalOut.setValue(output.getNextValue()*envelope.getNextValue()*(0.1-fabs(mix.getNextValue())));
        else
            totalOut.setValue(output.getNextValue()*(0.1-fabs(mix.getNextValue())));
        
        return totalOut.getNextValue();
    
    }
    
    /** Subclasses should override this to say whether they can play the given sound */
    virtual bool canPlaySound (SynthesiserSound* sound) override = 0;
    /** Subclasses should override this to render a waveshape */
    virtual double renderWaveShape (const double currentPhase, double frequency) = 0;
    virtual double getSampleVibrato() = 0;
    virtual double getSampleTremolo() = 0;
    
    enum FilterType {HPF, BPF, LPF};

    
private:
    LinearSmoothedValue<double> phaseIncrement;
    LinearSmoothedValue<double> phaseIncrement2;
    double phasePos = 0.0f;
    double posPhase = 0.0f;
    double sampleRate = 44100.0;
    int initialNote;
    double maxFreq;
    double minFreq;
    bool startRelease;
    double prev_sample;
        
protected:
    double frequency;
    LinearSmoothedValue<double> amplitude;
    bool tremoloIsActive;
    bool vibratoIsActive;
    bool adsrIsActive;
    Adsr* adsr;
    LinearSmoothedValue<double> mix;
    IIRFilters* filters;
    bool isFilterActivated;
    FilterType filterType;
    double frequencyControl;
    double qFactorControl;
    double gainControl;
    double currentMix;
    double currentFrequencyControl;
    int numSamplesBuffer;
    double offset = 10;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oscillator)
};

//==============================================================================
/**
 Sine sound
 */

class SineSound : public SynthesiserSound
{
public:
    SineSound () {}
    
    bool appliesToNote (int) override { return true; }
    
    bool appliesToChannel (int ) override { return true; }
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SineSound)
};



/**
 Sine voice that renders a sin waveshape
 */
class SineVoice : public Oscillator
{
public:
    SineVoice() {};
    
    bool canPlaySound (SynthesiserSound* sound) override { return dynamic_cast<SineSound*> (sound) != nullptr; }
    
    double renderWaveShape (const double currentPhase, double frequency) override
    {
        return offset*sin (currentPhase);
    }
	virtual double getSampleVibrato() override { return 0; };
    virtual double getSampleTremolo() override { return 0; };
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SineVoice)
};

//==============================================================================
/**
 Square sound
 */
class SquareSound : public SynthesiserSound
{
public:
    SquareSound() {}
    
    bool appliesToNote (int) override { return true; }
    
    bool appliesToChannel (int) override { return true; }
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquareSound)
};


/**
 Square voice that renders a square waveshape
 */
class SquareVoice : public Oscillator
{
private:
    double norm1;
    double norm2;
    double norm3;
    double norm4;
    
public:
    SquareVoice()
    {
        norm1 = normalization(10);
        norm2 = normalization(7);
        norm3 = normalization(4);
        norm4 = normalization(2);
    }
    
    bool canPlaySound (SynthesiserSound* sound) override { return dynamic_cast<SquareSound*> (sound) != nullptr; }
    

    double renderWaveShape (const double currentPhase, double frequency) override {
        LinearSmoothedValue<double> currentSample;

        // cout << frequency << endl;
        if(frequency<=1000)
        {
           // return (currentPhase < double_Pi ? 0.0 : 1.0);
            currentSample.setValue(0.0);
            for (int j = 1; j<=10; ++j){
                currentSample.setValue(currentSample.getNextValue() + (sin((2*j-1)*currentPhase) * (4/double_Pi))/ (2*j-1));
            }
            return offset*currentSample.getNextValue()/norm1;
        }
        else if(frequency<=3000){
            currentSample.setValue(0.0);
            for (int j = 1; j<=7; ++j){
                currentSample.setValue(currentSample.getNextValue() + (sin((2*j-1)*currentPhase) * (4/double_Pi))/ (2*j-1));
            }
            return offset*currentSample.getNextValue()/norm2;
        }
        else if(frequency<=5000){
            currentSample.setValue(0.0);
            for (int j = 1; j<=4; ++j){
                currentSample.setValue(currentSample.getNextValue() + (sin((2*j-1)*currentPhase) * (4/double_Pi))/ (2*j-1));
            }
            return 5*currentSample.getNextValue()/norm3;
        }
        else if(frequency<=10000)
        {
            currentSample.setValue(0.0);
            for (int j = 1; j<=2; ++j){
                currentSample.setValue(currentSample.getNextValue() + (sin((2*j-1)*currentPhase) * (4/double_Pi))/ (2*j-1));
            }
            return offset*currentSample.getNextValue()/norm4;
        }
        else{
            return offset*sin(currentPhase);
        }
        
    
    }
    
    double normalization(int numHarm)
    {
        double factor = 0;
        
        for (int i = 1;i<numHarm; ++i)
        {
            factor = factor + (4/double_Pi)/ (2*i-1);
        }
        
        return factor;
    }
    virtual double getSampleVibrato() override { return 0; };
    virtual double getSampleTremolo() override { return 0; };
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquareVoice)
};

class SquareLfo : public Oscillator
{
private:
    LinearSmoothedValue<double> valueLfo;
public:
    SquareLfo()
    {
    }
    
    bool canPlaySound (SynthesiserSound* sound) override { return dynamic_cast<SquareSound*> (sound) != nullptr; }
    double renderWaveShape (const double currentPhase, double frequency) override {
        valueLfo.setValue(0.0);
        for (int j = 1; j<=4; ++j){
            valueLfo.setValue(valueLfo.getNextValue() + (sin((2*j-1)*currentPhase) * (4/double_Pi))/ (2*j-1));
        }
        return valueLfo.getNextValue();
        //return (currentPhase < double_Pi ? 0.0 : 1.0);
    }
    virtual double getSampleVibrato() override { return 0; };
    virtual double getSampleTremolo() override { return 0; };
};

//==============================================================================
/**
 Sawtooth sound
 */
class SawSound : public SynthesiserSound
{

public:
    SawSound()
    {
    }
    
    bool appliesToNote (int) override { return true; }
    
    bool appliesToChannel (int ) override { return true; }
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SawSound)
};

/**
 Sawtooth voice that renders a sawtooth waveshape
 */
class SawVoice : public Oscillator
{
private:
    double norm1;
    double norm2;
    double norm3;
    double norm4;
    
public:
    SawVoice()
    {
        norm1 = normalization(10);
        norm2 = normalization(7);
        norm3 = normalization(4);
        norm4 = normalization(2);
    }
    
    bool canPlaySound (SynthesiserSound* sound) override { return dynamic_cast<SawSound*> (sound) != nullptr; }
    double renderWaveShape (const double currentPhase, double frequency) override
    {
        // cout << frequency << endl;
        LinearSmoothedValue<double> currentSample;
        
        // cout << frequency << endl;
        if(frequency<=1000)
        {
            // return (currentPhase < double_Pi ? 0.0 : 1.0);
            currentSample.setValue(0.0);
            for (int j = 1; j<=10; ++j){
                currentSample.setValue(currentSample.getNextValue() + ((2/double_Pi)*sin(j*currentPhase))/j);
                ;
            }
            return offset*currentSample.getNextValue()/norm1;
        }
        else if(frequency<=3000){
            currentSample.setValue(0.0);
            for (int j = 1; j<=7; ++j){
                currentSample.setValue(currentSample.getNextValue() + ((2/double_Pi)*sin(j*currentPhase))/j);
            }
            return offset*currentSample.getNextValue()/norm2;
        }
        else if(frequency<=5000){
            currentSample.setValue(0.0);
            for (int j = 1; j<=4; ++j){
                currentSample.setValue(currentSample.getNextValue() + ((2/double_Pi)*sin(j*currentPhase))/j);
            }
            return offset*currentSample.getNextValue()/norm3;
        }
        else if(frequency<=10000)
        {
            currentSample.setValue(0.0);
            for (int j = 1; j<=2; ++j){
                currentSample.setValue(currentSample.getNextValue() + ((2/double_Pi)*sin(j*currentPhase))/j);
            }
            return offset*currentSample.getNextValue()/norm4;
        }
        else{
            return offset*sin(currentPhase);
        }


        //return (1.0 / double_Pi) * currentPhase - 1.0;
    
    }
    double normalization(int numHarm)
    {
        double factor = 0;
        
        for (int i = 1;i<numHarm; ++i)
        {
            factor = factor + (2/double_Pi)/i;
        }
        
        return factor;
    }
    virtual double getSampleVibrato() override { return 0; };
    virtual double getSampleTremolo() override { return 0; };
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SawVoice)
};

class SawLfo : public Oscillator
{
public:
    LinearSmoothedValue<double> value2;
    SawLfo()
    {
    }
    
    bool canPlaySound (SynthesiserSound* sound) override { return dynamic_cast<SawSound*> (sound) != nullptr; }
    double renderWaveShape (const double currentPhase, double frequency) override
    {
        value2.setValue(0.0);
        for (int j = 1; j<=4; ++j){
            value2.setValue((value2.getNextValue() + ((2/double_Pi)*sin(j*currentPhase))/j));
        }
        return value2.getNextValue();
        //value2.setValue((1.0 / double_Pi) * currentPhase - 1.0);
        // return value2.getNextValue();
    }
    virtual double getSampleVibrato() override { return 0; };
    virtual double getSampleTremolo() override { return 0; };
};

//==============================================================================
/**
 Triangle sound
 */


class TriangleSound : public SynthesiserSound
{
public:
    TriangleSound() {}
    
    bool appliesToNote (int) override { return true; }
    
    bool appliesToChannel (int midiChannel) override { return true; }
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TriangleSound)
    
};



/**
 Triangle voice that renders a triangle waveshape
 */
class TriangleVoice : public Oscillator
{
    
private:
    double norm1;
    double norm2;
    double norm3;
    double norm4;
    
public:
    TriangleVoice()
    {
        norm1 = normalization(10);
        norm2 = normalization(7);
        norm3 = normalization(4);
        norm4 = normalization(2);
    }
    
    bool canPlaySound (SynthesiserSound* sound) override { return dynamic_cast<TriangleSound*> (sound) != nullptr; }
    double renderWaveShape (const double currentPhase, double frequency) override
    {
        // return (currentPhase < double_Pi ? -1.0 + (2.0 / double_Pi) * currentPhase
        //        :  3.0 - (2.0 / double_Pi) * currentPhase);
        LinearSmoothedValue<double> currentSample;
        if(frequency<=1000)
        {
            // return (currentPhase < double_Pi ? 0.0 : 1.0);
            currentSample.setValue(0.0);
            for (int j = 0; j<10; ++j){
                currentSample.setValue(currentSample.getNextValue() + pow(-1,j)*sin ((2*j+1)*currentPhase) * (8/pow(double_Pi,2))/ pow((2*j+1),2));
            }
            return offset*currentSample.getNextValue()/norm1;
        }
        else if(frequency<=3000){
            currentSample.setValue(0.0);
            for (int j = 0; j<7; ++j){
                currentSample.setValue(currentSample.getNextValue() + pow(-1,j)*sin ((2*j+1)*currentPhase) * (8/pow(double_Pi,2))/ pow((2*j+1),2));
            }
            return offset*currentSample.getNextValue()/norm2;
        }
        else if(frequency<=5000){
            currentSample.setValue(0.0);
            for (int j = 0; j<4; ++j){
                currentSample.setValue(currentSample.getNextValue() + pow(-1,j)*sin ((2*j+1)*currentPhase) * (8/pow(double_Pi,2))/ pow((2*j+1),2));
            }
            return offset*currentSample.getNextValue()/norm3;
        }
        else if(frequency<=10000)
        {
            currentSample.setValue(0.0);
            for (int j = 0; j<2; ++j){
                currentSample.setValue(currentSample.getNextValue() + pow(-1,j)*sin ((2*j+1)*currentPhase) * (8/pow(double_Pi,2))/ pow((2*j+1),2));
            }
            return offset*currentSample.getNextValue()/norm4;
        }
        else{
            return offset*sin(currentPhase);
        }

        
        
    }
    
    double normalization(int numHarm)
    {
        double factor = 0;
        
        for (int i = 0;i<numHarm; ++i)
        {
            factor = factor + pow(-1,i)*(8/pow(double_Pi,2))/ pow((2*i+1),2);
        }
        
        return factor;
    }
    virtual double getSampleVibrato() override { return 0; };
    virtual double getSampleTremolo() override { return 0; };
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TriangleVoice)
};

class TriangleLfo : public Oscillator
{
public:
    LinearSmoothedValue<double> value;
    TriangleLfo()
    {
    }
    
    bool canPlaySound (SynthesiserSound* sound) override { return dynamic_cast<TriangleSound*> (sound) != nullptr; }
    double renderWaveShape (const double currentPhase, double frequency) override
    {
        value.setValue((currentPhase < double_Pi ? -1.0 + (2.0 / double_Pi) * currentPhase:  3.0 - (2.0 / double_Pi) * currentPhase));
        return value.getNextValue();
    }
    virtual double getSampleVibrato() override { return 0; };
    virtual double getSampleTremolo() override { return 0; };
    
};

class VoicesEffectsSound : public SynthesiserSound
{
public:
    VoicesEffectsSound() {}
    
    bool appliesToNote (int) override { return true; }
    
    bool appliesToChannel (int midiChannel) override { return true; }
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoicesEffectsSound)
    
};

/**
 Play Voices. Effects include: tremolo, vibrato.
 */

class PlayVoices: public Oscillator
{
public:
    enum TypeVoice {SINE, SQUARE, SAW, TRIANGLE, WHITE_NOISE, PINK_NOISE};

private:
    float depthVibrato;
    float rateVibrato;
    float depthTremolo;
    float rateTremolo;
    float posPhaseVibrato;
    float posPhaseTremolo;
    LinearSmoothedValue<float> phaseIncrementVibrato;
    LinearSmoothedValue<float> phaseIncrementTremolo;
    SineVoice* sine;
    SquareVoice* square;
    SquareLfo* squareLfo;
    SawVoice* saw;
    SawLfo* sawLfo;
    TriangleVoice* triangle;
    TriangleLfo* triangleLfo;
    TypeVoice playVoice;
    TypeVoice modulationVibrato;
    TypeVoice modulationTremolo;
    NoiseGenerator* noise;
    
public:
    PlayVoices(float depthVibrato, float rateVibrato, float depthTremolo, float rateTremolo, TypeVoice playVoice, TypeVoice modulationVibrato, TypeVoice modulationTremolo, bool vibrato, bool tremolo)
    {
        this->depthVibrato = depthVibrato;
        this->rateVibrato = rateVibrato;
        this->depthTremolo = depthTremolo;
        this->rateTremolo = rateTremolo;
        this->playVoice = playVoice;
        this-> modulationVibrato = modulationVibrato;
        this-> modulationTremolo = modulationTremolo;
        posPhaseVibrato = 0.0f;
        posPhaseTremolo = 0.0f;
        phaseIncrementVibrato.setValue((2.0 * double_Pi * rateVibrato)/44100);
        phaseIncrementTremolo.setValue((2.0 * double_Pi * rateTremolo)/44100);
        sine = new SineVoice();
        square = new SquareVoice();
        squareLfo = new SquareLfo();
        saw = new SawVoice();
        sawLfo = new SawLfo();
        triangle = new TriangleVoice();
        triangleLfo = new TriangleLfo();
        this->vibratoIsActive = vibrato;
        this->tremoloIsActive = tremolo;
        noise = new NoiseGenerator();
        mix.setValue(0.0);
        isFilterActivated = false;
    }
    
    bool canPlaySound (SynthesiserSound* sound) override { return dynamic_cast<VoicesEffectsSound*> (sound) != nullptr; }
    
    
    double renderWaveShape (const double currentPhase,double frequency) override
    {
        switch(playVoice)
        {
            case SINE:
                return sine->renderWaveShape(currentPhase,frequency);
                break;
            case SQUARE:
                return square->renderWaveShape(currentPhase,frequency);
                break;
            case SAW:
                return saw->renderWaveShape(currentPhase,frequency);
                break;
            case TRIANGLE:
                return triangle->renderWaveShape(currentPhase,frequency);
                break;
            case WHITE_NOISE:
                return noise->getSampleWhiteNoise();
                break;
            case PINK_NOISE:
                return noise->getSamplePinkNoise();
                break;
        }
    }
    
    double getSampleVibrato() override
    {
        double output;
        
        switch(modulationVibrato)
        {
            case SINE:
                output = sine->renderWaveShape(posPhaseVibrato,frequency);
                break;
            case SQUARE:
                output = squareLfo->renderWaveShape(posPhaseVibrato,frequency);
                break;
            case SAW:
                output = sawLfo->renderWaveShape(posPhaseVibrato,frequency);
                break;
            case TRIANGLE:
                output = triangleLfo->renderWaveShape(posPhaseVibrato,frequency);
                break;
        }
        
        posPhaseVibrato += phaseIncrementVibrato.getNextValue();
        
        if(posPhaseVibrato>= 2.0*double_Pi)
            posPhaseVibrato -= 2.0*double_Pi;
        
        return output*depthVibrato;
    }
    
    double getSampleTremolo() override
    {
        double output;
        
        switch(modulationTremolo)
        {
            case SINE:
                output = sine->renderWaveShape(posPhaseTremolo,frequency);
                break;
            case SQUARE:
                output = squareLfo->renderWaveShape(posPhaseTremolo,frequency);
                break;
            case SAW:
                output = sawLfo->renderWaveShape(posPhaseTremolo,frequency);
                break;
            case TRIANGLE:
                output = triangleLfo->renderWaveShape(posPhaseTremolo,frequency);
                break;
        }
        
        posPhaseTremolo += phaseIncrementTremolo.getNextValue();
        
        if(posPhaseTremolo>= 2.0*double_Pi)
            posPhaseTremolo -= 2.0*double_Pi;
        
        return (1-depthTremolo)+depthTremolo*output;
    }
    
    
    void setAdsr(bool isActive)
    {
        this->adsr->setIsActive(isActive);
    }
    
    void setDepthVibrato(float value)
    {
        depthVibrato = value;
    }
    void setRateVibrato(float value)
    {
        rateVibrato = value;
        phaseIncrementVibrato.setValue((2.0 * double_Pi * rateVibrato)/44100);
    }
    void setDepthTremolo(float value)
    {
        depthTremolo = value;
    }
    void setRateTremolo(float value)
    {
        rateTremolo = value;
        phaseIncrementTremolo.setValue((2.0 * double_Pi * rateTremolo)/44100);
    }
    void setModulationVibrato(TypeVoice modulationType)
    {
        modulationVibrato = modulationType;
    }
    void setModulationTremolo(TypeVoice modulationType)
    {
        modulationTremolo = modulationType;
    }
    void setVibratoIsActive(bool value)
    {
        vibratoIsActive = value;
    }
    void setTremoloIsActive(bool value)
    {
        tremoloIsActive = value;
    }
    
    Adsr* getAdsr()
    {
        return adsr;
    }
    
    void setMix(double value)
    {
        mix.setValue(value);
    }
    
    void constructAdsr(double attack, double decay, double sustain, double release, bool isNoteReleased)
    {
        int attackSamples = attack*44100/1000;
        int releaseSamples = release*44100/1000;
        int decaySamples = decay*44100/1000;
        adsr = new Adsr(attackSamples,decaySamples,sustain,releaseSamples,isNoteReleased);
    }
    
    PlayVoices::TypeVoice getCurrentVoice()
    {
        return this->playVoice;
    }
    
    void setAmplitude(double value)
    {
        amplitude.setValue(value);
    }
    
    void setFilterType(FilterType type)
    {
        filterType = type;
    }
    
    void setGain(double value)
    {
        gainControl = value;
    }
    
    void setQFactor(double value)
    {
        qFactorControl = value;
    }
    
    void setFrequency(double value)
    {
        frequencyControl = value;
    }
    
    void turnOnFilter(bool value)
    {
        isFilterActivated = value;
    }
    
    void createFilters()
    {
        filters = new IIRFilters();
    }
    
    double getMixLevel()
    {
        return mix.getNextValue();
    }
    
    void setNumSamplesBuffer(int numSamplesBuffer)
    {
        this->numSamplesBuffer = numSamplesBuffer;
    }
    
    void setCurrentMix(double value)
    {
        this->currentMix = value;
    }

};



#endif  // OSCILLATORS_H_INCLUDED
