#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "SineWaveSound.h"

enum class WaveformType
{
    Sine = 0,
    Sawtooth,
    Square,
    Triangle
};

enum class FilterType
{
    Lowpass = 0,
    Highpass,
    Bandpass,
    Notch
};

class SineWaveVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*>(sound) != nullptr;
    }
    
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        currentAngle = 0.0;
        level = velocity * VELOCITY_SCALE;
        tailOff = 0.0;

        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();

        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;

        // Initialize LFO
        lfoAngle = 0.0;
        lfoAngleDelta = lfoRate * 2.0 * juce::MathConstants<double>::pi / getSampleRate();

        adsr.noteOn();
    }
    
    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            adsr.noteOff();
        }
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }
    
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                         int startSample, int numSamples) override
    {
        if (angleDelta != 0.0)
        {
            if (tailOff > 0.0)
            {
                while (--numSamples >= 0)
                {
                    auto oscillatorSample = generateWaveform();
                    auto rawSample = (float) (oscillatorSample * level * tailOff);
                    auto filteredSample = processFilter(rawSample);

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, filteredSample);

                    currentAngle += angleDelta;
                    ++startSample;

                    tailOff *= TAIL_OFF_MULTIPLIER;

                    if (tailOff <= TAIL_OFF_THRESHOLD)
                    {
                        clearCurrentNote();
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0)
                {
                    auto adsrValue = adsr.getNextSample();

                    // Generate waveform with LFO modulation
                    auto oscillatorSample = generateWaveform();
                    auto lfoModulation = generateLFO();

                    auto rawSample = (float) (oscillatorSample * level * adsrValue * (1.0 + lfoModulation * lfoDepth));

                    // Apply filter
                    auto filteredSample = processFilter(rawSample);
                    
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, filteredSample);
                    
                    currentAngle += angleDelta;
                    ++startSample;
                    
                    if (!adsr.isActive())
                    {
                        clearCurrentNote();
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
        }
    }
    
    void setADSRParameters(const juce::ADSR::Parameters& params)
    {
        adsr.setParameters(params);
    }
    
    void setFilterParameters(float cutoff, float resonance)
    {
        // Validate and clamp cutoff frequency (20Hz - 20kHz)
        filterCutoff = juce::jlimit(20.0f, 20000.0f, cutoff);

        // Validate and clamp resonance (0.1 - 10.0)
        filterResonance = juce::jlimit(0.1f, 10.0f, resonance);

        updateFilter();
    }

    void setWaveformType(WaveformType type) { waveformType = type; }
    void setFilterType(FilterType type) { filterType = type; updateFilter(); }
    void setLFOParameters(float rate, float depth, WaveformType shape)
    {
        lfoRate = rate;
        lfoDepth = depth;
        lfoWaveform = shape;
    }
    
    void prepareFilter(double sampleRate)
    {
        currentSampleRate = sampleRate;

        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = 512;
        spec.numChannels = 1;

        filter.prepare(spec);
        updateFilter();

        // Update LFO rate based on sample rate
        lfoAngleDelta = lfoRate * 2.0 * juce::MathConstants<double>::pi / sampleRate;
    }

private:
    // Audio processing constants
    static constexpr double DEFAULT_SAMPLE_RATE = 44100.0;
    static constexpr double TAIL_OFF_MULTIPLIER = 0.99;
    static constexpr double TAIL_OFF_THRESHOLD = 0.005;
    static constexpr double VELOCITY_SCALE = 0.15;
    
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double level = 0.0;
    double tailOff = 0.0;
    double currentSampleRate = DEFAULT_SAMPLE_RATE;
    
    juce::ADSR adsr;
    
    // Synthesis parameters
    WaveformType waveformType = WaveformType::Sine;
    FilterType filterType = FilterType::Lowpass;

    // LFO parameters
    float lfoRate = 2.0f; // Hz
    float lfoDepth = 0.0f; // 0.0 to 1.0
    WaveformType lfoWaveform = WaveformType::Sine;
    double lfoAngle = 0.0;
    double lfoAngleDelta = 0.0;

    // Filter components
    juce::dsp::StateVariableTPTFilter<float> filter;
    float filterCutoff = 1000.0f;
    float filterResonance = 0.7f;

    void updateFilter()
    {
        switch (filterType)
        {
            case FilterType::Lowpass:
                filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
                break;
            case FilterType::Highpass:
                filter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
                break;
            case FilterType::Bandpass:
                filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
                break;
            case FilterType::Notch:
                filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass); // Notch approximation
                break;
        }
        filter.setCutoffFrequency(filterCutoff);
        filter.setResonance(filterResonance);
    }

    double generateWaveform()
    {
        switch (waveformType)
        {
            case WaveformType::Sine:
                return std::sin(currentAngle);

            case WaveformType::Sawtooth:
                return 2.0 * (currentAngle / (2.0 * juce::MathConstants<double>::pi) -
                             std::floor(currentAngle / (2.0 * juce::MathConstants<double>::pi) + 0.5));

            case WaveformType::Square:
                return std::sin(currentAngle) > 0.0 ? 1.0 : -1.0;

            case WaveformType::Triangle:
            {
                double phase = currentAngle / (2.0 * juce::MathConstants<double>::pi);
                phase = phase - std::floor(phase); // Normalize to 0-1
                return phase < 0.5 ? (4.0 * phase - 1.0) : (3.0 - 4.0 * phase);
            }

            default:
                return std::sin(currentAngle);
        }
    }

    double generateLFO()
    {
        if (lfoDepth == 0.0f) return 0.0;

        double lfoSample = 0.0;
        switch (lfoWaveform)
        {
            case WaveformType::Sine:
                lfoSample = std::sin(lfoAngle);
                break;
            case WaveformType::Sawtooth:
                lfoSample = 2.0 * (lfoAngle / (2.0 * juce::MathConstants<double>::pi) -
                                  std::floor(lfoAngle / (2.0 * juce::MathConstants<double>::pi) + 0.5));
                break;
            case WaveformType::Square:
                lfoSample = std::sin(lfoAngle) > 0.0 ? 1.0 : -1.0;
                break;
            case WaveformType::Triangle:
            {
                double phase = lfoAngle / (2.0 * juce::MathConstants<double>::pi);
                phase = phase - std::floor(phase);
                lfoSample = phase < 0.5 ? (4.0 * phase - 1.0) : (3.0 - 4.0 * phase);
                break;
            }
        }

        lfoAngle += lfoAngleDelta;
        if (lfoAngle >= 2.0 * juce::MathConstants<double>::pi)
            lfoAngle -= 2.0 * juce::MathConstants<double>::pi;

        return lfoSample;
    }

    float processFilter(float sample)
    {
        return filter.processSample(0, sample);
    }
};