// Konda UI Controller
// TURBEAUX SOUNDS - User Interface Management

class KondaUI {
    constructor(engine) {
        this.engine = engine;
        this.spectrumCanvas = null;
        this.spectrumContext = null;
        this.isAnimating = false;
        this.keyboard = null;

        // Virtual keyboard mapping
        this.keyboardMapping = {
            // White keys: C, D, E, F, G, A, B
            'z': 60, 'x': 62, 'c': 64, 'v': 66, 'b': 68, 'n': 70, 'm': 72,
            ',': 74, '.': 76, '/': 78,
            // Black keys: C#, D#, F#, G#, A#
            's': 61, 'd': 63, 'g': 67, 'h': 69, 'j': 71, 'l': 73, ';': 75
        };

        this.pressedKeys = new Set();
        this.activeNotes = new Map();
    }

    initialize() {
        this.setupSpectrumAnalyzer();
        this.setupVirtualKeyboard();
        this.setupControlListeners();
        this.setupKeyboardListeners();
        this.startSpectrumAnimation();
        console.log('Konda UI initialized');
    }

    setupSpectrumAnalyzer() {
        this.spectrumCanvas = document.getElementById('spectrumCanvas');
        this.spectrumContext = this.spectrumCanvas.getContext('2d');

        // Set canvas resolution
        const rect = this.spectrumCanvas.getBoundingClientRect();
        this.spectrumCanvas.width = rect.width * window.devicePixelRatio;
        this.spectrumCanvas.height = rect.height * window.devicePixelRatio;
        this.spectrumContext.scale(window.devicePixelRatio, window.devicePixelRatio);
    }

    setupVirtualKeyboard() {
        const keyboard = document.getElementById('keyboard');
        keyboard.style.position = 'relative'; // Ensure relative positioning for absolute children
        keyboard.style.display = 'flex'; // Ensure flex layout for white keys
        keyboard.style.justifyContent = 'center';
        keyboard.style.gap = '2px'; // Match CSS gap

        // Create octave of keys (C4 to B4)
        const whiteKeys = ['C', 'D', 'E', 'F', 'G', 'A', 'B'];
        const blackKeyInfo = [
            { note: 'C#', midi: 61, position: 0.5 },  // Between C and D
            { note: 'D#', midi: 63, position: 1.5 },  // Between D and E
            { note: 'F#', midi: 66, position: 3.5 },  // Between F and G
            { note: 'G#', midi: 68, position: 4.5 },  // Between G and A
            { note: 'A#', midi: 70, position: 5.5 }   // Between A and B
        ];

        let midiNote = 60; // C4

        // Create white keys first (C, D, E, F, G, A, B = MIDI notes 60, 62, 64, 65, 67, 69, 71)
        const whiteMidiNotes = [60, 62, 64, 65, 67, 69, 71];
        whiteKeys.forEach((note, index) => {
            const key = this.createKey(note + '4', whiteMidiNotes[index], 'white');
            keyboard.appendChild(key);
        });

        // Create black keys with proper positioning
        // Need to position after white keys are rendered to get accurate positions
        setTimeout(() => {
            blackKeyInfo.forEach((blackKey, index) => {
                const key = this.createKey(blackKey.note + '4', blackKey.midi, 'black');
                key.style.position = 'absolute';
                key.style.top = '0';

                // Calculate position based on which white keys this black key sits between
                const whiteKeyWidth = 40; // CSS defined width
                const gap = 2; // CSS defined gap
                const blackKeyWidth = 25; // CSS defined width

                // Position black key centered between the appropriate white keys
                let leftPosition;
                switch(blackKey.note) {
                    case 'C#': // Between C (0) and D (1)
                        leftPosition = (whiteKeyWidth + gap/2) - (blackKeyWidth/2);
                        break;
                    case 'D#': // Between D (1) and E (2)
                        leftPosition = (whiteKeyWidth + gap) * 2 + (gap/2) - (blackKeyWidth/2);
                        break;
                    case 'F#': // Between F (3) and G (4)
                        leftPosition = (whiteKeyWidth + gap) * 3 + (whiteKeyWidth + gap/2) - (blackKeyWidth/2);
                        break;
                    case 'G#': // Between G (4) and A (5)
                        leftPosition = (whiteKeyWidth + gap) * 4 + (whiteKeyWidth + gap/2) - (blackKeyWidth/2);
                        break;
                    case 'A#': // Between A (5) and B (6)
                        leftPosition = (whiteKeyWidth + gap) * 5 + (whiteKeyWidth + gap/2) - (blackKeyWidth/2);
                        break;
                }

                key.style.left = `${leftPosition}px`;
                key.style.zIndex = '10';
                keyboard.appendChild(key);
            });
        }, 0);

        this.keyboard = keyboard;
    }

