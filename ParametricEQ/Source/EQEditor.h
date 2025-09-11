#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "EQProcessor.h"

class FrequencyResponseDisplay : public juce::Component, public juce::Timer
{
public:
    FrequencyResponseDisplay(EQProcessor& processor);
    void paint(juce::Graphics& g) override;
    void timerCallback() override;

private:
    EQProcessor& eqProcessor;
    std::vector<float> frequencies;
    std::vector<float> magnitudes;
};

class EQEditor : public juce::AudioProcessorEditor
{
public:
    EQEditor(EQProcessor& processor);
    ~EQEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    EQProcessor& eqProcessor;

    // GUI components
    juce::Slider lowShelfFreqSlider, lowShelfGainSlider;
    juce::Slider peak1FreqSlider, peak1GainSlider, peak1QSlider;
    juce::Slider peak2FreqSlider, peak2GainSlider, peak2QSlider;
    juce::Slider peak3FreqSlider, peak3GainSlider, peak3QSlider;
    juce::Slider highShelfFreqSlider, highShelfGainSlider;

    juce::Label lowShelfFreqLabel, lowShelfGainLabel;
    juce::Label peak1FreqLabel, peak1GainLabel, peak1QLabel;
    juce::Label peak2FreqLabel, peak2GainLabel, peak2QLabel;
    juce::Label peak3FreqLabel, peak3GainLabel, peak3QLabel;
    juce::Label highShelfFreqLabel, highShelfGainLabel;

    FrequencyResponseDisplay responseDisplay;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowShelfFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowShelfGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peak1FreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peak1GainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peak1QAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peak2FreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peak2GainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peak2QAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peak3FreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peak3GainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peak3QAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highShelfFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highShelfGainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQEditor)
};