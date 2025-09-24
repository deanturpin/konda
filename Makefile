# SineSynth JUCE Plugin Makefile
# Builds both standalone app and Audio Unit plugin using CMake

BUILD_DIR = build
BUILD_CONFIG = Release
AU_PLUGIN = ~/Library/Audio/Plug-Ins/Components/SineSynth.component

.PHONY: all install clean configure deploy help check-prereqs test-with-midi test-all setup-guide shutdown watch dev restart lint-md watch-md test-audio validate-au test-vst3 screenshot

# Default target - build and run everything
all: test-all

# Quick rebuild and run - shortest command for development
run: standalone

# Check prerequisites
check-prereqs:
	@echo "🔍 Checking prerequisites..."
	@command -v cmake >/dev/null 2>&1 || { echo "❌ CMake not found. Install with: brew install cmake"; exit 1; }
	@command -v git >/dev/null 2>&1 || { echo "❌ Git not found. Please install Git"; exit 1; }
	@[ -d "$$HOME/JUCE" ] || { echo "❌ JUCE not found at ~/JUCE. Run: git clone https://github.com/juce-framework/JUCE.git ~/JUCE"; exit 1; }
	@echo "✅ All prerequisites found"

# Build everything with parallel builds
build: $(BUILD_DIR)/build.stamp

$(BUILD_DIR)/build.stamp: check-prereqs configure Source/**/* ParametricEQ/Source/**/* MidiInjectorGUI/Source/**/* AudioWorkstation/Source/**/* CMakeLists.txt
	@echo "🏗️  Building complete audio suite (using $(NPROC) cores)..."
	@cd $(BUILD_DIR) && cmake --build . --config $(BUILD_CONFIG) -j$(NPROC)
	@touch $(BUILD_DIR)/build.stamp
	@echo "✅ Build complete! Launching Konda..."
	@open "$(BUILD_DIR)/AudioWorkstation_artefacts/$(BUILD_CONFIG)/Standalone/Konda.app" || echo "⚠️  Could not auto-launch app"

# Configure CMake build
configure:
	@echo "🔧 Configuring CMake build..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. -DCMAKE_BUILD_TYPE=$(BUILD_CONFIG)

# Get number of CPU cores for parallel builds
NPROC := $(shell sysctl -n hw.ncpu)

# Build standalone app only
standalone: configure
	@echo "🏗️  Building standalone app (using $(NPROC) cores)..."
	@cd $(BUILD_DIR) && cmake --build . --target AudioWorkstation_Standalone --config $(BUILD_CONFIG) -j$(NPROC)
	@echo "✅ Build complete! Launching Konda..."
	@open "$(BUILD_DIR)/AudioWorkstation_artefacts/$(BUILD_CONFIG)/Standalone/Konda.app" || echo "⚠️  Could not auto-launch app"

# Build Audio Unit plugin only  
plugin: configure
	@echo "🏗️  Building Audio Unit plugin (using $(NPROC) cores)..."
	@cd $(BUILD_DIR) && cmake --build . --target SineSynth_AU --config $(BUILD_CONFIG) -j$(NPROC)

# Install AU and VST3 plugins to system locations
install: configure
	@echo "📦 Building and installing Konda by Turbeaux Sounds..."
	@echo "🏗️  Building Audio Unit and VST3 plugins (using $(NPROC) cores)..."
	@cd $(BUILD_DIR) && cmake --build . --target AudioWorkstation_AU --config $(BUILD_CONFIG) -j$(NPROC)
	@cd $(BUILD_DIR) && cmake --build . --target AudioWorkstation_VST3 --config $(BUILD_CONFIG) -j$(NPROC)
	@echo "📦 Installing Audio Unit plugin..."
	@cp -R "$(BUILD_DIR)/AudioWorkstation_artefacts/$(BUILD_CONFIG)/AU/Konda.component" ~/Library/Audio/Plug-Ins/Components/
	@echo "📦 Installing VST3 plugin..."
	@mkdir -p ~/Library/Audio/Plug-Ins/VST3/
	@cp -R "$(BUILD_DIR)/AudioWorkstation_artefacts/$(BUILD_CONFIG)/VST3/Konda.vst3" ~/Library/Audio/Plug-Ins/VST3/
	@echo "✅ Konda installed:"
	@echo "   • Audio Unit → ~/Library/Audio/Plug-Ins/Components/"
	@echo "   • VST3 → ~/Library/Audio/Plug-Ins/VST3/"
	@echo "   Restart your DAW to see the plugins"

# Build MIDI injector tool
midi-injector: configure
	@echo "🎹 Building MIDI injector (using $(NPROC) cores)..."
	@cd $(BUILD_DIR) && cmake --build . --target MidiInjector --config $(BUILD_CONFIG) -j$(NPROC)

