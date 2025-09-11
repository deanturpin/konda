#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>

class MidiInjectorComponent : public juce::Component, public juce::Timer
{
public:
    MidiInjectorComponent()
    {
        startButton.setButtonText("Start MIDI Injection");
        stopButton.setButtonText("Stop");
        statusLabel.setText("Ready to inject MIDI patterns", juce::dontSendNotification);
        
        startButton.onClick = [this] { startInjection(); };
        stopButton.onClick = [this] { stopInjection(); };
        
        addAndMakeVisible(startButton);
        addAndMakeVisible(stopButton);
        addAndMakeVisible(statusLabel);
        
        stopButton.setEnabled(false);
        
        // Try to create virtual MIDI output
        midiOutput = juce::MidiOutput::createNewDevice("SineSynth MIDI Injector");
        if (midiOutput)
        {
            statusLabel.setText("MIDI device 'SineSynth MIDI Injector' created", juce::dontSendNotification);
        }
        else
        {
            statusLabel.setText("Failed to create MIDI device", juce::dontSendNotification);
        }
        
        setSize(400, 150);
    }
    
    ~MidiInjectorComponent()
    {
        stopTimer();
    }
    
    void resized() override
    {
        auto area = getLocalBounds().reduced(10);
        startButton.setBounds(area.removeFromTop(30));
        area.removeFromTop(10);
        stopButton.setBounds(area.removeFromTop(30));
        area.removeFromTop(10);
        statusLabel.setBounds(area);
    }
    
private:
    void startInjection()
    {
        if (midiOutput)
        {
            patternIndex = 0;
            startTimer(500); // Play note every 500ms
            startButton.setEnabled(false);
            stopButton.setEnabled(true);
            statusLabel.setText("Playing C major scale patterns...", juce::dontSendNotification);
        }
    }
    
    void stopInjection()
    {
        stopTimer();
        // Send all notes off
        if (midiOutput)
        {
            for (int note = 0; note < 128; ++note)
            {
                midiOutput->sendMessageNow(juce::MidiMessage::noteOff(1, note, 0.0f));
            }
        }
        startButton.setEnabled(true);
        stopButton.setEnabled(false);
        statusLabel.setText("Stopped", juce::dontSendNotification);
    }
    
    void timerCallback() override
    {
        if (!midiOutput) return;
        
        // C major scale: C, D, E, F, G, A, B, C
        static const int notes[] = {60, 62, 64, 65, 67, 69, 71, 72};
        static const float velocities[] = {0.8f, 0.6f, 0.9f, 0.7f, 0.85f, 0.75f, 0.65f, 1.0f};
        
        int note = notes[patternIndex % 8];
        float velocity = velocities[patternIndex % 8];
        
        // Send note on
        midiOutput->sendMessageNow(juce::MidiMessage::noteOn(1, note, velocity));
        
        // Schedule note off after 400ms
        juce::Timer::callAfterDelay(400, [this, note]()
        {
            if (midiOutput)
                midiOutput->sendMessageNow(juce::MidiMessage::noteOff(1, note, 0.0f));
        });
        
        patternIndex++;
        
        // Add chord every 4 notes
        if (patternIndex % 4 == 0)
        {
            juce::Timer::callAfterDelay(100, [this]()
            {
                if (midiOutput)
                {
                    // C major triad
                    midiOutput->sendMessageNow(juce::MidiMessage::noteOn(1, 60, 0.6f)); // C
                    midiOutput->sendMessageNow(juce::MidiMessage::noteOn(1, 64, 0.6f)); // E
                    midiOutput->sendMessageNow(juce::MidiMessage::noteOn(1, 67, 0.6f)); // G
                    
                    juce::Timer::callAfterDelay(300, [this]()
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
    
    juce::TextButton startButton, stopButton;
    juce::Label statusLabel;
    std::unique_ptr<juce::MidiOutput> midiOutput;
    int patternIndex = 0;
};

class MidiInjectorApp : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "MIDI Injector"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }
    
    void initialise(const juce::String&) override
    {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }
    
    void shutdown() override
    {
        mainWindow = nullptr;
    }
    
private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name) : DocumentWindow(name, juce::Colours::darkgrey, DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MidiInjectorComponent(), true);
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }
        
        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };
    
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(MidiInjectorApp)