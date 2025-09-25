#include "WorkstationEditor.h"

WorkstationEditor::WorkstationEditor(WorkstationProcessor& p) : juce::AudioProcessorEditor(&p), processor(p)
{
    // Main sections
    synthLabel.setText("SYNTHESIZER", juce::dontSendNotification);
    synthLabel.setFont(juce::FontOptions(16.0f).withStyle("bold"));
    synthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(synthLabel);
    
    eqLabel.setText("4-BAND PARAMETRIC EQ", juce::dontSendNotification);
    eqLabel.setFont(juce::FontOptions(16.0f).withStyle("bold"));
    eqLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(eqLabel);
    
    reverbLabel.setText("REVERB", juce::dontSendNotification);
    reverbLabel.setFont(juce::FontOptions(16.0f).withStyle("bold"));
    reverbLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbLabel);
    
    // Check if running as standalone or plugin
    bool isStandalone = processor.wrapperType == juce::AudioProcessor::wrapperType_Standalone;

    // Only show MIDI Pattern Generator in standalone mode
    if (isStandalone) {
        midiLabel.setText("MIDI PATTERN GENERATOR", juce::dontSendNotification);
        midiLabel.setFont(juce::FontOptions(16.0f).withStyle("bold"));
        midiLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(midiLabel);
    }
    
    // Synth controls setup - using horizontal linear sliders
    auto setupSlider = [this](juce::Slider& slider, juce::Label& label, const juce::String& text, const juce::String& paramId)
    {
        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(slider);
        
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centredLeft);
        label.setFont(juce::FontOptions(12.0f));
        addAndMakeVisible(label);
        
        sliderAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.getValueTreeState(), paramId, slider));
    };
    
    // Synth ADSR controls
    setupSlider(attackSlider, attackLabel, "Attack", "attack");
    setupSlider(decaySlider, decayLabel, "Decay", "decay");
    setupSlider(sustainSlider, sustainLabel, "Sustain", "sustain");
    setupSlider(releaseSlider, releaseLabel, "Release", "release");
    
    // Synth filter controls
    setupSlider(filterCutoffSlider, filterCutoffLabel, "Cutoff", "filterCutoff");
    filterCutoffSlider.setSkewFactor(0.25f); // More logarithmic scaling for better low-frequency control
    setupSlider(filterResonanceSlider, filterResonanceLabel, "Resonance", "filterResonance");
    filterResonanceSlider.setSkewFactor(0.5f); // Logarithmic scaling - more control in the sweet spot (0.1-2.0)
    setupSlider(distortionSlider, distortionLabel, "Distortion", "distortion");

    // New synthesis controls
    // Waveform selector
    waveformLabel.setText("Waveform", juce::dontSendNotification);
    waveformLabel.setJustificationType(juce::Justification::centredLeft);
    waveformLabel.setFont(juce::FontOptions(12.0f));
    addAndMakeVisible(waveformLabel);

    waveformSelector.addItemList({"Sine", "Sawtooth", "Square", "Triangle"}, 1);
    addAndMakeVisible(waveformSelector);
    waveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.getValueTreeState(), "waveform", waveformSelector);

    // Filter type selector
    filterTypeLabel.setText("Filter Type", juce::dontSendNotification);
    filterTypeLabel.setJustificationType(juce::Justification::centredLeft);
    filterTypeLabel.setFont(juce::FontOptions(12.0f));
    addAndMakeVisible(filterTypeLabel);

    filterTypeSelector.addItemList({"Lowpass", "Highpass", "Bandpass", "Notch"}, 1);
    addAndMakeVisible(filterTypeSelector);
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.getValueTreeState(), "filterType", filterTypeSelector);


    // LFO controls
    setupSlider(lfoRateSlider, lfoRateLabel, "LFO Rate", "lfoRate");
    setupSlider(lfoDepthSlider, lfoDepthLabel, "LFO Depth", "lfoDepth");

    // LFO waveform selector
    lfoWaveformLabel.setText("LFO Wave", juce::dontSendNotification);
    lfoWaveformLabel.setJustificationType(juce::Justification::centredLeft);
    lfoWaveformLabel.setFont(juce::FontOptions(12.0f));
    addAndMakeVisible(lfoWaveformLabel);

    lfoWaveformSelector.addItemList({"Sine", "Sawtooth", "Square", "Triangle"}, 1);
    addAndMakeVisible(lfoWaveformSelector);
    lfoWaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.getValueTreeState(), "lfoWaveform", lfoWaveformSelector);
    
    // EQ controls
    setupSlider(lowShelfFreqSlider, lowShelfLabel, "Low Shelf", "lowShelfFreq");
    setupSlider(lowShelfGainSlider, lowShelfLabel, "Low Gain", "lowShelfGain");
    
    setupSlider(peak1FreqSlider, peak1Label, "Peak 1 Freq", "peak1Freq");
    setupSlider(peak1GainSlider, peak1Label, "Peak 1 Gain", "peak1Gain");
    setupSlider(peak1QSlider, peak1Label, "Peak 1 Q", "peak1Q");
    
    
    setupSlider(peak3FreqSlider, peak3Label, "Peak 3 Freq", "peak3Freq");
    setupSlider(peak3GainSlider, peak3Label, "Peak 3 Gain", "peak3Gain");
    setupSlider(peak3QSlider, peak3Label, "Peak 3 Q", "peak3Q");
    
    setupSlider(highShelfFreqSlider, highShelfLabel, "High Shelf", "highShelfFreq");
    setupSlider(highShelfGainSlider, highShelfLabel, "High Gain", "highShelfGain");
    
    // Reverb controls
    setupSlider(reverbRoomSizeSlider, reverbRoomSizeLabel, "Room Size", "reverbRoomSize");
    setupSlider(reverbDampingSlider, reverbDampingLabel, "Damping", "reverbDamping");
    setupSlider(reverbWetLevelSlider, reverbWetLevelLabel, "Wet Level", "reverbWetLevel");
    setupSlider(reverbDryLevelSlider, reverbDryLevelLabel, "Dry Level", "reverbDryLevel");
    
    
    // Create visualizer and MIDI pattern components
    eqVisualizer = std::make_unique<EQVisualizerComponent>(processor);
    addAndMakeVisible(*eqVisualizer);
    
    midiPattern = std::make_unique<MIDIPatternComponent>(processor);
    addAndMakeVisible(*midiPattern);
    
    // Connect MIDI play button to visualiser colour changes
    midiPattern->setOnPlayCallback([this]() { 
        eqVisualizer->onPlayPressed(); 
    });
    
    // Setup MIDI device selector (only visible in standalone mode)
    // (isStandalone already defined above)

    midiDeviceLabel.setText("MIDI Device:", juce::dontSendNotification);
    midiDeviceLabel.setJustificationType(juce::Justification::centredLeft);
    midiDeviceLabel.setFont(juce::FontOptions(12.0f));

    refreshMidiButton.setButtonText("Refresh");
    refreshMidiButton.onClick = [this]() { refreshMidiDevices(); };
    
    midiDeviceSelector.onChange = [this]() {
        int selectedIndex = midiDeviceSelector.getSelectedItemIndex();
        if (selectedIndex >= 0)
        {
            juce::String deviceName = midiDeviceSelector.getItemText(selectedIndex);
            processor.setSelectedMidiDevice(deviceName);
        }
    };

    // Only show MIDI device controls in standalone mode
    if (isStandalone) {
        addAndMakeVisible(midiDeviceSelector);
        addAndMakeVisible(midiDeviceLabel);
        addAndMakeVisible(refreshMidiButton);

        // Initialize MIDI device list
        refreshMidiDevices();
    }
    
    // Setup randomize buttons
    synthRandomizeButton.setButtonText("Random");
    synthRandomizeButton.onClick = [this]() { randomizeSynth(); };
    addAndMakeVisible(synthRandomizeButton);
    
    eqRandomizeButton.setButtonText("Random");
    eqRandomizeButton.onClick = [this]() { randomizeEQ(); };
    addAndMakeVisible(eqRandomizeButton);
    
    reverbRandomizeButton.setButtonText("Random");
    reverbRandomizeButton.onClick = [this]() { randomizeReverb(); };
    addAndMakeVisible(reverbRandomizeButton);
    
    midiRandomizeButton.setButtonText("Random");
    midiRandomizeButton.onClick = [this]() { midiPattern->randomize(); };
    addAndMakeVisible(midiRandomizeButton);
    
    globalRandomizeButton.setButtonText("RANDOMIZE ALL");
    globalRandomizeButton.onClick = [this]() { 
        randomizeSynth(); 
        randomizeEQ(); 
        randomizeReverb();
        midiPattern->randomize(); 
    };
    addAndMakeVisible(globalRandomizeButton);
    
    // Set colors for different sections
    synthLabel.setColour(juce::Label::textColourId, juce::Colours::lightgreen);
    eqLabel.setColour(juce::Label::textColourId, juce::Colours::cyan);
    reverbLabel.setColour(juce::Label::textColourId, juce::Colours::magenta);
    midiLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    
    // Button colors for sections
    synthRandomizeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgreen);
    eqRandomizeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);
    reverbRandomizeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkmagenta);
    midiRandomizeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkorange);
    globalRandomizeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::purple);

    // Octave radio buttons setup (only in standalone mode)
    if (isStandalone) {
        octaveLabel.setText("OCTAVE", juce::dontSendNotification);
        octaveLabel.setFont(juce::FontOptions(12.0f).withStyle("bold"));
        octaveLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(octaveLabel);

        octave2Button.setButtonText("2");
        octave3Button.setButtonText("3");
        octave4Button.setButtonText("4");
        octave5Button.setButtonText("5");
        octave6Button.setButtonText("6");

        octave2Button.setRadioGroupId(1);
        octave3Button.setRadioGroupId(1);
        octave4Button.setRadioGroupId(1);
        octave5Button.setRadioGroupId(1);
        octave6Button.setRadioGroupId(1);

        // Set default octave (4)
        octave4Button.setToggleState(true, juce::dontSendNotification);

        // Octave change callbacks
        octave2Button.onClick = [this] { processor.setOctave(2); };
        octave3Button.onClick = [this] { processor.setOctave(3); };
        octave4Button.onClick = [this] { processor.setOctave(4); };
        octave5Button.onClick = [this] { processor.setOctave(5); };
        octave6Button.onClick = [this] { processor.setOctave(6); };

        addAndMakeVisible(octave2Button);
        addAndMakeVisible(octave3Button);
        addAndMakeVisible(octave4Button);
        addAndMakeVisible(octave5Button);
        addAndMakeVisible(octave6Button);
    }

    setSize(1200, 800);
    setResizable(true, true);
    setResizeLimits(800, 600, 1600, 1000);
}

void WorkstationEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void WorkstationEditor::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    
    // Bottom control strip: MIDI + Global controls
    bool isStandalone = processor.wrapperType == juce::AudioProcessor::wrapperType_Standalone;
    auto bottomStripHeight = isStandalone ? 150 : 90; // Much less height needed when MIDI controls hidden
    auto bottomStrip = bounds.removeFromBottom(bottomStripHeight);
    auto midiHeader = bottomStrip.removeFromTop(25);
    midiLabel.setBounds(midiHeader.removeFromLeft(150));
    globalRandomizeButton.setBounds(midiHeader.removeFromRight(120).reduced(2));
    midiRandomizeButton.setBounds(midiHeader.removeFromRight(80).reduced(2));

    // MIDI device selection row (only in standalone mode)
    if (isStandalone) {
        auto midiDeviceRow = bottomStrip.removeFromTop(25);
        midiDeviceLabel.setBounds(midiDeviceRow.removeFromLeft(80));
        refreshMidiButton.setBounds(midiDeviceRow.removeFromRight(70).reduced(2));
        midiDeviceSelector.setBounds(midiDeviceRow.reduced(2));
    }

    // Octave selection row (only in standalone mode)
    if (isStandalone) {
        auto octaveRow = bottomStrip.removeFromTop(30);
        octaveLabel.setBounds(octaveRow.removeFromLeft(60));
        int buttonWidth = 35;
        octave2Button.setBounds(octaveRow.removeFromLeft(buttonWidth).reduced(2));
        octave3Button.setBounds(octaveRow.removeFromLeft(buttonWidth).reduced(2));
        octave4Button.setBounds(octaveRow.removeFromLeft(buttonWidth).reduced(2));
        octave5Button.setBounds(octaveRow.removeFromLeft(buttonWidth).reduced(2));
        octave6Button.setBounds(octaveRow.removeFromLeft(buttonWidth).reduced(2));

        bottomStrip.removeFromTop(5); // Small gap
    }
    
    // MIDI pattern component in remaining bottom strip  
    midiPattern->setBounds(bottomStrip.reduced(5));
    
    // Right side: ALL controls (Synth + EQ) as horizontal sliders - increased width for better fit
    auto rightStrip = bounds.removeFromRight(350);
    
    // Synth section header
    auto synthHeader = rightStrip.removeFromTop(25);
    synthLabel.setBounds(synthHeader.removeFromLeft(150));
    synthRandomizeButton.setBounds(synthHeader.removeFromRight(80).reduced(2));
    
    // Synth controls (13 controls now - expanded from 7)
    auto synthControls = rightStrip.removeFromTop(260); // Compact height for all controls
    int sliderHeight = 18; // Reduced from 22
    int spacing = 1; // Reduced from 2
    int labelWidth = 75; // Slightly narrower labels

    auto setupSliderRow = [&](juce::Slider& slider, juce::Label& label) {
        auto row = synthControls.removeFromTop(sliderHeight);
        label.setBounds(row.removeFromLeft(labelWidth));
        slider.setBounds(row.reduced(2, 0));
        synthControls.removeFromTop(spacing);
    };

    auto setupComboRow = [&](juce::ComboBox& combo, juce::Label& label) {
        auto row = synthControls.removeFromTop(sliderHeight);
        label.setBounds(row.removeFromLeft(labelWidth));
        combo.setBounds(row.reduced(2, 0));
        synthControls.removeFromTop(spacing);
    };

    // Waveform and filter type at the top
    setupComboRow(waveformSelector, waveformLabel);
    setupComboRow(filterTypeSelector, filterTypeLabel);

    // ADSR controls
    setupSliderRow(attackSlider, attackLabel);
    setupSliderRow(decaySlider, decayLabel);
    setupSliderRow(sustainSlider, sustainLabel);
    setupSliderRow(releaseSlider, releaseLabel);

    // Filter controls
    setupSliderRow(filterCutoffSlider, filterCutoffLabel);
    setupSliderRow(filterResonanceSlider, filterResonanceLabel);

    // New synthesis controls
    setupSliderRow(distortionSlider, distortionLabel);

    // LFO controls
    setupSliderRow(lfoRateSlider, lfoRateLabel);
    setupSliderRow(lfoDepthSlider, lfoDepthLabel);
    setupComboRow(lfoWaveformSelector, lfoWaveformLabel);
    
    rightStrip.removeFromTop(10); // Section spacing
    
    // EQ section header
    auto eqHeader = rightStrip.removeFromTop(25);
    eqLabel.setBounds(eqHeader.removeFromLeft(120));
    eqRandomizeButton.setBounds(eqHeader.removeFromRight(80).reduced(2));
    
    // EQ controls (180px for 4 bands - compact layout)
    auto eqControls = rightStrip.removeFromTop(180);
    
    // Since we don't have individual labels for freq/gain/Q, we'll create descriptive text via lambda
    auto setupEQRow = [&](juce::Slider& slider, juce::Label& label, const juce::String& text) {
        auto row = eqControls.removeFromTop(sliderHeight);
        label.setText(text, juce::dontSendNotification);
        label.setBounds(row.removeFromLeft(labelWidth));
        slider.setBounds(row.reduced(2, 0));
        eqControls.removeFromTop(spacing);
    };
    
    // Start with high frequencies at the top
    setupEQRow(highShelfFreqSlider, highShelfLabel, "High Freq");
    
    auto highGainRow = eqControls.removeFromTop(sliderHeight);
    highGainRow.removeFromLeft(labelWidth);
    highShelfGainSlider.setBounds(highGainRow.reduced(2, 0));
    eqControls.removeFromTop(spacing);
    
    setupEQRow(peak3FreqSlider, peak3Label, "Peak3 Freq");
    
    auto p3GainRow = eqControls.removeFromTop(sliderHeight);
    p3GainRow.removeFromLeft(labelWidth);
    peak3GainSlider.setBounds(p3GainRow.reduced(2, 0));
    eqControls.removeFromTop(spacing);
    
    auto p3QRow = eqControls.removeFromTop(sliderHeight);
    p3QRow.removeFromLeft(labelWidth);
    peak3QSlider.setBounds(p3QRow.reduced(2, 0));
    eqControls.removeFromTop(spacing);
    
    
    setupEQRow(peak1FreqSlider, peak1Label, "Peak1 Freq");
    
    auto p1GainRow = eqControls.removeFromTop(sliderHeight);
    p1GainRow.removeFromLeft(labelWidth); // Skip label space
    peak1GainSlider.setBounds(p1GainRow.reduced(2, 0));
    eqControls.removeFromTop(spacing);
    
    auto p1QRow = eqControls.removeFromTop(sliderHeight);
    p1QRow.removeFromLeft(labelWidth); // Skip label space  
    peak1QSlider.setBounds(p1QRow.reduced(2, 0));
    eqControls.removeFromTop(spacing);
    
    setupEQRow(lowShelfFreqSlider, lowShelfLabel, "Low Freq");
    
    // Low gain at the bottom
    auto lowGainRow = eqControls.removeFromTop(sliderHeight);
    lowGainRow.removeFromLeft(labelWidth); // Skip label space
    lowShelfGainSlider.setBounds(lowGainRow.reduced(2, 0));
    
    rightStrip.removeFromTop(10); // Section spacing
    
    // Reverb section header
    auto reverbHeader = rightStrip.removeFromTop(25);
    reverbLabel.setBounds(reverbHeader.removeFromLeft(80));
    reverbRandomizeButton.setBounds(reverbHeader.removeFromRight(80).reduced(2));
    
    // Reverb controls (4 horizontal sliders - compact)
    auto reverbControls = rightStrip.removeFromTop(90);
    
    auto setupReverbRow = [&](juce::Slider& slider, juce::Label& label, const juce::String& text) {
        auto row = reverbControls.removeFromTop(sliderHeight);
        label.setText(text, juce::dontSendNotification);
        label.setBounds(row.removeFromLeft(labelWidth));
        slider.setBounds(row.reduced(2, 0));
        reverbControls.removeFromTop(spacing);
    };
    
    setupReverbRow(reverbRoomSizeSlider, reverbRoomSizeLabel, "Room Size");
    setupReverbRow(reverbDampingSlider, reverbDampingLabel, "Damping");
    setupReverbRow(reverbWetLevelSlider, reverbWetLevelLabel, "Wet Level");
    setupReverbRow(reverbDryLevelSlider, reverbDryLevelLabel, "Dry Level");
    
    rightStrip.removeFromTop(10); // Section spacing
    
    
    bounds.removeFromRight(10); // Spacing
    
    // CENTER: Large FFT spectrum analyzer takes remaining space
    eqVisualizer->setBounds(bounds);
}

