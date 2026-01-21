#!/bin/bash

BUILD_JOBS=4
CVC5_REPO=https://github.com/cvc5/cvc5
CVC5_TAG=cvc5-1.3.1
TAU_SHARED_PREFIX="${HOME}/.tau"
WITH_MINGW64="no"
BUILD_IF_NOT_EXISTS="libcvc5.so"

CVC5_SOURCE_DIR="${TAU_SHARED_PREFIX}/cvc5"
CVC5_BUILD_DIR="build"
CVC5_PREFIX="${TAU_SHARED_PREFIX}/cvc5/dist"
CVC5_CONFIGURE_ARGS=()

# Overwrite default values if provided in CLI arguments
for arg in "${@:1}"; do
	if [[ $arg == --w64 ]]; then
		WITH_MINGW64="yes"
		continue
	fi
	if [[ $arg == --darwin ]]; then
		BUILD_IF_NOT_EXISTS="libcvc5.dylib"
	fi
	if [[ $arg == -DTAU_BUILD_JOBS=* ]]; then
		BUILD_JOBS="${arg#-DTAU_BUILD_JOBS=}"
		continue
	fi
	if [[ $arg == -DTAU_SHARED_PREFIX=* ]]; then
		TAU_SHARED_PREFIX="${arg#-DTAU_SHARED_PREFIX=}"
		continue
	fi
	if [[ $arg == -DCVC5_TAG=* ]]; then
		CVC5_TAG="${arg#-DCVC5_TAG=}"
		continue
	fi
done
if [ "$WITH_MINGW64" = "yes" ]; then
	CVC5_BUILD_DIR="build-w64"
	CVC5_PREFIX="${TAU_SHARED_PREFIX}/cvc5/dist-w64"
	CVC5_CONFIGURE_ARGS+=(--win64)
	BUILD_IF_NOT_EXISTS="libcvc5.dll"
fi
CVC5_CONFIGURE_ARGS+=("--name=${CVC5_BUILD_DIR}")
CVC5_CONFIGURE_ARGS+=("--prefix=${CVC5_PREFIX}")

echo "WITH_MINGW64: ${WITH_MINGW64}"
echo "TAU_SHARED_PREFIX: ${TAU_SHARED_PREFIX}"
echo "CVC5_TAG: ${CVC5_TAG}"
echo "CVC5_BUILD_DIR: ${CVC5_BUILD_DIR}"
echo "CVC5_PREFIX: ${CVC5_PREFIX}"
echo "CVC5_CONFIGURE_ARGS: ${CVC5_CONFIGURE_ARGS[*]}"

if [ ! -d "${CVC5_SOURCE_DIR}" ]; then
	git clone $CVC5_REPO ${CVC5_SOURCE_DIR}
	cd ${CVC5_SOURCE_DIR}
	git checkout ${CVC5_TAG}
else
        cd ${CVC5_SOURCE_DIR}
	echo "cvc5 source directory already exists, skipping clone and checkout"
fi

# TODO add check for CVC5_TAG and update or warn if different

REBUILD="yes"
if [ -f "${CVC5_PREFIX}/lib/${BUILD_IF_NOT_EXISTS}" ]; then
	echo "${CVC5_PREFIX}/lib/${BUILD_IF_NOT_EXISTS} already exists, skipping cvc5 building"
	REBUILD="no"
fi
if [ -f "${CVC5_PREFIX}/lib64/${BUILD_IF_NOT_EXISTS}" ]; then
	echo "${CVC5_PREFIX}/lib64/${BUILD_IF_NOT_EXISTS} already exists, skipping cvc5 building"
	REBUILD="no"
fi
if [ -f "${CVC5_PREFIX}/bin/${BUILD_IF_NOT_EXISTS}" ]; then
	echo "${CVC5_PREFIX}/bin/${BUILD_IF_NOT_EXISTS} already exists, skipping cvc5 building"
	REBUILD="no"
fi
if [ "$REBUILD" = "yes" ]; then
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
fi
