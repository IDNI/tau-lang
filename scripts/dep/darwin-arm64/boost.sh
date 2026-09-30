#!/bin/bash
# The Boost package for darwin-arm64.
# scripts/dep/common/boost.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=darwin-arm64
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

_dep_boost_target_setup() {
	# b2 names the macOS variant clang-darwin from the toolset plus this.
	DEP_BOOST_TARGET_OS="darwin"
	# A dylib's install_name would bake the staging prefix and fail the
	# path audit; Tau links Boost statically anyway.
	BOOST_LINK_MODE="static"
	DEP_BOOST_B2_LINK="static"
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/boost.sh"