void WorkstationEditor::randomizeSynth()
{
    auto& random = juce::Random::getSystemRandom();

    // Randomize waveform type (0-3: Sine, Saw, Square, Triangle)
    waveformSelector.setSelectedItemIndex(random.nextInt(4));

    // Randomize filter type (0-3: Lowpass, Highpass, Bandpass, Notch)
    filterTypeSelector.setSelectedItemIndex(random.nextInt(4));

    // Randomize ADSR (keeping musical values)
    attackSlider.setValue(random.nextFloat() * 2.0f); // 0-2 seconds
    decaySlider.setValue(random.nextFloat() * 3.0f); // 0-3 seconds
    sustainSlider.setValue(0.1f + random.nextFloat() * 0.8f); // 0.1-0.9
    releaseSlider.setValue(random.nextFloat() * 4.0f); // 0-4 seconds

    // Randomize filter (keeping reasonable values)
    filterCutoffSlider.setValue(100.0f + random.nextFloat() * 3000.0f); // 100-3100 Hz (musical range)
    filterResonanceSlider.setValue(0.1f + random.nextFloat() * 3.0f); // 0.1-3.1 (musical range)


    // Randomize distortion (mild to heavy)
    distortionSlider.setValue(1.0f + random.nextFloat() * 5.0f); // 1.0-6.0

    // Randomize LFO parameters
    lfoRateSlider.setValue(0.1f + random.nextFloat() * 10.0f); // 0.1-10 Hz (musical range)
    lfoDepthSlider.setValue(random.nextFloat() * 0.5f); // 0-0.5 (not too extreme)
    lfoWaveformSelector.setSelectedItemIndex(random.nextInt(4)); // Random LFO waveform
}

