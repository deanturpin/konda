# TURBEAUX SOUNDS Audio Workstation - Installation Guide

## 🎵 The World's First FFT-Centered Audio Plugin!

**TURBEAUX SOUNDS Audio Workstation** is a revolutionary audio plugin that combines synthesis, EQ, and MIDI generation with spectacular multi-colored visualization.

### ✨ Unique Features:
- **FFT-Centered Interface**: Large spectrum analyzer as main display
- **Multi-Colored EQ**: Individual band color coding (Red, Orange, Yellow, Cyan, Light Blue)
- **Real-time Visualization**: Live frequency response with glow effects and sparkles
- **Integrated Synthesis**: 4-voice polyphonic sine synthesizer with ADSR and filter
- **Built-in MIDI Generation**: 7 musical modes × 5 melody patterns
- **Professional Branding**: Audio-reactive "TURBEAUX SOUNDS" display

---

## 🍎 macOS Installation (Audio Unit)

### Prerequisites:
- macOS 10.13 or later
- Logic Pro, GarageBand, or compatible AU host
- Intel or Apple Silicon Mac

### Installation Steps:

1. **Copy the Plugin**:
   ```bash
   cp -R "TURBEAUX SOUNDS Audio Workstation.component" ~/Library/Audio/Plug-Ins/Components/
   ```

2. **Alternative Manual Installation**:
   - Open Finder
   - Press `Cmd+Shift+G` and enter: `~/Library/Audio/Plug-Ins/Components/`
   - Drag the `TURBEAUX SOUNDS Audio Workstation.component` folder here

3. **Restart Your DAW**:
   - Quit Logic Pro/GarageBand completely
   - Relaunch your DAW
   - The plugin will appear under: **Instrument → AU Instruments → Turbeaux Sounds**

---

## 🎹 Using the Plugin

### In Logic Pro:
1. Create new **Software Instrument** track
2. Click the **Instrument** slot → **AU Instruments** → **Turbeaux Sounds** → **TURBEAUX SOUNDS Audio Workstation**
3. The revolutionary FFT-centered interface will open!

### Interface Overview:
- **Center**: Large real-time spectrum analyzer with frequency labels
- **Top**: Synthesis controls (ADSR, Filter, Distortion)
- **Left**: 5-band EQ with multi-colored visualization
- **Bottom**: MIDI generation and randomization
- **Branding**: "TURBEAUX SOUNDS" pulses with audio levels

### Quick Start:
1. **Play notes** on your MIDI keyboard or Logic's on-screen keyboard
2. **Watch the FFT** - see your notes as colorful frequency peaks!  
3. **Adjust EQ bands** - each band shows in a different color on the spectrum
4. **Try MIDI Generator** - click "Generate MIDI" for automatic musical patterns
5. **Randomize** - use random buttons to explore new sounds instantly

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

## 🔧 Troubleshooting

### Plugin Not Appearing:
1. Verify the `.component` file is in the correct location
2. Check permissions: `ls -la ~/Library/Audio/Plug-Ins/Components/`
3. Restart your DAW completely
4. In Logic Pro: **Logic Pro → Preferences → Audio → Plug-in Manager → Reset & Rescan Selection**

### No Sound:
1. Check your MIDI input is connected
2. Verify audio output routing in your DAW
3. Try the built-in MIDI generator first

### Performance Issues:
- The FFT display is optimized for 30fps
- On older Macs, try reducing the window size

---

## 📊 Technical Specifications

- **Format**: Audio Unit (AU) v2
- **Type**: Instrument Plugin (Synthesizer)
- **Polyphony**: 4 voices
- **Synthesis**: Pure sine wave with phase accumulation
- **Filter**: Lowpass with cutoff (20Hz-5kHz) and resonance (0.1-5.0)
- **EQ**: 5-band parametric with individual color visualization
- **FFT**: 1024-point real-time analysis at 30fps
- **MIDI**: 7 musical modes, 5 pattern types, intelligent randomization

---

## 🚀 What's Next?

This is version **1.0.0** of the TURBEAUX SOUNDS Audio Workstation. Coming soon:
- **VST3 version** for Windows/macOS cross-compatibility
- **Web version** for browser-based audio production
- **Additional synthesis engines** and effects
- **Preset system** for saving your favorite settings

---

## 💬 Support & Feedback

- **Issues**: Report bugs or request features on GitHub
- **Community**: Share your creations with #TurbeauxSounds
- **Updates**: Watch the repository for new releases

**Thank you for trying the world's first FFT-centered audio plugin!** 🎵✨

---

*Built with JUCE framework • Developed with Claude Code • Made with ❤️ for music producers*