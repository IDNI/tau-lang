#!/bin/bash
# The Spot CLI package for linux-arm64.
# scripts/dep/common/spot.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=linux-arm64
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

# Spot has no CMake configure to take the toolchain's target flag, so a cross
# build reaches it through the flags instead.
_dep_spot_target_setup() {
	if dep_target_is_cross linux-arm64; then
		_DEP_SPOT_CROSS_ENV=(CFLAGS="$(dep_var TAU_DEP_CFLAGS "")"
			CXXFLAGS="$(dep_var TAU_DEP_CXXFLAGS "")")
	fi
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/spot.sh"
