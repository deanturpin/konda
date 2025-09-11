#include "EQEditor.h"

FrequencyResponseDisplay::FrequencyResponseDisplay(EQProcessor& processor)
    : eqProcessor(processor)
{
    startTimerHz(30); // 30 FPS refresh
}

void FrequencyResponseDisplay::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    
    auto bounds = getLocalBounds().toFloat();
    
    // Draw grid
    g.setColour(juce::Colours::darkgrey);
    
    // Frequency grid lines (octaves)
    const float logMin = std::log10(20.0f);
    const float logMax = std::log10(20000.0f);
    
    for (float freq = 100.0f; freq < 20000.0f; freq *= 10.0f)
    {
        float x = bounds.getWidth() * (std::log10(freq) - logMin) / (logMax - logMin);
        g.drawVerticalLine(x, bounds.getY(), bounds.getBottom());
    }
    
    // dB grid lines
    for (float db = -20.0f; db <= 20.0f; db += 5.0f)
    {
        float y = bounds.getCentreY() - (db / 24.0f) * (bounds.getHeight() * 0.4f);
        g.drawHorizontalLine(y, bounds.getX(), bounds.getRight());
    }
    
    // Draw 0dB line
    g.setColour(juce::Colours::grey);
    g.drawHorizontalLine(bounds.getCentreY(), bounds.getX(), bounds.getRight());
    
    // Draw frequency response curve
    if (frequencies.size() > 1 && magnitudes.size() == frequencies.size())
    {
        juce::Path responseCurve;
        bool first = true;
        
        for (size_t i = 0; i < frequencies.size(); ++i)
        {
            float x = bounds.getWidth() * (std::log10(frequencies[i]) - logMin) / (logMax - logMin);
            float y = bounds.getCentreY() - (magnitudes[i] / 24.0f) * (bounds.getHeight() * 0.4f);
            
            if (first)
            {
                responseCurve.startNewSubPath(x, y);
                first = false;
            }
            else
            {
                responseCurve.lineTo(x, y);
            }
        }
        
        g.setColour(juce::Colours::yellow);
        g.strokePath(responseCurve, juce::PathStrokeType(2.0f));
    }
}

void FrequencyResponseDisplay::timerCallback()
{
    eqProcessor.getFrequencyResponse(frequencies, magnitudes);
    repaint();
}

EQEditor::EQEditor(EQProcessor& processor)
    : AudioProcessorEditor(&processor), 
      eqProcessor(processor),
      responseDisplay(processor)
{
    setSize(800, 600);
    
    // Setup sliders and labels
    auto setupSlider = [this](juce::Slider& slider, juce::Label& label, const juce::String& text, 
                              juce::Slider::SliderStyle style = juce::Slider::Rotary)
    {
        slider.setSliderStyle(style);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        addAndMakeVisible(slider);
        
        label.setText(text, juce::dontSendNotification);
        label.attachToComponent(&slider, false);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    };
    
    setupSlider(lowShelfFreqSlider, lowShelfFreqLabel, "Low Freq");
    setupSlider(lowShelfGainSlider, lowShelfGainLabel, "Low Gain");
    
    setupSlider(peak1FreqSlider, peak1FreqLabel, "Peak1 Freq");
    setupSlider(peak1GainSlider, peak1GainLabel, "Peak1 Gain");
    setupSlider(peak1QSlider, peak1QLabel, "Peak1 Q");
    
    setupSlider(peak2FreqSlider, peak2FreqLabel, "Peak2 Freq");
    setupSlider(peak2GainSlider, peak2GainLabel, "Peak2 Gain");
    setupSlider(peak2QSlider, peak2QLabel, "Peak2 Q");
    
    setupSlider(peak3FreqSlider, peak3FreqLabel, "Peak3 Freq");
    setupSlider(peak3GainSlider, peak3GainLabel, "Peak3 Gain");
    setupSlider(peak3QSlider, peak3QLabel, "Peak3 Q");
    
    setupSlider(highShelfFreqSlider, highShelfFreqLabel, "High Freq");
    setupSlider(highShelfGainSlider, highShelfGainLabel, "High Gain");
    
    addAndMakeVisible(responseDisplay);
    
    // Create parameter attachments
    auto& params = eqProcessor.getValueTreeState();
    
    lowShelfFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "lowShelfFreq", lowShelfFreqSlider);
    lowShelfGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "lowShelfGain", lowShelfGainSlider);
        
    peak1FreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "peak1Freq", peak1FreqSlider);
    peak1GainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "peak1Gain", peak1GainSlider);
    peak1QAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "peak1Q", peak1QSlider);
        
    peak2FreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "peak2Freq", peak2FreqSlider);
    peak2GainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "peak2Gain", peak2GainSlider);
    peak2QAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "peak2Q", peak2QSlider);
        
    peak3FreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "peak3Freq", peak3FreqSlider);
    peak3GainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "peak3Gain", peak3GainSlider);
    peak3QAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "peak3Q", peak3QSlider);
        
    highShelfFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "highShelfFreq", highShelfFreqSlider);
    highShelfGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "highShelfGain", highShelfGainSlider);
}

EQEditor::~EQEditor() {}

void EQEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawFittedText("Parametric EQ - Audio Analysis Tool", 
                     getLocalBounds().removeFromTop(40), 
                     juce::Justification::centred, 1);
}

void EQEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(40); // Title area
    
    // Frequency response display (top half)
    responseDisplay.setBounds(bounds.removeFromTop(bounds.getHeight() / 2).reduced(10));
    
    bounds.removeFromTop(10); // Spacing
    
    // Controls area (bottom half)
    auto controlsArea = bounds.reduced(10);
    
    int sliderWidth = controlsArea.getWidth() / 13; // 13 controls
    int sliderHeight = controlsArea.getHeight();
    
    lowShelfFreqSlider.setBounds(controlsArea.removeFromLeft(sliderWidth));
    lowShelfGainSlider.setBounds(controlsArea.removeFromLeft(sliderWidth));
    
    peak1FreqSlider.setBounds(controlsArea.removeFromLeft(sliderWidth));
    peak1GainSlider.setBounds(controlsArea.removeFromLeft(sliderWidth));
    peak1QSlider.setBounds(controlsArea.removeFromLeft(sliderWidth));
    
    peak2FreqSlider.setBounds(controlsArea.removeFromLeft(sliderWidth));
    peak2GainSlider.setBounds(controlsArea.removeFromLeft(sliderWidth));
    peak2QSlider.setBounds(controlsArea.removeFromLeft(sliderWidth));
    
    peak3FreqSlider.setBounds(controlsArea.removeFromLeft(sliderWidth));
    peak3GainSlider.setBounds(controlsArea.removeFromLeft(sliderWidth));
    peak3QSlider.setBounds(controlsArea.removeFromLeft(sliderWidth));
    
    highShelfFreqSlider.setBounds(controlsArea.removeFromLeft(sliderWidth));
    highShelfGainSlider.setBounds(controlsArea.removeFromLeft(sliderWidth));
}