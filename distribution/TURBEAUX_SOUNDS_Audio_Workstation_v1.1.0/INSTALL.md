# TURBEAUX SOUNDS Audio Workstation v1.1.0 - Installation Guide

## 🎵 The World's First FFT-Centered Audio Plugin! (Now with VST3!)

**TURBEAUX SOUNDS Audio Workstation v1.1.0** is a revolutionary audio plugin that combines synthesis, EQ, and MIDI generation with spectacular multi-colored visualization.

### ✨ What's New in v1.1.0:
- ✅ **VST3 Plugin Added** - Universal compatibility across DAWs!
- ✅ **Both Formats Included** - Audio Unit + VST3 in one package
- ✅ **Cross-Platform Ready** - Works with Logic Pro, Live, Studio One, Reaper, and more
- ✅ **Improved Parameter Handling** - Resolved VST3 automation conflicts

### 🌟 Unique Features:
- **FFT-Centered Interface**: Large spectrum analyzer as main display
- **Multi-Colored EQ**: Individual band color coding (Red, Orange, Yellow, Cyan, Light Blue)
- **Real-time Visualization**: Live frequency response with glow effects and sparkles
- **Integrated Synthesis**: 4-voice polyphonic sine synthesizer with ADSR and filter
- **Built-in MIDI Generation**: 7 musical modes × 5 melody patterns
- **Professional Branding**: Audio-reactive "TURBEAUX SOUNDS" display

---

## 🍎 macOS Installation

### Prerequisites:
- macOS 10.13 or later
- Logic Pro, GarageBand, Ableton Live, Studio One, Reaper, or compatible host
- Intel or Apple Silicon Mac

### Option 1: Complete Installation (Both Formats)

```bash
# Install Audio Unit (for Logic Pro/GarageBand)
cp -R "TURBEAUX SOUNDS Audio Workstation.component" ~/Library/Audio/Plug-Ins/Components/

# Install VST3 (for Live, Studio One, Reaper, etc.)
cp -R "TURBEAUX SOUNDS Audio Workstation.vst3" ~/Library/Audio/Plug-Ins/VST3/
```

### Option 2: Manual Installation
1. **Audio Unit Installation**:
   - Open Finder, press `Cmd+Shift+G`
   - Enter: `~/Library/Audio/Plug-Ins/Components/`
   - Drag `TURBEAUX SOUNDS Audio Workstation.component` here

2. **VST3 Installation**:
   - Open Finder, press `Cmd+Shift+G`  
   - Enter: `~/Library/Audio/Plug-Ins/VST3/`
   - Drag `TURBEAUX SOUNDS Audio Workstation.vst3` here

3. **Restart Your DAW** completely

---

## 🎹 DAW-Specific Usage

### Logic Pro / GarageBand (Audio Unit):
1. Create new **Software Instrument** track
2. Click **Instrument** slot → **AU Instruments** → **Turbeaux Sounds**
3. Select **TURBEAUX SOUNDS Audio Workstation**

### Ableton Live (VST3):
1. Create new **MIDI Track**
2. **Instrument Rack** → **Browse Instruments** → **VST3** → **Turbeaux Sounds**
3. Select **TURBEAUX SOUNDS Audio Workstation**

### Studio One (VST3):
1. **Drag & Drop** from **Browser** → **Instruments** → **VST3** → **Turbeaux Sounds**
2. Or use **Insert** → **Instrument** → **TURBEAUX SOUNDS Audio Workstation**

### Reaper (VST3):
1. **Insert** → **Virtual Instrument on New Track**
2. **Browse** → **VST3** → **Turbeaux Sounds** → **TURBEAUX SOUNDS Audio Workstation**

---

## 🎚️ The Multi-Colored EQ Magic

This is the **world's first plugin** with individual EQ band color visualization:

