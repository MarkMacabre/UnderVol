#include <JuceHeader.h>

class UnderVolAudioProcessor : public juce::AudioProcessor
{
public:
    UnderVolAudioProcessor();
    ~UnderVolAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // Parameter definitions
    enum Parameters
    {
        thresholdParam = 0,
        ratioParam,
        attackParam,
        releaseParam,
        numParameters
    };

    // Audio processing interface
    void setParameter (int parameterIndex, float newValue);
    float getParameter (int parameterIndex) const;
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    // State management
    float threshold;
    float ratio;
    float attack;
    float release;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnderVolAudioProcessor);
};