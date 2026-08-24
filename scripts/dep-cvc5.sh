#!/bin/bash

source "$(dirname "${BASH_SOURCE[0]}")/env"

CVC5_REPO=https://github.com/cvc5/cvc5

dep_entry "$@"

BUILD_JOBS="$(dep_jobs)"
TAU_SHARED_PREFIX="$(dep_shared_prefix)"
CVC5_TAG="$(dep_var CVC5_TAG cvc5-1.3.1)"

CVC5_BUILD_DIR="build"
CVC5_SOURCE_DIR="${TAU_SHARED_PREFIX}/cvc5"
CVC5_PREFIX="${TAU_SHARED_PREFIX}/cvc5/dist"
BUILD_IF_NOT_EXISTS="libcvc5.so"
CVC5_CONFIGURE_ARGS=()

case "$DEP_TARGET" in
	w64)
		CVC5_BUILD_DIR="build-w64"
		CVC5_PREFIX="${TAU_SHARED_PREFIX}/cvc5/dist-w64"
		CVC5_CONFIGURE_ARGS+=(--win64)
		BUILD_IF_NOT_EXISTS="libcvc5.dll"
		;;
	darwin)
		BUILD_IF_NOT_EXISTS="libcvc5.dylib"
		;;
esac
CVC5_CONFIGURE_ARGS+=("--name=${CVC5_BUILD_DIR}")
CVC5_CONFIGURE_ARGS+=("--prefix=${CVC5_PREFIX}")

echo "WITH_MINGW64: $([[ $DEP_TARGET == w64 ]] && echo yes || echo no)"
echo "TAU_SHARED_PREFIX: ${TAU_SHARED_PREFIX}"
echo "CVC5_TAG: ${CVC5_TAG}"
echo "CVC5_BUILD_DIR: ${CVC5_BUILD_DIR}"
echo "CVC5_PREFIX: ${CVC5_PREFIX}"
echo "CVC5_CONFIGURE_ARGS: ${CVC5_CONFIGURE_ARGS[*]}"

dep_git_source "$CVC5_REPO" "$CVC5_SOURCE_DIR" "$CVC5_TAG"

# TODO add check for CVC5_TAG and update or warn if different

dep_done_if_exists "${CVC5_PREFIX}/lib/${BUILD_IF_NOT_EXISTS}" "cvc5 building"
dep_done_if_exists "${CVC5_PREFIX}/lib64/${BUILD_IF_NOT_EXISTS}" "cvc5 building"
dep_done_if_exists "${CVC5_PREFIX}/bin/${BUILD_IF_NOT_EXISTS}" "cvc5 building"

echo "${CVC5_PREFIX}/lib(64)?/${BUILD_IF_NOT_EXISTS} does not exist, building cvc5"
cd "${CVC5_SOURCE_DIR}"
echo "Configuring cvc5" && \
	./configure.sh --no-gpl --auto-download \
		-DSKIP_SET_RPATH=ON \
		-DCMAKE_INSTALL_RPATH=\${ORIGIN} \
		-DCMAKE_BUILD_RPATH=\${ORIGIN} \
		-DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON \
		"${CVC5_CONFIGURE_ARGS[@]}" && \
echo "Building cvc5" && \
	cd ${CVC5_BUILD_DIR} && \
	make -j ${BUILD_JOBS} && \
echo "Installing cvc5" && \
	make install
