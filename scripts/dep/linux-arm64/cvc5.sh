#!/bin/bash
# The cvc5 package for linux-arm64.
# scripts/dep/common/cvc5.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=linux-arm64
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

# Linux carries GMP in /usr/include, or in its multiarch directory on Debian
# and Ubuntu; cvc5's FindGMP finds it in either. A cross build must not take
# the GMP of another host arch.
_dep_cvc5_gmp_header() {
	if ! dep_target_is_cross linux-arm64; then
		local h
		for h in /usr/include/gmp.h /usr/include/aarch64-linux-gnu/gmp.h; do
			[ -f "$h" ] && { printf '%s' "$h"; break; }
		done
	fi
	return 0
}

# The build host's own interpreter, for cvc5's build scripts. A cross job can
# put the target venv first on PATH, and that interpreter runs only under qemu.
_dep_cvc5_host_python() {
	local c m
	m="$(uname -m)"
	for c in "$(command -v python3 2>/dev/null || true)" /usr/bin/python3; do
		[ -n "$c" ] && [ -x "$c" ] || continue
		if "$c" -c 'import platform, sys; sys.exit(platform.machine() != sys.argv[1])' \
				"$m" > /dev/null 2>&1; then
			printf '%s' "$c"
			return 0
		fi
	done
	return 1
}

_dep_cvc5_target_setup() {
	local _py
	# A cross build from x86 configures with the aarch64 toolchain file,
	# whose find-root-path modes keep cvc5 off the host's x86 GMP.
	if dep_target_is_cross linux-arm64; then
		if [ -z "$DEP_CVC5_TOOLCHAIN" ]; then
			echo "dep-cvc5: linux-arm64 needs -DTAU_DEP_TOOLCHAIN" >&2
			exit 2
		fi
		_DEP_CVC5_TARGET_ARGS+=(
			-DCMAKE_TOOLCHAIN_FILE="$DEP_CVC5_TOOLCHAIN")
		_py="$(_dep_cvc5_host_python || true)"
		if [ -z "$_py" ]; then
			echo "dep-cvc5: linux-arm64 needs a build-host python3" >&2
			exit 2
		fi
		_DEP_CVC5_TARGET_ARGS+=("-DPython_EXECUTABLE=$_py")
		# cvc5's FindGMP takes the GMP --host from TOOLCHAIN_PREFIX, which
		# only cvc5's own toolchain file sets. An empty --host builds a
		# static x86 GMP that cvc5 cannot link.
		_DEP_CVC5_TARGET_ARGS+=(-DTOOLCHAIN_PREFIX=aarch64-linux-gnu)
		# GMP configures at build time and would pick the gcc cross compiler.
		_DEP_CVC5_BUILD_ENV=(
			CC="$DEP_CVC5_CC --target=aarch64-linux-gnu"
			CXX="$DEP_CVC5_CXX --target=aarch64-linux-gnu")
	fi
	_DEP_CVC5_COMPILER_ENV=(CC="$DEP_CVC5_CC" CXX="$DEP_CVC5_CXX")
	# The toolchain file sets Python_EXECUTABLE to TAU_PYTHON, the arm64
	# interpreter for the binding, and that shadows the -D above.
	if dep_target_is_cross linux-arm64; then
		_DEP_CVC5_COMPILER_ENV+=(TAU_PYTHON=)
	fi
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/cvc5.sh"
