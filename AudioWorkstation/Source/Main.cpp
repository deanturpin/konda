/*
  ==============================================================================

   Konda WebAssembly Main Application
   TURBEAUX SOUNDS - Visual Audio Production Tools

  ==============================================================================
*/

#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1
#include "../../../juce_wasm/modules/juce_gui_basics/juce_gui_basics.h"
#include "WorkstationProcessor.h"
#include "WorkstationEditor.h"

//==============================================================================
class KondaApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    KondaApplication() = default;
    ~KondaApplication() override = default;

    const juce::String getApplicationName() override       { return "Konda"; }
    const juce::String getApplicationVersion() override    { return "1.2.2"; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& /*commandLine*/) override
    {
        // Create the main window
        mainWindow.reset (new MainWindow (getApplicationName()));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const juce::String&) override {}

private:
    //==============================================================================
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow (const juce::String& name)
            : DocumentWindow (name,
                             juce::Desktop::getInstance().getDefaultLookAndFeel()
                                 .findColour (juce::ResizableWindow::backgroundColourId),
                             DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (false);
            setResizable (true, true);
            setResizeLimits (800, 600, 10000, 10000);

            // Create the audio processor and editor
            processor = std::make_unique<WorkstationAudioProcessor>();
            editor = std::make_unique<WorkstationAudioProcessorEditor>(*processor);

            setContentNonOwned (editor.get(), true);

            // Set initial size based on editor
            if (auto* ed = editor.get())
            {
                setSize (ed->getWidth(), ed->getHeight());
            }
            else
            {
                setSize (1200, 800); // Default size
            }

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        std::unique_ptr<WorkstationAudioProcessor> processor;
        std::unique_ptr<WorkstationAudioProcessorEditor> editor;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (KondaApplication)