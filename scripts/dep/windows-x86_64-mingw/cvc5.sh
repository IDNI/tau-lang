#!/bin/bash
# The cvc5 package for windows-x86_64-mingw.
# scripts/dep/common/cvc5.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=windows-x86_64-mingw
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

# No system GMP serves this target, so cvc5 downloads its own.
_dep_cvc5_gmp_header() {
	return 0
}

# cvc5's own mingw64 toolchain picks the cross compilers; --win64 enables it.
_dep_cvc5_target_setup() {
	_DEP_CVC5_TARGET_ARGS=(--win64)
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/cvc5.sh"
