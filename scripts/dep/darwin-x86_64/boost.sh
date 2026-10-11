#!/bin/bash
# The Boost package for darwin-x86_64.
# scripts/dep/common/boost.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=darwin-x86_64
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

_dep_boost_target_setup() {
	# b2 names the macOS variant clang-darwin from the toolset plus this.
	DEP_BOOST_TARGET_OS="darwin"
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/boost.sh"
