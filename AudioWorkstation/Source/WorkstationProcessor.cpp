#include "WorkstationProcessor.h"
#include "WorkstationEditor.h"

WorkstationProcessor::WorkstationProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      valueTreeState(*this, nullptr, "PARAMETERS", 
      {
          // Synth parameters
          std::make_unique<juce::AudioParameterFloat>("attack", "Attack", 0.1f, 1.0f, 0.1f),
          std::make_unique<juce::AudioParameterFloat>("decay", "Decay", 0.1f, 1.0f, 0.1f),
          std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain", 0.0f, 1.0f, 1.0f),
          std::make_unique<juce::AudioParameterFloat>("release", "Release", 0.1f, 3.0f, 0.4f),
          std::make_unique<juce::AudioParameterFloat>("filterCutoff", "Filter Cutoff", minFrequency, 5000.0f, 800.0f),
          std::make_unique<juce::AudioParameterFloat>("filterResonance", "Filter Resonance", 0.1f, 5.0f, 0.5f),
          std::make_unique<juce::AudioParameterFloat>("distortion", "Distortion", 1.0f, 10.0f, 1.0f),

          // Octave control
          std::make_unique<juce::AudioParameterInt>("octave", "Octave", 2, 6, 4),

          // EQ parameters
          std::make_unique<juce::AudioParameterFloat>("lowShelfFreq", "Low Shelf Freq", minFrequency, 500.0f, 80.0f),
          std::make_unique<juce::AudioParameterFloat>("lowShelfGain", "Low Shelf Gain", -24.0f, 24.0f, 0.0f),
          std::make_unique<juce::AudioParameterFloat>("peak1Freq", "Peak 1 Freq", 100.0f, 2000.0f, 400.0f),
          std::make_unique<juce::AudioParameterFloat>("peak1Gain", "Peak 1 Gain", -24.0f, 24.0f, 0.0f),
          std::make_unique<juce::AudioParameterFloat>("peak1Q", "Peak 1 Q", 0.1f, 10.0f, 0.7f),
          std::make_unique<juce::AudioParameterFloat>("peak3Freq", "Peak 3 Freq", 2000.0f, 20000.0f, 4000.0f),
          std::make_unique<juce::AudioParameterFloat>("peak3Gain", "Peak 3 Gain", -24.0f, 24.0f, 0.0f),
          std::make_unique<juce::AudioParameterFloat>("peak3Q", "Peak 3 Q", 0.1f, 10.0f, 0.7f),
          std::make_unique<juce::AudioParameterFloat>("highShelfFreq", "High Shelf Freq", 5000.0f, 20000.0f, 8000.0f),
          std::make_unique<juce::AudioParameterFloat>("highShelfGain", "High Shelf Gain", -24.0f, 24.0f, 0.0f),
          
          // Reverb parameters
          std::make_unique<juce::AudioParameterFloat>("reverbRoomSize", "Reverb Room Size", 0.0f, 1.0f, 0.3f),
          std::make_unique<juce::AudioParameterFloat>("reverbDamping", "Reverb Damping", 0.0f, 1.0f, 0.5f),
          std::make_unique<juce::AudioParameterFloat>("reverbWetLevel", "Reverb Wet Level", 0.0f, 1.0f, 0.2f),
          std::make_unique<juce::AudioParameterFloat>("reverbDryLevel", "Reverb Dry Level", 0.0f, 1.0f, 0.8f),
          
      })
      , forwardFFT(fftOrder)
{
    // Setup synthesizer
    synth.addSound(new SineWaveSound());
    for (auto i = 0; i < 4; ++i)
        synth.addVoice(new SineWaveVoice());
    
    // Initialise FFT data
    fftData.fill(0.0f);
    magnitudes.fill(0.0f);
    fftPeakHold.fill(0.0f);
    
    // Initialize MIDI device list
    refreshMidiDevices();
}

void WorkstationProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    // Prepare synthesizer
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    for (auto i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SineWaveVoice*>(synth.getVoice(i)))
        {
            voice->prepareFilter(sampleRate);
        }
    }
    
    // Prepare EQ chain
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;
    
    eqChain.prepare(spec);
    updateEQParameters();
    
    // Prepare reverb
    reverb.prepare(spec);
    updateReverbParameters();
    
}

void WorkstationProcessor::releaseResources()
{
}

void WorkstationProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    // Generate built-in MIDI patterns if enabled
    if (patternPlaying)
    {
        generateMIDIPattern(midiMessages, buffer.getNumSamples());
    }

    // Apply octave transposition to all incoming MIDI
    int octaveParam = valueTreeState.getRawParameterValue("octave")->load();
    int octaveShift = (octaveParam - 4) * 12; // Offset from default octave 4

    if (octaveShift != 0)
    {
        juce::MidiBuffer transposedMidi;

        for (const auto& message : midiMessages)
        {
            auto midiMessage = message.getMessage();

            if (midiMessage.isNoteOn() || midiMessage.isNoteOff())
            {
                int newNote = midiMessage.getNoteNumber() + octaveShift;
                newNote = juce::jlimit(0, 127, newNote); // Keep in valid MIDI range

                if (midiMessage.isNoteOn())
                    midiMessage = juce::MidiMessage::noteOn(midiMessage.getChannel(), newNote, midiMessage.getVelocity());
                else
                    midiMessage = juce::MidiMessage::noteOff(midiMessage.getChannel(), newNote, midiMessage.getVelocity());
            }

            transposedMidi.addEvent(midiMessage, message.samplePosition);
        }

        midiMessages.swapWith(transposedMidi);
    }

    // Update synth parameters if they've changed
    updateSynthParameters();
    
    // Process synthesizer
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    // Apply distortion
    float distortionAmount = valueTreeState.getRawParameterValue("distortion")->load();
    if (distortionAmount > 1.0f)
    {
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                // Soft clipping distortion
                float input = channelData[sample] * distortionAmount;
                channelData[sample] = std::tanh(input) / distortionAmount * 0.7f; // Compensate gain
            }
        }
    }
    
    
    // Update parameters
    updateEQParameters();
    updateReverbParameters();
    
    // Process EQ
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    eqChain.process(context);
    
    
    // Process Reverb
    reverb.process(context);
    
    // Capture audio data for visualization
    if (buffer.getNumSamples() > 0 && buffer.getNumChannels() > 0)
    {
        const float* channelData = buffer.getReadPointer(0);
        
        // Waveform capture (every 4th sample)
        for (int i = 0; i < buffer.getNumSamples(); i += 4)
        {
            waveformBuffer[waveformIndex] = channelData[i];
            waveformIndex = (waveformIndex + 1) % waveformSize;
        }
        
        // FFT data collection
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            fftData[fftDataIndex] = channelData[i];
            fftDataIndex++;
            
            if (fftDataIndex >= fftSize)
            {
                // Process FFT with peak hold
                processFFTWithPeakHold(fftData, magnitudes);
                fftDataIndex = 0;
            }
        }
    }
}

void WorkstationProcessor::updateSynthParameters()
{
    auto currentAttack = valueTreeState.getRawParameterValue("attack")->load();
    auto currentDecay = valueTreeState.getRawParameterValue("decay")->load();
    auto currentSustain = valueTreeState.getRawParameterValue("sustain")->load();
    auto currentRelease = valueTreeState.getRawParameterValue("release")->load();
    auto currentFilterCutoff = valueTreeState.getRawParameterValue("filterCutoff")->load();
    auto currentFilterResonance = valueTreeState.getRawParameterValue("filterResonance")->load();
    
    bool parametersChanged = (currentAttack != lastAttack || currentDecay != lastDecay ||
                             currentSustain != lastSustain || currentRelease != lastRelease ||
                             currentFilterCutoff != lastFilterCutoff || currentFilterResonance != lastFilterResonance);
    
    if (parametersChanged)
    {
        for (auto i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto voice = dynamic_cast<SineWaveVoice*>(synth.getVoice(i)))
            {
                voice->setADSRParameters({currentAttack, currentDecay, currentSustain, currentRelease});
                voice->setFilterParameters(currentFilterCutoff, currentFilterResonance);
            }
        }
        
        lastAttack = currentAttack;
        lastDecay = currentDecay;
        lastSustain = currentSustain;
        lastRelease = currentRelease;
        lastFilterCutoff = currentFilterCutoff;
        lastFilterResonance = currentFilterResonance;
    }
}

