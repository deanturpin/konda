# Android APK Sideloading Guide

## 📱 Installing Konda APK (Sideloading)

You can install the Konda Android app directly from the APK file without using the Google Play Store.

### Download APK
1. Go to [GitHub Releases](https://github.com/deanturpin/ts/releases/latest)
2. Download `Konda-debug.apk` (auto-built from source)
3. Or build your own using the [Android Build Guide](README_ANDROID.md)

### Enable Sideloading on Your Device

#### Android 8.0+ (Recommended Method)
1. Download the APK file to your phone
2. Tap the APK file in your Downloads folder
3. Android will prompt: "Install unknown apps"
4. Tap "Settings" → Enable "Allow from this source"
5. Go back and tap "Install"

#### Alternative Method (Older Android)
1. Go to **Settings** → **Security**
2. Enable **"Unknown sources"** or **"Install unknown apps"**
3. Download and tap the APK file to install

### Installation Steps
1. **Download APK**: Get `Konda-debug.apk` from GitHub releases
2. **Enable Sideloading**: Follow steps above for your Android version
3. **Install**: Tap the APK file and select "Install"
4. **Grant Permissions**: Allow audio recording when prompted
5. **Connect MIDI**: Use USB-C adapter for MIDI keyboards

### USB MIDI Setup
1. Connect MIDI keyboard via USB-C adapter
2. Launch Konda app
3. Grant USB device permission when prompted
4. Start playing with full synthesis and effects!

### Features You Get
- ✅ **4-voice polyphonic synthesizer**
- ✅ **4-band parametric EQ with color visualization**
- ✅ **Real-time FFT spectrum analyzer**
- ✅ **USB MIDI keyboard support**
- ✅ **Touch-optimized controls**
- ✅ **Low-latency audio engine**

### System Requirements
- **Android 6.0+** (API level 23)
- **USB OTG/Host support** for MIDI keyboards
- **2GB RAM** recommended
- **100MB storage** for app

### Security Note
Sideloading APKs bypasses Play Store security checks. Only install APKs from trusted sources like:
- ✅ Official GitHub releases (auto-built from verified source code)
- ✅ APKs you build yourself from source code
- ❌ Random APK files from unknown websites

### Troubleshooting

#### "App not installed" Error
- Check you have enough storage space
- Uninstall any previous versions first
- Ensure APK file downloaded completely

#### USB MIDI Not Working
- Verify USB adapter supports data transfer (not just charging)
- Check MIDI keyboard is USB Class Compliant
- Grant USB permissions when prompted
- Try a different USB-C adapter

#### Audio Issues
- Grant audio recording permission in app settings
- Close other audio apps before using Konda
- Try increasing audio buffer size if crackling occurs

---

**Enjoy mobile music production with Konda!** 🎵📱