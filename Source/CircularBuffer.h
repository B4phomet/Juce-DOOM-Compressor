/*
  ==============================================================================
    CircularBuffer.h
    Created: 2 Apr 2020 2:07:25pm
    Author:  Alex
  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class CircularBuffer
{
public:
    CircularBuffer();
    CircularBuffer(int bufferSize, int delayLength);
    float getData();
    void setData(float data);
    void nextSample();
private:
    juce::AudioSampleBuffer buffer;
    int writeIndex;
    int readIndex;
    int delayLength;
};