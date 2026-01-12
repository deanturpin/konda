#!/bin/bash

# Konda DMG Creator
# Creates a simple DMG with installation script

set -e

# Get version from git tag (latest tag)
VERSION=$(git describe --tags --abbrev=0 2>/dev/null || echo "dev")
# Strip 'v' prefix if present
VERSION="${VERSION#v}"
APP_NAME="Konda"
DMG_NAME="Konda_${VERSION}_Installer"
TEMP_DMG="temp_${DMG_NAME}.dmg"
FINAL_DMG="${DMG_NAME}.dmg"
VOLUME_NAME="Konda ${VERSION}"

echo "🎵 Creating Konda ${VERSION} Installer..."

# Clean up any existing build
rm -f "${TEMP_DMG}" "${FINAL_DMG}"
rm -rf dmg_temp

# Create temporary directory for DMG contents
mkdir -p dmg_temp

# Copy plugins
echo "📦 Copying Konda plugins..."
cp -R "build/AudioWorkstation_artefacts/Release/AU/Konda.component" dmg_temp/
cp -R "build/AudioWorkstation_artefacts/Release/VST3/Konda.vst3" dmg_temp/

# Create helper script to open installation folders
cat > dmg_temp/Open_Plugin_Folders.command << 'OPEN_SCRIPT'
#!/bin/bash

# Open the plugin installation folders
AU_DEST="$HOME/Library/Audio/Plug-Ins/Components"
VST3_DEST="$HOME/Library/Audio/Plug-Ins/VST3"

# Create directories if they don't exist
mkdir -p "${AU_DEST}"
mkdir -p "${VST3_DEST}"

# Open both folders
open "${AU_DEST}"
open "${VST3_DEST}"

echo "✅ Plugin folders opened!"
echo "Now drag Konda.component to the Components folder"
echo "and Konda.vst3 to the VST3 folder"
OPEN_SCRIPT

# Make script executable
chmod +x dmg_temp/Open_Plugin_Folders.command
xattr -c dmg_temp/Open_Plugin_Folders.command 2>/dev/null || true

# Create README
cat > dmg_temp/README.txt << READMETEXT
Konda ${VERSION} - Installation Instructions
========================================

DRAG AND DROP INSTALLATION:

1. Double-click "Open_Plugin_Folders.command"
   (This opens your plugin installation folders)

2. Drag "Konda.component" to the Components folder
   Drag "Konda.vst3" to the VST3 folder

3. Run this command in Terminal to fix permissions:
   xattr -cr ~/Library/Audio/Plug-Ins/Components/Konda.component
   xattr -cr ~/Library/Audio/Plug-Ins/VST3/Konda.vst3

4. Restart your DAW

QUICK TERMINAL INSTALL (Copy and paste all lines):
cd ~/Library/Audio/Plug-Ins/Components
cp -R /Volumes/Konda*/Konda.component .
xattr -cr Konda.component
cd ~/Library/Audio/Plug-Ins/VST3
cp -R /Volumes/Konda*/Konda.vst3 .
xattr -cr Konda.vst3
echo "✅ Installation complete! Restart your DAW."

FEATURES:
• FFT-centered interface with large spectrum analyser
• 4-band parametric EQ with multi-coloured visualisation
• 4-voice polyphonic synthesis
• Built-in MIDI pattern generator
• Multiple waveforms and filter types

COMPATIBILITY:
• Logic Pro (Audio Unit)
• Ableton Live (VST3)
• Studio One (VST3)
• Reaper (VST3/AU)
• Any AU or VST3 compatible DAW

SUPPORT:
https://github.com/deanturpin/konda

© 2025 Audieaux
READMETEXT

echo "💾 Creating disk image..."

# Create the DMG with specific size
hdiutil create -size 50m -fs HFS+ -volname "${VOLUME_NAME}" "${TEMP_DMG}"

# Mount the DMG
hdiutil attach "${TEMP_DMG}" -mountpoint "/Volumes/${VOLUME_NAME}"

# Copy contents to mounted DMG
echo "📁 Copying files to disk image..."
cp -R dmg_temp/* "/Volumes/${VOLUME_NAME}/"

# Set window appearance
echo "🎨 Setting custom layout..."
sleep 2

# Unmount the DMG
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
echo "🎉 Installer created successfully!"
echo "Users can now:"
echo "1. Open the DMG"
echo "2. Double-click 'Install_Konda.command'"
echo "3. Start making music with Konda!"
