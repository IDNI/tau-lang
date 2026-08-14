#!/bin/bash

# Serve the browser REPL page (js/repl/CMakeLists.txt's output: index.html,
# tau_repl.js/.wasm, vendor/) with the COOP/COEP headers SharedArrayBuffer
# needs, so the page's own sw.js fallback (js/repl/sw.js) is never exercised
# on a server that already does this.
#
# Usage:
#   ./dev tau-repl-serve [port] [build-dir]
#
# build-dir defaults to build/emscripten-pthread (the emscripten-pthread
# preset's binary dir, see CMakePresets.json); port defaults to 8088.

source "$(dirname "${BASH_SOURCE[0]}")/env"

PORT="${1:-8088}"
BUILD_DIR="${2:-${REPO_ROOT}/build/emscripten-pthread}"

if [ ! -f "${BUILD_DIR}/index.html" ]; then
	echo "Error: ${BUILD_DIR}/index.html not found." >&2
	echo "Build first: ./dev preset emscripten-pthread -DTAU_BUILD_JOBS=4" >&2
	exit 1
fi

echo "Serving ${BUILD_DIR} on http://localhost:${PORT}/"
node -e "
const http = require('http'), fs = require('fs'), path = require('path');
const dir = '${BUILD_DIR}';
const mime = {
	'.html': 'text/html',
	'.js': 'application/javascript',
	'.mjs': 'application/javascript',
	'.wasm': 'application/wasm',
	'.css': 'text/css',
	'.data': 'application/octet-stream',
	'.tau': 'text/plain',
};
http.createServer((req, res) => {
	const pathname = new URL(req.url, 'http://x').pathname;
	const fp = path.join(dir, pathname === '/' ? 'index.html' : pathname);
	if (!fp.startsWith(dir)) { res.writeHead(403); res.end(); return; }
	res.setHeader('Cross-Origin-Opener-Policy', 'same-origin');
	res.setHeader('Cross-Origin-Embedder-Policy', 'require-corp');
	let body;
	try { body = fs.readFileSync(fp); }
	catch (e) { res.writeHead(404); res.end(); return; }
	res.writeHead(200, { 'Content-Type': mime[path.extname(fp)] || 'application/octet-stream' });
	res.end(body);
}).listen(${PORT}, '0.0.0.0', () => console.log('Ready'));
"
