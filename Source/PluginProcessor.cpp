/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DOOMCompressorAudioProcessor::DOOMCompressorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

DOOMCompressorAudioProcessor::~DOOMCompressorAudioProcessor()
{
}

//==============================================================================
const juce::String DOOMCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DOOMCompressorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DOOMCompressorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DOOMCompressorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DOOMCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DOOMCompressorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DOOMCompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DOOMCompressorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DOOMCompressorAudioProcessor::getProgramName (int index)
{
    return {};
}

void DOOMCompressorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DOOMCompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //initialize filters for multiband processing
    lowBandFilterL1.setCoefficients(coefficients.makeLowPass(sampleRate, cLow));
    lowMidBandFilterL1.setCoefficients(coefficients.makeHighPass(sampleRate, cLow));
    highMidBandFilterL1.setCoefficients(coefficients.makeLowPass(sampleRate, cHigh));
    highBandFilterL1.setCoefficients(coefficients.makeHighPass(sampleRate, cHigh));

    lowBandFilterR1.setCoefficients(coefficients.makeLowPass(sampleRate, cLow));
    lowMidBandFilterR1.setCoefficients(coefficients.makeHighPass(sampleRate, cLow));
    highMidBandFilterR1.setCoefficients(coefficients.makeLowPass(sampleRate, cHigh));
    highBandFilterR1.setCoefficients(coefficients.makeHighPass(sampleRate, cHigh));

    lowBandFilterL2.setCoefficients(coefficients.makeLowPass(sampleRate, cLow));
    lowMidBandFilterL2.setCoefficients(coefficients.makeHighPass(sampleRate, cLow));
    highMidBandFilterL2.setCoefficients(coefficients.makeLowPass(sampleRate, cHigh));
    highBandFilterL2.setCoefficients(coefficients.makeHighPass(sampleRate, cHigh));

    lowBandFilterR2.setCoefficients(coefficients.makeLowPass(sampleRate, cLow));
    lowMidBandFilterR2.setCoefficients(coefficients.makeHighPass(sampleRate, cLow));
    highMidBandFilterR2.setCoefficients(coefficients.makeLowPass(sampleRate, cHigh));
    highBandFilterR2.setCoefficients(coefficients.makeHighPass(sampleRate, cHigh));

    //initialize compressors
    for (int channel = 0; channel < getNumInputChannels(); channel++)
    {
        allCompressors.add(Compressor());
    }
}

void DOOMCompressorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DOOMCompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DOOMCompressorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    //making attack/release values usable for the compressor
    at = 1 - std::pow(juce::MathConstants<float>::euler, ((1 / getSampleRate()) * -2.2f) / (cAtk / 1000.0f));
    rt = 1 - std::pow(juce::MathConstants<float>::euler, ((1 / getSampleRate()) * -2.2f) / (cAtk / 1000.0f));

    //add Gain to the whole signal
    buffer.applyGain(addDB * 10);

    float sampleRate = getSampleRate();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //multiband processing
    if (multibandOff == false)
    {
    
        //initialize low, mid and high band
        juce::AudioSampleBuffer lowOutput;
        juce::AudioSampleBuffer midOutput;
        juce::AudioSampleBuffer highOutput;

        lowOutput.makeCopyOf(buffer);
        midOutput.makeCopyOf(buffer);
        highOutput.makeCopyOf(buffer);
        
        //set filters in case cutoffs changed
        lowBandFilterL1.setCoefficients(coefficients.makeLowPass(sampleRate, cLow));
        lowMidBandFilterL1.setCoefficients(coefficients.makeHighPass(sampleRate, cLow));
        highMidBandFilterL1.setCoefficients(coefficients.makeLowPass(sampleRate, cHigh));
        highBandFilterL1.setCoefficients(coefficients.makeHighPass(sampleRate, cHigh));

        lowBandFilterR1.setCoefficients(coefficients.makeLowPass(sampleRate, cLow));
        lowMidBandFilterR1.setCoefficients(coefficients.makeHighPass(sampleRate, cLow));
        highMidBandFilterR1.setCoefficients(coefficients.makeLowPass(sampleRate, cHigh));
        highBandFilterR1.setCoefficients(coefficients.makeHighPass(sampleRate, cHigh));

        lowBandFilterL2.setCoefficients(coefficients.makeLowPass(sampleRate, cLow));
        lowMidBandFilterL2.setCoefficients(coefficients.makeHighPass(sampleRate, cLow));
        highMidBandFilterL2.setCoefficients(coefficients.makeLowPass(sampleRate, cHigh));
        highBandFilterL2.setCoefficients(coefficients.makeHighPass(sampleRate, cHigh));

        lowBandFilterR2.setCoefficients(coefficients.makeLowPass(sampleRate, cLow));
        lowMidBandFilterR2.setCoefficients(coefficients.makeHighPass(sampleRate, cLow));
        highMidBandFilterR2.setCoefficients(coefficients.makeLowPass(sampleRate, cHigh));
        highBandFilterR2.setCoefficients(coefficients.makeHighPass(sampleRate, cHigh));

        //apply filters
        lowBandFilterL1.processSamples(lowOutput.getWritePointer(0), numSamples);
        lowBandFilterR1.processSamples(lowOutput.getWritePointer(1), numSamples);

        lowBandFilterL2.processSamples(lowOutput.getWritePointer(0), numSamples);
        lowBandFilterR2.processSamples(lowOutput.getWritePointer(1), numSamples);

        lowMidBandFilterL1.processSamples(midOutput.getWritePointer(0), numSamples);
        lowMidBandFilterR1.processSamples(midOutput.getWritePointer(1), numSamples);

        lowMidBandFilterL2.processSamples(midOutput.getWritePointer(0), numSamples);
        lowMidBandFilterR2.processSamples(midOutput.getWritePointer(1), numSamples);

        highMidBandFilterL1.processSamples(midOutput.getWritePointer(0), numSamples);
        highMidBandFilterR1.processSamples(midOutput.getWritePointer(1), numSamples);

        highMidBandFilterL2.processSamples(midOutput.getWritePointer(0), numSamples);
        highMidBandFilterR2.processSamples(midOutput.getWritePointer(1), numSamples);

        highBandFilterL1.processSamples(highOutput.getWritePointer(0), numSamples);
        highBandFilterR1.processSamples(highOutput.getWritePointer(1), numSamples);

        highBandFilterL2.processSamples(highOutput.getWritePointer(0), numSamples);
        highBandFilterR2.processSamples(highOutput.getWritePointer(1), numSamples);

        //compress each band
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            for (int channel = 0; channel < totalNumInputChannels; ++channel)
            {
                auto* lowData = lowOutput.getWritePointer(channel);
                auto* midData = midOutput.getWritePointer(channel);
                auto* highData = highOutput.getWritePointer(channel);
                Compressor* comp = &allCompressors.getReference(channel);
                lowData[i] = comp->compressSample(lowData[i], at, rt);
                midData[i] = comp->compressSample(midData[i], at, rt);
                highData[i] = comp->compressSample(highData[i], at, rt);
            }
        }

        buffer.clear();

        //rebuild the buffer from the single bands
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            buffer.addFrom(channel, 0, lowOutput, channel, 0, numSamples, 1.0 / 3.0);
            buffer.addFrom(channel, 0, midOutput, channel, 0, numSamples, 1.0 / 3.0);
            buffer.addFrom(channel, 0, highOutput, channel, 0, numSamples, 1.0 / 3.0);
        }
    }
    
    //if multiband processing is off, just compress the entire buffer
    else
    {
        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            for (int channel = 0; channel < totalNumInputChannels; ++channel)
            {
                auto* data = buffer.getWritePointer(channel);
                Compressor* comp = &allCompressors.getReference(channel);
                data[sample] = comp->compressSample(data[sample], at, rt);
            }
        }
    }

    //some leftover code for automatic makeupgain which I don't know whether I'll need it again
    
    /*double outputGain = 0.0;
    for (int i = 0; i < buffer.getNumChannels(); ++i)
        outputGain += buffer.getRMSLevel(i, 0, buffer.getNumSamples());

    auto makeup = (1.0 > 0.0) ? outputGain / 1.0 : 1.0;
    for (int i = 0; i < buffer.getNumChannels(); ++i)
        buffer.applyGainRamp(i, 0, buffer.getNumSamples(), previousMakeup, makeup);

    previousMakeup = makeup;*/

    //if any sample value is above 0dB, make it 0
    for (int channel = 0; channel < numChannels; channel++)
    {
        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            auto* channelData = buffer.getWritePointer(channel);
            if (channelData[sample] > 1.0)
                channelData[sample] = 1.0f;
            else if (channelData[sample] < -1.0)
                channelData[sample] = -1.0f;
        }
    }
    
    //apply the output gain
    buffer.applyGain(vGain/100);
}



//==============================================================================
bool DOOMCompressorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DOOMCompressorAudioProcessor::createEditor()
{
    return new DOOMCompressorAudioProcessorEditor (*this);
}

//==============================================================================
void DOOMCompressorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DOOMCompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DOOMCompressorAudioProcessor();
}