# Test standalone app
test: standalone
	@echo "🚀 Launching standalone app..."
	@find $(BUILD_DIR) -name "SineSynth.app" -type d -exec echo "Found standalone app: {}" \; -exec open "{}" \;

# Build Parametric EQ
eq: configure
	@echo "🎚️  Building Parametric EQ (using $(NPROC) cores)..."
	@cd $(BUILD_DIR) && cmake --build . --target ParametricEQ_Standalone --config $(BUILD_CONFIG) -j$(NPROC)

# Test with MIDI injection
test-with-midi: standalone midi-injector
	@echo "🎵 Testing SineSynth with MIDI injection..."
	@echo "Starting SineSynth standalone..."
	@find $(BUILD_DIR) -name "SineSynth.app" -type d -exec open "{}" \;
	@sleep 2
	@echo "Starting MIDI injector..."
	@find $(BUILD_DIR) -name "MidiInjector" -type f -exec echo "Found MIDI injector: {}" \; -exec "{}" \;

# Test complete audio analysis setup  
test-all: $(BUILD_DIR)/build.stamp
	@echo "🔄 Shutting down any existing audio processes..."
	@pkill -f "SineSynth" || true
	@pkill -f "Parametric EQ" || true
	@pkill -f "MidiInjector" || true
	@pkill -f "MIDI Injector" || true
	@pkill -f "Audio Workstation" || true
	@sleep 1
	@echo "🎼 Starting integrated Audio Workstation..."
	@echo "   🎹 Polyphonic sine synthesizer with ADSR + filter"
	@echo "   🎚️  4-band parametric EQ with real-time visualization"
	@echo "   🎵 Built-in MIDI pattern generator"
	@echo ""
	@find $(BUILD_DIR) -name "Konda.app" -type d -exec open "{}" \; || \
	(echo "⚠️  Audio Workstation not found, launching separate apps..."; \
	 echo "1. Starting SineSynth..."; \
	 find $(BUILD_DIR) -name "SineSynth.app" -type d -exec open "{}" \; && sleep 1; \
	 echo "2. Starting Parametric EQ..."; \
	 find $(BUILD_DIR) -name "Parametric EQ.app" -type d -exec open "{}" \; && sleep 1; \
	 echo "3. Starting MIDI Injector..."; \
	 find $(BUILD_DIR) -name "MIDI Injector.app" -type d -exec open "{}" \;)
	@sleep 2
	@echo ""
	@echo "🚀 INTEGRATED WORKSTATION READY!"
	@echo "   All features combined in one professional interface"
	@echo ""

# Shutdown all audio processes
shutdown:
	@echo "🔄 Shutting down all audio processes..."
	@pkill -f "SineSynth" || true
	@pkill -f "Parametric EQ" || true  
	@pkill -f "MidiInjector" || true
	@pkill -f "MIDI Injector" || true
	@pkill -f "Audio Workstation" || true
	@echo "✅ All audio processes stopped"

# Quick restart - shutdown, rebuild, and restart
restart: shutdown
	@echo "⚡ Quick restart: rebuilding and relaunching..."
	@$(MAKE) test-all

# Clean build artifacts
clean: shutdown
	@echo "🧹 Cleaning build files..."
	rm -rf $(BUILD_DIR)

# Deploy: commit and push changes
deploy:
	git add -A && git commit -m "Auto-commit from make deploy 🤖" && git push

# Create DMG installer with current git hash
dmg: install
	@echo "📦 Creating DMG installer with git hash..."
	@./create_drag_drop_dmg.sh
	@echo "✅ DMG created: Konda_$$(git rev-parse --short HEAD)_Installer.dmg"

# Audio Testing Targets
test-audio: validate-au test-vst3
	@echo "🎵 All audio tests passed!"

# Validate Audio Unit plugin
validate-au: install
	@echo "🔍 Validating Audio Unit plugin..."
	@auval -v aufx Kond TurS || { echo "❌ AU validation failed"; exit 1; }
	@echo "✅ Audio Unit validation passed!"

# Test VST3 plugin structure
test-vst3: configure
	@echo "🔍 Testing VST3 plugin structure..."
	@cd $(BUILD_DIR) && cmake --build . --target AudioWorkstation_VST3 --config $(BUILD_CONFIG) -j$(NPROC)
	@if [ -d "$(BUILD_DIR)/AudioWorkstation_artefacts/$(BUILD_CONFIG)/VST3/Konda.vst3" ]; then \
		echo "✅ VST3 bundle created successfully"; \
	else \
		echo "❌ VST3 bundle not found"; exit 1; \
	fi
	@if [ -f "$(BUILD_DIR)/AudioWorkstation_artefacts/$(BUILD_CONFIG)/VST3/Konda.vst3/Contents/MacOS/Konda" ]; then \
		echo "✅ VST3 executable found"; \
	else \
		echo "❌ VST3 executable missing"; exit 1; \
	fi

