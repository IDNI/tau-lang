#!/bin/bash
# The cvc5 package for darwin-arm64.
# scripts/dep/common/cvc5.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=darwin-arm64
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

# macOS has no system GMP, so FindGMP picks the Homebrew copy when its prefix
# search reaches it.
_dep_cvc5_gmp_header() {
	if command -v brew > /dev/null 2>&1; then
		local prefix
		prefix="$(brew --prefix gmp 2>/dev/null || true)"
		if [ -n "$prefix" ] && [ -f "${prefix}/include/gmp.h" ]; then
			printf '%s' "${prefix}/include/gmp.h"
		fi
	fi
	return 0
}

# macOS has no $ORIGIN and no absolute install_name: a relocatable
# package resolves through @rpath, matching Tau's own install rpath.
_dep_cvc5_target_setup() {
	DEP_CVC5_INSTALL_RPATH='@loader_path:@loader_path/../lib'
	DEP_CVC5_BUILD_RPATH='@loader_path'
	_DEP_CVC5_TARGET_ARGS=(-DCMAKE_INSTALL_NAME_DIR=@rpath)
	_DEP_CVC5_COMPILER_ENV=(CC="$DEP_CVC5_CC" CXX="$DEP_CVC5_CXX")
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/cvc5.sh"
