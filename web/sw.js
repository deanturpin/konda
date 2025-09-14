// Konda Service Worker for PWA capabilities
const CACHE_NAME = 'konda-v1.2.2';
const urlsToCache = [
  './',
  './index.html',
  './style.css',
  './konda-engine.js',
  './konda-ui.js',
  './app.js'
];

self.addEventListener('install', event => {
  event.waitUntil(
    caches.open(CACHE_NAME)
      .then(cache => cache.addAll(urlsToCache))
  );
});

self.addEventListener('fetch', event => {
  event.respondWith(
    caches.match(event.request)
      .then(response => {
        // Return cached version or fetch from network
        return response || fetch(event.request);
      }
    )
  );
});