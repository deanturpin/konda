#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include "WorkstationProcessor.h"

class EQVisualizerComponent : public juce::Component, public juce::Timer
{
public:
    EQVisualizerComponent(WorkstationProcessor& p) : processor(p)
    {
        startTimer(60); // 15fps updates (60ms interval)
        generateRandomColours(); // Start with random colours
    }
    
    void generateRandomColours()
    {
        auto& random = juce::Random::getSystemRandom();
        
        // Generate two random base hues for gradient
        baseHue1 = random.nextFloat(); // 0.0 to 1.0
        baseHue2 = random.nextFloat();
        
        // Ensure the hues are different enough for good contrast
        while (std::abs(baseHue1 - baseHue2) < 0.2f)
        {
            baseHue2 = random.nextFloat();
        }
    }
    
    void onPlayPressed()
    {
        generateRandomColours();
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        
        auto bounds = getLocalBounds().toFloat().reduced(15);
        
        // Full-screen FFT Spectrum Analyser
        processor.getFFTData(fftData);
        processor.getFFTPeakHold(peakHoldData);
        
        if (!fftData.empty() && !peakHoldData.empty())
        {
            
            // Spectrum bars - main colorful display
            for (size_t i = 1; i < fftData.size() && i < 300; ++i)
            {
                float freq = (float)i * (22050.0f / 512.0f);
                if (freq < 30.0f || freq > 30000.0f) continue;
                
                auto x = juce::mapFromLog10(freq, 30.0f, 30000.0f) * bounds.getWidth() + bounds.getX();
                
                float magnitude = fftData[i];
                float peakMagnitude = peakHoldData[i];
                
                float scaledMag = juce::jlimit(0.0f, 1.0f, magnitude * 2000.0f);
                float scaledPeak = juce::jlimit(0.0f, 1.0f, peakMagnitude * 2000.0f);
                
                auto height = scaledMag * bounds.getHeight() * 0.85f;
                auto peakHeight = scaledPeak * bounds.getHeight() * 0.85f;
                auto y = bounds.getBottom() - height;
                auto peakY = bounds.getBottom() - peakHeight;
                
                // Simple two-colour gradient based on frequency position
                float freqPosition = (float)i / 300.0f; // Position in spectrum (0.0 to 1.0)
                
                // Interpolate between the two base hues
                float hue = baseHue1 + (baseHue2 - baseHue1) * freqPosition;
                
                // Enhanced spectrum with glow effect 🌟
                // Background glow for stronger signals
                if (scaledMag > 0.3f)
                {
                    g.setColour(juce::Colour::fromHSV(hue, 0.6f, 0.4f + scaledMag * 0.3f, 0.3f));
                    g.drawLine(x, bounds.getBottom(), x, y, 6.0f); // Wider glow
                }
                
                // Main spectrum bar with enhanced brightness dynamics
                float dynamicSaturation = 0.7f + scaledMag * 0.3f; // More vivid at higher levels
                float dynamicBrightness = 0.5f + scaledMag * 0.5f; // Brighter at higher levels
                g.setColour(juce::Colour::fromHSV(hue, dynamicSaturation, dynamicBrightness, 0.85f));
                g.drawLine(x, bounds.getBottom(), x, y, 3.0f);
                
                // Enhanced peak hold with sparkle effect
                if (scaledPeak > 0.01f)
                {
                    // Bright peak indicator
                    g.setColour(juce::Colour::fromHSV(hue, 0.9f, 0.9f + scaledPeak * 0.1f, 0.95f));
                    g.drawLine(x, peakY - 1, x, peakY + 1, 2.5f);
                    
                    // Add sparkle for very high peaks
                    if (scaledPeak > 0.7f)
                    {
                        g.setColour(juce::Colours::white.withAlpha(0.8f));
                        g.fillEllipse(x - 1, peakY - 1, 2, 2); // Sparkle dot
                    }
                }
            }
            
            // Draw EQ response curve overlay
            processor.getFrequencyResponse(frequencies, magnitudes);
            if (!frequencies.empty() && !magnitudes.empty())
            {
                juce::Path eqCurvePath;
                bool pathStarted = false;
                
                for (size_t i = 0; i < frequencies.size(); ++i)
                {
                    float freq = frequencies[i];
                    if (freq < 30.0f || freq > 18000.0f) continue;
                    
                    auto x = juce::mapFromLog10(freq, 30.0f, 30000.0f) * bounds.getWidth() + bounds.getX();
                    
                    // Convert dB to visual position (centre line = 0dB, ±24dB range)
                    float gainDb = magnitudes[i];
                    float normalizedGain = (gainDb + 24.0f) / 48.0f; // Map -24dB to +24dB into 0-1 range
                    normalizedGain = juce::jlimit(0.0f, 1.0f, normalizedGain);
                    
                    auto y = bounds.getBottom() - (normalizedGain * bounds.getHeight());
                    
                    if (!pathStarted)
                    {
                        eqCurvePath.startNewSubPath(x, y);
                        pathStarted = true;
                    }
                    else
                    {
                        eqCurvePath.lineTo(x, y);
                    }
                }
                
                // Draw EQ curve as bright white line
                g.setColour(juce::Colours::white.withAlpha(0.9f));
                g.strokePath(eqCurvePath, juce::PathStrokeType(2.5f));
                
                // Draw centre reference line (0dB)
                float centreY = bounds.getCentreY();
                g.setColour(juce::Colours::white.withAlpha(0.3f));
                g.drawLine(bounds.getX(), centreY, bounds.getRight(), centreY, 1.0f);
            }
            
            // REVOLUTIONARY MULTI-COLORED EQ VISUALIZATION! 🎨
            processor.getIndividualBandResponses(frequencies, lowShelfResponse, peak1Response, 
                                               peak3Response, highShelfResponse);
            if (!frequencies.empty() && !lowShelfResponse.empty())
            {
                // Individual colored band responses
                auto drawBandResponse = [&](const std::vector<float>& response, juce::Colour color) {
                    juce::Path bandPath;
                    bool pathStarted = false;
                    
                    for (size_t i = 0; i < frequencies.size(); ++i)
                    {
                        float freq = frequencies[i];
                        if (freq < 30.0f || freq > 18000.0f) continue;
                        
                        auto x = juce::mapFromLog10(freq, 30.0f, 30000.0f) * bounds.getWidth() + bounds.getX();
                        
                        // Convert dB to visual position
                        float gainDb = response[i];
                        float normalizedGain = (gainDb + 24.0f) / 48.0f;
                        normalizedGain = juce::jlimit(0.0f, 1.0f, normalizedGain);
                        
                        auto y = bounds.getBottom() - (normalizedGain * bounds.getHeight());
                        
                        if (!pathStarted)
                        {
                            bandPath.startNewSubPath(x, y);
                            pathStarted = true;
                        }
                        else
                        {
                            bandPath.lineTo(x, y);
                        }
                    }
                    
                    // Draw colored band response
                    g.setColour(color.withAlpha(0.7f));
                    g.strokePath(bandPath, juce::PathStrokeType(1.8f));
                };
                
                // Draw each EQ band in its signature color
                drawBandResponse(lowShelfResponse, juce::Colours::red);           // Red: Low Shelf
                drawBandResponse(peak1Response, juce::Colours::orange);          // Orange: Peak 1  
                drawBandResponse(peak3Response, juce::Colours::lightblue);       // Light Blue: Peak 3
                drawBandResponse(highShelfResponse, juce::Colours::cyan);        // Cyan: High Shelf
            }
            
            // Draw professional grid
            g.setFont(juce::FontOptions(12.0f));
            
            // Major frequency lines with labels (logarithmically spaced)
            g.setColour(juce::Colours::darkgrey.withAlpha(0.6f));
            for (float freq : {50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000})
            {
                if (freq >= 30.0f && freq <= 30000.0f)
                {
                    auto x = juce::mapFromLog10(freq, 30.0f, 30000.0f) * bounds.getWidth() + bounds.getX();
                    g.drawLine(x, bounds.getY(), x, bounds.getBottom(), 1.0f);
                    
                    // Frequency labels
                    juce::String label = freq >= 1000.0f ? juce::String(freq / 1000.0f, 1) + "k" : juce::String((int)freq);
                    g.setColour(juce::Colours::lightgrey);
                    g.drawText(label, x - 15, bounds.getBottom() + 5, 30, 15, juce::Justification::centred);
                    g.setColour(juce::Colours::darkgrey.withAlpha(0.6f)); // Reset color for next line
                }
            }
            
            // Level grid lines
            g.setColour(juce::Colours::darkgrey.withAlpha(0.3f));
            for (int i = 1; i < 5; ++i)
            {
                auto y = bounds.getY() + (bounds.getHeight() * i / 5);
                g.drawLine(bounds.getX(), y, bounds.getRight(), y, 1.0f);
            }
        }
        
        // Konda by Turbeaux Sounds - Audio-Reactive Branding
        processor.getFFTData(fftData);
        float audioLevel = 0.0f;
        if (!fftData.empty())
        {
            // Calculate average audio level for pulsing effect
            for (size_t i = 1; i < std::min(fftData.size(), size_t(100)); ++i)
            {
                audioLevel += fftData[i];
            }
            audioLevel /= 100.0f;
            audioLevel = juce::jlimit(0.0f, 1.0f, audioLevel * 1000.0f); // Scale and limit
        }
        
        // Pulsing transparency based on audio level
        float pulseAlpha = 0.4f + audioLevel * 0.3f; // Base 40% + up to 30% more
        
        // Add subtle color tint based on current hue
        float tintAmount = audioLevel * 0.15f; // Subtle color influence
        juce::Colour brandColor = juce::Colours::white.withAlpha(pulseAlpha);
        if (tintAmount > 0.05f)
        {
            brandColor = juce::Colour::fromHSV(baseHue1, tintAmount, 1.0f, pulseAlpha);
        }
        
        g.setColour(brandColor);
        g.setFont(juce::FontOptions(108.0f).withStyle("bold"));
        g.drawText("KONDA", bounds, juce::Justification::centred);
    }
    
