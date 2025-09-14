#pragma once
#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1
#include "../../../juce_wasm/modules/juce_audio_processors/juce_audio_processors.h"
#include "../../../juce_wasm/modules/juce_dsp/juce_dsp.h"
#include "SineWaveVoice.h"
#include "SineWaveSound.h"

class WorkstationProcessor : public juce::AudioProcessor
{
public:
    WorkstationProcessor();
    ~WorkstationProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Audio Workstation"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getValueTreeState() { return valueTreeState; }

    // EQ frequency response for visualization
    void getFrequencyResponse(std::vector<float>& frequencies, std::vector<float>& magnitudes);
    void getIndividualBandResponses(std::vector<float>& frequencies, 
                                   std::vector<float>& lowShelfResponse,
                                   std::vector<float>& peak1Response,
                                   std::vector<float>& peak3Response,
                                   std::vector<float>& highShelfResponse);
    
    // Live audio waveform data
    void getAudioWaveform(std::vector<float>& waveformData);
    
    // FFT spectrum data
    void getFFTData(std::vector<float>& fftData);
    void getFFTPeakHold(std::vector<float>& peakHoldData);
    
    // Built-in MIDI pattern generator
    void setPatternPlaying(bool shouldPlay);
    bool isPatternPlaying() const { return patternPlaying; }
    void setKey(int newKey) { rootKey = newKey; }
    void setMode(int newMode) { currentMode = newMode; }
    void setMelodyPattern(int pattern) { melodyPattern = pattern; }
    
    // MIDI device management
    juce::StringArray getAvailableMidiDevices();
    void setSelectedMidiDevice(const juce::String& deviceName);
    juce::String getSelectedMidiDevice() const { return selectedMidiDevice; }
    void refreshMidiDevices();

private:
    // Synthesizer
    juce::Synthesiser synth;
    
    // EQ Chain (4 bands)
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowShelfFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highShelfFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> peakFilter1;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> peakFilter3;

    juce::dsp::ProcessorChain<
        decltype(lowShelfFilter),
        decltype(peakFilter1),
        decltype(peakFilter3),
        decltype(highShelfFilter)
    > eqChain;
    
    // Reverb
    juce::dsp::Reverb reverb;
    

    juce::AudioProcessorValueTreeState valueTreeState;
    double currentSampleRate = 44100.0;
    
    // Global frequency range settings
    static constexpr float minFrequency = 20.0f;
    static constexpr float maxFrequency = 30000.0f;
    
    // Parameter cache for optimization
    float lastAttack = -1.0f, lastDecay = -1.0f, lastSustain = -1.0f, lastRelease = -1.0f;
    float lastFilterCutoff = -1.0f, lastFilterResonance = -1.0f;
    
    // Built-in pattern generator
    bool patternPlaying = false;
    int patternIndex = 0;
    int samplesUntilNextNote = 0;
    const int samplesPerNote = 11025; // 250ms at 44.1kHz (double tempo)
    std::vector<int> activeNotes;
    int rootKey = 60; // C4 by default
    int currentMode = 0; // Major by default
    int melodyPattern = 0; // Scale pattern by default
    
    // Audio waveform capture
    static constexpr int waveformSize = 512;
    std::array<float, waveformSize> waveformBuffer;
    int waveformIndex = 0;
    
    // FFT analysis
    static constexpr int fftOrder = 10;
    static constexpr int fftSize = 1 << fftOrder; // 1024
    juce::dsp::FFT forwardFFT;
    
    // FFT data
    std::array<float, fftSize * 2> fftData;
    std::array<float, fftSize / 2> magnitudes;
    std::array<float, fftSize / 2> fftPeakHold; // Slow-decaying peak hold
    int fftDataIndex = 0;
    
    void updateSynthParameters();
    void updateEQParameters();
    void updateReverbParameters();
    void generateMIDIPattern(juce::MidiBuffer& midiBuffer, int numSamples);
    void processFFT(std::array<float, fftSize * 2>& fftData, std::array<float, fftSize / 2>& magnitudes);
    void processFFTWithPeakHold(std::array<float, fftSize * 2>& fftData, std::array<float, fftSize / 2>& magnitudes);
    
    // MIDI device management
    juce::String selectedMidiDevice;
    juce::StringArray availableMidiDeviceNames;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WorkstationProcessor)
};