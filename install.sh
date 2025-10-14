#!/bin/bash

# Konda Installation Script
# Automatically installs Konda plugin and fixes macOS quarantine issues

set -e

APP_NAME="Konda"
AU_SOURCE="build/AudioWorkstation_artefacts/Release/AU/Konda.component"
VST3_SOURCE="build/AudioWorkstation_artefacts/Release/VST3/Konda.vst3"
AU_DEST="$HOME/Library/Audio/Plug-Ins/Components"
VST3_DEST="$HOME/Library/Audio/Plug-Ins/VST3"

echo "🎵 Installing ${APP_NAME}..."
echo ""

# Check if build exists
if [ ! -d "${AU_SOURCE}" ] && [ ! -d "${VST3_SOURCE}" ]; then
    echo "❌ Error: Plugin files not found!"
    echo "   Please run 'make' first to build the plugins."
    exit 1
fi

# Create plugin directories if they don't exist
mkdir -p "${AU_DEST}"
mkdir -p "${VST3_DEST}"

# Install Audio Unit
if [ -d "${AU_SOURCE}" ]; then
    echo "📦 Installing Audio Unit (Logic Pro)..."
    cp -R "${AU_SOURCE}" "${AU_DEST}/"
    xattr -cr "${AU_DEST}/Konda.component"
    echo "✅ Audio Unit installed: ${AU_DEST}/Konda.component"
else
    echo "⚠️  Audio Unit not found (skipping)"
fi

# Install VST3
if [ -d "${VST3_SOURCE}" ]; then
    echo "📦 Installing VST3..."
    cp -R "${VST3_SOURCE}" "${VST3_DEST}/"
    xattr -cr "${VST3_DEST}/Konda.vst3"
    echo "✅ VST3 installed: ${VST3_DEST}/Konda.vst3"
else
    echo "⚠️  VST3 not found (skipping)"
fi

echo ""
echo "🎉 Installation complete!"
echo ""
echo "Next steps:"
echo "1. Restart your DAW (Logic Pro, Ableton, etc.)"
echo "2. Rescan plugins if needed"
echo "3. Look for 'Konda' in your plugin list"
echo ""
echo "The quarantine flags have been removed automatically."
echo ""
