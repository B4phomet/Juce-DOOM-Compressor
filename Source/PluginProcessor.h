/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Compressor.h"

//==============================================================================
/**
*/
class DOOMCompressorAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DOOMCompressorAudioProcessor();
    ~DOOMCompressorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //here begins my code
    bool multibandOff{ true };

    //filters for multiband
    juce::IIRFilter lowBandFilterL1, lowBandFilterR1, lowBandFilterL2, lowBandFilterR2;
    juce::IIRFilter lowMidBandFilterL1, lowMidBandFilterR1, lowMidBandFilterL2, lowMidBandFilterR2;
    juce::IIRFilter highMidBandFilterL1, highMidBandFilterR1, highMidBandFilterL2, highMidBandFilterR2;
    juce::IIRFilter highBandFilterL1, highBandFilterR1, highBandFilterL2, highBandFilterR2;
    juce::IIRCoefficients coefficients;

    int numChannels;

    float cGate{ 0.00000001f };
    float addDB{ 200 };
    float vGain{ 1.0 };

    float cLow{ 200 };
    float cHigh{ 10000 };

    float cAtk{ 0.1 };
    float at;
    float cRls{ 0.1 };
    float rt;
    float ratio{ INFINITY };

    double makeup, previousMakeup;

private:
    juce::Array<Compressor> allCompressors;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DOOMCompressorAudioProcessor)
};
