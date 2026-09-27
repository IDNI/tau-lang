#!/bin/bash

source "$(dirname "${BASH_SOURCE[0]}")/env"

PRESET_RUN_BIN="tau"
cd "${REPO_ROOT}"

# resolve_jobs drops -DTAU_BUILD_JOBS from the configure arguments and exports it
# instead, so the cache keeps its empty default and every later reconfigure
# re-resolves the count. The typed spelling survives that drop and is cached.
preset_args=()
for arg in "$@"; do
	preset_args+=("$arg")
	case "$arg" in
		-DTAU_BUILD_JOBS=*)
			preset_args+=("-DTAU_BUILD_JOBS:STRING=${arg#-DTAU_BUILD_JOBS=}")
			;;
	esac
done
preset_entry "${preset_args[@]}"
