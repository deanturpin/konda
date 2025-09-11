#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class EQProcessor : public juce::AudioProcessor
{
public:
    EQProcessor();
    ~EQProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Parametric EQ"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}

    juce::AudioProcessorValueTreeState& getValueTreeState() { return valueTreeState; }

    // Frequency response data for visualization (thread-safe copy)
    void getFrequencyResponse(std::vector<float>& frequencies, std::vector<float>& magnitudes);

private:
    juce::AudioProcessorValueTreeState valueTreeState;
    
    // EQ bands
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowShelfFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highShelfFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> peakFilter1;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> peakFilter2;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> peakFilter3;

    juce::dsp::ProcessorChain<
        decltype(lowShelfFilter),
        decltype(peakFilter1),
        decltype(peakFilter2),
        decltype(peakFilter3),
        decltype(highShelfFilter)
    > processorChain;

    // Parameter pointers
    std::atomic<float>* lowShelfFreq = nullptr;
    std::atomic<float>* lowShelfGain = nullptr;
    std::atomic<float>* peak1Freq = nullptr;
    std::atomic<float>* peak1Gain = nullptr;
    std::atomic<float>* peak1Q = nullptr;
    std::atomic<float>* peak2Freq = nullptr;
    std::atomic<float>* peak2Gain = nullptr;
    std::atomic<float>* peak2Q = nullptr;
    std::atomic<float>* peak3Freq = nullptr;
    std::atomic<float>* peak3Gain = nullptr;
    std::atomic<float>* peak3Q = nullptr;
    std::atomic<float>* highShelfFreq = nullptr;
    std::atomic<float>* highShelfGain = nullptr;

    double currentSampleRate = 44100.0;
    
    void updateFilters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQProcessor)
};