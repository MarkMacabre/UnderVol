#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// ReverseGateDSPProcessor Implementation
//==============================================================================

ReverseGateDSPProcessor::ReverseGateDSPProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", AudioChannelSet::stereo(), true)
        .withOutput("Output", AudioChannelSet::stereo(), true))
{
    // Initialize parameters with APVTS
    parameters.createAndAddParameter(std::make_unique<AudioParameterFloat>(
        "threshold", "Threshold", NormalisableRange<float>(-60.0f, 0.0f), -24.0f, "", 
        AudioProcessorParameter::genericParameter, 
        [](float value, int) { return String(value, 2) + " dB"; },
        nullptr
    ));
    
    parameters.createAndAddParameter(std::make_unique<AudioParameterFloat>(
        "attack", "Attack", NormalisableRange<float>(0.001f, 1.0f, 0.001f, 0.3f), 0.005f, "", 
        AudioProcessorParameter::genericParameter,
        [](float value, int) { return String(value * 1000, 2) + " ms"; },
        nullptr
    ));
    
    parameters.createAndAddParameter(std::make_unique<AudioParameterFloat>(
        "release", "Release", NormalisableRange<float>(0.01f, 5.0f, 0.01f, 0.3f), 0.1f, "", 
        AudioProcessorParameter::genericParameter,
        [](float value, int) { return String(value * 1000, 2) + " ms"; },
        nullptr
    ));
    
    parameters.createAndAddParameter(std::make_unique<AudioParameterFloat>(
        "range", "Range", NormalisableRange<float>(-60.0f, 0.0f), -60.0f, "", 
        AudioProcessorParameter::genericParameter,
        [](float value, int) { return String(value, 2) + " dB"; },
        nullptr
    ));
    
    parameters.createAndAddParameter(std::make_unique<AudioParameterFloat>(
        "hold", "Hold", NormalisableRange<float>(0.0f, 2.0f, 0.001f, 0.3f), 0.02f, "", 
        AudioProcessorParameter::genericParameter,
        [](float value, int) { return String(value * 1000, 2) + " ms"; },
        nullptr
    ));
    
    parameters.createAndAddParameter(std::make_unique<AudioParameterFloat>(
        "lookahead", "Lookahead", NormalisableRange<float>(0.0f, 0.1f, 0.0001f, 0.3f), 0.005f, "", 
        AudioProcessorParameter::genericParameter,
        [](float value, int) { return String(value * 1000, 2) + " ms"; },
        nullptr
    ));
    
    parameters.createAndAddParameter(std::make_unique<AudioParameterFloat>(
        "mix", "Mix", NormalisableRange<float>(0.0f, 1.0f), 1.0f, "", 
        AudioProcessorParameter::genericParameter,
        [](float value, int) { return String(value * 100, 1) + "%"; },
        nullptr
    ));
    
    parameters.state = ValueTree(Identifier("PARAMETERS"));
}

ReverseGateDSPProcessor::~ReverseGateDSPProcessor()
{
}

void ReverseGateDSPProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Setup DSP with sample rate
}

void ReverseGateDSPProcessor::releaseResources()
{
    // Cleanup resources
}

void ReverseGateDSPProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDelay noDelay;
    
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Get parameter values
    float thresholdValue = *parameters.getRawParameterValue("threshold");
    float attackValue = *parameters.getRawParameterValue("attack");
    float releaseValue = *parameters.getRawParameterValue("release");
    float rangeValue = *parameters.getRawParameterValue("range");
    float holdValue = *parameters.getRawParameterValue("hold");
    float lookaheadValue = *parameters.getRawParameterValue("lookahead");
    float mixValue = *parameters.getRawParameterValue("mix");

    // Convert dB to linear
    float thresholdLinear = powf(10.0f, thresholdValue / 20.0f);
    float rangeLinear = powf(10.0f, rangeValue / 20.0f);

    // Process each channel
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float inputSample = channelData[sample];
            float absInput = fabsf(inputSample);
            
            // Reverse gate logic: below threshold = passes, above = attenuated
            float outputSample = inputSample;
            
            if (absInput > thresholdLinear)
            {
                // Above threshold - attenuate based on range
                outputSample = inputSample * rangeLinear;
            }
            
            // Apply mix (dry/wet blending)
            outputSample = inputSample * (1.0f - mixValue) + outputSample * mixValue;
            
            channelData[sample] = outputSample;
        }
    }
}

const String ReverseGateDSPProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ReverseGateDSPProcessor::acceptsMidi() const
{
    return true;
}

bool ReverseGateDSPProcessor::producesMidi() const
{
    return false;
}

double ReverseGateDSPProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ReverseGateDSPProcessor::getNumPrograms()
{
    return 1;
}

int ReverseGateDSPProcessor::getCurrentProgram()
{
    return 0;
}

void ReverseGateDSPProcessor::setCurrentProgram(int index)
{
}

const String ReverseGateDSPProcessor::getProgramName(int index)
{
    return "Default";
}

void ReverseGateDSPProcessor::changeProgramName(int index, const String& newName)
{
}

void ReverseGateDSPProcessor::getStateInformation(MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ReverseGateDSPProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
        parameters.replaceState(ValueTree::fromXml(*xmlState));
}

AudioProcessorEditor* ReverseGateDSPProcessor::createEditor()
{
    return new ReverseGateEditor(*this);
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverseGateDSPProcessor();
}