// Service worker that adds the COOP/COEP response headers tau_repl.js needs
// for SharedArrayBuffer (its FTXUI REPL is built -pthread). A registration
// fallback for servers that do not set these headers themselves (see
// scripts/tau-repl-serve.sh, which does).
self.addEventListener('install', () => self.skipWaiting());
self.addEventListener('activate', e => e.waitUntil(self.clients.claim()));
self.addEventListener('fetch', e => {
  if (e.request.mode != 'navigate' &&
    !e.request.url.includes('.worker.js')) {
    return;
  }

  e.respondWith((async () => {
    const response = await fetch(e.request);

    const newHeaders = new Headers(response.headers);
    newHeaders.set('Cross-Origin-Embedder-Policy', 'require-corp');
    newHeaders.set('Cross-Origin-Opener-Policy', 'same-origin');

    const moddedResponse = new Response(response.body, {
      status: response.status,
      statusText: response.statusText,
      headers: newHeaders,
    });

    return moddedResponse;
  })());
});
