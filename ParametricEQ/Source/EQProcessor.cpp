#include "EQProcessor.h"
#include "EQEditor.h"

EQProcessor::EQProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      valueTreeState(*this, nullptr, "PARAMETERS", 
      {
          std::make_unique<juce::AudioParameterFloat>("lowShelfFreq", "Low Shelf Freq", 20.0f, 500.0f, 80.0f),
          std::make_unique<juce::AudioParameterFloat>("lowShelfGain", "Low Shelf Gain", -24.0f, 24.0f, 0.0f),
          std::make_unique<juce::AudioParameterFloat>("peak1Freq", "Peak 1 Freq", 100.0f, 2000.0f, 400.0f),
          std::make_unique<juce::AudioParameterFloat>("peak1Gain", "Peak 1 Gain", -24.0f, 24.0f, 0.0f),
          std::make_unique<juce::AudioParameterFloat>("peak1Q", "Peak 1 Q", 0.1f, 10.0f, 0.7f),
          std::make_unique<juce::AudioParameterFloat>("peak2Freq", "Peak 2 Freq", 500.0f, 5000.0f, 1000.0f),
          std::make_unique<juce::AudioParameterFloat>("peak2Gain", "Peak 2 Gain", -24.0f, 24.0f, 0.0f),
          std::make_unique<juce::AudioParameterFloat>("peak2Q", "Peak 2 Q", 0.1f, 10.0f, 0.7f),
          std::make_unique<juce::AudioParameterFloat>("peak3Freq", "Peak 3 Freq", 2000.0f, 15000.0f, 4000.0f),
          std::make_unique<juce::AudioParameterFloat>("peak3Gain", "Peak 3 Gain", -24.0f, 24.0f, 0.0f),
          std::make_unique<juce::AudioParameterFloat>("peak3Q", "Peak 3 Q", 0.1f, 10.0f, 0.7f),
          std::make_unique<juce::AudioParameterFloat>("highShelfFreq", "High Shelf Freq", 5000.0f, 20000.0f, 8000.0f),
          std::make_unique<juce::AudioParameterFloat>("highShelfGain", "High Shelf Gain", -24.0f, 24.0f, 0.0f),
      })
{
    lowShelfFreq = valueTreeState.getRawParameterValue("lowShelfFreq");
    lowShelfGain = valueTreeState.getRawParameterValue("lowShelfGain");
    peak1Freq = valueTreeState.getRawParameterValue("peak1Freq");
    peak1Gain = valueTreeState.getRawParameterValue("peak1Gain");
    peak1Q = valueTreeState.getRawParameterValue("peak1Q");
    peak2Freq = valueTreeState.getRawParameterValue("peak2Freq");
    peak2Gain = valueTreeState.getRawParameterValue("peak2Gain");
    peak2Q = valueTreeState.getRawParameterValue("peak2Q");
    peak3Freq = valueTreeState.getRawParameterValue("peak3Freq");
    peak3Gain = valueTreeState.getRawParameterValue("peak3Gain");
    peak3Q = valueTreeState.getRawParameterValue("peak3Q");
    highShelfFreq = valueTreeState.getRawParameterValue("highShelfFreq");
    highShelfGain = valueTreeState.getRawParameterValue("highShelfGain");
}

EQProcessor::~EQProcessor() {}

void EQProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;
    
    processorChain.prepare(spec);
    updateFilters();
}

void EQProcessor::releaseResources() {}

void EQProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    
    updateFilters();
    
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    
    processorChain.process(context);
}

