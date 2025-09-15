# Konda Android Build Guide

## 📱 Building the Android App

Since APK distribution requires signed releases and play store approval, we provide the source code for you to build your own APK.

### Prerequisites
- Android Studio (latest version)
- Android SDK 23+ (Android 6.0+)
- USB debugging enabled on your Android device

### Quick Build Steps

1. **Install Android Studio**
   ```bash
   # Download from: https://developer.android.com/studio
   ```

2. **Prepare Project**
   ```bash
   ./build_android.sh  # Creates android project structure
   ```

3. **Open in Android Studio**
   - Open Android Studio
   - Choose "Open an Existing Project"
   - Select the `android/` folder

4. **Build APK**
   - Connect your Android device via USB
   - Enable "USB Debugging" in Developer Options
   - Click "Run" (green triangle) in Android Studio
   - Or build manually: `cd android && ./gradlew assembleRelease`

### Features You'll Get
- ✅ Full 4-voice polyphonic synthesizer
- ✅ 4-band parametric EQ with color visualization
- ✅ Real-time FFT spectrum analyzer
- ✅ USB MIDI keyboard support (USB-C adapter required)
- ✅ Touch-optimized controls
- ✅ Low-latency audio engine

### USB MIDI Setup
1. Connect MIDI keyboard via USB-C adapter
2. Launch Konda app
3. Grant USB device permissions
4. Play with full polyphony and effects!

### System Requirements
- **Android 6.0+** (API level 23)
- **USB OTG/Host support** for MIDI keyboards
- **2GB RAM** recommended
- **100MB storage** for app

### Troubleshooting

#### Build Errors
- Ensure Android SDK 23+ is installed
- Update Gradle if prompted
- Check JUCE paths in CMakeLists.txt

#### USB MIDI Not Working
- Verify USB adapter supports data transfer
- Check MIDI keyboard is USB Class Compliant
- Grant USB permissions when prompted

### Why No Pre-built APK?
- **Security**: Users can verify source code
- **Customization**: Modify features as needed
- **Learning**: Great way to explore Android audio development
- **Compatibility**: Build for your specific device/Android version

---

**Happy Android music making!** 🎵📱