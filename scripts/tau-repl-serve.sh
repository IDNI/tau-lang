#!/bin/bash

# Serve the browser REPL page (js/repl/CMakeLists.txt's output: index.html,
# tau_repl_web.js/.wasm, vendor/) with the COOP/COEP headers SharedArrayBuffer
# needs, so the page's own sw.js fallback (js/repl/sw.js) is never exercised
# on a server that already does this.
#
# Usage:
#   ./dev tau-repl-serve [port] [build-dir]
#
# build-dir defaults to build/release-wasm-repl-browser (the release-wasm-repl-browser
# preset's binary dir, see CMakePresets.json); port defaults to 8088.

source "$(dirname "${BASH_SOURCE[0]}")/env"

PORT="${1:-8088}"
BUILD_DIR="${2:-${REPO_ROOT}/build/release-wasm-repl-browser}"

if [ ! -f "${BUILD_DIR}/index.html" ]; then
	echo "Error: ${BUILD_DIR}/index.html not found." >&2
	echo "Build first: ./dev preset release-wasm-repl-browser -DTAU_BUILD_JOBS=4" >&2
	exit 1
fi

echo "Serving ${BUILD_DIR} on http://localhost:${PORT}/"
# The shared server (external/parser/js/tau-wasm-terminal/serve.js) sends the
# COOP/COEP headers.
exec node "${PARSER_ROOT}/js/tau-wasm-terminal/serve.js" "${BUILD_DIR}" "${PORT}"
