#!/bin/bash
# The Boost package for linux-arm64.
# scripts/dep/common/boost.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=linux-arm64
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

# A native arm64 host lets b2 read the arch itself; a cross build from
# x86 must name it or b2 adds the host's x86 flags.
_dep_boost_target_setup() {
	if dep_target_is_cross linux-arm64; then
		DEP_BOOST_B2_ARCH="arm"
	fi
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/boost.sh"
