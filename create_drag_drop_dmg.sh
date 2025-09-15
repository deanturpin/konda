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

# Create installation helper script instead of symbolic links
echo "🔗 Creating installation shortcuts..."

# Create a helper script that opens the correct folders
cat > "dmg_temp/Open Audio Units Folder.command" << 'SCRIPT'
#!/bin/bash
open "$HOME/Library/Audio/Plug-Ins/Components"
SCRIPT

cat > "dmg_temp/Open VST3 Folder.command" << 'SCRIPT'
#!/bin/bash
open "$HOME/Library/Audio/Plug-Ins/VST3"
SCRIPT

# Make the scripts executable
chmod +x "dmg_temp/Open Audio Units Folder.command"
chmod +x "dmg_temp/Open VST3 Folder.command"

# Create README
cat > dmg_temp/README.txt << 'README'
Konda v1.3.0 - Installation Instructions
========================================

QUICK INSTALL:
1. Double-click "Open Audio Units Folder.command"
2. Drag "Konda.component" into the opened folder
3. Double-click "Open VST3 Folder.command"
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
        set position of item "Open Audio Units Folder.command" of container window to {450, 150}
        set position of item "Open VST3 Folder.command" of container window to {450, 250}
        set position of item "README.txt" of container window to {300, 350}
        
        update without registering applications
        delay 2
    end tell
end tell
APPLESCRIPT

# Unmount the DMG with force if needed
echo "📤 Ejecting disk image..."
hdiutil detach "/Volumes/${VOLUME_NAME}" -force || true

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