    void timerCallback() override
    {
        repaint();
    }
    
private:
    WorkstationProcessor& processor;
    std::vector<float> frequencies, magnitudes;
    std::vector<float> waveformData, fftData, peakHoldData;
    std::vector<float> lowShelfResponse, peak1Response, peak3Response, highShelfResponse;
    float baseHue1, baseHue2;
};

class MIDIPatternComponent : public juce::Component
{
public:
    MIDIPatternComponent(WorkstationProcessor& p) : processor(p)
    {
        // Only show play button in standalone mode (not when running as plugin)
        bool isStandalone = processor.wrapperType == juce::AudioProcessor::wrapperType_Standalone;

        playStopButton.setButtonText("Play");
        playStopButton.onClick = [this] {
            if (processor.isPatternPlaying()) {
                // Stop
                processor.setPatternPlaying(false);
                playStopButton.setButtonText("Play");
            } else {
                // Play
                processor.setPatternPlaying(true);
                playStopButton.setButtonText("Stop");

                // Trigger colour change in visualiser
                if (onPlayCallback)
                    onPlayCallback();
            }
        };

        // Only make visible in standalone mode
        if (isStandalone) {
            addAndMakeVisible(playStopButton);
        }

        // Key controls - up/down buttons with display
        keyUpButton.setButtonText("▲");
        keyDownButton.setButtonText("▼");
        currentKey = 0; // Start with C

        keyUpButton.onClick = [this] {
            currentKey = (currentKey + 1) % 12;
            processor.setKey(60 + currentKey); // C4 + offset
            updateKeyDisplay();
        };

        keyDownButton.onClick = [this] {
            currentKey = (currentKey + 11) % 12; // +11 = -1 in mod 12
            processor.setKey(60 + currentKey);
            updateKeyDisplay();
        };

        // Only add key controls in standalone mode
        if (isStandalone) {
            addAndMakeVisible(keyUpButton);
            addAndMakeVisible(keyDownButton);
            addAndMakeVisible(keyDisplayLabel);
        }

        // Mode selector
        modeSelector.addItem("Major", 1);
        modeSelector.addItem("Minor", 2);
        modeSelector.addItem("Dorian", 3);
        modeSelector.addItem("Phrygian", 4);
        modeSelector.addItem("Lydian", 5);
        modeSelector.addItem("Mixolydian", 6);
        modeSelector.addItem("Locrian", 7);
        modeSelector.setSelectedId(1); // Major
        modeSelector.onChange = [this] {
            processor.setMode(modeSelector.getSelectedId() - 1);
        };

        // Pattern selector
        patternSelector.addItem("Scale", 1);
        patternSelector.addItem("Arpeggios", 2);
        patternSelector.addItem("Chord Tones", 3);
        patternSelector.addItem("Step Pattern", 4);
        patternSelector.addItem("Cascading", 5);
        patternSelector.setSelectedId(1); // Scale
        patternSelector.onChange = [this] {
            processor.setMelodyPattern(patternSelector.getSelectedId() - 1);
        };

        // Only add selectors in standalone mode
        if (isStandalone) {
            addAndMakeVisible(modeSelector);
            addAndMakeVisible(patternSelector);
        }
        
        keyDisplayLabel.setText("C", juce::dontSendNotification);
        keyDisplayLabel.setJustificationType(juce::Justification::centred);
        keyDisplayLabel.setFont(juce::FontOptions(14.0f).withStyle("bold"));
        
    }
    
