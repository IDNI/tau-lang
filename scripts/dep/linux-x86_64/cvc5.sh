#!/bin/bash
# The cvc5 package for linux-x86_64.
# scripts/dep/common/cvc5.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=linux-x86_64
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

# Linux carries GMP in /usr/include, or in its multiarch directory on Debian
# and Ubuntu; cvc5's FindGMP finds it in either. A cross build must not take
# the GMP of another host arch.
_dep_cvc5_gmp_header() {
	if ! dep_target_is_cross linux-x86_64; then
		local h
		for h in /usr/include/gmp.h /usr/include/x86_64-linux-gnu/gmp.h; do
			[ -f "$h" ] && { printf '%s' "$h"; break; }
		done
	fi
	return 0
}

_dep_cvc5_target_setup() {
	_DEP_CVC5_COMPILER_ENV=(CC="$DEP_CVC5_CC" CXX="$DEP_CVC5_CXX")
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/cvc5.sh"
