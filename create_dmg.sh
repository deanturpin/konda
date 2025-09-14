#!/bin/bash

# Konda DMG Creator
# Creates a professional macOS disk image with installer

set -e

VERSION="1.2.0"
APP_NAME="Konda"
DMG_NAME="Konda_v${VERSION}_Installer"
TEMP_DMG="temp_${DMG_NAME}.dmg"
FINAL_DMG="${DMG_NAME}.dmg"

echo "🎵 Creating Konda v${VERSION} Installer DMG..."

# Clean up any existing build
rm -f "${TEMP_DMG}" "${FINAL_DMG}"
rm -rf dmg_temp

# Create temporary directory for DMG contents
mkdir -p dmg_temp

# Copy plugins
echo "📦 Copying Konda plugins..."
cp -R "build/AudioWorkstation_artefacts/Release/AU/Konda.component" dmg_temp/
cp -R "build/AudioWorkstation_artefacts/Release/VST3/Konda.vst3" dmg_temp/

# Create installer script
cat > dmg_temp/Install_Konda.command << 'EOF'
#!/bin/bash

# Konda Installer Script
# Installs both Audio Unit and VST3 plugins

echo "🎵 Installing Konda by Turbeaux Sounds..."
echo ""

# Get the directory of this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Create plugin directories if they don't exist
mkdir -p ~/Library/Audio/Plug-Ins/Components/
mkdir -p ~/Library/Audio/Plug-Ins/VST3/

# Install Audio Unit
echo "📦 Installing Audio Unit..."
if cp -R "$SCRIPT_DIR/Konda.component" ~/Library/Audio/Plug-Ins/Components/; then
    echo "✅ Audio Unit installed successfully"
else
    echo "❌ Failed to install Audio Unit"
    exit 1
fi

# Install VST3
echo "📦 Installing VST3..."
if cp -R "$SCRIPT_DIR/Konda.vst3" ~/Library/Audio/Plug-Ins/VST3/; then
    echo "✅ VST3 installed successfully"
else
    echo "❌ Failed to install VST3"
    exit 1
fi

echo ""
echo "🎉 Konda installed successfully!"
echo ""
echo "To use Konda:"
echo "• Logic Pro: AU Instruments → Turbeaux Sounds → Konda"
echo "• Ableton Live: VST3 → Turbeaux Sounds → Konda"
echo "• Other DAWs: Look for Turbeaux Sounds → Konda"
echo ""
echo "Please restart your DAW to see the plugin."
echo ""
read -p "Press Enter to close..."

EOF

chmod +x dmg_temp/Install_Konda.command

# Create README
cat > dmg_temp/README.txt << 'EOF'
Konda v1.2.0 - A Turbeaux Sounds Plugin
=====================================

INSTALLATION:
Double-click "Install_Konda.command" to automatically install both formats.

MANUAL INSTALLATION:
1. Copy Konda.component to ~/Library/Audio/Plug-Ins/Components/
2. Copy Konda.vst3 to ~/Library/Audio/Plug-Ins/VST3/
3. Restart your DAW

FEATURES:
• FFT-centered interface with large spectrum analyzer
• Multi-colored EQ - each band displays in unique color  
• 4-voice polyphonic synthesis
• Built-in MIDI pattern generator
• Real-time visual feedback

SYSTEM REQUIREMENTS:
• macOS 10.13 or later
• Intel or Apple Silicon Mac
• Any AU or VST3 compatible DAW

SUPPORT:
https://github.com/deanturpin/ts

Konda is part of the Turbeaux Sounds plugin suite.
EOF

# Create Applications alias for easy drag-and-drop (if we had an app)
# For plugins, we'll create shortcuts to plugin folders instead

# Create shortcuts info
cat > dmg_temp/Plugin_Locations.txt << 'EOF'
Plugin Installation Locations:

Audio Unit Plugins:
~/Library/Audio/Plug-Ins/Components/

VST3 Plugins:
~/Library/Audio/Plug-Ins/VST3/

Use the installer script for automatic installation,
or manually copy the plugin files to these locations.
EOF

echo "💾 Creating disk image..."

# Create the DMG
hdiutil create -srcfolder dmg_temp -volname "Konda v${VERSION}" -fs HFS+ -fsargs "-c c=64,a=16,e=16" -format UDRW "${TEMP_DMG}"

# Mount the DMG to customize it
MOUNT_DIR="/Volumes/Konda v${VERSION}"
hdiutil attach "${TEMP_DMG}"

# Wait for mount
sleep 2

# Set the DMG window appearance (requires AppleScript)
osascript << EOF
tell application "Finder"
    tell disk "Konda v${VERSION}"
        open
        set current view of container window to icon view
        set toolbar visible of container window to false
        set statusbar visible of container window to false
        set the bounds of container window to {100, 100, 800, 500}
        set viewOptions to the icon view options of container window
        set arrangement of viewOptions to not arranged
        set icon size of viewOptions to 80
        set background picture of viewOptions to file ".background:background.png"
        
        -- Position items
        set position of item "Install_Konda.command" of container window to {200, 200}
        set position of item "README.txt" of container window to {500, 200}
        set position of item "Konda.component" of container window to {200, 320}
        set position of item "Konda.vst3" of container window to {350, 320}
        set position of item "Plugin_Locations.txt" of container window to {500, 320}
        
        update without registering applications
        delay 2
    end tell
end tell
EOF

# Unmount the DMG
hdiutil detach "${MOUNT_DIR}"

# Convert to compressed read-only DMG
echo "📦 Compressing disk image..."
hdiutil convert "${TEMP_DMG}" -format UDZO -imagekey zlib-level=9 -o "${FINAL_DMG}"

# Clean up
rm -f "${TEMP_DMG}"
rm -rf dmg_temp

# Make sure the plugins were built
if [ ! -f "build/AudioWorkstation_artefacts/Release/AU/Konda.component/Contents/MacOS/Konda" ]; then
    echo "❌ Plugins not found. Run 'make install' first."
    exit 1
fi

echo "✅ Created: ${FINAL_DMG}"
echo "📊 Size: $(du -h "${FINAL_DMG}" | cut -f1)"
echo ""
echo "🎉 Konda installer DMG created successfully!"
echo "Users can now double-click the DMG and run the installer."

EOF

chmod +x create_dmg.sh