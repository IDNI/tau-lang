#!/bin/bash

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PARSER_ROOT="${REPO_ROOT}/external/parser"
DEV_ROOT="${PARSER_ROOT}"

source "${PARSER_ROOT}/scripts/devrc"

PRESET_RUN_BIN="tau"
cd "${REPO_ROOT}"
preset_entry "$@"
