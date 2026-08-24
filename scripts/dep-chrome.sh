#!/bin/bash

set -euo pipefail

source "$(dirname "${BASH_SOURCE[0]}")/env"

dep_entry "$@"

TAU_SHARED_PREFIX="$(dep_shared_prefix)"
# Chrome for Testing build the wasm browser test harness
# (bindings/js/tests/browser-harness.js) is pinned to; bump together with a
# manual check that the suite still passes in it.
CHROME_TAG="$(dep_var CHROME_TAG 150.0.7871.24)"
# @puppeteer/browsers itself, pinned so this script's own behaviour doesn't
# drift with whatever "latest" resolves to on the day it happens to run.
BROWSERS_CLI_TAG="$(dep_var BROWSERS_CLI_TAG 3.2.0)"

CHROME_PREFIX="${TAU_SHARED_PREFIX}/chrome"
CHROME_BIN="${CHROME_PREFIX}/chrome/linux-${CHROME_TAG}/chrome-linux64/chrome"

dep_done_if_exists "$CHROME_BIN" "chrome installing"

mkdir -p "$CHROME_PREFIX"

echo "Installing Chrome for Testing ${CHROME_TAG} into ${CHROME_PREFIX}"
npx --yes "@puppeteer/browsers@${BROWSERS_CLI_TAG}" \
	install "chrome@${CHROME_TAG}" --path "$CHROME_PREFIX"