void EQProcessor::updateFilters()
{
    // Low shelf filter
    auto& lowShelf = processorChain.get<0>();
    *lowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        currentSampleRate, lowShelfFreq->load(), 0.7f, juce::Decibels::decibelsToGain(lowShelfGain->load()));
    
    // Peak filters
    auto& peak1 = processorChain.get<1>();
    *peak1.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        currentSampleRate, peak1Freq->load(), peak1Q->load(), juce::Decibels::decibelsToGain(peak1Gain->load()));
        
    auto& peak2 = processorChain.get<2>();
    *peak2.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        currentSampleRate, peak2Freq->load(), peak2Q->load(), juce::Decibels::decibelsToGain(peak2Gain->load()));
        
    auto& peak3 = processorChain.get<3>();
    *peak3.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        currentSampleRate, peak3Freq->load(), peak3Q->load(), juce::Decibels::decibelsToGain(peak3Gain->load()));
    
    // High shelf filter
    auto& highShelf = processorChain.get<4>();
    *highShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        currentSampleRate, highShelfFreq->load(), 0.7f, juce::Decibels::decibelsToGain(highShelfGain->load()));
}

void EQProcessor::getFrequencyResponse(std::vector<float>& frequencies, std::vector<float>& magnitudes)
{
    frequencies.clear();
    magnitudes.clear();
    
    const int numPoints = 512;
    const float minFreq = 20.0f;
    const float maxFreq = 20000.0f;
    
    // Take snapshot of current parameter values to avoid reading atomics multiple times
    const float currentLowShelfFreq = lowShelfFreq->load();
    const float currentLowShelfGain = lowShelfGain->load();
    const float currentPeak1Freq = peak1Freq->load();
    const float currentPeak1Gain = peak1Gain->load();
    const float currentPeak1Q = peak1Q->load();
    const float currentPeak2Freq = peak2Freq->load();
    const float currentPeak2Gain = peak2Gain->load();
    const float currentPeak2Q = peak2Q->load();
    const float currentPeak3Freq = peak3Freq->load();
    const float currentPeak3Gain = peak3Gain->load();
    const float currentPeak3Q = peak3Q->load();
    const float currentHighShelfFreq = highShelfFreq->load();
    const float currentHighShelfGain = highShelfGain->load();
    const double sampleRate = currentSampleRate;
    
    for (int i = 0; i < numPoints; ++i)
    {
        float freq = minFreq * std::pow(maxFreq / minFreq, i / float(numPoints - 1));
        frequencies.push_back(freq);
        
        // Calculate combined response of all filters using snapshot values
        float magnitude = 1.0f;
        
        // Low shelf
        auto lowShelfCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            sampleRate, currentLowShelfFreq, 0.7f, juce::Decibels::decibelsToGain(currentLowShelfGain));
        magnitude *= std::abs(lowShelfCoeffs->getMagnitudeForFrequency(freq, sampleRate));
        
        // Peak filters
        auto peak1Coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate, currentPeak1Freq, currentPeak1Q, juce::Decibels::decibelsToGain(currentPeak1Gain));
        magnitude *= std::abs(peak1Coeffs->getMagnitudeForFrequency(freq, sampleRate));
        
        auto peak2Coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate, currentPeak2Freq, currentPeak2Q, juce::Decibels::decibelsToGain(currentPeak2Gain));
        magnitude *= std::abs(peak2Coeffs->getMagnitudeForFrequency(freq, sampleRate));
        
        auto peak3Coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate, currentPeak3Freq, currentPeak3Q, juce::Decibels::decibelsToGain(currentPeak3Gain));
        magnitude *= std::abs(peak3Coeffs->getMagnitudeForFrequency(freq, sampleRate));
        
        // High shelf
        auto highShelfCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            sampleRate, currentHighShelfFreq, 0.7f, juce::Decibels::decibelsToGain(currentHighShelfGain));
        magnitude *= std::abs(highShelfCoeffs->getMagnitudeForFrequency(freq, sampleRate));
        
        magnitudes.push_back(juce::Decibels::gainToDecibels(magnitude));
    }
}

juce::AudioProcessorEditor* EQProcessor::createEditor()
{
    return new EQEditor(*this);
}

// Required for standalone builds
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EQProcessor();
}