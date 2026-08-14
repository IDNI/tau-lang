// Tau REPL browser page - xterm.js glue around the FTXUI REPL built as
// tau_repl.js/tau_repl.wasm (js/repl/CMakeLists.txt).
(function() {
'use strict';

// COOP/COEP via ServiceWorker - required for SharedArrayBuffer (pthreads)
if ('serviceWorker' in navigator && !window.crossOriginIsolated) {
  const url_sw = new URL('./sw.js', location.href);
  navigator.serviceWorker.register(url_sw).then(() => location.reload());
  return;
}

// --- Terminal setup ---

const term_element = document.querySelector('#terminal');
const term = new Terminal();
term.options.scrollback = 1000;
term.options.convertEol = true; // REPL output uses bare \n; map to \r\n
term.options.fontFamily =
  "'DejaVu Sans Mono', 'Liberation Mono', 'Noto Sans Mono', monospace";
term.open(term_element);

const fit_addon = new FitAddon.FitAddon();
term.loadAddon(fit_addon);

const stdin_buffer  = [];
const stdout_buffer = [];
const stderr_buffer = [];

const stdin = () => stdin_buffer.shift() || 0;

const stdout = code => {
  if (code == 0 || code == 10) {
    if (code == 10) stdout_buffer.push(code);
    if (stdout_buffer.length) {
      term.write(new Uint8Array(stdout_buffer));
      stdout_buffer.length = 0;
    }
  } else {
    stdout_buffer.push(code);
  }
};

// stderr renders into the terminal, like native tau on a tty (this is also
// where Boost.Log's default sink -- the welcome banner -- ends up).
const stderr = code => {
  if (code == 0 || code == 10) {
    if (code == 10) stderr_buffer.push(code);
    if (stderr_buffer.length) {
      term.write(new Uint8Array(stderr_buffer));
      stderr_buffer.length = 0;
    }
  } else {
    stderr_buffer.push(code);
  }
};

const onBinary = e => {
  for (const c of e) stdin_buffer.push(c.charCodeAt(0));
};

term.onBinary(onBinary);
term.onData(onBinary);
window.term = term;
window.sendReplInput = text => {
  for (const c of text) stdin_buffer.push(c.charCodeAt(0));
};
term.resize(140, 43);

// --- Module glue (must be set BEFORE loading the WASM script) ---

window.Module = {
  preRun: [],
  onRuntimeInitialized: () => {
    if (window.Module._ftxui_on_resize === undefined) return;
    fit_addon.fit();
    const resize_handler = () => {
      const dims = fit_addon.proposeDimensions();
      term.resize(dims.cols, dims.rows);
      window.Module._ftxui_on_resize(dims.cols, dims.rows);
      fit_addon.fit();
    };
    const resize_observer = new ResizeObserver(resize_handler);
    resize_observer.observe(term_element);
    resize_handler();
  },
};

window.Module.preRun.push(() => {
  FS.init(stdin, stdout, stderr);
});

// --- Spec file upload ---

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

// --- Load the WASM runtime ---

var script = document.createElement('script');
script.src = 'tau_repl.js';
document.head.appendChild(script);

})();
