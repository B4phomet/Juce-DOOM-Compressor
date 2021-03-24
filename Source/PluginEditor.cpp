/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DOOMCompressorAudioProcessorEditor::DOOMCompressorAudioProcessorEditor (DOOMCompressorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    //low band cutoff slider
    rsCompLow.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    rsCompLow.setRange(0, 20000, 1);
    rsCompLow.setValue(audioProcessor.cLow);
    rsCompLow.setDoubleClickReturnValue(true, 200);
    rsCompLow.addListener(this);
    addAndMakeVisible(rsCompLow);

    tLow.setColour(juce::Colour::fromRGB(219, 219, 219));
    tLow.setFontHeight(13);
    tLow.setText("Low Crossover");
    addAndMakeVisible(tLow);

    //high band cutoff slider
    rsCompHigh.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    rsCompHigh.setRange(0, 20000, 1);
    rsCompHigh.setValue(audioProcessor.cHigh);
    rsCompHigh.setDoubleClickReturnValue(true, 10000);
    rsCompHigh.addListener(this);
    addAndMakeVisible(rsCompHigh);

    tHigh.setColour(juce::Colour::fromRGB(219, 219, 219));
    tHigh.setFontHeight(13);
    tHigh.setText("High Crossover");
    addAndMakeVisible(tHigh);

    //multiband switch
    tbMultibandOnOff.setButtonText("Multiband Off");
    tbMultibandOnOff.setClickingTogglesState(true);
    tbMultibandOnOff.setToggleState(!audioProcessor.multibandOff, juce::sendNotification);
    tbMultibandOnOff.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(250, 131, 41));
    tbMultibandOnOff.addListener(this);
    addAndMakeVisible(tbMultibandOnOff);

    //attack slider
    rsAtk.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    rsAtk.setRange(0.0f, 10.0f, 0.01f);
    rsAtk.setValue(audioProcessor.cAtk);
    rsAtk.setDoubleClickReturnValue(true, 0.1f);
    rsAtk.addListener(this);
    addAndMakeVisible(rsAtk);

    tAtk.setColour(juce::Colour::fromRGB(219, 219, 219));
    tAtk.setFontHeight(13);
    tAtk.setText("Attack");
    addAndMakeVisible(tAtk);

    //release slider
    rsRls.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    rsRls.setRange(0.0f, 10.0f, 0.01f);
    rsRls.setValue(audioProcessor.cRls);
    rsRls.setDoubleClickReturnValue(true, 0.1f);
    rsRls.addListener(this);
    addAndMakeVisible(rsRls);

    tRls.setColour(juce::Colour::fromRGB(219, 219, 219));
    tRls.setFontHeight(13);
    tRls.setText("Release");
    addAndMakeVisible(tRls);

    //some text for aesthetics
    tDoom.setColour(juce::Colour::fromRGB(250, 131, 41));
    tDoom.setFontHeight(40);
    tDoom.setFontHorizontalScale(1.1f);
    tDoom.setText("DOOM");
    addAndMakeVisible(tDoom);

    tComp.setColour(juce::Colour::fromRGB(219, 219, 219));
    tComp.setFontHeight(20);
    tComp.setText("Compressor");
    addAndMakeVisible(tComp);

    tBaph.setColour(juce::Colour::fromRGB(180, 180, 180));
    tBaph.setFontHeight(15);
    tBaph.setText("by Baphomet");
    addAndMakeVisible(tBaph);

    //output Gain Slider
    rsGain.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    rsGain.setRange(0, 100, 1);
    rsGain.setValue(audioProcessor.vGain);
    rsGain.setDoubleClickReturnValue(true, 100);
    rsGain.addListener(this);
    addAndMakeVisible(rsGain);

    tGain.setColour(juce::Colour::fromRGB(219, 219, 219));
    tGain.setFontHeight(13);
    tGain.setText("Output");
    addAndMakeVisible(tGain);

    //threshold slider
    sGate.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    sGate.setRange(-200, 0, 1);
    sGate.setValue(-audioProcessor.addDB);
    sGate.setDoubleClickReturnValue(true, -200);
    sGate.addListener(this);
    addAndMakeVisible(sGate);

    tGate.setColour(juce::Colour::fromRGB(219, 219, 219));
    tGate.setFontHeight(13);
    tGate.setText("Threshold");
    addAndMakeVisible(tGate);

    setSize(290, 225);
}

DOOMCompressorAudioProcessorEditor::~DOOMCompressorAudioProcessorEditor()
{

}

