cmake_minimum_required(VERSION 3.22)

project(SineSynth VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find JUCE - adjust path as needed
# find_package(PkgConfig REQUIRED)  # Not needed for JUCE

# Add JUCE as a subdirectory (assumes JUCE is installed)
# If you have JUCE installed via package manager or git submodule:
if(EXISTS "${CMAKE_SOURCE_DIR}/JUCE/CMakeLists.txt")
    add_subdirectory(JUCE)
elseif(EXISTS "/usr/local/share/JUCE/CMakeLists.txt")
    add_subdirectory(/usr/local/share/JUCE JUCE)
elseif(EXISTS "$ENV{HOME}/JUCE/CMakeLists.txt")
    add_subdirectory($ENV{HOME}/JUCE JUCE)
else()
    message(FATAL_ERROR "JUCE not found. Please install JUCE or set JUCE_DIR")
endif()

# Create the plugin target
juce_add_plugin(SineSynth
    COMPANY_NAME "YourCompany"
    PLUGIN_MANUFACTURER_CODE Yrcm
    PLUGIN_CODE Sins
    FORMATS AU Standalone
    PRODUCT_NAME "SineSynth"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

# Add source files
target_sources(SineSynth PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/SineWaveVoice.h
    Source/SineWaveSound.h
)

# Link required JUCE modules
target_link_libraries(SineSynth PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_dsp
)

# Compiler definitions for JUCE
target_compile_definitions(SineSynth PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# Set bundle identifier for macOS
if(APPLE)
    set_target_properties(SineSynth PROPERTIES
        BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER com.yourcompany.SineSynth
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    )
endif()

# Create MIDI Injector tool (console version)
add_executable(MidiInjector
    MidiInjector/main.cpp
)

target_link_libraries(MidiInjector PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_events
    juce::juce_core
)

target_compile_definitions(MidiInjector PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

# Create MIDI Injector GUI app
juce_add_gui_app(MidiInjectorGUI
    COMPANY_NAME "YourCompany"
    PRODUCT_NAME "MIDI Injector"
)

target_sources(MidiInjectorGUI PRIVATE
    MidiInjectorGUI/Source/MidiInjectorApp.cpp
    MidiInjectorGUI/Source/MidiInjectorApp.h
)

target_link_libraries(MidiInjectorGUI PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_events
    juce::juce_core
    juce::juce_gui_basics
)

target_compile_definitions(MidiInjectorGUI PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

# Create Parametric EQ tool
juce_add_plugin(ParametricEQ
    COMPANY_NAME "YourCompany"
    PLUGIN_MANUFACTURER_CODE Yrcm
    PLUGIN_CODE PrEQ
    FORMATS Standalone
    PRODUCT_NAME "Parametric EQ"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(ParametricEQ PRIVATE
    ParametricEQ/Source/EQProcessor.cpp
    ParametricEQ/Source/EQProcessor.h
    ParametricEQ/Source/EQEditor.cpp
    ParametricEQ/Source/EQEditor.h
)

target_link_libraries(ParametricEQ PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_dsp
)

target_compile_definitions(ParametricEQ PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

# Create integrated Audio Workstation
juce_add_plugin(AudioWorkstation
    COMPANY_NAME "YourCompany"
    PLUGIN_MANUFACTURER_CODE Yrcm
    PLUGIN_CODE AuWk
    FORMATS Standalone
    PRODUCT_NAME "Audio Workstation"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(AudioWorkstation PRIVATE
    AudioWorkstation/Source/WorkstationProcessor.cpp
    AudioWorkstation/Source/WorkstationProcessor.h
    AudioWorkstation/Source/WorkstationEditor.cpp
    AudioWorkstation/Source/WorkstationEditor.h
    Source/SineWaveVoice.h
    Source/SineWaveSound.h
)

target_link_libraries(AudioWorkstation PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_dsp
)

target_compile_definitions(AudioWorkstation PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

# Install targets
if(APPLE)
    # Install AU plugin to the correct location
    install(TARGETS SineSynth_AU
        DESTINATION "$ENV{HOME}/Library/Audio/Plug-Ins/Components"
        OPTIONAL
    )
    
    # Install standalone app
    install(TARGETS SineSynth_Standalone
        DESTINATION "/Applications"
        OPTIONAL
    )
endif()