#!/bin/bash

source "$(dirname "${BASH_SOURCE[0]}")/env"

BOOST_REPO=https://github.com/boostorg/boost

dep_entry "$@"

BUILD_JOBS="$(dep_jobs)"
TAU_SHARED_PREFIX="$(dep_shared_prefix)"
BOOST_TAG="$(dep_var BOOST_TAG 65c1319)"   # Boost 1.86
TAU_BUILD_PIC="$(dep_var TAU_BUILD_PIC OFF)"

BUILD_IF_NOT_EXISTS="libboost_log.a"
[[ $DEP_TARGET == darwin ]] && BUILD_IF_NOT_EXISTS="libboost_log.dylib"

BOOST_SOURCE_DIR="${TAU_SHARED_PREFIX}/boost"
if [[ $DEP_TARGET == w64 ]]; then
	BOOST_BUILD_DIR="${BOOST_SOURCE_DIR}/build-w64"
	BOOST_PREFIX="${TAU_SHARED_PREFIX}/boost/dist-w64"
elif [[ $DEP_TARGET == emscripten ]]; then
	BOOST_BUILD_DIR="${BOOST_SOURCE_DIR}/build-wasm"
	BOOST_PREFIX="${TAU_SHARED_PREFIX}/boost/dist-wasm"
else
	BOOST_BUILD_DIR="${BOOST_SOURCE_DIR}/build"
	BOOST_PREFIX="${TAU_SHARED_PREFIX}/boost/dist"
fi

dep_git_source "$BOOST_REPO" "$BOOST_SOURCE_DIR" "$BOOST_TAG"

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
	# pthreads would drag SharedArrayBuffer and COOP/COEP into the library
	B2_ARGS+=("threading=single")
	B2_ARGS+=("define=BOOST_LOG_WITHOUT_SYSLOG")
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
./b2 ${USER_CONFIG_ARG} --prefix=$BOOST_PREFIX --build-dir="$BOOST_BUILD_DIR" "${B2_ARGS[@]}" install