void WorkstationProcessor::updateEQParameters()
{
    // Low shelf filter
    auto& lowShelf = eqChain.get<0>();
    *lowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        currentSampleRate, 
        valueTreeState.getRawParameterValue("lowShelfFreq")->load(), 
        0.7f, 
        juce::Decibels::decibelsToGain(valueTreeState.getRawParameterValue("lowShelfGain")->load()));
    
    // Peak filters
    auto& peak1 = eqChain.get<1>();
    *peak1.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        currentSampleRate,
        valueTreeState.getRawParameterValue("peak1Freq")->load(),
        valueTreeState.getRawParameterValue("peak1Q")->load(),
        juce::Decibels::decibelsToGain(valueTreeState.getRawParameterValue("peak1Gain")->load()));
        
    auto& peak3 = eqChain.get<2>();
    *peak3.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        currentSampleRate,
        valueTreeState.getRawParameterValue("peak3Freq")->load(),
        valueTreeState.getRawParameterValue("peak3Q")->load(),
        juce::Decibels::decibelsToGain(valueTreeState.getRawParameterValue("peak3Gain")->load()));
    
    // High shelf filter
    auto& highShelf = eqChain.get<3>();
    *highShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        currentSampleRate,
        valueTreeState.getRawParameterValue("highShelfFreq")->load(),
        0.7f,
        juce::Decibels::decibelsToGain(valueTreeState.getRawParameterValue("highShelfGain")->load()));
}

void WorkstationProcessor::updateReverbParameters()
{
    juce::dsp::Reverb::Parameters reverbParams;
    reverbParams.roomSize = valueTreeState.getRawParameterValue("reverbRoomSize")->load();
    reverbParams.damping = valueTreeState.getRawParameterValue("reverbDamping")->load();
    reverbParams.wetLevel = valueTreeState.getRawParameterValue("reverbWetLevel")->load();
    reverbParams.dryLevel = valueTreeState.getRawParameterValue("reverbDryLevel")->load();
    reverbParams.width = 1.0f;      // Full stereo width
    reverbParams.freezeMode = 0.0f; // No freeze mode
    
    reverb.setParameters(reverbParams);
}


