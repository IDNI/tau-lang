#!/bin/bash

set -euo pipefail

# The existence check must come first: devrc (and dep_entry/dep_shared_prefix)
# live inside the very external/parser submodule this checks for, so they
# cannot be sourced to report the error below.
TAU_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PARSER_DEP_EMSDK_SH="${TAU_ROOT}/external/parser/scripts/dep-emsdk.sh"

if [ ! -f "$PARSER_DEP_EMSDK_SH" ]; then
	echo "error: ${PARSER_DEP_EMSDK_SH} not found; the external/parser submodule is not initialised." >&2
	echo "hint: run './dev dep-emsdk' (it initializes the submodule itself, once)" >&2
	exit 1
fi

exec "$PARSER_DEP_EMSDK_SH" "$@"
