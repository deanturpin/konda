# TURBEAUX SOUNDS Audio Workstation

FFT-centered audio plugin with multi-colored EQ visualization for macOS.

## 🎵 Features

### Visual Design
- **FFT-Centered Interface** - Large real-time spectrum analyzer as the main interface element
- **Multi-Colored EQ** - Each band displays in its own color on the spectrum (Red, Orange, Yellow, Light Blue)
- **Real-Time Analysis** - 30fps frequency visualization with professional grid lines

### Audio Engine
- **4-Voice Polyphonic Synthesis** - Pure sine wave generation with phase accumulation
- **ADSR Envelope** - Attack, Decay, Sustain, Release with logarithmic scaling
- **Resonant Lowpass Filter** - 20Hz-5kHz range with improved control curves
- **4-Band Parametric EQ** - Low shelf, two parametric peaks, high shelf
- **Soft-Clipping Distortion** - With automatic gain compensation

### MIDI Generation
- **7 Musical Modes** - Major, Minor, Dorian, Phrygian, Lydian, Mixolydian, Locrian
- **5 Pattern Types** - Scales, Arpeggios, Chord Tones, Step Patterns, Cascading
- **Smart Randomization** - Avoids pattern repetition for musical variety
- **Bass Lines** - Automatic bass notes an octave below melody

## 📦 Installation

### Quick Install (Plugins)
```bash
make install
```
This installs both Audio Unit and VST3 plugins to your system.

### DAW Compatibility
- **Audio Unit** - Logic Pro, GarageBand, MainStage
- **VST3** - Ableton Live, Studio One, Reaper, Cubase, FL Studio

### System Requirements
- macOS 10.13 or later
- Intel or Apple Silicon Mac
- JUCE framework (auto-downloaded during build)

## 🛠️ Development

### Prerequisites
```bash
# Install CMake
brew install cmake

# Clone JUCE framework
git clone https://github.com/juce-framework/JUCE.git ~/JUCE
```

### Build Commands
```bash
make              # Build and launch complete suite
make install      # Install AU + VST3 plugins
make clean        # Clean build artifacts
make test-all     # Launch standalone app for testing
make help         # Show all available commands
```

### Project Structure
```
├── AudioWorkstation/     # Main plugin source
│   ├── Source/
│   │   ├── WorkstationProcessor.cpp  # Audio processing
│   │   └── WorkstationEditor.cpp     # GUI implementation
├── CMakeLists.txt       # Build configuration
├── Makefile            # Build automation
└── docs/               # GitHub Pages website
```

## 🌐 Download

Pre-built binaries available at: https://deanturpin.github.io/ts/

## 📊 Technical Details

### Build System
- **Framework**: JUCE 7.0.7+
- **Build Tool**: CMake 3.15+
- **Compiler**: Xcode Command Line Tools
- **Architecture**: Universal Binary (Intel + Apple Silicon)

### Audio Processing
- **Sample Rate**: Host-dependent
- **Buffer Size**: Host-dependent
- **FFT Size**: 1024 points
- **Processing**: Single-precision floating point

### Code Statistics
- **Lines of Code**: ~2,300+ 
- **Languages**: C++ (JUCE framework)
- **Build Time**: ~30 seconds (8 cores)

## 🤝 Contributing

Issues and pull requests welcome at: https://github.com/deanturpin/ts

## 📄 License

Built with JUCE framework (GPL/Commercial dual license)

---

*TURBEAUX SOUNDS - Visual audio production tools*