    void updateKeyDisplay()
    {
        static const juce::String keyNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
        juce::String keyName = keyNames[currentKey];
        keyDisplayLabel.setText(keyName, juce::dontSendNotification);
    }
    
    void resized() override
    {
        auto area = getLocalBounds().reduced(5);

        // Top row: Key controls, selectors and buttons (only in standalone mode)
        auto controlRow = area.removeFromTop(30);

        // Only layout controls if they're visible (standalone mode)
        if (keyUpButton.isVisible() && keyDownButton.isVisible()) {
            // Key controls on left (compact design)
            auto keyArea = controlRow.removeFromLeft(80);
            auto buttonWidth = 25;
            keyDownButton.setBounds(keyArea.removeFromLeft(buttonWidth).reduced(2));
            keyDisplayLabel.setBounds(keyArea.removeFromLeft(30).reduced(2));
            keyUpButton.setBounds(keyArea.removeFromLeft(buttonWidth).reduced(2));

            controlRow.removeFromLeft(10); // Spacing
        }

        // Mode and pattern selectors in middle (only if visible)
        if (modeSelector.isVisible() && patternSelector.isVisible()) {
            auto selectorWidth = controlRow.getWidth() * 0.6f / 2; // Take 60% for 2 selectors
            modeSelector.setBounds(controlRow.removeFromLeft(selectorWidth).reduced(15, 2));
            patternSelector.setBounds(controlRow.removeFromLeft(selectorWidth).reduced(15, 2));

            controlRow.removeFromLeft(10); // Spacing
        }

        // Play button on right (only if visible)
        if (playStopButton.isVisible()) {
            playStopButton.setBounds(controlRow.removeFromRight(70).reduced(2));
        }
    }
    
