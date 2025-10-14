#!/bin/bash

# Konda DMG Creator
# Creates a simple DMG with installation script

set -e

# Get git commit hash (short version)
GIT_HASH=$(git rev-parse --short HEAD)
VERSION="${GIT_HASH}"
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

# Create installation script for DMG
cat > dmg_temp/Install_Konda.command << 'INSTALL_SCRIPT'
#!/bin/bash

# Konda Installation Script
# Double-click this file to install Konda plugins

APP_NAME="Konda"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
AU_DEST="$HOME/Library/Audio/Plug-Ins/Components"
VST3_DEST="$HOME/Library/Audio/Plug-Ins/VST3"

echo ""
echo "🎵 Installing ${APP_NAME}..."
echo ""

# Create plugin directories if they don't exist
mkdir -p "${AU_DEST}"
mkdir -p "${VST3_DEST}"

# Install Audio Unit
if [ -d "${SCRIPT_DIR}/Konda.component" ]; then
    echo "📦 Installing Audio Unit (Logic Pro)..."
    cp -R "${SCRIPT_DIR}/Konda.component" "${AU_DEST}/"
    xattr -cr "${AU_DEST}/Konda.component"
    echo "✅ Audio Unit installed"
else
    echo "⚠️  Audio Unit not found"
fi

# Install VST3
if [ -d "${SCRIPT_DIR}/Konda.vst3" ]; then
    echo "📦 Installing VST3..."
    cp -R "${SCRIPT_DIR}/Konda.vst3" "${VST3_DEST}/"
    xattr -cr "${VST3_DEST}/Konda.vst3"
    echo "✅ VST3 installed"
else
    echo "⚠️  VST3 not found"
fi

echo ""
echo "🎉 Installation complete!"
echo ""
echo "Next steps:"
echo "1. Restart your DAW (Logic Pro, Ableton, etc.)"
echo "2. Rescan plugins if needed"
echo "3. Look for 'Konda' in your plugin list"
echo ""
echo "Press any key to close this window..."
read -n 1
INSTALL_SCRIPT

# Make install script executable
chmod +x dmg_temp/Install_Konda.command

# Create README
cat > dmg_temp/README.txt << READMETEXT
Konda ${VERSION} - Installation Instructions
========================================

QUICK INSTALL:
1. Double-click "Install_Konda.command"
2. Enter your password if prompted
3. Restart your DAW

That's it! The installer will:
- Copy plugins to the correct locations
- Fix macOS security permissions automatically
- Install both Audio Unit (Logic Pro) and VST3 formats

MANUAL INSTALL (if script doesn't work):
1. Copy "Konda.component" to ~/Library/Audio/Plug-Ins/Components/
2. Copy "Konda.vst3" to ~/Library/Audio/Plug-Ins/VST3/
3. Run in Terminal:
   xattr -cr ~/Library/Audio/Plug-Ins/Components/Konda.component
   xattr -cr ~/Library/Audio/Plug-Ins/VST3/Konda.vst3

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

© 2025 Turbeaux Sounds
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