- **🔴 Red**: Low Shelf filter (bass frequencies)
- **🟠 Orange**: Peak 1 filter (low-mid frequencies)  
- **🟡 Yellow**: Peak 2 filter (mid frequencies)
- **🟢 Cyan**: Peak 3 filter (upper-mid frequencies)
- **🔵 Light Blue**: High Shelf filter (treble frequencies)

**Watch in real-time** as each EQ adjustment changes the corresponding colored line on the spectrum!

---

## 🎹 Interface Overview & Quick Start

### Layout:
- **Center**: Large real-time spectrum analyzer with frequency labels
- **Top**: Synthesis controls (ADSR, Filter, Distortion)  
- **Left**: 5-band EQ with multi-colored visualization
- **Bottom**: MIDI generation and randomization
- **Branding**: "TURBEAUX SOUNDS" pulses with audio levels

### Quick Start:
1. **Play notes** on your MIDI keyboard or DAW's piano roll
2. **Watch the FFT** - see your notes as colorful frequency peaks!  
3. **Adjust EQ bands** - each band shows in a different color on the spectrum
4. **Try MIDI Generator** - click "Generate MIDI" for automatic musical patterns
5. **Randomize** - use random buttons to explore new sounds instantly

---

## 🔧 Troubleshooting

### Plugin Not Appearing:
1. **Check Installation Path**:
   - AU: `~/Library/Audio/Plug-Ins/Components/`
   - VST3: `~/Library/Audio/Plug-Ins/VST3/`
2. **Verify Permissions**: Plugins should be readable by your user account
3. **Restart DAW Completely**: Some DAWs cache plugin lists
4. **Rescan Plugins**:
   - Logic Pro: **Preferences** → **Audio** → **Plug-in Manager** → **Reset & Rescan**
   - Live: **Preferences** → **Plug-Ins** → **Rescan**
   - Studio One: **Preferences** → **Locations** → **VST Plug-Ins** → **Reset Blacklist**

### No Sound:
1. Check MIDI input is connected in your DAW
2. Verify audio output routing
3. Try the built-in MIDI generator first
4. Check DAW's plugin delay compensation

### Performance Issues:
- The FFT display is optimized for 30fps
- On older Macs, try reducing the plugin window size
- Close other intensive plugins if experiencing dropouts

---

## 🌍 Format Compatibility Chart

```
macOS DAWs:
├── Audio Unit (AU):
│   ├── Logic Pro ✅
│   ├── GarageBand ✅  
│   └── MainStage ✅
└── VST3:
    ├── Ableton Live ✅
    ├── Studio One ✅
    ├── Cubase ✅
    ├── Reaper ✅
    ├── FL Studio ✅
    └── Bitwig Studio ✅
```

---

## 📊 Technical Specifications

- **Formats**: Audio Unit (AU) v2 + VST3
- **Type**: Instrument Plugin (Synthesizer)
- **Polyphony**: 4 voices
- **Synthesis**: Pure sine wave with phase accumulation
- **Filter**: Lowpass with cutoff (20Hz-5kHz) and resonance (0.1-5.0)
- **EQ**: 5-band parametric with individual color visualization
- **FFT**: 1024-point real-time analysis at 30fps
- **MIDI**: 7 musical modes, 5 pattern types, intelligent randomization
- **Compatibility**: macOS 10.13+ (Intel + Apple Silicon)

---

## 🚀 What's Next?

Coming in future releases:
- **Windows VST3** for cross-platform compatibility
- **Web Audio Version** for browser-based music production
- **Additional Synthesis Engines** and effects
- **Preset System** for saving your favorite settings
- **MIDI Learn** for hardware controller integration

---

## 💬 Support & Updates

- **Issues**: Report bugs or request features on GitHub
- **Updates**: Watch the repository for new releases  
- **Community**: Share your creations with #TurbeauxSounds

**Thank you for trying the world's first FFT-centered audio plugin with dual-format support!** 🎵✨

---

*Built with JUCE framework • Developed with Claude Code • Made with ❤️ for music producers*