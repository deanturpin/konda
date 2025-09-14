#!/bin/bash

# Konda Drag & Drop DMG Creator
# Creates a professional macOS disk image with drag-and-drop installation

set -e

VERSION="1.2.2"
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

# Create symbolic links to plugin folders
echo "🔗 Creating installation shortcuts..."
ln -s ~/Library/Audio/Plug-Ins/Components "dmg_temp/Audio Units Folder"
ln -s ~/Library/Audio/Plug-Ins/VST3 "dmg_temp/VST3 Folder"

# Create README
cat > dmg_temp/README.txt << 'README'
Konda v1.2.2 - Installation Instructions
========================================

QUICK INSTALL:
1. Drag "Konda.component" to "Audio Units Folder"
2. Drag "Konda.vst3" to "VST3 Folder"
3. Restart your DAW

WHAT'S NEW IN v1.2.2:
• Professional drag & drop installer
• Streamlined 4-band parametric EQ
• Enhanced audio input handling
• Improved interface layout

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
        set position of item "Konda.component" of container window to {150, 150}
        set position of item "Konda.vst3" of container window to {150, 250}
        set position of item "Audio Units Folder" of container window to {450, 150}
        set position of item "VST3 Folder" of container window to {450, 250}
        set position of item "README.txt" of container window to {300, 350}
        
        update without registering applications
        delay 2
    end tell
end tell
APPLESCRIPT

# Unmount the DMG
hdiutil detach "/Volumes/${VOLUME_NAME}"

# Convert to compressed read-only DMG
echo "📦 Compressing disk image..."
hdiutil convert "${TEMP_DMG}" -format UDZO -imagekey zlib-level=9 -o "${FINAL_DMG}"

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

