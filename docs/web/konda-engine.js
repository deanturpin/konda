// Konda Web Audio Engine
// TURBEAUX SOUNDS - Professional Web Audio Workstation

class KondaEngine {
    constructor() {
        this.audioContext = null;
        this.isStarted = false;
        this.masterGain = null;
        this.analyser = null;
        this.dataArray = null;
        this.voices = [];
        this.maxVoices = 4;

        // Synthesis parameters
        this.synthParams = {
            attack: 0.1,
            decay: 0.2,
            sustain: 0.7,
            release: 0.5,
            filterCutoff: 800,
            resonance: 0.5
        };

        // EQ parameters
        this.eqParams = {
            lowGain: 0,
            peak1Freq: 400,
            peak1Gain: 0,
            peak1Q: 1,
            peak2Freq: 2000,
            peak2Gain: 0,
            peak2Q: 1,
            highGain: 0
        };

        // MIDI pattern generator
        this.patternPlayer = null;
        this.isPlaying = false;

        // Musical modes (intervals in semitones)
        this.modes = {
            major: [0, 2, 4, 5, 7, 9, 11],
            minor: [0, 2, 3, 5, 7, 8, 10],
            dorian: [0, 2, 3, 5, 7, 9, 10],
            phrygian: [0, 1, 3, 5, 7, 8, 10],
            lydian: [0, 2, 4, 6, 7, 9, 11],
            mixolydian: [0, 2, 4, 5, 7, 9, 10],
            locrian: [0, 1, 3, 5, 6, 8, 10]
        };
    }

    async initialize() {
        try {
            this.audioContext = new (window.AudioContext || window.webkitAudioContext)();

            // Create master gain
            this.masterGain = this.audioContext.createGain();
            this.masterGain.gain.value = 0.5;

            // Create analyser for spectrum visualization
            this.analyser = this.audioContext.createAnalyser();
            this.analyser.fftSize = 2048;
            this.analyser.smoothingTimeConstant = 0.8;
            this.dataArray = new Uint8Array(this.analyser.frequencyBinCount);

            // Create EQ chain
            this.createEQChain();

            // Connect: masterGain -> EQ -> analyser -> destination
            this.masterGain.connect(this.lowShelf);
            this.highShelf.connect(this.analyser);
            this.analyser.connect(this.audioContext.destination);

            this.isStarted = true;
            console.log('Konda Engine initialized successfully');
            return true;
        } catch (error) {
            console.error('Failed to initialize audio context:', error);
            return false;
        }
    }

    createEQChain() {
        // Low shelf
        this.lowShelf = this.audioContext.createBiquadFilter();
        this.lowShelf.type = 'lowshelf';
        this.lowShelf.frequency.value = 100;
        this.lowShelf.gain.value = 0;

        // Peak 1
        this.peak1 = this.audioContext.createBiquadFilter();
        this.peak1.type = 'peaking';
        this.peak1.frequency.value = 400;
        this.peak1.Q.value = 1;
        this.peak1.gain.value = 0;

        // Peak 2
        this.peak2 = this.audioContext.createBiquadFilter();
        this.peak2.type = 'peaking';
        this.peak2.frequency.value = 2000;
        this.peak2.Q.value = 1;
        this.peak2.gain.value = 0;

        // High shelf
        this.highShelf = this.audioContext.createBiquadFilter();
        this.highShelf.type = 'highshelf';
        this.highShelf.frequency.value = 8000;
        this.highShelf.gain.value = 0;

        // Connect EQ chain
        this.lowShelf.connect(this.peak1);
        this.peak1.connect(this.peak2);
        this.peak2.connect(this.highShelf);
    }

    resume() {
        if (this.audioContext && this.audioContext.state === 'suspended') {
            return this.audioContext.resume();
        }
        return Promise.resolve();
    }

