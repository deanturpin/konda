# Konda Installation Guide

Part of the Turbeaux Sounds plugin suite.

**Now available on Android with USB MIDI support!** 🤖

## Quick Install

### Android 📱

#### Installation
1. Download [Konda.apk from releases](https://github.com/deanturpin/ts/releases/latest)
2. Enable "Install from Unknown Sources" in Android settings
3. Install the APK file
4. Grant audio recording permissions when prompted

#### USB MIDI Setup
1. Connect MIDI keyboard to phone via USB-C adapter
2. Launch Konda
3. Grant USB device permission when prompted
4. Play with full polyphony and effects! 🎹

**Requirements**: Android 6.0+, USB OTG/Host mode support

### macOS

#### Audio Unit (Logic Pro, GarageBand, MainStage)

```bash
# Extract the downloaded archive
tar -xzf Konda_v1.2.0_Universal_AU_VST3.tar.gz

# Install Audio Unit
cp -R Konda.component ~/Library/Audio/Plug-Ins/Components/

# Restart your DAW
```

#### VST3 (Ableton Live, Studio One, Reaper, Cubase)

```bash
# Extract the downloaded archive
tar -xzf Konda_v1.2.0_Universal_AU_VST3.tar.gz

# Install VST3
cp -R Konda.vst3 ~/Library/Audio/Plug-Ins/VST3/

# Restart your DAW
```

## Finding Konda in Your DAW

### Logic Pro

1. Create a Software Instrument track
2. Click the Instrument slot
3. Navigate to: **AU Instruments** → **Turbeaux Sounds** → **Konda**

### Ableton Live

1. Create a MIDI track
2. In the Browser, navigate to: **Plug-ins** → **VST3** → **Turbeaux Sounds**
3. Drag **Konda** to your track

### Studio One

1. Browse to: **Instruments** → **VST3** → **Turbeaux Sounds**
2. Drag **Konda** to the arrangement

### Reaper

1. Insert → Virtual Instrument on New Track
2. Browse → VST3 → Turbeaux Sounds → Konda

## Manual Installation

If you prefer using Finder:

1. **Extract** the downloaded .tar.gz file by double-clicking it
2. **Open Finder** and press `Cmd+Shift+G`
3. For **Audio Unit**, enter: `~/Library/Audio/Plug-Ins/Components/`
   - Drag the `Konda.component` file here
4. For **VST3**, enter: `~/Library/Audio/Plug-Ins/VST3/`
   - Drag the `Konda.vst3` file here
5. **Restart your DAW**

## Troubleshooting

### Plugin Not Appearing?

#### Logic Pro

- Go to **Preferences** → **Plug-in Manager**
- Click **Reset & Rescan Selection**

#### Ableton Live

- Go to **Preferences** → **Plug-ins**
- Click **Rescan**

#### Studio One

- Go to **Preferences** → **Locations** → **VST Plug-ins**
- Click **Reset Blocklist**

#### Reaper

- Go to **Options** → **Preferences** → **VST**
- Click **Re-scan**

### No Sound?

1. Check MIDI input is connected in your DAW
2. Try the built-in MIDI generator (bottom section of plugin)
3. Verify audio output routing in your DAW
4. Check volume levels in both plugin and DAW

## System Requirements

- **macOS**: 10.13 or later
- **Architecture**: Intel or Apple Silicon (Universal Binary)
- **DAW**: Any AU or VST3 compatible host

## Features

- **FFT-centered interface** with large spectrum analyzer
- **Multi-colored EQ** - each band displays in unique color
- **4-voice polyphonic synthesis**
- **Built-in MIDI pattern generator**
- **Real-time visual feedback**

## Support

- **Issues**: <https://github.com/deanturpin/ts/issues>
- **Latest Release**: <https://github.com/deanturpin/ts/releases>

---

Konda is part of the Turbeaux Sounds plugin suite
