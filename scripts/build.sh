#!/bin/bash

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PARSER_ROOT="${REPO_ROOT}/external/parser"
DEV_ROOT="${PARSER_ROOT}"

source "${PARSER_ROOT}/scripts/devrc"

dev_pre_configure() {
	local tau_shared_prefix="${HOME}/.tau" arg
	for arg in "${DEV_CMAKE[@]}"; do
		if [[ $arg == -DTAU_SHARED_PREFIX=* ]]; then
			tau_shared_prefix="${arg#-DTAU_SHARED_PREFIX=}"
			break
		fi
	done
	echo "TAU_SHARED_PREFIX: ${tau_shared_prefix}"

	git submodule status | while read -r line; do
		git_submod=$(echo "$line" | awk '{print $2}')
		if [[ $line == -* ]]; then
			echo "Initializing submodule ${git_submod}"
			git submodule update --init --recursive "${git_submod}"
		else
			echo "Submodule ${git_submod} is already initialized"
		fi
	done
}

cd "${REPO_ROOT}"
build_entry "$@"
