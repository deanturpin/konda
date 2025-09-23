#!/bin/bash

# Konda Drag & Drop DMG Creator
# Creates a professional macOS disk image with drag-and-drop installation

set -e

VERSION="1.3.0"
APP_NAME="Konda"
DMG_NAME="Konda_v${VERSION}_Installer"
TEMP_DMG="temp_${DMG_NAME}.dmg"
FINAL_DMG="${DMG_NAME}.dmg"
VOLUME_NAME="Konda v${VERSION}"

echo "🎵 Creating Konda v${VERSION} Drag & Drop Installer..."

# Clean up any existing build
rm -f "${TEMP_DMG}" "${FINAL_DMG}"
rm -rf dmg_temp

# Create temporary directory for DMG contents
mkdir -p dmg_temp

# Copy plugins
echo "📦 Copying Konda plugins..."
cp -R "build/AudioWorkstation_artefacts/Release/AU/Konda.component" dmg_temp/
cp -R "build/AudioWorkstation_artefacts/Release/VST3/Konda.vst3" dmg_temp/

# Create installation helper apps instead of shell scripts
echo "🔗 Creating installation shortcuts..."

# Create AppleScript applications which are more trusted by macOS
mkdir -p "dmg_temp/Open Audio Units Folder.app/Contents/MacOS"
mkdir -p "dmg_temp/Open Audio Units Folder.app/Contents/Resources"
mkdir -p "dmg_temp/Open VST3 Folder.app/Contents/MacOS"
mkdir -p "dmg_temp/Open VST3 Folder.app/Contents/Resources"

# Create Info.plist for Audio Units app
cat > "dmg_temp/Open Audio Units Folder.app/Contents/Info.plist" << 'PLIST'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>Open Audio Units Folder</string>
    <key>CFBundleIdentifier</key>
    <string>com.turbeaux.konda.au-opener</string>
    <key>CFBundleName</key>
    <string>Open Audio Units Folder</string>
    <key>CFBundleVersion</key>
    <string>1.0</string>
</dict>
</plist>
PLIST

# Create executable script for Audio Units
cat > "dmg_temp/Open Audio Units Folder.app/Contents/MacOS/Open Audio Units Folder" << 'SCRIPT'
#!/bin/bash
open "$HOME/Library/Audio/Plug-Ins/Components"
SCRIPT

# Create Info.plist for VST3 app
cat > "dmg_temp/Open VST3 Folder.app/Contents/Info.plist" << 'PLIST'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>Open VST3 Folder</string>
    <key>CFBundleIdentifier</key>
    <string>com.turbeaux.konda.vst3-opener</string>
    <key>CFBundleName</key>
    <string>Open VST3 Folder</string>
    <key>CFBundleVersion</key>
    <string>1.0</string>
</dict>
</plist>
PLIST

# Create executable script for VST3
cat > "dmg_temp/Open VST3 Folder.app/Contents/MacOS/Open VST3 Folder" << 'SCRIPT'
#!/bin/bash
open "$HOME/Library/Audio/Plug-Ins/VST3"
SCRIPT

# Make the executables runnable
chmod +x "dmg_temp/Open Audio Units Folder.app/Contents/MacOS/Open Audio Units Folder"
chmod +x "dmg_temp/Open VST3 Folder.app/Contents/MacOS/Open VST3 Folder"

# Create README
cat > dmg_temp/README.txt << 'README'
Konda v1.3.0 - Installation Instructions
========================================

QUICK INSTALL:
1. Double-click "Open Audio Units Folder" app
2. Drag "Konda.component" into the opened folder
3. Double-click "Open VST3 Folder" app
4. Drag "Konda.vst3" into the opened folder
5. Restart your DAW

WHAT'S NEW IN v1.3.0:
• Fixed drag & drop installer with proper folder access
• Android app support with USB MIDI
• Context-aware UI (adapts to plugin vs standalone)
• Enhanced cross-platform compatibility

FEATURES:
• FFT-centered interface with large spectrum analyzer
• 4-band parametric EQ with multi-colored visualization
• 4-voice polyphonic synthesis
• Built-in MIDI pattern generator

COMPATIBILITY:
• Logic Pro (Audio Unit)
• Ableton Live (VST3)
• Studio One (VST3)
• Reaper (VST3/AU)
• Any AU or VST3 compatible DAW

SUPPORT:
https://github.com/deanturpin/ts

© 2025 Turbeaux Sounds
README

echo "💾 Creating disk image..."

# Create the DMG with specific size
hdiutil create -size 50m -fs HFS+ -volname "${VOLUME_NAME}" "${TEMP_DMG}"

# Mount the DMG
hdiutil attach "${TEMP_DMG}" -mountpoint "/Volumes/${VOLUME_NAME}"

# Copy contents to mounted DMG
echo "📁 Copying files to disk image..."
cp -R dmg_temp/* "/Volumes/${VOLUME_NAME}/"

# Set custom icon positions using AppleScript
echo "🎨 Setting custom layout..."
sleep 2  # Give the mount time to settle
osascript << APPLESCRIPT
tell application "Finder"
    tell disk "${VOLUME_NAME}"
        open
        set current view of container window to icon view
        set toolbar visible of container window to false
        set statusbar visible of container window to false
        set the bounds of container window to {100, 100, 750, 500}
        set viewOptions to the icon view options of container window
        set arrangement of viewOptions to not arranged
        set icon size of viewOptions to 72
        
        -- Position items
        set position of item "Konda.component" of container window to {120, 150}
        set position of item "Konda.vst3" of container window to {120, 250}
        set position of item "Open Audio Units Folder.app" of container window to {450, 150}
        set position of item "Open VST3 Folder.app" of container window to {450, 250}
        set position of item "README.txt" of container window to {300, 350}
        
        update without registering applications
        delay 2
    end tell
end tell
APPLESCRIPT

# Unmount the DMG with force if needed
echo "📤 Ejecting disk image..."
sleep 2
hdiutil detach "/Volumes/${VOLUME_NAME}" -force || true
sleep 1

# Convert to compressed read-only DMG
echo "📦 Compressing disk image..."
if [ -f "${TEMP_DMG}" ]; then
    hdiutil convert "${TEMP_DMG}" -format UDZO -imagekey zlib-level=9 -o "${FINAL_DMG}"
else
    echo "❌ Temporary DMG not found!"
    exit 1
fi

# Clean up
rm -f "${TEMP_DMG}"
rm -rf dmg_temp

echo "✅ Created: ${FINAL_DMG}"
echo "📊 Size: $(du -h "${FINAL_DMG}" | cut -f1)"
echo ""
echo "🎉 Drag & Drop installer created successfully!"
echo "Users can now:"
echo "1. Open the DMG"
echo "2. Drag plugins to their respective folders"
echo "3. Start making music with Konda!"

