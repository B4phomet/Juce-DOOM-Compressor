/*
  ==============================================================================

    Compressor.cpp
    Created: 23 Mar 2021 2:54:28pm
    Author:  fschl

  ==============================================================================
*/

#include "Compressor.h"

Compressor::Compressor()
{
    buffer = CircularBuffer(150, 20);
    tav = 0.01;
    rms = 0;
    gain = 1;
}

float Compressor::compressSample(float data, float attack, float release)
{
    //calculate RMS values of input
    rms = (1 - tav) * rms + tav * std::pow(data, 2.0f);
    float dbRMS = 10 * std::log10(rms);

    //calculate the compressor gain
    float slope = 1 - (1 / INFINITY);
    float dbGain = std::min(0.0f, (slope * (0.0f - dbRMS)));
    float newGain = std::pow(10, dbGain / 20);
    
    //apply attack/release curve to comp gain
    float coeff;
    if (newGain < gain) coeff = attack;
    else coeff = release;
    gain = (1 - coeff) * gain + coeff * newGain;

    //compress the sample
    float compressedSample = gain * buffer.getData();
    buffer.setData(data);
    buffer.nextSample();
    return compressedSample;
}