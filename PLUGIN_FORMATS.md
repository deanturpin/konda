# TURBEAUX SOUNDS Audio Workstation - Plugin Format Guide

## DAW Compatibility Matrix

```
macOS:
├── Audio Unit (AU) ←─ Native Apple format
│   ├── Logic Pro ✓
│   ├── GarageBand ✓
│   └── MainStage ✓
├── VST3 ←─ Cross-platform industry standard
│   ├── Ableton Live ✓
│   ├── Studio One ✓
│   ├── Cubase ✓
│   ├── Reaper ✓
│   └── FL Studio ✓
└── AAX ←─ Avid Pro Tools only
    └── Pro Tools ✓ (requires PACE licensing)

Windows:
├── VST3 ←─ Primary format (Steinberg)
│   ├── Cubase ✓
│   ├── Studio One ✓
│   ├── Ableton Live ✓
│   ├── Reaper ✓
│   ├── FL Studio ✓
│   └── Bitwig Studio ✓
├── AAX ←─ Pro Tools only
│   └── Pro Tools ✓ (requires PACE licensing)
└── CLAP ←─ New open standard
    ├── Bitwig Studio ✓
    └── Reaper ✓ (beta support)

Linux:
├── VST3 ←─ Growing support
│   ├── Reaper ✓
│   ├── Bitwig Studio ✓
│   └── Ardour ✓
├── CLAP ←─ Native Linux support
│   ├── Bitwig Studio ✓
│   └── Reaper ✓
└── LV2 ←─ Linux native
    ├── Ardour ✓
    └── Qtractor ✓

Web Browsers:
├── WebAssembly (WASM) ←─ Universal compatibility
│   ├── Chrome ✓ (Web Audio API + MIDI)
│   ├── Firefox ✓ (Web Audio API)
│   ├── Safari ✓ (WebKit Audio)
│   └── Edge ✓ (Chromium-based)
├── Progressive Web App (PWA) ←─ App-like experience
│   ├── Offline capability ✓
│   ├── Mobile responsive ✓
│   └── No installation required ✓
└── Web MIDI API ←─ Hardware controller support
    ├── Chrome ✓
    ├── Edge ✓
    └── Opera ✓
```

## Format Priority for Distribution:
1. **WebAssembly**: Universal access - no installation, instant demos, mobile support
2. **VST3**: Universal plugin compatibility (Windows/macOS/Linux)
3. **Audio Unit**: Essential for Logic Pro users (macOS only)  
4. **AAX**: Pro Tools access but requires expensive PACE licensing
5. **CLAP**: Future-proof open standard, growing adoption

## Current Status:
- ✅ **Audio Unit (AU)**: Built and ready for Logic Pro/GarageBand
- ⚠️ **VST3**: Build error - parameter automation conflict (fixing)
- 🔄 **WebAssembly**: Planned for universal browser access
- 📋 **Windows/Linux**: Planned for cross-platform distribution

## Installation:
- **macOS AU**: Copy `.component` file to `~/Library/Audio/Plug-Ins/Components/`
- **VST3**: Copy `.vst3` file to `~/Library/Audio/Plug-Ins/VST3/`
- **Standalone**: Runs as independent application

The TURBEAUX SOUNDS Audio Workstation combines:
- **FFT-Centered Visual Design**: Real-time spectrum analysis as primary interface
- **Multi-Colored EQ Visualization**: Individual band color coding for instant parameter understanding
- **Professional Synthesis**: 4-voice polyphonic engine with ADSR, filter, and effects
- **Built-in MIDI Generation**: 7 musical modes × 5 patterns with intelligent randomization