void WorkstationProcessor::getFrequencyResponse(std::vector<float>& frequencies, std::vector<float>& magnitudes)
{
    frequencies.clear();
    magnitudes.clear();
    
    const int numPoints = 512;
    const float minFreq = minFrequency;
    const float maxFreq = maxFrequency;
    
    // Take snapshot of current parameter values
    const float currentLowShelfFreq = valueTreeState.getRawParameterValue("lowShelfFreq")->load();
    const float currentLowShelfGain = valueTreeState.getRawParameterValue("lowShelfGain")->load();
    const float currentPeak1Freq = valueTreeState.getRawParameterValue("peak1Freq")->load();
    const float currentPeak1Gain = valueTreeState.getRawParameterValue("peak1Gain")->load();
    const float currentPeak1Q = valueTreeState.getRawParameterValue("peak1Q")->load();
    const float currentPeak3Freq = valueTreeState.getRawParameterValue("peak3Freq")->load();
    const float currentPeak3Gain = valueTreeState.getRawParameterValue("peak3Gain")->load();
    const float currentPeak3Q = valueTreeState.getRawParameterValue("peak3Q")->load();
    const float currentHighShelfFreq = valueTreeState.getRawParameterValue("highShelfFreq")->load();
    const float currentHighShelfGain = valueTreeState.getRawParameterValue("highShelfGain")->load();
    
    for (int i = 0; i < numPoints; ++i)
    {
        float freq = minFreq * std::pow(maxFreq / minFreq, i / float(numPoints - 1));
        frequencies.push_back(freq);
        
        float magnitude = 1.0f;
        
        // Calculate combined response
        auto lowShelfCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            currentSampleRate, currentLowShelfFreq, 0.7f, juce::Decibels::decibelsToGain(currentLowShelfGain));
        magnitude *= std::abs(lowShelfCoeffs->getMagnitudeForFrequency(freq, currentSampleRate));
        
        auto peak1Coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            currentSampleRate, currentPeak1Freq, currentPeak1Q, juce::Decibels::decibelsToGain(currentPeak1Gain));
        magnitude *= std::abs(peak1Coeffs->getMagnitudeForFrequency(freq, currentSampleRate));
        
        auto peak3Coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            currentSampleRate, currentPeak3Freq, currentPeak3Q, juce::Decibels::decibelsToGain(currentPeak3Gain));
        magnitude *= std::abs(peak3Coeffs->getMagnitudeForFrequency(freq, currentSampleRate));
        
        auto highShelfCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            currentSampleRate, currentHighShelfFreq, 0.7f, juce::Decibels::decibelsToGain(currentHighShelfGain));
        magnitude *= std::abs(highShelfCoeffs->getMagnitudeForFrequency(freq, currentSampleRate));
        
        magnitudes.push_back(juce::Decibels::gainToDecibels(magnitude));
    }
}

void WorkstationProcessor::getIndividualBandResponses(std::vector<float>& frequencies, 
                                                     std::vector<float>& lowShelfResponse,
                                                     std::vector<float>& peak1Response,
                                                     std::vector<float>& peak3Response,
                                                     std::vector<float>& highShelfResponse)
{
    frequencies.clear();
    lowShelfResponse.clear();
    peak1Response.clear();
    peak3Response.clear();
    highShelfResponse.clear();
    
    const int numPoints = 512;
    const float minFreq = minFrequency;
    const float maxFreq = maxFrequency;
    
    // Get current parameter values
    const float currentLowShelfFreq = valueTreeState.getRawParameterValue("lowShelfFreq")->load();
    const float currentLowShelfGain = valueTreeState.getRawParameterValue("lowShelfGain")->load();
    const float currentPeak1Freq = valueTreeState.getRawParameterValue("peak1Freq")->load();
    const float currentPeak1Gain = valueTreeState.getRawParameterValue("peak1Gain")->load();
    const float currentPeak1Q = valueTreeState.getRawParameterValue("peak1Q")->load();
    const float currentPeak3Freq = valueTreeState.getRawParameterValue("peak3Freq")->load();
    const float currentPeak3Gain = valueTreeState.getRawParameterValue("peak3Gain")->load();
    const float currentPeak3Q = valueTreeState.getRawParameterValue("peak3Q")->load();
    const float currentHighShelfFreq = valueTreeState.getRawParameterValue("highShelfFreq")->load();
    const float currentHighShelfGain = valueTreeState.getRawParameterValue("highShelfGain")->load();
    
    for (int i = 0; i < numPoints; ++i)
    {
        float freq = minFreq * std::pow(maxFreq / minFreq, i / float(numPoints - 1));
        frequencies.push_back(freq);
        
        // Calculate individual band responses
        auto lowShelfCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            currentSampleRate, currentLowShelfFreq, 0.7f, juce::Decibels::decibelsToGain(currentLowShelfGain));
        lowShelfResponse.push_back(juce::Decibels::gainToDecibels(
            std::abs(lowShelfCoeffs->getMagnitudeForFrequency(freq, currentSampleRate))));
        
        auto peak1Coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            currentSampleRate, currentPeak1Freq, currentPeak1Q, juce::Decibels::decibelsToGain(currentPeak1Gain));
        peak1Response.push_back(juce::Decibels::gainToDecibels(
            std::abs(peak1Coeffs->getMagnitudeForFrequency(freq, currentSampleRate))));
        
        auto peak3Coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            currentSampleRate, currentPeak3Freq, currentPeak3Q, juce::Decibels::decibelsToGain(currentPeak3Gain));
        peak3Response.push_back(juce::Decibels::gainToDecibels(
            std::abs(peak3Coeffs->getMagnitudeForFrequency(freq, currentSampleRate))));
        
        auto highShelfCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            currentSampleRate, currentHighShelfFreq, 0.7f, juce::Decibels::decibelsToGain(currentHighShelfGain));
        highShelfResponse.push_back(juce::Decibels::gainToDecibels(
            std::abs(highShelfCoeffs->getMagnitudeForFrequency(freq, currentSampleRate))));
    }
}

