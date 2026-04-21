// Source/PluginProcessor.h
#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include <JuceHeader.h>

class ReverseGateDSPProcessor : public AudioProcessor
{
public:
    ReverseGateDSPProcessor();
    ~ReverseGateDSPProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
    void releaseResources() override;

    AudioProcessorValueTreeState parameters;

protected:
    // Parameters
    float threshold;
    float attack;
    float release;
    float range;
    float hold;
    float lookahead;
    float mix;
    float sidechain;
};

#endif // PLUGINPROCESSOR_H_INCLUDED
