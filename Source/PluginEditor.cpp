#include "PluginProcessor.h"
#include "PluginEditor.h"

SineSynthAudioProcessorEditor::SineSynthAudioProcessorEditor (SineSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (600, 300);

    attackSlider.setSliderStyle(juce::Slider::Rotary);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(attackSlider);
    
    decaySlider.setSliderStyle(juce::Slider::Rotary);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(decaySlider);
    
    sustainSlider.setSliderStyle(juce::Slider::Rotary);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(sustainSlider);
    
    releaseSlider.setSliderStyle(juce::Slider::Rotary);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(releaseSlider);
    
    filterCutoffSlider.setSliderStyle(juce::Slider::Rotary);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(filterCutoffSlider);
    
    filterResonanceSlider.setSliderStyle(juce::Slider::Rotary);
    filterResonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(filterResonanceSlider);

    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.attachToComponent(&attackSlider, false);
    addAndMakeVisible(attackLabel);
    
    decayLabel.setText("Decay", juce::dontSendNotification);
    decayLabel.attachToComponent(&decaySlider, false);
    addAndMakeVisible(decayLabel);
    
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    sustainLabel.attachToComponent(&sustainSlider, false);
    addAndMakeVisible(sustainLabel);
    
    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.attachToComponent(&releaseSlider, false);
    addAndMakeVisible(releaseLabel);
    
    filterCutoffLabel.setText("Filter Cutoff", juce::dontSendNotification);
    filterCutoffLabel.attachToComponent(&filterCutoffSlider, false);
    addAndMakeVisible(filterCutoffLabel);
    
    filterResonanceLabel.setText("Filter Resonance", juce::dontSendNotification);
    filterResonanceLabel.attachToComponent(&filterResonanceSlider, false);
    addAndMakeVisible(filterResonanceLabel);

    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "attack", attackSlider);
    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "decay", decaySlider);
    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "sustain", sustainSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "release", releaseSlider);
    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "filterCutoff", filterCutoffSlider);
    filterResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "filterResonance", filterResonanceSlider);
}

SineSynthAudioProcessorEditor::~SineSynthAudioProcessorEditor()
{
}

void SineSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("SineSynth", getLocalBounds().removeFromTop(30), juce::Justification::centred, 1);
}

void SineSynthAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(30);
    
    auto sliderArea = area.removeFromTop(200);
    
    attackSlider.setBounds(sliderArea.removeFromLeft(100).reduced(10));
    decaySlider.setBounds(sliderArea.removeFromLeft(100).reduced(10));
    sustainSlider.setBounds(sliderArea.removeFromLeft(100).reduced(10));
    releaseSlider.setBounds(sliderArea.removeFromLeft(100).reduced(10));
    filterCutoffSlider.setBounds(sliderArea.removeFromLeft(100).reduced(10));
    filterResonanceSlider.setBounds(sliderArea.removeFromLeft(100).reduced(10));
}