/*
  ==============================================================================

    IIRFilters.cpp
    Created: 8 Dec 2016 7:49:52pm
    Author:  Copyright © 2017 Alessandro Ragano, Luis Germain Arango
 
    VST PlugIn Technology by Steinberg Media Technologies
  ==============================================================================
*/

#include <stdio.h>
#include "../JuceLibraryCode/JuceHeader.h"
#pragma once

using namespace std;

class IIRFilters : public IIRFilter, public IIRCoefficients
{
private:
    IIRFilter* filter;
    IIRCoefficients coefficients;
    
public:
    IIRFilters ()
    {
        filter = new IIRFilter();
        coefficients = *new IIRCoefficients();
    }
    
    ~IIRFilters()
    {
        delete filter;
    }

    void lowPassFilter(double fCutOff)
    {
        coefficients = IIRCoefficients::makeLowPass(44100, fCutOff);
        filter->setCoefficients(coefficients);
    }
    
    void highPassFilter(double fCutOff)
    {
        coefficients = IIRCoefficients::makeHighPass(44100, fCutOff);
        filter->setCoefficients(coefficients);
    }
    
    void bandPassFilter(double fCentral, double qFactor)
    {
        coefficients = IIRCoefficients::makeBandPass(44100, fCentral, qFactor);
        filter->setCoefficients(coefficients);
    }
    
    IIRFilter* getFilter()
    {
        return filter;
    }
};