    createKey(label, midiNote, type) {
        const key = document.createElement('div');
        key.className = `key ${type}`;
        key.dataset.note = midiNote;

        const keyLabel = document.createElement('span');
        keyLabel.className = 'key-label';
        keyLabel.textContent = label;
        key.appendChild(keyLabel);

        // Mouse events
        key.addEventListener('mousedown', (e) => {
            e.preventDefault();
            this.playKeyboardNote(midiNote, key);
        });

        key.addEventListener('mouseup', (e) => {
            e.preventDefault();
            this.releaseKeyboardNote(midiNote, key);
        });

        key.addEventListener('mouseleave', (e) => {
            this.releaseKeyboardNote(midiNote, key);
        });

        // Touch events for mobile
        key.addEventListener('touchstart', (e) => {
            e.preventDefault();
            this.playKeyboardNote(midiNote, key);
        });

        key.addEventListener('touchend', (e) => {
            e.preventDefault();
            this.releaseKeyboardNote(midiNote, key);
        });

        return key;
    }

    setupControlListeners() {
        // Power button
        const powerButton = document.getElementById('powerButton');
        powerButton.addEventListener('click', async () => {
            if (!this.engine.isStarted) {
                const success = await this.engine.initialize();
                if (success) {
                    await this.engine.resume();
                    powerButton.textContent = '⚡ ACTIVE';
                    powerButton.classList.add('active');
                }
            } else {
                await this.engine.resume();
            }
        });

        // Master volume
        const masterVolume = document.getElementById('masterVolume');
        const volumeValue = document.getElementById('volumeValue');
        masterVolume.addEventListener('input', (e) => {
            const value = parseInt(e.target.value);
            this.engine.setMasterVolume(value);
            volumeValue.textContent = value + '%';
        });

        // Synth controls
        this.setupSliderControl('attack', 'synthParams', (value) => value + 's');
        this.setupSliderControl('decay', 'synthParams', (value) => value + 's');
        this.setupSliderControl('sustain', 'synthParams', (value) => value);
        this.setupSliderControl('release', 'synthParams', (value) => value + 's');
        this.setupSliderControl('filterCutoff', 'synthParams', (value) => value + 'Hz');
        this.setupSliderControl('resonance', 'synthParams', (value) => value);

        // EQ controls
        this.setupSliderControl('lowGain', 'eqParams', (value) => value + 'dB');
        this.setupSliderControl('peak1Freq', 'eqParams', (value) => this.formatFrequency(value));
        this.setupSliderControl('peak1Gain', 'eqParams', (value) => value + 'dB');
        this.setupSliderControl('peak1Q', 'eqParams', (value) => 'Q:' + value);
        this.setupSliderControl('peak2Freq', 'eqParams', (value) => this.formatFrequency(value));
        this.setupSliderControl('peak2Gain', 'eqParams', (value) => value + 'dB');
        this.setupSliderControl('peak2Q', 'eqParams', (value) => 'Q:' + value);
        this.setupSliderControl('highGain', 'eqParams', (value) => value + 'dB');

        // MIDI controls
        const tempoSlider = document.getElementById('tempo');
        const tempoValue = tempoSlider.nextElementSibling;
        tempoSlider.addEventListener('input', (e) => {
            tempoValue.textContent = e.target.value + ' BPM';
        });

        const playButton = document.getElementById('playPattern');
        const stopButton = document.getElementById('stopPattern');
        const randomButton = document.getElementById('randomAll');

        playButton.addEventListener('click', () => {
            const mode = document.getElementById('musicalMode').value;
            const pattern = document.getElementById('pattern').value;
            const tempo = parseInt(document.getElementById('tempo').value);

            this.engine.startPattern(mode, pattern, tempo);
            playButton.classList.add('playing');
        });

        stopButton.addEventListener('click', () => {
            this.engine.stopPattern();
            playButton.classList.remove('playing');
        });

        randomButton.addEventListener('click', () => {
            this.engine.randomizeAll();
        });
    }

