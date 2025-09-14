// Konda Web Audio Workstation - Main Application
// TURBEAUX SOUNDS - Professional Audio Production

class KondaApp {
    constructor() {
        this.engine = null;
        this.ui = null;
        this.isInitialized = false;
    }

    async initialize() {
        try {
            console.log('🎵 Initializing Konda Web Audio Workstation...');

            // Create engine and UI
            this.engine = new KondaEngine();
            this.ui = new KondaUI(this.engine);

            // Initialize UI first (doesn't require audio context)
            this.ui.initialize();

            // Make globally available
            window.kondaEngine = this.engine;
            window.kondaUI = this.ui;

            this.isInitialized = true;
            console.log('✅ Konda Web Audio Workstation ready!');

            this.showWelcomeMessage();

        } catch (error) {
            console.error('❌ Failed to initialize Konda:', error);
            this.showErrorMessage(error);
        }
    }

    showWelcomeMessage() {
        console.log(`
🎼 KONDA - TURBEAUX SOUNDS Web Audio Workstation
═══════════════════════════════════════════════

✨ Features:
• 4-voice polyphonic synthesizer with ADSR
• Real-time FFT spectrum analyzer
• 4-band parametric EQ with live visualization
• MIDI pattern generator with 7 musical modes
• Virtual keyboard with computer key mapping

🎹 Keyboard Controls:
• White keys: Z X C V B N M , . /
• Black keys: S D G H J L ;

🚀 Click the "⚡ START" button to begin!

Built with Web Audio API - Compatible with all modern browsers
        `);
    }

    showErrorMessage(error) {
        const message = `
❌ Konda Initialization Error

${error.message}

Please ensure:
• Your browser supports Web Audio API
• Audio permissions are granted
• Browser is not muted

Supported browsers:
• Chrome 66+
• Firefox 60+
• Safari 14.1+
• Edge 79+
        `;

        console.error(message);
        alert('Konda failed to initialize. Check console for details.');
    }
}

// Auto-initialize when DOM is ready
document.addEventListener('DOMContentLoaded', async () => {
    const app = new KondaApp();
    await app.initialize();

    // Make app globally available for debugging
    window.kondaApp = app;
});

// Handle page visibility changes
document.addEventListener('visibilitychange', () => {
    if (!document.hidden && window.kondaEngine?.audioContext?.state === 'suspended') {
        window.kondaEngine.resume();
    }
});

// Prevent accidental page refresh during performance
window.addEventListener('beforeunload', (e) => {
    if (window.kondaEngine?.isPlaying) {
        e.preventDefault();
        e.returnValue = 'Are you sure you want to leave? Your performance will be stopped.';
        return e.returnValue;
    }
});

// Service Worker registration for PWA capabilities (if available)
if ('serviceWorker' in navigator) {
    navigator.serviceWorker.register('./sw.js').catch(err => {
        console.log('Service Worker registration failed:', err);
    });
}

console.log('🎵 Konda Web Audio Workstation loading...');