#!/bin/bash

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PARSER_ROOT="${REPO_ROOT}/external/parser"
DEV_ROOT="${PARSER_ROOT}"
source "${PARSER_ROOT}/scripts/devrc"

cd "${REPO_ROOT}"
test_entry Debug -DTAU_BUILD_TESTS=ON "$@"