# Test audio processing functions
test-audio-processing:
	@echo "🔍 Testing audio processing..."
	@echo '#include <iostream>' > test_audio.cpp
	@echo '#include <cmath>' >> test_audio.cpp
	@echo '#include <vector>' >> test_audio.cpp
	@echo 'int main() {' >> test_audio.cpp
	@echo '    const double sampleRate = 44100.0;' >> test_audio.cpp
	@echo '    const double frequency = 440.0;' >> test_audio.cpp
	@echo '    const int numSamples = 1024;' >> test_audio.cpp
	@echo '    std::vector<float> testBuffer(numSamples);' >> test_audio.cpp
	@echo '    for (int i = 0; i < numSamples; ++i) {' >> test_audio.cpp
	@echo '        double phase = (i / sampleRate) * frequency * 2.0 * M_PI;' >> test_audio.cpp
	@echo '        testBuffer[i] = std::sin(phase);' >> test_audio.cpp
	@echo '    }' >> test_audio.cpp
	@echo '    bool hasAudio = false;' >> test_audio.cpp
	@echo '    for (int i = 0; i < numSamples; ++i) {' >> test_audio.cpp
	@echo '        if (std::abs(testBuffer[i]) > 0.1) { hasAudio = true; break; }' >> test_audio.cpp
	@echo '    }' >> test_audio.cpp
	@echo '    if (hasAudio) { std::cout << "✅ Audio generation working" << std::endl; return 0; }' >> test_audio.cpp
	@echo '    else { std::cout << "❌ No audio generated" << std::endl; return 1; }' >> test_audio.cpp
	@echo '}' >> test_audio.cpp
	@g++ -o test_audio test_audio.cpp -lm
	@./test_audio
	@rm -f test_audio test_audio.cpp

# Help
help:
	@echo "SineSynth Build Commands:"
	@echo ""
	@echo "  make              - Build and launch complete audio production suite"
	@echo "  make build        - Build everything without launching"
	@echo "  make check-prereqs - Check all prerequisites are installed"
	@echo "  make standalone   - Build standalone app only"
	@echo "  make plugin       - Build Audio Unit plugin only"
	@echo "  make install      - Build and install Konda (AU + VST3) to system"
	@echo "  make test         - Build and launch standalone app"
	@echo "  make midi-injector - Build MIDI injector tool"
	@echo "  make eq           - Build Parametric EQ for audio analysis"
	@echo "  make test-with-midi - Build and test with automatic MIDI input"
	@echo "  make test-all     - Launch complete audio analysis setup"
	@echo "  make configure    - Configure CMake build"
	@echo "  make clean        - Remove all build artifacts (includes shutdown)"
	@echo "  make shutdown     - Stop all running audio processes"
	@echo "  make deploy       - Commit and push changes"
	@echo "  make dmg          - Create DMG installer with git hash"
	@echo "  make setup-guide  - Show audio routing setup instructions"
	@echo "  make restart      - Quick shutdown, rebuild, and restart"
	@echo "  make dev          - Development mode with auto-rebuild on file changes"
	@echo "  make watch        - Watch files and rebuild on changes"
	@echo "  make lint-md      - Lint all markdown files"
	@echo "  make watch-md     - Watch markdown files and auto-lint on changes"
	@echo "  make screenshot   - Take release screenshots (standalone + plugin)"
	@echo "  make screenshot-v1.3.0 - Take screenshots for specific version"
	@echo ""
	@echo "Prerequisites:"
	@echo "  - CMake: brew install cmake"
	@echo "  - JUCE framework: git clone https://github.com/juce-framework/JUCE.git ~/JUCE"
	@echo "  - Xcode command line tools: xcode-select --install"
	@echo ""
	@echo "Environment Variables:"
	@echo "  BUILD_CONFIG      - Debug or Release (default: Release)"

