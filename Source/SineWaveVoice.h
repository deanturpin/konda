#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "SineWaveSound.h"

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
                    auto currentSample = (float) (std::sin(currentAngle) * level * tailOff);
                    
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);
                    
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
                    auto rawSample = (float) (std::sin(currentAngle) * level * adsrValue);
                    
                    // Apply filter
                    auto filteredSample = lowpassFilter.processSample(0, rawSample);
                    
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
    
    void prepareFilter(double sampleRate)
    {
        currentSampleRate = sampleRate;
        
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = 512;
        spec.numChannels = 1;
        
        lowpassFilter.prepare(spec);
        updateFilter();
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
    
    // Filter components
    juce::dsp::StateVariableTPTFilter<float> lowpassFilter;
    float filterCutoff = 1000.0f;
    float filterResonance = 0.7f;
    
    void updateFilter()
    {
        lowpassFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        lowpassFilter.setCutoffFrequency(filterCutoff);
        lowpassFilter.setResonance(filterResonance);
    }
};