void WorkstationProcessor::getAudioWaveform(std::vector<float>& waveformData)
{
    waveformData.clear();
    waveformData.reserve(waveformSize);
    
    // Copy from circular buffer starting from current position
    for (int i = 0; i < waveformSize; ++i)
    {
        int index = (waveformIndex + i) % waveformSize;
        waveformData.push_back(waveformBuffer[index]);
    }
}

void WorkstationProcessor::getFFTData(std::vector<float>& fftData)
{
    fftData.clear();
    fftData.reserve(fftSize / 2);
    
    // Copy FFT magnitudes  
    for (int i = 0; i < fftSize / 2; ++i)
    {
        fftData.push_back(magnitudes[i]);
    }
}

void WorkstationProcessor::getFFTPeakHold(std::vector<float>& peakHoldData)
{
    peakHoldData.clear();
    peakHoldData.reserve(fftSize / 2);
    
    // Copy FFT peak hold data
    for (int i = 0; i < fftSize / 2; ++i)
    {
        peakHoldData.push_back(fftPeakHold[i]);
    }
}

void WorkstationProcessor::processFFT(std::array<float, fftSize * 2>& fftData, std::array<float, fftSize / 2>& magnitudes)
{
    // Copy data for FFT processing
    std::array<float, fftSize * 2> tempFFTData;
    std::fill(tempFFTData.begin(), tempFFTData.end(), 0.0f);
    
    // Copy real data to first half
    for (int j = 0; j < fftSize; ++j)
    {
        tempFFTData[j] = fftData[j];
    }
    
    // Perform FFT
    forwardFFT.performFrequencyOnlyForwardTransform(tempFFTData.data());
    
    // Convert to magnitudes with enhanced smoothing
    std::vector<float> tempMagnitudes(fftSize / 2);
    
    for (int j = 0; j < fftSize / 2; ++j)
    {
        float real = tempFFTData[j];
        float imag = tempFFTData[j + fftSize / 2];
        float magnitude = std::sqrt(real * real + imag * imag) / fftSize; // Normalise
        tempMagnitudes[j] = magnitude;
    }
    
    // Apply frequency domain smoothing (neighbour averaging)
    for (int j = 1; j < fftSize / 2 - 1; ++j)
    {
        float smoothed = tempMagnitudes[j] * 0.5f + 
                       (tempMagnitudes[j-1] + tempMagnitudes[j+1]) * 0.25f;
        
        // Enhanced temporal smoothing with different rates for attack/decay
        float currentMag = magnitudes[j];
        if (smoothed > currentMag)
        {
            // Faster attack
            magnitudes[j] = currentMag * 0.6f + smoothed * 0.4f;
        }
        else
        {
            // Slower decay for smoother appearance
            magnitudes[j] = currentMag * 0.8f + smoothed * 0.2f;
        }
    }
}

