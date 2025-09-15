#!/bin/bash

# Konda Android Build Script
# Builds Android APK with USB MIDI support

echo "🤖 Building Konda for Android..."

# Check for Android NDK
if [ -z "$ANDROID_NDK_HOME" ]; then
    echo "❌ Error: ANDROID_NDK_HOME not set"
    echo "Please install Android NDK and set ANDROID_NDK_HOME environment variable"
    exit 1
fi

# Check for JUCE
if [ ! -d "$HOME/JUCE" ]; then
    echo "❌ Error: JUCE not found at $HOME/JUCE"
    echo "Please install JUCE framework"
    exit 1
fi

# Create Android project structure
mkdir -p android/app/src/main/java/com/turbeaux/konda
mkdir -p android/app/src/main/jni
mkdir -p android/app/src/main/res/layout
mkdir -p android/app/src/main/AndroidManifest.xml

# Generate Android manifest with USB host permission
cat > android/app/src/main/AndroidManifest.xml << 'EOF'
<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.turbeaux.konda">

    <!-- USB host mode for MIDI keyboards -->
    <uses-feature android:name="android.hardware.usb.host" android:required="true" />

    <!-- Audio permissions -->
    <uses-permission android:name="android.permission.RECORD_AUDIO" />
    <uses-permission android:name="android.permission.MODIFY_AUDIO_SETTINGS" />

    <!-- USB permissions -->
    <uses-permission android:name="android.permission.USB_PERMISSION" />

    <application
        android:allowBackup="true"
        android:label="Konda"
        android:theme="@android:style/Theme.NoTitleBar.Fullscreen">

        <activity
            android:name=".MainActivity"
            android:configChanges="orientation|keyboardHidden|screenSize"
            android:launchMode="singleTask"
            android:screenOrientation="landscape">

            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>

            <!-- USB device attached intent -->
            <intent-filter>
                <action android:name="android.hardware.usb.action.USB_DEVICE_ATTACHED" />
            </intent-filter>

            <meta-data
                android:name="android.hardware.usb.action.USB_DEVICE_ATTACHED"
                android:resource="@xml/device_filter" />
        </activity>
    </application>
</manifest>
EOF

# Create USB device filter for MIDI devices
mkdir -p android/app/src/main/res/xml
cat > android/app/src/main/res/xml/device_filter.xml << 'EOF'
<?xml version="1.0" encoding="utf-8"?>
<resources>
    <!-- USB MIDI Class -->
    <usb-device class="1" subclass="3" />
</resources>
EOF

# Create gradle build file
cat > android/app/build.gradle << 'EOF'
apply plugin: 'com.android.application'

android {
    compileSdkVersion 33
    buildToolsVersion "33.0.0"

    defaultConfig {
        applicationId "com.turbeaux.konda"
        minSdkVersion 23  // Android 6.0+ for USB MIDI
        targetSdkVersion 33
        versionCode 1
        versionName "1.0"

        externalNativeBuild {
            cmake {
                arguments "-DANDROID_STL=c++_shared"
                cppFlags "-std=c++17 -frtti -fexceptions"
            }
        }

        ndk {
            abiFilters 'armeabi-v7a', 'arm64-v8a', 'x86', 'x86_64'
        }
    }

    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android.txt')
        }
    }

    externalNativeBuild {
        cmake {
            path "CMakeLists.txt"
        }
    }
}

dependencies {
    implementation 'androidx.appcompat:appcompat:1.6.1'
    implementation 'com.google.android.material:material:1.9.0'
}
EOF

echo "✅ Android project structure created"
echo ""
echo "📱 To build for Android:"
echo "1. Install Android Studio"
echo "2. Open the 'android' folder as a project"
echo "3. Connect your Android device with USB debugging enabled"
echo "4. Build and run the app"
echo ""
echo "🎹 USB MIDI Setup:"
echo "- Connect MIDI keyboard to phone via USB-C adapter"
echo "- Grant USB permission when prompted"
echo "- Konda will auto-detect MIDI devices"
echo ""
echo "📦 Features on Android:"
echo "- Full synthesis engine"
echo "- 4-band parametric EQ"
echo "- FFT spectrum visualization"
echo "- USB MIDI keyboard support"
echo "- Low-latency audio (OpenSL ES)"