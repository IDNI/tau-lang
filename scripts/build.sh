#!/bin/bash

VERBOSE=""
BUILD_TYPE="${1:-Release}"

if [ "$2" = "-v" ]; then
	VERBOSE="1"
	shift
fi

case "${BUILD_TYPE}" in
	"Debug")
		SUFFIX="Debug"
		;;
	"Release")
		SUFFIX="Release"
		;;
	"RelWithDebInfo")
		SUFFIX="RelWithDebInfo"
		;;
	"Coverage")
		SUFFIX="Coverage"
		;;
	*)
		echo "Unknown build type: ${BUILD_TYPE}"
		exit 1
		;;
esac

BUILD_DIR="build-${SUFFIX}"

echo "Building in ${BUILD_DIR} directory"

# Extract TAU_SHARED_PREFIX from CLI arguments
TAU_SHARED_PREFIX="${HOME}/.tau"
for arg in "${@:2}"; do
	if [[ $arg == -DTAU_SHARED_PREFIX=* ]]; then
		TAU_SHARED_PREFIX="${arg#-DTAU_SHARED_PREFIX=}"
		break
	fi
done

echo "TAU_SHARED_PREFIX: ${TAU_SHARED_PREFIX}"

# Set number of build jobs (0 to use half of available logical CPU cores)
TAU_BUILD_JOBS=0
for arg in "${@:2}"; do
	if [[ $arg == -DTAU_BUILD_JOBS=* ]]; then
		TAU_BUILD_JOBS="${arg#-DTAU_BUILD_JOBS=}"
		break
	fi
done
if [ $TAU_BUILD_JOBS -eq 0 ]; then
	TAU_BUILD_JOBS=$(cmake -P cmake/processor-counter.cmake 2>&1 || echo "1")
	TAU_BUILD_JOBS=$((TAU_BUILD_JOBS / 2))
	echo "TAU_BUILD_JOBS: ${TAU_BUILD_JOBS} (half of available cores)"
fi

git submodule status | while read -r LINE; do
	GIT_SUBMOD=$(echo $LINE | awk '{print $2}')
	if [[ $LINE == -* ]]; then
		echo "Initializing submodule $GIT_SUBMOD"
		git submodule update --init --recursive $GIT_SUBMOD
	else
		echo "Submodule ${GIT_SUBMOD} is already initialized"
	fi
done

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"
rm -f ./CMakeCache.txt

NINJA_BIN="$(which ninja 2>&1)";
if [ $? -ne 0 ]; then
	NINJA_BIN="$(which ninja-build 2>&1)";
	if [ $? -ne 0 ]; then
		NINJA_BIN="";
	fi
fi

if [ -z $NINJA_BIN ]; then
	echo "Configuring with make build system"
	cmake .. -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" ${@:2}
	echo "Building with make"
	echo "Parallel jobs: ${TAU_BUILD_JOBS}"
	cmake --build . -- -j ${TAU_BUILD_JOBS} ${VERBOSE:+VERBOSE=1}
	STATUS=$?
else
	echo "Configuring with Ninja build system"
	cmake .. -G Ninja -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" ${@:2}
	echo "Building with Ninja"
	echo "Parallel jobs: ${TAU_BUILD_JOBS}"
	ninja -j ${TAU_BUILD_JOBS} ${VERBOSE:+-v}
	STATUS=$?
fi

cd ..

exit $STATUS
