/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DubwayAudioProcessor::DubwayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(new AudioParameterFloat("gain", "Gain", 0.0f, 1.0f, 1.0f));
    addParameter(new AudioParameterFloat("feedback", "Delay feedback", 0.0f, 1.0f, 0.35f));
    addParameter(new AudioParameterFloat("mix", "Dry / Wet", 0.0f, 1.0f, 0.5f));
}

DubwayAudioProcessor::~DubwayAudioProcessor()
{
}

//==============================================================================
const String DubwayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DubwayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DubwayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DubwayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DubwayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DubwayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DubwayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DubwayAudioProcessor::setCurrentProgram (int index)
{
}

const String DubwayAudioProcessor::getProgramName (int index)
{
    return {};
}

void DubwayAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DubwayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    int delayMilliseconds = 200;
    auto delaySamples = (int) std::round(sampleRate * delayMilliseconds / 1000.0);
    delayBuffer.setSize(2, delaySamples);
    delayBuffer.clear();
    delayBufferPos = 0;

}

void DubwayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DubwayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void DubwayAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    auto& parameters = getParameters();
    float gain      = parameters[0]->getValue();
    float feedback  = parameters[1]->getValue();
    float mix       = parameters[2]->getValue();

    int delayBufferSize = delayBuffer.getNumSamples();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        int delayPos = delayBufferPos;

        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            float drySample = channelData[i];

            float delaySample = delayBuffer.getSample(channel, delayPos) * feedback;
            delayBuffer.setSample(channel, delayPos, drySample + delaySample);

            delayPos++;
            if (delayPos == delayBufferSize)
                delayPos = 0;

            channelData[i] = (drySample * (1.0f - mix)) + (delaySample * mix);
            channelData[i] *= gain;
        }
        
    }

    delayBufferPos += buffer.getNumSamples();
    if (delayBufferPos >= delayBufferSize)
        delayBufferPos -= delayBufferSize;


}

//==============================================================================
bool DubwayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DubwayAudioProcessor::createEditor()
{
    return new DubwayAudioProcessorEditor (*this);
    //return new GenericAudioProcessorEditor(*this);
}

//==============================================================================
void DubwayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DubwayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DubwayAudioProcessor();
}