    void setOnPlayCallback(std::function<void()> callback)
    {
        onPlayCallback = callback;
    }
    
    void randomize()
    {
        // Only randomize if selectors are visible (standalone mode)
        if (!modeSelector.isVisible() || !patternSelector.isVisible()) {
            return;
        }

        // Only randomize mode and pattern, keep the key unchanged
        // Get current mode to avoid picking the same one
        int currentMode = modeSelector.getSelectedId();
        int newMode;
        do {
            newMode = 1 + juce::Random::getSystemRandom().nextInt(7);
        } while (newMode == currentMode && juce::Random::getSystemRandom().nextFloat() < 0.7f); // 70% chance to pick different mode

        int currentPattern = patternSelector.getSelectedId();
        int newPattern;
        do {
            newPattern = 1 + juce::Random::getSystemRandom().nextInt(5);
        } while (newPattern == currentPattern && juce::Random::getSystemRandom().nextFloat() < 0.8f); // 80% chance to pick different pattern

        modeSelector.setSelectedId(newMode);
        patternSelector.setSelectedId(newPattern);

        // Update the processor (key stays the same)
        processor.setMode(modeSelector.getSelectedId() - 1);
        processor.setMelodyPattern(patternSelector.getSelectedId() - 1);
    }
    
private:
    WorkstationProcessor& processor;
    juce::TextButton playStopButton;
    juce::TextButton keyUpButton, keyDownButton;
    juce::Label keyDisplayLabel;
    juce::ComboBox modeSelector, patternSelector;
    int currentKey;
    std::function<void()> onPlayCallback;
};

class WorkstationEditor : public juce::AudioProcessorEditor
{
public:
    WorkstationEditor(WorkstationProcessor&);
    ~WorkstationEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    void randomizeSynth();
    void randomizeEQ();
    void randomizeReverb();
    void refreshMidiDevices();

private:
    WorkstationProcessor& processor;
    
    // Synth controls
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    juce::Slider filterCutoffSlider, filterResonanceSlider, distortionSlider;
    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel;
    juce::Label filterCutoffLabel, filterResonanceLabel, distortionLabel;
    
    // EQ controls
    juce::Slider lowShelfFreqSlider, lowShelfGainSlider;
    juce::Slider peak1FreqSlider, peak1GainSlider, peak1QSlider;
    juce::Slider peak3FreqSlider, peak3GainSlider, peak3QSlider;
    juce::Slider highShelfFreqSlider, highShelfGainSlider;
    
    // EQ labels
    juce::Label lowShelfLabel, peak1Label, peak3Label, highShelfLabel;
    
    // Reverb controls
    juce::Slider reverbRoomSizeSlider, reverbDampingSlider, reverbWetLevelSlider, reverbDryLevelSlider;
    juce::Label reverbRoomSizeLabel, reverbDampingLabel, reverbWetLevelLabel, reverbDryLevelLabel;
    
    
    // Visualizer and MIDI
    std::unique_ptr<EQVisualizerComponent> eqVisualizer;
    std::unique_ptr<MIDIPatternComponent> midiPattern;
    
    // MIDI device selection
    juce::ComboBox midiDeviceSelector;
    juce::Label midiDeviceLabel;
    juce::TextButton refreshMidiButton;

    // Octave selection
    juce::Label octaveLabel;
    juce::ToggleButton octave2Button, octave3Button, octave4Button, octave5Button, octave6Button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> octave2Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> octave3Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> octave4Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> octave5Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> octave6Attachment;
    
    // Section labels
    juce::Label synthLabel, eqLabel, reverbLabel, midiLabel;
    
    // Randomize buttons
    juce::TextButton synthRandomizeButton, eqRandomizeButton, reverbRandomizeButton, midiRandomizeButton, globalRandomizeButton;
    
    // Attachments
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WorkstationEditor)
};