    setupSliderControl(id, paramType, formatter) {
        const slider = document.getElementById(id);
        const valueDisplay = slider.parentElement.querySelector('.value');

        slider.addEventListener('input', (e) => {
            const value = parseFloat(e.target.value);

            if (paramType === 'synthParams') {
                this.engine.updateSynthParam(id, value);
            } else if (paramType === 'eqParams') {
                this.engine.updateEQParam(id, value);
            }

            if (valueDisplay && formatter) {
                valueDisplay.textContent = formatter(value);
            }
        });
    }

    setupKeyboardListeners() {
        document.addEventListener('keydown', (e) => {
            if (this.pressedKeys.has(e.code)) return; // Prevent key repeat

            const midiNote = this.keyboardMapping[e.key.toLowerCase()];
            if (midiNote) {
                e.preventDefault();
                this.pressedKeys.add(e.code);
                this.playKeyboardNote(midiNote);
            }
        });

        document.addEventListener('keyup', (e) => {
            const midiNote = this.keyboardMapping[e.key.toLowerCase()];
            if (midiNote) {
                e.preventDefault();
                this.pressedKeys.delete(e.code);
                this.releaseKeyboardNote(midiNote);
            }
        });

        // Prevent context menu on right-click for keys
        document.addEventListener('contextmenu', (e) => {
            if (e.target.classList.contains('key')) {
                e.preventDefault();
            }
        });
    }

    playKeyboardNote(midiNote, keyElement = null) {
        if (this.activeNotes.has(midiNote)) return;

        const frequency = this.engine.midiToFrequency(midiNote);
        const voice = this.engine.playNote(frequency, 0.7);

        if (voice) {
            this.activeNotes.set(midiNote, voice);

            if (keyElement) {
                keyElement.classList.add('pressed');
            } else {
                // Find key element by midi note
                const key = this.keyboard.querySelector(`[data-note="${midiNote}"]`);
                if (key) key.classList.add('pressed');
            }
        }
    }

    releaseKeyboardNote(midiNote, keyElement = null) {
        if (this.activeNotes.has(midiNote)) {
            this.engine.releaseNote(this.engine.midiToFrequency(midiNote));
            this.activeNotes.delete(midiNote);
        }

        if (keyElement) {
            keyElement.classList.remove('pressed');
        } else {
            // Find key element by midi note
            const key = this.keyboard.querySelector(`[data-note="${midiNote}"]`);
            if (key) key.classList.remove('pressed');
        }
    }

    startSpectrumAnimation() {
        if (this.isAnimating) return;

        this.isAnimating = true;
        const animate = () => {
            if (!this.isAnimating) return;

            this.drawSpectrum();
            requestAnimationFrame(animate);
        };

        animate();
    }