    createVoice(frequency, velocity = 0.5) {
        if (!this.isStarted) return null;

        const voice = {
            oscillator: this.audioContext.createOscillator(),
            gainNode: this.audioContext.createGain(),
            filter: this.audioContext.createBiquadFilter(),
            frequency: frequency,
            startTime: this.audioContext.currentTime
        };

        // Configure oscillator
        voice.oscillator.type = 'sine';
        voice.oscillator.frequency.value = frequency;

        // Configure filter
        voice.filter.type = 'lowpass';
        voice.filter.frequency.value = this.synthParams.filterCutoff;
        voice.filter.Q.value = this.synthParams.resonance;

        // Configure ADSR envelope
        const attackTime = this.synthParams.attack;
        const decayTime = this.synthParams.decay;
        const sustainLevel = this.synthParams.sustain * velocity;

        voice.gainNode.gain.value = 0;
        voice.gainNode.gain.setValueAtTime(0, voice.startTime);
        voice.gainNode.gain.linearRampToValueAtTime(velocity, voice.startTime + attackTime);
        voice.gainNode.gain.linearRampToValueAtTime(sustainLevel, voice.startTime + attackTime + decayTime);

        // Connect audio graph
        voice.oscillator.connect(voice.filter);
        voice.filter.connect(voice.gainNode);
        voice.gainNode.connect(this.masterGain);

        voice.oscillator.start(voice.startTime);

        return voice;
    }

    playNote(frequency, velocity = 0.5, duration = null) {
        // Stop oldest voice if at limit
        if (this.voices.length >= this.maxVoices) {
            this.stopVoice(this.voices[0]);
        }

        const voice = this.createVoice(frequency, velocity);
        if (voice) {
            this.voices.push(voice);

            // Auto-release note if duration specified
            if (duration) {
                setTimeout(() => {
                    this.releaseNote(frequency);
                }, duration * 1000);
            }
        }

        return voice;
    }

    releaseNote(frequency) {
        const voiceIndex = this.voices.findIndex(v => Math.abs(v.frequency - frequency) < 1);
        if (voiceIndex !== -1) {
            this.releaseVoice(this.voices[voiceIndex]);
        }
    }

    releaseVoice(voice) {
        const releaseTime = this.synthParams.release;
        const now = this.audioContext.currentTime;

        voice.gainNode.gain.cancelScheduledValues(now);
        voice.gainNode.gain.setValueAtTime(voice.gainNode.gain.value, now);
        voice.gainNode.gain.linearRampToValueAtTime(0, now + releaseTime);

        // Clean up after release
        setTimeout(() => {
            this.stopVoice(voice);
        }, releaseTime * 1000 + 100);
    }

    stopVoice(voice) {
        const index = this.voices.indexOf(voice);
        if (index !== -1) {
            try {
                voice.oscillator.stop();
                voice.oscillator.disconnect();
                voice.filter.disconnect();
                voice.gainNode.disconnect();
            } catch (e) {
                // Voice might already be stopped
            }
            this.voices.splice(index, 1);
        }
    }

    stopAllNotes() {
        this.voices.forEach(voice => {
            this.releaseVoice(voice);
        });
    }

    updateSynthParam(param, value) {
        this.synthParams[param] = value;

        // Update active voices for filter parameters
        if (param === 'filterCutoff') {
            this.voices.forEach(voice => {
                voice.filter.frequency.value = value;
            });
        } else if (param === 'resonance') {
            this.voices.forEach(voice => {
                voice.filter.Q.value = value;
            });
        }
    }

    updateEQParam(param, value) {
        this.eqParams[param] = value;

        switch (param) {
            case 'lowGain':
                this.lowShelf.gain.value = value;
                break;
            case 'peak1Freq':
                this.peak1.frequency.value = value;
                break;
            case 'peak1Gain':
                this.peak1.gain.value = value;
                break;
            case 'peak1Q':
                this.peak1.Q.value = value;
                break;
            case 'peak2Freq':
                this.peak2.frequency.value = value;
                break;
            case 'peak2Gain':
                this.peak2.gain.value = value;
                break;
            case 'peak2Q':
                this.peak2.Q.value = value;
                break;
            case 'highGain':
                this.highShelf.gain.value = value;
                break;
        }
    }

