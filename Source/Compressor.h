/*
  ==============================================================================

    Compressor.h
    Created: 23 Mar 2021 2:54:28pm
    Author:  fschl

  ==============================================================================
*/

#include "CircularBuffer.h"
#pragma once

class Compressor
{
public:
    Compressor();
    float compressSample(float data, float attack, float release);
private:
    CircularBuffer buffer;
    float tav, rms, gain;
};