#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_events/juce_events.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <iostream>
#include <thread>
#include <chrono>

class MidiInjector : public juce::Timer
{
public:
    MidiInjector()
    {
        // Find available MIDI output devices
        auto midiOutputs = juce::MidiOutput::getAvailableDevices();
        
        std::cout << "Available MIDI outputs:\n";
        for (int i = 0; i < midiOutputs.size(); ++i)
        {
            std::cout << "  " << i << ": " << midiOutputs[i].name << "\n";
        }
        
        // Try to open the first available MIDI output
        if (!midiOutputs.isEmpty())
        {
            midiOutput = juce::MidiOutput::openDevice(midiOutputs[0].identifier);
            if (midiOutput)
            {
                std::cout << "Opened MIDI output: " << midiOutputs[0].name << "\n";
            }
        }
        
        if (!midiOutput)
        {
            // Create virtual MIDI output
            midiOutput = juce::MidiOutput::createNewDevice("SineSynth MIDI Injector");
            if (midiOutput)
            {
                std::cout << "Created virtual MIDI output: SineSynth MIDI Injector\n";
            }
        }
        
        if (!midiOutput)
        {
            std::cerr << "Failed to create MIDI output device\n";
            return;
        }
        
        std::cout << "Starting MIDI injection...\n";
        std::cout << "Playing C major scale pattern\n";
        std::cout << "Press Ctrl+C to stop\n";
        
        startTimer(2000); // Start playing every 2 seconds
    }
    
    ~MidiInjector()
    {
        stopTimer();
        if (midiOutput)
        {
            // Send all notes off
            for (int note = 0; note < 128; ++note)
            {
                midiOutput->sendMessageNow(juce::MidiMessage::noteOff(1, note, 0.0f));
            }
        }
    }
    
    void timerCallback() override
    {
        if (!midiOutput)
            return;
            
        playPattern();
    }
    
private:
    void playPattern()
    {
        // C major scale: C, D, E, F, G, A, B, C
        static const int notes[] = {60, 62, 64, 65, 67, 69, 71, 72}; // MIDI note numbers
        static const float velocities[] = {0.8f, 0.6f, 0.9f, 0.7f, 0.85f, 0.75f, 0.65f, 1.0f};
        static int patternIndex = 0;
        
        int note = notes[patternIndex % 8];
        float velocity = velocities[patternIndex % 8];
        
        std::cout << "Playing note " << note << " (velocity " << velocity << ")\n";
        
        // Send note on
        midiOutput->sendMessageNow(juce::MidiMessage::noteOn(1, note, velocity));
        
        // Schedule note off after 500ms
        juce::Timer::callAfterDelay(500, [this, note]()
        {
            if (midiOutput)
            {
                midiOutput->sendMessageNow(juce::MidiMessage::noteOff(1, note, 0.0f));
            }
        });
        
        patternIndex++;
        
        // Add some variation - occasionally play chords
        if (patternIndex % 4 == 0)
        {
            // Play a C major triad
            juce::Timer::callAfterDelay(100, [this]()
            {
                if (midiOutput)
                {
                    std::cout << "Playing C major chord\n";
                    midiOutput->sendMessageNow(juce::MidiMessage::noteOn(1, 60, 0.6f)); // C
                    midiOutput->sendMessageNow(juce::MidiMessage::noteOn(1, 64, 0.6f)); // E
                    midiOutput->sendMessageNow(juce::MidiMessage::noteOn(1, 67, 0.6f)); // G
                    
                    juce::Timer::callAfterDelay(800, [this]()
                    {
                        if (midiOutput)
                        {
                            midiOutput->sendMessageNow(juce::MidiMessage::noteOff(1, 60, 0.0f));
                            midiOutput->sendMessageNow(juce::MidiMessage::noteOff(1, 64, 0.0f));
                            midiOutput->sendMessageNow(juce::MidiMessage::noteOff(1, 67, 0.0f));
                        }
                    });
                }
            });
        }
    }
    
    std::unique_ptr<juce::MidiOutput> midiOutput;
};

class MidiInjectorApp
{
public:
    int run()
    {
        // Initialize JUCE
        juce::initialiseJuce_GUI();
        
        injector = std::make_unique<MidiInjector>();
        
        std::cout << "\nMIDI Injector running...\n";
        std::cout << "This will send MIDI notes to test your SineSynth\n";
        std::cout << "Make sure SineSynth standalone is running\n";
        std::cout << "Press Enter to quit\n\n";
        
        // Wait for user input
        std::cin.get();
        
        injector.reset();
        juce::shutdownJuce_GUI();
        
        return 0;
    }
    
private:
    std::unique_ptr<MidiInjector> injector;
};

int main()
{
    MidiInjectorApp app;
    return app.run();
}