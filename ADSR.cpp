/*
 ==============================================================================
 
 ADSR.cpp
 Created: 30 Nov 2016 5:47:18pm
 Author:  Copyright © 2017 Alessandro Ragano, Luis Germain Arango 
 
 VST PlugIn Technology by Steinberg Media Technologies
 ==============================================================================
 */

#include "../JuceLibraryCode/JuceHeader.h"
#pragma once
using namespace std;

class Adsr{
    
private:
    int numberAttackSamples;
    int numberDecaySamples;
    double levelSustain;
    int numberReleaseSamples;
    bool isNoteReleased;
    LinearSmoothedValue<double> angularCoefficient;
    LinearSmoothedValue<int> indexIncrement;
    LinearSmoothedValue<int> indexSample;
    LinearSmoothedValue<int> indexRelease;
    bool releaseIsOver;
    bool isActive;
    LinearSmoothedValue<double> sample;
    LinearSmoothedValue<double> outputRelease;
    LinearSmoothedValue<double> lastSample;
    bool isDecay;
    bool isAttack;
    bool check;
    double angularCoefficient2;
    LinearSmoothedValue<double> output;
    LinearSmoothedValue<double> pastSample;
    
    
public:
    Adsr()
    {
        indexIncrement.setValue(1);
        indexSample.setValue(1);
        indexRelease.setValue(1);
        angularCoefficient.setValue(0.0);
        this->releaseIsOver = false;
        isDecay = false;
        isAttack = false;
        check = false;
        output.setValue(0.0);
        pastSample.setValue(0.0);
    }
    Adsr(int numberAttackSamples, int numberDecaySamples, double levelSustain, int numberReleaseSamples, bool isNoteReleased)
    {
        this->numberAttackSamples = numberAttackSamples;
        this->numberDecaySamples = numberDecaySamples;
        this->levelSustain = levelSustain;
        this->numberReleaseSamples = numberReleaseSamples;
        this->isNoteReleased = isNoteReleased;
        indexIncrement.setValue(1);
        indexSample.setValue(1);
        indexRelease.setValue(1);
        angularCoefficient.setValue(0.0);
        this->releaseIsOver = false;
        isDecay = false;
        isAttack = false;
        check = false;
        this->isActive = false;
        output.setValue(0.0);
        pastSample.setValue(0.0);
    }
    
    
    
    LinearSmoothedValue<double> getSample(double sample)
    {
        this->sample.setValue(sample);
        //if(sample > 1)
        //    this->sample = 1;
        // ATTACK
        output.setValue(0.0);
        if(!isNoteReleased)
        {
            if(indexSample.getNextValue()<=numberAttackSamples)
            {
                angularCoefficient.setValue(this->sample.getNextValue()/numberAttackSamples);
                output.setValue(angularCoefficient.getNextValue()*indexSample.getNextValue());
                lastSample.setValue(indexSample.getNextValue());
                isAttack  = true;
            }
            
            // DECAY
            else if(indexSample.getNextValue()>=numberAttackSamples && indexSample.getNextValue()<=numberDecaySamples+numberAttackSamples)
            {
                angularCoefficient.setValue(((levelSustain*this->sample.getNextValue())-this->sample.getNextValue())/(numberDecaySamples));                
                output.setValue(this->sample.getNextValue() + angularCoefficient.getNextValue()*(indexSample.getNextValue()-numberAttackSamples));
                lastSample.setValue(levelSustain/(numberDecaySamples)*(indexSample.getNextValue()-numberAttackSamples));
                isAttack = false;
                isDecay = true;
            }
            
            // SUSTAIN
            else if(indexSample.getNextValue()>=numberDecaySamples+numberAttackSamples)
            {
                isDecay = false;
                isAttack = false;
                output.setValue(levelSustain*this->sample.getNextValue());
                lastSample.setValue(levelSustain);
            }
        }
        // RELEASE
        else
        {
            if(isAttack)
            {
                LinearSmoothedValue<double> y;
                LinearSmoothedValue<double> x;
                
                y.setValue(lastSample.getNextValue()*this->sample.getNextValue()/numberAttackSamples);
                x.setValue(numberReleaseSamples);
                angularCoefficient.setValue(-y.getNextValue()/x.getNextValue());
                output.setValue(y.getNextValue() + angularCoefficient.getNextValue()*indexRelease.getNextValue());
            }
            
            else if (isDecay)
            {
                LinearSmoothedValue<double> y;
                LinearSmoothedValue<double> x;
                y.setValue(this->sample.getNextValue() + lastSample.getNextValue()*this->sample.getNextValue()*(1-1/levelSustain));
                x.setValue(numberReleaseSamples);
                angularCoefficient.setValue(-y.getNextValue()/x.getNextValue());
                output.setValue(y.getNextValue() + angularCoefficient.getNextValue()*indexRelease.getNextValue());
            }
            else
            {
                angularCoefficient.setValue(-lastSample.getNextValue()*this->sample.getNextValue()/numberReleaseSamples);
                output.setValue(lastSample.getNextValue()*this->sample.getNextValue() + angularCoefficient.getNextValue()*indexRelease.getNextValue());
            }
            
            indexRelease.setValue(indexRelease.getNextValue()+ indexIncrement.getNextValue());
            if(indexRelease.getNextValue()==numberReleaseSamples){
                releaseIsOver = true;
            }
            if(angularCoefficient.getNextValue()>0)
            {
                output.setValue(0.0);
            }
        }
        if(!isNoteReleased)
        indexSample.setValue(indexSample.getNextValue() + indexIncrement.getNextValue());
        //if(output.getNextValue()>1)
        //    output.setValue(1.0);
        return output.getNextValue();
        
    }
    
    void setIsNoteReleased(bool released)
    {
        this->isNoteReleased = released;
    }
    
    bool getIsNoteReleased()
    {
        return this->isNoteReleased;
    }
    void setIndexAdsr(int index)
    {
        indexSample.setValue(index);
    }
    
    void setIsActive (bool isActive)
    {
        this->isActive = isActive;
    }
    
    bool getIsActive ()
    {
        return this->isActive;
    }
    
    bool getReleaseIsOver ()
    {
        return this->releaseIsOver;
    }
    
    int getNumberReleaseSamples ()
    {
        return this->numberReleaseSamples;
    }
    
    double getAmpRelease()
    {
        return levelSustain;
    }
    
    void setIndexRelease(int value)
    {
        indexRelease.setValue(value);
    }
    
    void setReleaseIsOver(bool releaseIsOver)
    {
        this->releaseIsOver = releaseIsOver;
    }
    
    void setNumberAttackSamples(int value)
    {
        numberAttackSamples = value;
    }
    
    void setNumberDecaySamples(int value)
    {
        numberDecaySamples = value;
    }
    
    void setLevelSustain(double value)
    {
        levelSustain = value;
    }
    
    void setNumberReleaseSamples(int value)
    {
        numberReleaseSamples = value;
    }
    
    void defaultAdsr()
    {
        this->numberAttackSamples = 1*44100/1000;
        this->numberDecaySamples = 1*44100/1000;
        this->levelSustain = 1;
        this->numberReleaseSamples = 1*44100/1000;
    }
    
    void setAngularCoefficient()
    {
        angularCoefficient = 0;
    }
};

