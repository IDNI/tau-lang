#!/bin/bash
# The Boost package for windows-x86_64-mingw.
# scripts/dep/common/boost.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=windows-x86_64-mingw
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

_dep_boost_user_config() {
	cat > "${1}/user-config.jam" <<EOF
using gcc : mingw64 : ${2}
        :
        <rc>x86_64-w64-mingw32-windres
        <archiver>x86_64-w64-mingw32-ar
;
EOF
}

_dep_boost_target_setup() {
	DEP_BOOST_TARGET_OS="windows"
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/boost.sh"
