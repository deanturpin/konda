# Audio Production Suite - JUCE-based Audio Tools

A complete audio production and analysis toolkit featuring an enhanced polyphonic synthesizer, parametric EQ, and automated testing tools.

## Tools Included

### 1. Enhanced SineSynth
- 4-voice polyphony
- MIDI note input with proper frequency calculation
- ADSR envelope (Attack, Decay, Sustain, Release)
- **Lowpass filter with cutoff frequency and resonance controls**
- 6 rotary knobs for real-time parameter control
- Available as Logic Pro Audio Unit plugin and standalone app

### 2. Parametric EQ (NEW)
- Professional 5-band equalizer for audio analysis
- Low shelf, 3 parametric peaks, high shelf
- Real-time frequency response visualization
- Perfect for analyzing synthesizer output characteristics

### 3. MIDI Injector (NEW)
- Automated MIDI pattern generator for testing
- Plays C major scale patterns and chord progressions
- Virtual MIDI device creation
- Essential for hands-free testing during development

## Class Hierarchy

```
juce::AudioProcessor
└── SineSynthAudioProcessor
    ├── Contains juce::Synthesiser
    └── Contains juce::AudioProcessorValueTreeState

juce::AudioProcessorEditor
└── SineSynthAudioProcessorEditor
    └── Contains ADSR parameter controls

juce::SynthesiserSound
└── SineWaveSound
    └── Defines which notes/channels this synth responds to

juce::SynthesiserVoice
└── SineWaveVoice
    ├── Generates sine wave audio
    ├── Handles MIDI note on/off events
    ├── Calculates frequency from MIDI note numbers
    └── Contains juce::ADSR for envelope shaping
```

## Architecture Overview

### Audio Processing Flow
1. **MIDI Input** → `SineSynthAudioProcessor::processBlock()`
2. **Note Events** → `juce::Synthesiser` distributes to available `SineWaveVoice` instances
3. **Voice Processing** → Each voice generates sine waves with ADSR envelope
4. **Audio Output** → Mixed voices sent to host DAW

### Parameter Management
- Uses `juce::AudioProcessorValueTreeState` for parameter automation
- Four ADSR parameters: Attack, Decay, Sustain, Release
- GUI controls linked via `SliderAttachment` for real-time updates

### Voice Management
- 4 simultaneous voices for polyphonic playback
- Each `SineWaveVoice` can play any `SineWaveSound`
- Automatic voice allocation and release

## File Structure

```
Source/
├── PluginProcessor.h/cpp    # Main audio processor and MIDI handling
├── PluginEditor.h/cpp       # GUI with ADSR controls
├── SineWaveVoice.h          # Sine wave generator with envelope
└── SineWaveSound.h          # Sound definition class
```

## Complete Audio Analysis Workflow

### Professional Testing Setup
```bash
make test-all     # Launches complete analysis environment:
                  # 1. Enhanced SineSynth with filter controls
                  # 2. Parametric EQ for frequency analysis  
                  # 3. MIDI Injector for automated testing
```

This creates a professional audio development environment where you can:
1. **Generate** - SineSynth creates audio with ADSR + filter shaping
2. **Analyze** - Parametric EQ visualizes frequency response in real-time
3. **Test** - MIDI Injector provides consistent automated input patterns

### Code Statistics

Generated using David A. Wheeler's SLOCCount:

```
Current codebase: Enhanced audio production suite
- SineSynth: Polyphonic synthesizer with filter controls
- Parametric EQ: 5-band EQ with frequency visualization  
- MIDI Injector: Automated testing tool
- Build system: CMake + Make with prerequisites checking
- Development: VS Code integration with F5 debugging
```

A complete professional audio development toolkit!

## Prerequisites

Before building, install these dependencies:

```bash
# Install CMake
brew install cmake

# Install JUCE framework
git clone https://github.com/juce-framework/JUCE.git ~/JUCE

# Install Xcode command line tools (if not already installed)
xcode-select --install
```

Check all prerequisites are met:
```bash
make check-prereqs
```

## Building

### Quick Build (Recommended)
```bash
make              # Build complete audio production suite
make test-all     # Launch all tools for complete audio analysis
make test         # Build and launch SineSynth standalone only
make install      # Build and install AU plugin to Logic Pro
make help         # Show all available commands
```

### Individual Tool Builds
```bash
make standalone   # Enhanced SineSynth standalone app
make plugin       # SineSynth Audio Unit for Logic Pro
make eq           # Parametric EQ analyzer
make midi-injector # MIDI pattern generator
```

### Manual Build Options

#### Option 1: Audio Unit Plugin
1. Install JUCE framework
2. Open `SineSynth.jucer` in Projucer
3. Set JUCE module paths in Projucer
4. Generate Xcode project
5. Build in Xcode
6. Copy the built AU to `~/Library/Audio/Plug-Ins/Components/`

#### Option 2: Standalone App (No DAW Required)
1. In Projucer, the project is already configured for standalone builds
2. Generate and build the Xcode project
3. Run the standalone app directly - it includes built-in MIDI input and audio output
4. Perfect for testing without needing Logic Pro or other DAW

## Loading in Logic Pro

1. **Install the Plugin**
   - After building, copy `SineSynth.component` to `~/Library/Audio/Plug-Ins/Components/`
   - Restart Logic Pro if it was running

2. **Create a Software Instrument Track**
   - In Logic Pro: Track → New Software Instrument Track
   - Or use shortcut: ⌥⌘S

3. **Load SineSynth**
   - Click the Instrument slot in the track's channel strip
   - Navigate: AU Instruments → YourCompany → SineSynth
   - Double-click to load

4. **Connect MIDI Input**
   - The track will automatically receive MIDI from your controller
   - Or use Logic's onscreen keyboard: Window → Show Musical Typing (⌘K)

5. **Test the Synth**
   - Play notes on your MIDI controller or computer keyboard
   - Adjust ADSR parameters using the rotary knobs in the plugin window
   - Record MIDI or audio as needed

6. **Troubleshooting**
   - If plugin doesn't appear: Check AU validation in Logic Pro → Preferences → Audio → Plug-In Manager
   - Reset plugin database: Hold ⌥ while starting Logic Pro
   - Check Console.app for any loading errors

## Technical Details

### Enhanced SineSynth Architecture
- **Oscillator**: `std::sin()` with phase accumulation for pure sine waves
- **Filter**: JUCE DSP `StateVariableTPTFilter` with lowpass topology
- **Envelope**: JUCE `ADSR` with configurable attack, decay, sustain, release
- **Voice Management**: 4-voice polyphonic allocation with automatic cleanup

### Parametric EQ Implementation
- **Filter Types**: Low shelf, 3x peaking, high shelf using JUCE DSP IIR coefficients
- **Real-time Analysis**: 30fps frequency response calculation and visualization
- **Parameter Range**: ±24dB gain, 20Hz-20kHz frequency range, 0.1-10.0 Q factor

### MIDI Injector Features  
- **Pattern Generation**: C major scale with chord variations every 4th note
- **Virtual MIDI**: Creates "SineSynth MIDI Injector" virtual device
- **Timing**: 2-second intervals with 500ms note durations

### Development Tools
- **CMake Build System**: Cross-platform JUCE integration
- **Make Interface**: Single-command building with dependency checking
- **VS Code Integration**: F5 debugging with automatic builds