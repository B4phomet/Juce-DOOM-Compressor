/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DOOMCompressorAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                            public juce::Slider::Listener,
                                            public juce::Button::Listener
{
public:
    DOOMCompressorAudioProcessorEditor (DOOMCompressorAudioProcessor&);
    ~DOOMCompressorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

private:
    juce::TextButton tbMultibandOnOff;
    juce::Slider rsCompLow, rsCompHigh, rsAtk, rsRls, rsGain, sGate;
    juce::DrawableText tDoom, tComp, tBaph, tLow, tAtk, tRls, tHigh, tGain, tGate;

    DOOMCompressorAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DOOMCompressorAudioProcessorEditor)
};
