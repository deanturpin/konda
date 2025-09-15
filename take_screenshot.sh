#!/bin/bash

# Automated Screenshot Script for Konda Audio Workstation
# Takes screenshots of both standalone and plugin modes for release documentation

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SCREENSHOTS_DIR="$SCRIPT_DIR/release_screenshots"
VERSION=${1:-"current"}

echo "📸 Taking release screenshots for Konda v$VERSION..."

# Create screenshots directory
mkdir -p "$SCREENSHOTS_DIR"

# Function to wait for application to fully load
wait_for_app() {
    local app_name="$1"
    local timeout=10
    local count=0

    echo "⏳ Waiting for $app_name to fully load..."
    while ! pgrep -f "$app_name" > /dev/null && [ $count -lt $timeout ]; do
        sleep 1
        ((count++))
    done

    if [ $count -ge $timeout ]; then
        echo "❌ Timeout waiting for $app_name to start"
        return 1
    fi

    # Additional wait for UI to stabilise
    sleep 3
    echo "✅ $app_name is ready"
}

# Function to take screenshot of specific window
take_window_screenshot() {
    local window_name="$1"
    local output_file="$2"
    local window_id

    echo "📷 Taking screenshot of '$window_name'..."

    # Get window ID by name
    window_id=$(osascript -e "
        tell application \"System Events\"
            set frontApp to first application process whose frontmost is true
            set frontAppName to name of frontApp
            set windowList to every window of frontApp
            repeat with aWindow in windowList
                if name of aWindow contains \"$window_name\" then
                    return id of aWindow
                end if
            end repeat
        end tell
    " 2>/dev/null || echo "")

    if [ -n "$window_id" ]; then
        # Use screencapture with window ID
        screencapture -l"$window_id" -x "$output_file"
        echo "✅ Screenshot saved: $output_file"
    else
        echo "⚠️  Window '$window_name' not found, taking full screen"
        screencapture -x "$output_file"
    fi
}

# Function to clean up processes
cleanup() {
    echo "🧹 Cleaning up..."
    pkill -f "Konda" 2>/dev/null || true
    pkill -f "Logic Pro" 2>/dev/null || true
    sleep 2
}

# Ensure clean start
cleanup

echo "🎹 1. Taking Standalone Mode Screenshot..."

# Launch standalone application
"$SCRIPT_DIR/build/AudioWorkstation_artefacts/Release/Standalone/Konda.app/Contents/MacOS/Konda" &
STANDALONE_PID=$!

# Wait for app to load
if wait_for_app "Konda"; then
    # Bring to front and take screenshot
    osascript -e 'tell application "Konda" to activate' 2>/dev/null || true
    sleep 1

    take_window_screenshot "Konda" "$SCREENSHOTS_DIR/konda_v${VERSION}_standalone.png"
else
    echo "❌ Failed to start standalone application"
fi

# Clean up standalone
kill $STANDALONE_PID 2>/dev/null || true
sleep 2

echo "🎚️  2. Taking Plugin Mode Screenshot (Logic Pro)..."

# Check if Logic Pro is available
if [ -d "/Applications/Logic Pro.app" ]; then
    # Launch Logic Pro
    open "/Applications/Logic Pro.app"

    if wait_for_app "Logic Pro"; then
        echo "📝 Please manually:"
        echo "   1. Create a new project in Logic Pro"
        echo "   2. Add Konda as an Audio Unit instrument"
        echo "   3. Open the Konda plugin interface"
        echo "   4. Press ENTER when ready for screenshot..."
        read -r

        # Take screenshot of Logic Pro with plugin
        take_window_screenshot "Logic Pro" "$SCREENSHOTS_DIR/konda_v${VERSION}_plugin_logic.png"

        echo "📝 You can now close Logic Pro or leave it open"
    else
        echo "❌ Failed to start Logic Pro"
    fi
else
    echo "⚠️  Logic Pro not found, skipping plugin screenshot"
    echo "💡 Alternative: Open any DAW and load Konda as AU/VST3 plugin"
fi

# Generate comparison screenshot if previous version exists
if [ "$VERSION" != "current" ] && [ -f "$SCREENSHOTS_DIR/konda_v${VERSION}_standalone.png" ]; then
    echo "🖼️  3. Generating comparison images..."

    # Create side-by-side comparison if previous version exists
    PREV_VERSION=$(ls "$SCREENSHOTS_DIR"/konda_v*_standalone.png | grep -v "v${VERSION}" | tail -1 | sed 's/.*_v\([^_]*\)_.*/\1/')

    if [ -n "$PREV_VERSION" ] && [ -f "$SCREENSHOTS_DIR/konda_v${PREV_VERSION}_standalone.png" ]; then
        echo "📊 Creating comparison: v$PREV_VERSION vs v$VERSION"

        # Use ImageMagick to create side-by-side comparison
        if command -v magick >/dev/null 2>&1; then
            magick "$SCREENSHOTS_DIR/konda_v${PREV_VERSION}_standalone.png" "$SCREENSHOTS_DIR/konda_v${VERSION}_standalone.png" +append "$SCREENSHOTS_DIR/comparison_v${PREV_VERSION}_vs_v${VERSION}.png"
            echo "✅ Comparison image created"
        else
            echo "💡 Install ImageMagick for automatic comparison generation: brew install imagemagick"
        fi
    fi
fi

echo ""
echo "📸 Screenshot session complete!"
echo "📁 Screenshots saved in: $SCREENSHOTS_DIR"
echo ""
echo "📋 Files created:"
ls -la "$SCREENSHOTS_DIR" | grep "v${VERSION}"

echo ""
echo "💡 Usage for next release:"
echo "   ./take_screenshot.sh 1.3.0"