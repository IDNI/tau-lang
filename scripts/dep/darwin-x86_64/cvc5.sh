#!/bin/bash
# The cvc5 package for darwin-x86_64.
# scripts/dep/common/cvc5.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=darwin-x86_64
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

# macOS has no system GMP. The package builds the pinned GMP as a shared
# library and ships it, so the Homebrew copy never enters the package.
_dep_cvc5_gmp_header() {
	return 0
}

# macOS has no $ORIGIN and no absolute install_name: a relocatable
# package resolves through @rpath, matching Tau's own install rpath.
_dep_cvc5_target_setup() {
	# GMP's libtool 2.4.6 reads an unset target as macOS 10.0 and links GMP
	# with -flat_namespace -undefined suppress. An 11.0 target links it normally.
	export MACOSX_DEPLOYMENT_TARGET=11.0
	DEP_CVC5_INSTALL_RPATH='@loader_path:@loader_path/../lib'
	DEP_CVC5_BUILD_RPATH='@loader_path'
	# BUILD_GMP keeps FindGMP from the Homebrew GMP on the default search path.
	_DEP_CVC5_TARGET_ARGS=(-DCMAKE_INSTALL_NAME_DIR=@rpath -DBUILD_GMP=ON)
	_DEP_CVC5_COMPILER_ENV=(CC="$DEP_CVC5_CC" CXX="$DEP_CVC5_CXX")
}

# GMP installs with its build path as install name, and cvc5 records that path.
# @rpath makes the package relocatable. The edit breaks the ad-hoc signature,
# which arm64 requires, so each changed library is signed again.
_dep_cvc5_target_postinstall() {
	local prefix="$1" work="$2" lib dep changed
	for lib in "$prefix"/lib/*.dylib; do
		[ -f "$lib" ] && [ ! -L "$lib" ] || continue
		changed=0
		case "$(otool -D "$lib" | sed -n 2p)" in
			"$work"/*)
				install_name_tool -id "@rpath/$(basename "$lib")" "$lib" || return 1
				changed=1
				;;
		esac
		while IFS= read -r dep; do
			install_name_tool -change "$dep" "@rpath/$(basename "$dep")" "$lib" \
				|| return 1
			changed=1
		done < <(otool -L "$lib" | awk 'NR > 1 { print $1 }' | grep -F "${work}/")
		case "$(basename "$lib")" in
			libgmp*)
				if ! otool -l "$lib" | grep -A2 LC_RPATH | grep -qF '@loader_path'; then
					install_name_tool -add_rpath @loader_path "$lib" || return 1
					changed=1
				fi
				;;
		esac
		if [ "$changed" = 1 ]; then
			codesign --force --sign - "$lib" || return 1
		fi
	done
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/cvc5.sh"