// Helper function for post-EQ with peak hold
void WorkstationProcessor::processFFTWithPeakHold(std::array<float, fftSize * 2>& fftData, std::array<float, fftSize / 2>& magnitudes)
{
    processFFT(fftData, magnitudes);
    
    // Update peak hold for post-EQ only
    for (int j = 0; j < fftSize / 2; ++j)
    {
        if (magnitudes[j] > fftPeakHold[j])
        {
            fftPeakHold[j] = magnitudes[j]; // Instant attack
        }
        else
        {
            fftPeakHold[j] *= 0.995f; // Very slow decay (hold peaks longer)
        }
    }
}

void WorkstationProcessor::setPatternPlaying(bool shouldPlay)
{
    patternPlaying = shouldPlay;
    
    // If stopping, turn off all active notes immediately
    if (!shouldPlay)
    {
        for (int i = 0; i < 128; ++i)
        {
            synth.noteOff(1, i, 0.0f, true); // All notes off
        }
        activeNotes.clear();
        patternIndex = 0;
        samplesUntilNextNote = 0;
    }
}

juce::AudioProcessorEditor* WorkstationProcessor::createEditor()
{
    return new WorkstationEditor(*this);
}

void WorkstationProcessor::generateMIDIPattern(juce::MidiBuffer& midiBuffer, int numSamples)
{
    // Define different musical modes (intervals from root)
    static const int modes[][8] = {
        {0, 2, 4, 5, 7, 9, 11, 12}, // Major (Ionian)
        {0, 2, 3, 5, 7, 9, 10, 12}, // Minor (Natural Minor/Aeolian)
        {0, 2, 3, 5, 7, 8, 10, 12}, // Dorian
        {0, 1, 3, 5, 7, 8, 10, 12}, // Phrygian
        {0, 2, 4, 6, 7, 9, 11, 12}, // Lydian
        {0, 2, 4, 5, 7, 9, 10, 12}, // Mixolydian
        {0, 1, 3, 5, 6, 8, 10, 12}, // Locrian
    };
    
    // Define melody patterns
    static const int melodyPatterns[][16] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0, 0}, // Scale up/down
        {0, 2, 4, 2, 0, 3, 5, 3, 0, 4, 6, 4, 0, 7, 0, 0}, // Arpeggios
        {0, 4, 7, 4, 0, 3, 6, 3, 0, 5, 7, 5, 0, 2, 0, 0}, // Chord tones
        {0, 2, 1, 3, 2, 4, 3, 5, 4, 6, 5, 7, 6, 7, 6, 5}, // Step pattern
        {7, 5, 3, 1, 0, 2, 4, 6, 7, 6, 4, 2, 0, 1, 3, 5}, // Descending/ascending
    };
    
    static const float velocities[] = {0.8f, 0.6f, 0.9f, 0.7f, 0.85f, 0.75f, 0.65f, 1.0f, 0.7f, 0.8f, 0.6f, 0.9f, 0.75f, 0.85f, 0.65f, 0.9f};
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        if (samplesUntilNextNote <= 0)
        {
            // Stop any currently playing notes
            for (int note : activeNotes)
            {
                midiBuffer.addEvent(juce::MidiMessage::noteOff(1, note), sample);
            }
            activeNotes.clear();
            
            // Get the current scale and pattern
            const int* currentScale = modes[currentMode];
            const int* currentPattern = melodyPatterns[melodyPattern];
            
            // Play new note(s)
            int patternStep = patternIndex % 16;
            int scaleStep = currentPattern[patternStep];
            int note = rootKey + currentScale[scaleStep];
            float velocity = velocities[patternIndex % 16];
            
            if (note >= 0 && note < 128)
            {
                midiBuffer.addEvent(juce::MidiMessage::noteOn(1, note, velocity), sample);
                activeNotes.push_back(note);
                
                // Add harmony based on mode
                if (patternIndex % 4 == 0)
                {
                    // Add third and fifth
                    int thirdNote = rootKey + currentScale[(scaleStep + 2) % 7];
                    int fifthNote = rootKey + currentScale[(scaleStep + 4) % 7];
                    
                    if (thirdNote < 128)
                    {
                        midiBuffer.addEvent(juce::MidiMessage::noteOn(1, thirdNote, velocity * 0.6f), sample);
                        activeNotes.push_back(thirdNote);
                    }
                    if (fifthNote < 128)
                    {
                        midiBuffer.addEvent(juce::MidiMessage::noteOn(1, fifthNote, velocity * 0.5f), sample);
                        activeNotes.push_back(fifthNote);
                    }
                }
                
                // Add bass root note an octave below with every melody note
                int bassNote = rootKey - 12; // One octave down
                if (bassNote >= 0)
                {
                    midiBuffer.addEvent(juce::MidiMessage::noteOn(1, bassNote, velocity * 0.6f), sample);
                    activeNotes.push_back(bassNote);
                }
            }
            
            // Sync to host tempo (Logic Pro, etc.) or use fixed tempo for standalone
            auto playHead = getPlayHead();
            if (playHead != nullptr)
            {
                auto positionInfo = playHead->getPosition();
                if (positionInfo.hasValue() && positionInfo->getBpm().hasValue())
                {
                    // Use host BPM for perfect sync with Logic Pro
                    double hostBpm = *positionInfo->getBpm();
                    double notesPerSecond = hostBpm / 60.0 * 2.0; // 2 notes per beat (eighth notes)
                    int hostSyncedSamples = (int)(currentSampleRate / notesPerSecond);
                    samplesUntilNextNote = hostSyncedSamples;
                }
                else
                {
                    // Fallback to fixed tempo if host info unavailable
                    samplesUntilNextNote = samplesPerNote;
                }
            }
            else
            {
                // Standalone mode - use fixed tempo
                samplesUntilNextNote = samplesPerNote;
            }
            patternIndex++;
        }
        
        samplesUntilNextNote--;
    }
}