void WorkstationEditor::randomizeEQ()
{
    auto& random = juce::Random::getSystemRandom();
    
    // Low shelf
    lowShelfFreqSlider.setValue(80.0f + random.nextFloat() * 320.0f); // 80-400 Hz
    lowShelfGainSlider.setValue(-12.0f + random.nextFloat() * 24.0f); // -12 to +12 dB
    
    // Peak filters with musical frequency ranges
    peak1FreqSlider.setValue(200.0f + random.nextFloat() * 800.0f); // 200-1000 Hz
    peak1GainSlider.setValue(-12.0f + random.nextFloat() * 24.0f);
    peak1QSlider.setValue(0.5f + random.nextFloat() * 4.5f); // 0.5-5.0
    
    
    peak3FreqSlider.setValue(2000.0f + random.nextFloat() * 18000.0f); // 2-20 kHz
    peak3GainSlider.setValue(-12.0f + random.nextFloat() * 24.0f);
    peak3QSlider.setValue(0.5f + random.nextFloat() * 4.5f);
    
    // High shelf (keep in audible range)
    highShelfFreqSlider.setValue(6000.0f + random.nextFloat() * 14000.0f); // 6-20 kHz (audible range)
    highShelfGainSlider.setValue(-12.0f + random.nextFloat() * 24.0f);
}