    drawSpectrum() {
        if (!this.spectrumContext || !this.engine.isStarted) return;

        const canvas = this.spectrumCanvas;
        const ctx = this.spectrumContext;
        const rect = canvas.getBoundingClientRect();
        const width = rect.width;
        const height = rect.height;

        // Clear canvas
        ctx.fillStyle = '#000';
        ctx.fillRect(0, 0, width, height);

        // Get spectrum data
        const dataArray = this.engine.getSpectrumData();
        if (!dataArray) return;

        const bufferLength = dataArray.length;
        const sampleRate = this.engine.audioContext.sampleRate;
        const nyquist = sampleRate / 2;

        // Use logarithmic frequency mapping
        const minFreq = 20;
        const maxFreq = Math.min(nyquist, 20000);
        const logMin = Math.log(minFreq);
        const logMax = Math.log(maxFreq);
        const logRange = logMax - logMin;

        // Draw frequency bars with logarithmic mapping
        const numBars = Math.min(bufferLength / 2, 400); // Limit for performance
        const barWidth = width / numBars;

        for (let i = 0; i < numBars; i++) {
            // Map logarithmically from screen position to frequency
            const logFreq = logMin + (i / numBars) * logRange;
            const freq = Math.exp(logFreq);

            // Convert frequency to FFT bin index
            const binIndex = Math.floor((freq / nyquist) * bufferLength);

            if (binIndex < bufferLength) {
                const value = dataArray[binIndex] / 255;
                const barHeight = value * height * 1.2; // Slight amplification for visibility

                // Create HSV color based on frequency (logarithmic)
                const hue = (i / numBars) * 240; // Blue to red across log scale
                const saturation = 70 + (value * 30);
                const brightness = 50 + (value * 50);

                ctx.fillStyle = `hsl(${hue}, ${saturation}%, ${brightness}%)`;
                ctx.fillRect(i * barWidth, height - barHeight, barWidth, barHeight);

                // Add glow effect for strong signals
                if (value > 0.3) {
                    ctx.shadowBlur = 6;
                    ctx.shadowColor = `hsl(${hue}, 100%, 70%)`;
                    ctx.fillRect(i * barWidth, height - barHeight, barWidth, barHeight);
                    ctx.shadowBlur = 0;
                }

                // Add sparkle effects for peaks
                if (value > 0.7) {
                    ctx.fillStyle = '#ffffff';
                    ctx.fillRect(i * barWidth + barWidth/2, height - barHeight - 5, 2, 2);
                }
            }
        }

        // Draw frequency grid lines (now properly aligned)
        ctx.strokeStyle = 'rgba(255, 255, 255, 0.1)';
        ctx.lineWidth = 1;

        // Professional frequency markers
        const frequencies = [50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000];
        frequencies.forEach(freq => {
            if (freq >= minFreq && freq <= maxFreq) {
                const logPos = (Math.log(freq) - logMin) / logRange;
                const x = logPos * width;
                ctx.beginPath();
                ctx.moveTo(x, 0);
                ctx.lineTo(x, height);
                ctx.stroke();
            }
        });

        // Add dB grid lines (horizontal)
        ctx.strokeStyle = 'rgba(255, 255, 255, 0.05)';
        for (let db = 0; db < height; db += height / 6) {
            ctx.beginPath();
            ctx.moveTo(0, db);
            ctx.lineTo(width, db);
            ctx.stroke();
        }
    }

    formatFrequency(value) {
        if (value >= 1000) {
            return (value / 1000).toFixed(1) + 'kHz';
        }
        return Math.round(value) + 'Hz';
    }

    updateAllControls() {
        // Update synth controls
        Object.keys(this.engine.synthParams).forEach(param => {
            const slider = document.getElementById(param);
            const valueDisplay = slider?.parentElement.querySelector('.value');

            if (slider) {
                slider.value = this.engine.synthParams[param];

                if (valueDisplay) {
                    let formattedValue;
                    switch (param) {
                        case 'attack':
                        case 'decay':
                        case 'release':
                            formattedValue = this.engine.synthParams[param] + 's';
                            break;
                        case 'filterCutoff':
                            formattedValue = this.formatFrequency(this.engine.synthParams[param]);
                            break;
                        default:
                            formattedValue = this.engine.synthParams[param];
                    }
                    valueDisplay.textContent = formattedValue;
                }
            }
        });

        // Update EQ controls
        Object.keys(this.engine.eqParams).forEach(param => {
            const slider = document.getElementById(param);
            const valueDisplay = slider?.parentElement.querySelector('.value');

            if (slider) {
                slider.value = this.engine.eqParams[param];

                if (valueDisplay) {
                    let formattedValue;
                    if (param.includes('Freq')) {
                        formattedValue = this.formatFrequency(this.engine.eqParams[param]);
                    } else if (param.includes('Gain')) {
                        formattedValue = this.engine.eqParams[param] + 'dB';
                    } else if (param.includes('Q')) {
                        formattedValue = 'Q:' + this.engine.eqParams[param];
                    }
                    valueDisplay.textContent = formattedValue;
                }
            }
        });
    }
}

// Export for global use
window.KondaUI = KondaUI;