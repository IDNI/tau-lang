#!/bin/bash
# The @idni/tau-lang npm package for wasm32-emscripten.
# scripts/dep/common/tau-js.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=wasm32-emscripten
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

source "$(dirname "${BASH_SOURCE[0]}")/../common/tau-js.sh"