void WorkstationEditor::randomizeReverb()
{
    auto& random = juce::Random::getSystemRandom();
    
    // Room size (small to large)
    reverbRoomSizeSlider.setValue(0.1f + random.nextFloat() * 0.8f); // 0.1-0.9
    
    // Damping (bright to dark)
    reverbDampingSlider.setValue(random.nextFloat()); // 0.0-1.0
    
    // Wet level (subtle to prominent)
    reverbWetLevelSlider.setValue(0.1f + random.nextFloat() * 0.6f); // 0.1-0.7
    
    // Dry level (usually keep high)
    reverbDryLevelSlider.setValue(0.5f + random.nextFloat() * 0.5f); // 0.5-1.0
}

void WorkstationEditor::refreshMidiDevices()
{
    midiDeviceSelector.clear();
    
    processor.refreshMidiDevices();
    auto deviceNames = processor.getAvailableMidiDevices();
    
    int selectedIndex = -1;
    juce::String currentSelection = processor.getSelectedMidiDevice();
    
    for (int i = 0; i < deviceNames.size(); ++i)
    {
        midiDeviceSelector.addItem(deviceNames[i], i + 1);
        if (deviceNames[i] == currentSelection)
            selectedIndex = i;
    }
    
    if (selectedIndex >= 0)
        midiDeviceSelector.setSelectedItemIndex(selectedIndex, juce::dontSendNotification);
    else if (deviceNames.size() > 0)
    {
        // Auto-select the last device (most recently connected)
        midiDeviceSelector.setSelectedItemIndex(deviceNames.size() - 1, juce::dontSendNotification);
        processor.setSelectedMidiDevice(deviceNames[deviceNames.size() - 1]);
    }
}