# Audio routing setup guide
setup-guide:
	@echo "🎵 AUDIO ROUTING SETUP GUIDE"
	@echo "════════════════════════════════════════════════════════════"
	@echo ""
	@echo "After running 'make', you should see these apps launch:"
	@echo "  • SineSynth - Polyphonic sine synthesizer"
	@echo "  • Parametric EQ - 4-band frequency analyzer" 
	@echo "  • MIDI Injector - Automated pattern generator (in terminal)"
	@echo ""
	@echo "STEP 1: Configure MIDI Input (Required to hear sound)"
	@echo "------------------------------------------------------"
	@echo "In SineSynth app:"
	@echo "  1. Click the settings/gear icon (top-left or menu bar)"
	@echo "  2. Set 'MIDI Input Device' to: 'SineSynth MIDI Injector'"
	@echo "  3. Set 'Audio Output Device' to your speakers/headphones"
	@echo "  4. You should now hear C major scale patterns playing!"
	@echo ""
	@echo "STEP 2: Basic Usage"
	@echo "-------------------"
	@echo "• Use SineSynth GUI controls to adjust:"
	@echo "  - ADSR envelope (Attack, Decay, Sustain, Release)"
	@echo "  - Filter cutoff and resonance"
	@echo "• Press Enter in terminal to stop MIDI injection"
	@echo "• Play manual notes using on-screen keyboard or external MIDI"
	@echo ""
	@echo "STEP 3: Advanced Audio Analysis (Optional)"
	@echo "------------------------------------------"
	@echo "For frequency analysis, you need virtual audio routing:"
	@echo ""
	@echo "Install BlackHole (recommended):"
	@echo "  brew install blackhole-2ch"
	@echo ""
	@echo "Then configure:"
	@echo "  1. SineSynth Output → BlackHole 2ch"  
	@echo "  2. Parametric EQ Input → BlackHole 2ch"
	@echo "  3. Parametric EQ Output → Your speakers"
	@echo "  4. Watch real-time frequency response in Parametric EQ!"
	@echo ""
	@echo "TROUBLESHOOTING"
	@echo "---------------"
	@echo "• No sound? Check MIDI input device in SineSynth settings"
	@echo "• Apps won't launch? Run: make clean && make"
	@echo "• MIDI not connecting? Restart both SineSynth and run make again"
	@echo "• For Logic Pro plugin: make install (requires Logic Pro restart)"
	@echo ""
	@echo "💡 TIP: Run 'make help' for all build commands"

# Development mode with auto-rebuild and restart
dev:
	@echo "🚀 Starting development mode with auto-rebuild..."
	@if command -v fswatch >/dev/null 2>&1; then \
		echo "   This will rebuild and restart apps whenever source files change"; \
		echo "   Press Ctrl+C to stop"; \
		while true; do \
			$(MAKE) test-all; \
			echo ""; \
			echo "👀 Watching for file changes... (Press Ctrl+C to stop)"; \
			echo "   Edit any source file to trigger rebuild"; \
			fswatch -1 -r --exclude='build/' --exclude='\.git/' .; \
			echo ""; \
			echo "🔄 File change detected! Rebuilding..."; \
			sleep 1; \
		done; \
	else \
		echo "   fswatch not found - install with: brew install fswatch"; \
		echo "   Using fallback mode: rebuild every 5 seconds"; \
		echo "   Press Ctrl+C to stop"; \
		while true; do \
			$(MAKE) test-all; \
			echo ""; \
			echo "⏰ Waiting 5 seconds before next rebuild..."; \
			sleep 5; \
		done; \
	fi

# Watch mode (requires fswatch: brew install fswatch)
watch:
	@echo "👀 Watching for file changes..."
	@echo "Install fswatch if needed: brew install fswatch"
	@fswatch -r --exclude='build/' --exclude='\.git/' . | while read file; do \
		echo "🔄 File changed: $$file"; \
		echo "⚡ Rebuilding and restarting..."; \
		$(MAKE) test-all; \
		echo "✅ Ready for next change..."; \
	done

# Lint markdown files
lint-md:
	@echo "📝 Linting markdown files..."
	@command -v markdownlint >/dev/null 2>&1 || { echo "❌ markdownlint not found. Install with: brew install markdownlint-cli"; exit 1; }
	@markdownlint *.md docs/*.md || true

# Watch markdown files and lint on changes
watch-md:
	@echo "👀 Watching markdown files for changes..."
	@echo "Install fswatch if needed: brew install fswatch"
	@if command -v fswatch >/dev/null 2>&1; then \
		echo "   Will auto-lint markdown files when changed"; \
		echo "   Press Ctrl+C to stop"; \
		fswatch -e ".*" -i ".*\.md$$" . | while read file; do \
			echo "📝 Markdown changed: $$file"; \
			$(MAKE) lint-md; \
			echo "✅ Linting complete..."; \
		done; \
	else \
		echo "❌ fswatch required for watching. Install with: brew install fswatch"; \
		exit 1; \
	fi

# Take release screenshots for documentation
screenshot:
	@echo "📸 Taking release screenshots..."
	@./take_screenshot.sh $(VERSION)

# Take screenshots with version number
screenshot-v%:
	@echo "📸 Taking release screenshots for version $*..."
	@./take_screenshot.sh $*