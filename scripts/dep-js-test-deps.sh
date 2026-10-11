#!/bin/bash

set -euo pipefail

source "$(dirname "${BASH_SOURCE[0]}")/env"

dep_entry "$@"

# bindings/js/package.json is the dev-tooling manifest for the wasm browser
# test harness (bindings/js/tests/{browser-harness,run-in-chrome,
# run-suite-in-chrome}.js) -- not bindings/js/package.json.in, which
# configure_file's into the build directory as the *published* npm package.
JS_TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../bindings/js" && pwd)"

dep_done_if_exists "${JS_TEST_DIR}/node_modules/puppeteer-core/package.json" \
	"JS browser-test devDependencies installing"

echo "Installing bindings/js devDependencies (puppeteer-core) for the browser test harness"
npm install --prefix "$JS_TEST_DIR" --no-audit --no-fund