// MIDI device management functions
juce::StringArray WorkstationProcessor::getAvailableMidiDevices()
{
    auto devices = juce::MidiInput::getAvailableDevices();
    availableMidiDeviceNames.clear();
    
    for (const auto& device : devices)
    {
        availableMidiDeviceNames.add(device.name);
    }
    
    return availableMidiDeviceNames;
}

void WorkstationProcessor::setSelectedMidiDevice(const juce::String& deviceName)
{
    selectedMidiDevice = deviceName;
}

void WorkstationProcessor::refreshMidiDevices()
{
    auto devices = juce::MidiInput::getAvailableDevices();
    availableMidiDeviceNames.clear();
    
    for (const auto& device : devices)
    {
        availableMidiDeviceNames.add(device.name);
    }
    
    // Auto-select most recently connected device (last in list)
    if (!devices.isEmpty() && selectedMidiDevice.isEmpty())
    {
        selectedMidiDevice = devices.getLast().name;
    }
    // Check if previously selected device is still available
    else if (!selectedMidiDevice.isEmpty())
    {
        bool deviceStillAvailable = false;
        for (const auto& device : devices)
        {
            if (device.name == selectedMidiDevice)
            {
                deviceStillAvailable = true;
                break;
            }
        }
        
        // If previous device is gone, select the most recent one
        if (!deviceStillAvailable && !devices.isEmpty())
        {
            selectedMidiDevice = devices.getLast().name;
        }
    }
}

void WorkstationProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save both parameter state and MIDI device preference
    auto state = valueTreeState.copyState();
    
    // Add MIDI device selection to the state
    juce::ValueTree midiSettings("MidiSettings");
    midiSettings.setProperty("selectedDevice", selectedMidiDevice, nullptr);
    state.appendChild(midiSettings, nullptr);
    
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void WorkstationProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(valueTreeState.state.getType()))
        {
            juce::ValueTree newState = juce::ValueTree::fromXml(*xmlState);
            
            // Restore MIDI device preference
            auto midiSettings = newState.getChildWithName("MidiSettings");
            if (midiSettings.isValid())
            {
                selectedMidiDevice = midiSettings.getProperty("selectedDevice", "");
            }
            
            valueTreeState.replaceState(newState);
        }
    }
}

// Required for standalone builds
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WorkstationProcessor();
}