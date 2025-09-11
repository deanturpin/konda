#include "PluginProcessor.h"
#include "PluginEditor.h"

SineSynthAudioProcessor::SineSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    valueTreeState(*this, nullptr, "PARAMETERS", 
    {
        std::make_unique<juce::AudioParameterFloat>("attack", "Attack", 0.1f, 1.0f, 0.1f),
        std::make_unique<juce::AudioParameterFloat>("decay", "Decay", 0.1f, 1.0f, 0.1f),
        std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain", 0.0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("release", "Release", 0.1f, 3.0f, 0.4f),
        std::make_unique<juce::AudioParameterFloat>("filterCutoff", "Filter Cutoff", 100.0f, 10000.0f, 1000.0f),
        std::make_unique<juce::AudioParameterFloat>("filterResonance", "Filter Resonance", 0.1f, 10.0f, 0.7f)
    })
{
    synth.addSound(new SineWaveSound());
    
    for (auto i = 0; i < 4; ++i)
        synth.addVoice(new SineWaveVoice());
        
    attackParam = valueTreeState.getRawParameterValue("attack");
    decayParam = valueTreeState.getRawParameterValue("decay");
    sustainParam = valueTreeState.getRawParameterValue("sustain");
    releaseParam = valueTreeState.getRawParameterValue("release");
    filterCutoffParam = valueTreeState.getRawParameterValue("filterCutoff");
    filterResonanceParam = valueTreeState.getRawParameterValue("filterResonance");
}

SineSynthAudioProcessor::~SineSynthAudioProcessor()
{
}

const juce::String SineSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SineSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SineSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SineSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SineSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SineSynthAudioProcessor::getNumPrograms()
{
    return 1;
}

int SineSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SineSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SineSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void SineSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void SineSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    for (auto i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SineWaveVoice*>(synth.getVoice(i)))
        {
            voice->setADSRParameters({
                *attackParam, *decayParam, *sustainParam, *releaseParam
            });
            voice->prepareFilter(sampleRate);
        }
    }
}

void SineSynthAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SineSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SineSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Only update parameters if they've changed
    auto currentAttack = attackParam->load();
    auto currentDecay = decayParam->load();
    auto currentSustain = sustainParam->load();
    auto currentRelease = releaseParam->load();
    auto currentFilterCutoff = filterCutoffParam->load();
    auto currentFilterResonance = filterResonanceParam->load();
    
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
        
        // Cache current values
        lastAttack = currentAttack;
        lastDecay = currentDecay;
        lastSustain = currentSustain;
        lastRelease = currentRelease;
        lastFilterCutoff = currentFilterCutoff;
        lastFilterResonance = currentFilterResonance;
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

bool SineSynthAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SineSynthAudioProcessor::createEditor()
{
    return new SineSynthAudioProcessorEditor (*this);
}

void SineSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = valueTreeState.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SineSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (valueTreeState.state.getType()))
            valueTreeState.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SineSynthAudioProcessor();
}