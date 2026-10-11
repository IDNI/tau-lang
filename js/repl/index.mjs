// Tau REPL browser page - the shared terminal host
// (external/parser/js/tau-wasm-terminal/terminal.js, copied beside this file
// by js/repl/CMakeLists.txt) plus Tau's spec-file upload.
(function() {
'use strict';

// The shared start() registers the COOP/COEP service worker (returning null
// while the page reloads) and sets up xterm, the resize/_ftxui_on_resize
// wiring, the immediate-flush stdout+stderr queue and the stdin callback, then
// loads the program script. window.term and window.sendReplInput come from it.
const terminal = tau_wasm_terminal.start({
  element: document.querySelector('#terminal'),
  program: 'tau_repl_web.js',
  arguments: [],
});
if (!terminal) return; // service worker reload in progress

// --- Spec file upload (Tau-specific) ---

var upload   = document.getElementById('upload');
var statusEl = document.getElementById('status');

// tau's REPL has no "load <path>" command (unlike tgf's grammar loader): the
// uploaded spec is fed as REPL input instead, the same as a user pasting or
// typing it in, and evaluated line by line as it arrives.
upload.addEventListener('change', function(e) {
  var file = e.target.files[0];
  if (!file) return;
  var path = '/' + file.name;
  var reader = new FileReader();
  reader.onload = function() {
    if (typeof FS === 'undefined') {
      statusEl.textContent = 'WASM not ready, retrying...';
      setTimeout(function() { reader.onload(); }, 100);
      return;
    }
    var bytes = new Uint8Array(reader.result);
    FS.writeFile(path, bytes);
    statusEl.textContent = 'running: ' + path;
    sendReplInput(new TextDecoder().decode(bytes));
  };
  reader.readAsArrayBuffer(file);
});

})();
