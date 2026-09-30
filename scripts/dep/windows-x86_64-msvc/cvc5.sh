#!/bin/bash
# The cvc5 package for windows-x86_64-msvc.
# scripts/dep/common/cvc5.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=windows-x86_64-msvc
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

# No system GMP serves this target. -DCVC5_CMAKE_PREFIX can point at an
# MSVC-compatible GMP when --auto-download cannot supply one.
_dep_cvc5_gmp_header() {
	return 0
}

# A real python for cvc5's FindPython: Git Bash PATH can carry the
# WindowsApps store stub, which imports nothing useful.
_dep_cvc5_python() {
	local c d
	for d in python3 python; do
		c="$(command -v "$d" 2>/dev/null || true)"
		[ -n "$c" ] || continue
		case "$c" in *WindowsApps*) continue ;; esac
		if "$c" -c "import sys" > /dev/null 2>&1; then
			if command -v cygpath > /dev/null 2>&1; then
				cygpath -m "$c"
			else
				printf '%s' "$c"
			fi
			return 0
		fi
	done
	return 1
}

# Native MSVC: Ninja + the production build type. cl.exe has no flag-encoded
# path map.
_dep_cvc5_target_setup() {
	local _py
	DEP_CVC5_GENERATOR="Ninja"
	DEP_CVC5_PREFIX_MAP=OFF
	# cl reaches CMake only as CC/CXX: configure.sh splits a -D value at
	# spaces, and cl sits under "Program Files".
	_DEP_CVC5_TARGET_ARGS=(--ninja production)
	_py="$(_dep_cvc5_python || true)"
	if [ -n "$_py" ]; then
		_DEP_CVC5_TARGET_ARGS+=("-DPython_EXECUTABLE=$_py"
			"-DPython3_EXECUTABLE=$_py")
	fi
	if [ -n "$CVC5_CMAKE_PREFIX" ]; then
		_DEP_CVC5_TARGET_ARGS+=("-DCMAKE_PREFIX_PATH=$CVC5_CMAKE_PREFIX")
	fi
	_DEP_CVC5_COMPILER_ENV=(CC="$DEP_CVC5_CC" CXX="$DEP_CVC5_CXX")
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/cvc5.sh"
