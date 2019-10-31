/*
  ==============================================================================

    NoiseGenerator.cpp
    Created: 9 Dec 2016 11:38:13am
    Author:  Copyright © 2017 Alessandro Ragano, Luis Germain Arango, Robin Whittle(http://www.firstpr.com.au/dsp/pink-noise/)
 
    VST PlugIn Technology by Steinberg Media Technologies
  ==============================================================================
*/
#include "../JuceLibraryCode/JuceHeader.h"


class NoiseGenerator {
private:
    Random* noise;
public:
    NoiseGenerator(){
        noise = new Random();
    }
    
    double getSampleWhiteNoise()
    {
        return noise->nextDouble()*0.25-0.125;
    }
    
    double getSamplePinkNoise()
    {
        LinearSmoothedValue<double> b0;
        LinearSmoothedValue<double> b1;
        LinearSmoothedValue<double> b2;
        LinearSmoothedValue<double> b3;
        LinearSmoothedValue<double> b4;
        LinearSmoothedValue<double> b5;
        LinearSmoothedValue<double> b6;
        LinearSmoothedValue<double> pink;

    
        b0.setValue(0.99886 * b0.getNextValue() + getSampleWhiteNoise() * 0.0555179);
        b1.setValue(0.99332 * b1.getNextValue() + getSampleWhiteNoise() * 0.0750759);
        b2.setValue (0.96900 * b2.getNextValue() + getSampleWhiteNoise()  * 0.1538520);
        b3.setValue(0.86650 * b3.getNextValue() + getSampleWhiteNoise()  * 0.3104856);
        b4.setValue(0.55000 * b4.getNextValue() + getSampleWhiteNoise()  * 0.5329522);
        b5.setValue(-0.7616 * b5.getNextValue() - getSampleWhiteNoise()  * 0.0168980);
        b6.setValue(getSampleWhiteNoise()  * 0.115926);
        pink.setValue(b0.getNextValue() + b1.getNextValue() + b2.getNextValue() + b3.getNextValue() + b4.getNextValue() + b5.getNextValue() + b6.getNextValue() + getSampleWhiteNoise()  * 0.5362);
        
        return pink.getNextValue();
    }
    

};
