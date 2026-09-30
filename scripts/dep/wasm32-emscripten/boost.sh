#!/bin/bash
# The Boost package for wasm32-emscripten.
# scripts/dep/common/boost.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=wasm32-emscripten
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

_dep_boost_user_config() {
	local emscripten_dir="${TAU_SHARED_PREFIX}/emsdk/upstream/emscripten"
	cat > "${1}/user-config.jam" <<EOF
using emscripten : : ${emscripten_dir}/em++ ;
EOF
}

_dep_boost_target_setup() {
	# em++ rejects the -m64 that address-model=64 becomes, and needs no PIC.
	DEP_BOOST_TARGET_OS=""
	DEP_BOOST_B2_TOOLSET="emscripten"
	DEP_BOOST_B2_LINK="static"
	DEP_BOOST_B2_ADDRESS_MODEL="32"
	DEP_BOOST_B2_PIC=""
	case " $DEP_BOOST_CXXFLAGS " in
		*" -pthread "*) DEP_BOOST_B2_THREADING="multi" ;;
		*) DEP_BOOST_B2_THREADING="single" ;;
	esac
	DEP_BOOST_B2_DEFINE="BOOST_LOG_WITHOUT_SYSLOG"
}

source "$(dirname "${BASH_SOURCE[0]}")/../common/boost.sh"
