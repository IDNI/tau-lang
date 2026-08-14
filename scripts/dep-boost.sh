#!/bin/bash

source "$(dirname "${BASH_SOURCE[0]}")/env"

BOOST_REPO=https://github.com/boostorg/boost

dep_entry "$@"

BUILD_JOBS="$(dep_jobs)"
TAU_SHARED_PREFIX="$(dep_shared_prefix)"
BOOST_TAG="$(dep_var BOOST_TAG 65c1319)"   # Boost 1.86
TAU_BUILD_PIC="$(dep_var TAU_BUILD_PIC OFF)"
# ON builds the REPL's threaded dist (dist-wasm-pthread) instead of the
# library's threading=single one (dist-wasm) -- emcc requires every linked
# object to agree on -pthread, so tau_repl cannot link against the plain one.
TAU_BOOST_PTHREAD="$(dep_var TAU_BOOST_PTHREAD OFF)"

BUILD_IF_NOT_EXISTS="libboost_log.a"
[[ $DEP_TARGET == darwin ]] && BUILD_IF_NOT_EXISTS="libboost_log.dylib"

BOOST_SOURCE_DIR="${TAU_SHARED_PREFIX}/boost"
if [[ $DEP_TARGET == w64 ]]; then
	BOOST_BUILD_DIR="${BOOST_SOURCE_DIR}/build-w64"
	BOOST_PREFIX="${TAU_SHARED_PREFIX}/boost/dist-w64"
elif [[ $DEP_TARGET == emscripten && $TAU_BOOST_PTHREAD == ON ]]; then
	BOOST_BUILD_DIR="${BOOST_SOURCE_DIR}/build-wasm-pthread"
	BOOST_PREFIX="${TAU_SHARED_PREFIX}/boost/dist-wasm-pthread"
elif [[ $DEP_TARGET == emscripten ]]; then
	BOOST_BUILD_DIR="${BOOST_SOURCE_DIR}/build-wasm"
	BOOST_PREFIX="${TAU_SHARED_PREFIX}/boost/dist-wasm"
else
	BOOST_BUILD_DIR="${BOOST_SOURCE_DIR}/build"
	BOOST_PREFIX="${TAU_SHARED_PREFIX}/boost/dist"
fi

dep_git_source "$BOOST_REPO" "$BOOST_SOURCE_DIR" "$BOOST_TAG"

# The wasm dist bakes in a wasm exception-handling encoding
# (WASM_LEGACY_EXCEPTIONS, cmake/tau-common.cmake), and the two encodings are
# link-incompatible -- a dist built under the other one fails wasm-ld with
# undefined __cpp_exception / __wasm_lpad_context / __gxx_wasm_personality_v0,
# the same 21 errors B6 first hit (.local/build-emscripten.md). The checks
# below only look for the .a file, so a dist left over from a previous
# encoding would otherwise be reused silently. A stamp file records which
# encoding produced the current dist; wiping the prefix on a mismatch forces
# the checks below to rebuild it instead of trusting a stale .a.
#
# The prefix alone is not enough: b2's own dependency tracking is mtime-based
# (it does not hash toolset flags), so re-running b2 against an untouched
# BOOST_BUILD_DIR relinks the *same* .o files -- still carrying whichever
# encoding built them -- into a fresh-looking .a. Wipe the build dir too, or
# the stamp mismatch triggers a rebuild in name only.
EH_ABI="wasm-standard-exceptions"
if [[ $DEP_TARGET == emscripten ]]; then
	EH_ABI_STAMP="${BOOST_PREFIX}/.tau-eh-abi"
	if [[ "$(cat "$EH_ABI_STAMP" 2>/dev/null)" != "$EH_ABI" ]]; then
		echo "${BOOST_PREFIX}: missing or stale EH-ABI stamp, forcing a rebuild"
		rm -rf "$BOOST_PREFIX" "$BOOST_BUILD_DIR"
	fi
fi

dep_done_if_exists "${BOOST_PREFIX}/lib/${BUILD_IF_NOT_EXISTS}" "boost building"
dep_done_if_exists "${BOOST_PREFIX}/lib64/${BUILD_IF_NOT_EXISTS}" "boost building"

# initialize needed dependencies
git submodule update --init tools/boostdep
git submodule update --init libs/log
python3 tools/boostdep/depinst/depinst.py log

mkdir -p "$BOOST_PREFIX"

B2_ARGS=()
B2_ARGS+=("variant=release")
if [[ $DEP_TARGET == emscripten ]]; then
	B2_ARGS+=("address-model=32")
else
	B2_ARGS+=("address-model=64")
# if [ "$TAU_BUILD_PIC" = "ON" ]; then
	B2_ARGS+=("cxxflags=-fPIC")
# fi
fi

USER_CONFIG_ARG=""
if [[ $DEP_TARGET == w64 ]]; then
	# create user-config.jam for Windows/mingw-w64 build
	cat > "$BOOST_SOURCE_DIR/user-config.jam" << EOF
using gcc : mingw64 : x86_64-w64-mingw32-g++
        :
        <rc>x86_64-w64-mingw32-windres
        <archiver>x86_64-w64-mingw32-ar
;
EOF
	USER_CONFIG_ARG="--user-config=./user-config.jam"
	B2_ARGS+=("target-os=windows")
elif [[ $DEP_TARGET == emscripten ]]; then
	EMSCRIPTEN_DIR="${TAU_SHARED_PREFIX}/emsdk/upstream/emscripten"
	cat > "$BOOST_SOURCE_DIR/user-config.jam" << EOF
using emscripten : : ${EMSCRIPTEN_DIR}/em++ ;
EOF
	USER_CONFIG_ARG="--user-config=./user-config.jam"
	B2_ARGS+=("toolset=emscripten")
	B2_ARGS+=("link=static")
	if [[ $TAU_BOOST_PTHREAD == ON ]]; then
		# threading=multi selects Boost's mt code paths; emscripten.jam's
		# target-os is "none" (toolset.add-defaults <toolset>emscripten:
		# <target-os>none), which gcc.jam's threading-flags table does not
		# cover, so -pthread is passed explicitly rather than relied on to
		# be added automatically -- it must match tau_repl's own -pthread.
		B2_ARGS+=("threading=multi")
		B2_ARGS+=("cxxflags=-pthread")
		B2_ARGS+=("linkflags=-pthread")
	else
		# pthreads would drag SharedArrayBuffer and COOP/COEP into the library
		B2_ARGS+=("threading=single")
	fi
	B2_ARGS+=("define=BOOST_LOG_WITHOUT_SYSLOG")
	# Must agree with tau's own compile+link flags (cmake/tau-common.cmake) --
	# the standardized wasm exception-handling encoding, not emsdk's
	# legacy-by-default one (B6/D2).
	B2_ARGS+=("cxxflags=-sWASM_LEGACY_EXCEPTIONS=0")
	B2_ARGS+=("linkflags=-sWASM_LEGACY_EXCEPTIONS=0")
else
	# remove user-config.jam if it exists from previous Windows/wasm build
	rm -f "$BOOST_SOURCE_DIR/user-config.jam"
	if [ "$(uname)" = "Darwin" ]; then
		B2_ARGS+=("target-os=darwin")
	else
		B2_ARGS+=("target-os=linux")
	fi
fi


# build boost
./bootstrap.sh --with-libraries=log && \
./b2 ${USER_CONFIG_ARG} --prefix=$BOOST_PREFIX --build-dir="$BOOST_BUILD_DIR" "${B2_ARGS[@]}" install && \
{ [[ $DEP_TARGET != emscripten ]] || echo "$EH_ABI" > "$EH_ABI_STAMP"; }