//==============================================================================
void DOOMCompressorAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void DOOMCompressorAudioProcessorEditor::resized()
{
    //low band cutoff slider
    rsCompLow.setBounds(5, 0, 80, 80);
    rsCompLow.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);

    tLow.setBoundingBox(juce::Parallelogram<float>(juce::Rectangle<float>(rsCompLow.getX(), rsCompLow.getY() + rsCompLow.getHeight() + 5, 80, 13)));

    //high band cutoff slider
    rsCompHigh.setBounds(rsCompLow.getX() + rsCompLow.getWidth() + 10, rsCompLow.getY(), rsCompLow.getWidth(), rsCompLow.getHeight());
    rsCompHigh.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);

    tHigh.setBoundingBox(juce::Parallelogram<float>(juce::Rectangle<float>(rsCompHigh.getX(), rsCompHigh.getY() + rsCompHigh.getHeight() + 5, 80, 13)));

    //multiband switch
    tbMultibandOnOff.setBounds(tHigh.getX() - 50, tLow.getY() + tLow.getHeight() + 7, 90, 20);

    //attack slider
    rsAtk.setBounds(rsCompLow.getX(), tbMultibandOnOff.getY() + tbMultibandOnOff.getHeight(), rsCompLow.getWidth(), rsCompLow.getHeight());
    rsAtk.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);

    tAtk.setBoundingBox(juce::Parallelogram<float>(juce::Rectangle<float>(rsAtk.getX() + 25, rsAtk.getY() + rsAtk.getHeight() + 5, 70, 13)));

    //release slider
    rsRls.setBounds(rsCompHigh.getX(), tbMultibandOnOff.getY() + tbMultibandOnOff.getHeight(), rsCompLow.getWidth(), rsCompLow.getHeight());
    rsRls.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);

    tRls.setBoundingBox(juce::Parallelogram<float>(juce::Rectangle<float>(rsRls.getX() + 20, rsRls.getY() + rsRls.getHeight() + 5, 70, 13)));

    //text for aesthetics
    tDoom.setBoundingBox(juce::Parallelogram<float>(juce::Rectangle<float>(190, 0, 90, 40)));
    
    tComp.setBoundingBox(juce::Parallelogram<float>(juce::Rectangle<float>(tDoom.getX(), tDoom.getY() + tDoom.getHeight() - 5, 90, 20)));

    tBaph.setBoundingBox(juce::Parallelogram<float>(juce::Rectangle<float>(tDoom.getX() + 5, tComp.getY() + tComp.getHeight(), 80, 15)));

    //output gain slider
    rsGain.setBounds(tDoom.getX() + 5, tBaph.getY() + tBaph.getHeight() + 15, 80, 80);
    rsGain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);

    tGain.setBoundingBox(juce::Parallelogram<float>(juce::Rectangle<float>(rsGain.getX() + 22, rsGain.getY() + rsGain.getHeight() + 5, 40, 13)));

    //threshold slider
    sGate.setBounds(rsGain.getX() + 52, tGain.getY() + tGain.getHeight() + 20, 40, 20);

    tGate.setBoundingBox(juce::Parallelogram<float>(juce::Rectangle<float>(rsGain.getX() - 2, sGate.getY() + 3, 50, 13)));
}

void DOOMCompressorAudioProcessorEditor::buttonClicked(juce::Button *button)
{
    //set multiband switch text according to multiband state and change the corresponding bool in the Processor class
    if (button == &tbMultibandOnOff)
    {
        if (tbMultibandOnOff.getToggleState() == false)
        {
            tbMultibandOnOff.setButtonText("Multiband On");
            audioProcessor.multibandOff = false;
        }
        
        else if (tbMultibandOnOff.getToggleState() == true)
        {
            tbMultibandOnOff.setButtonText("Multiband Off");
            audioProcessor.multibandOff = true;
        }
    }
}

//adjust corresponding values in Processor class when the sliders are adjusted
void DOOMCompressorAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &rsGain)
    {
        audioProcessor.vGain = rsGain.getValue();
    }

    if (slider == &rsCompLow)
    {
        audioProcessor.cLow = rsCompLow.getValue();
    }

    if (slider == &rsCompHigh)
    {
        audioProcessor.cHigh = rsCompHigh.getValue();
    }

    if (slider == &sGate)
    {
        audioProcessor.cGate = pow(10,sGate.getValue() / 20);
        audioProcessor.addDB = -sGate.getValue();
    }

    if (slider == &rsAtk)
    {
        audioProcessor.cAtk = rsAtk.getValue();
    }

    if (slider == &rsRls)
    {
        audioProcessor.cRls = rsRls.getValue();
    }
}