    setMasterVolume(volume) {
        if (this.masterGain) {
            this.masterGain.gain.value = volume / 100;
        }
    }

    getSpectrumData() {
        if (this.analyser) {
            this.analyser.getByteFrequencyData(this.dataArray);
            return this.dataArray;
        }
        return null;
    }

    // MIDI Pattern Player
    startPattern(mode, pattern, tempo = 120) {
        this.stopPattern();

        const scale = this.modes[mode] || this.modes.major;
        const baseNote = 60; // C4
        const noteDuration = 60 / tempo; // Duration in seconds

        let patternNotes = [];

        // Generate pattern based on type
        switch (pattern) {
            case 'scale':
                patternNotes = scale.map(interval => baseNote + interval);
                patternNotes.push(baseNote + 12); // Octave
                break;
            case 'arpeggios':
                patternNotes = [0, 2, 4, 6, 4, 2].map(i => baseNote + scale[i % scale.length]);
                break;
            case 'chords':
                // Play triads
                for (let i = 0; i < scale.length; i += 2) {
                    patternNotes.push(baseNote + scale[i]);
                    patternNotes.push(baseNote + scale[(i + 2) % scale.length]);
                    patternNotes.push(baseNote + scale[(i + 4) % scale.length]);
                }
                break;
            case 'steps':
                patternNotes = [0, 1, 0, 2, 0, 3, 0, 4].map(i => baseNote + scale[i % scale.length]);
                break;
            case 'cascade':
                for (let octave = 0; octave < 2; octave++) {
                    scale.forEach(interval => {
                        patternNotes.push(baseNote + interval + (octave * 12));
                    });
                }
                break;
        }

        this.isPlaying = true;
        let currentNote = 0;

        const playNextNote = () => {
            if (!this.isPlaying) return;

            if (patternNotes.length > 0) {
                const midiNote = patternNotes[currentNote % patternNotes.length];
                const frequency = this.midiToFrequency(midiNote);

                this.playNote(frequency, 0.6, noteDuration * 0.8);

                currentNote++;
                setTimeout(playNextNote, noteDuration * 1000);
            }
        };

        playNextNote();
    }

    stopPattern() {
        this.isPlaying = false;
        this.stopAllNotes();
    }

    midiToFrequency(midiNote) {
        return 440 * Math.pow(2, (midiNote - 69) / 12);
    }

    // Random parameter generation
    randomizeAll() {
        // Random synth parameters
        this.updateSynthParam('attack', Math.random() * 1.5 + 0.01);
        this.updateSynthParam('decay', Math.random() * 1.5 + 0.01);
        this.updateSynthParam('sustain', Math.random() * 0.8 + 0.2);
        this.updateSynthParam('release', Math.random() * 2 + 0.1);
        this.updateSynthParam('filterCutoff', Math.random() * 4000 + 200);
        this.updateSynthParam('resonance', Math.random() * 2.5 + 0.1);

        // Random EQ parameters
        this.updateEQParam('lowGain', (Math.random() - 0.5) * 24);
        this.updateEQParam('peak1Freq', Math.random() * 1980 + 20);
        this.updateEQParam('peak1Gain', (Math.random() - 0.5) * 24);
        this.updateEQParam('peak1Q', Math.random() * 8 + 0.5);
        this.updateEQParam('peak2Freq', Math.random() * 18000 + 1000);
        this.updateEQParam('peak2Gain', (Math.random() - 0.5) * 24);
        this.updateEQParam('peak2Q', Math.random() * 8 + 0.5);
        this.updateEQParam('highGain', (Math.random() - 0.5) * 24);

        // Trigger UI update
        if (window.kondaUI) {
            window.kondaUI.updateAllControls();
        }
    }
}

// Export for global use
window.KondaEngine = KondaEngine;