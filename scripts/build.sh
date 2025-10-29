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

# Extract TAU_SHARED_PREFIX from CLI arguments
TAU_SHARED_PREFIX="${HOME}/.tau"
for arg in "${@:2}"; do
	if [[ $arg == -DTAU_SHARED_PREFIX=* ]]; then
		TAU_SHARED_PREFIX="${arg#-DTAU_SHARED_PREFIX=}"
		break
	fi
done

git submodule status | while read -r LINE; do
	GIT_SUBMOD=$(echo $LINE | awk '{print $2}')
	if [[ $LINE == -* ]]; then
		# Skip external/cvc5 if libcvc5.so already exists
		if [[ $GIT_SUBMOD == "external/cvc5" && -f ${TAU_SHARED_PREFIX}/lib/libcvc5.so ]]; then
			echo "Skipping submodule $GIT_SUBMOD - libcvc5.so already exists at ${TAU_SHARED_PREFIX}/lib/libcvc5.so"
			continue
		fi
		echo "Initializing submodule $GIT_SUBMOD"
		git submodule update --init --recursive $GIT_SUBMOD
	else
		echo "Submodule ${GIT_SUBMOD} is already initialized"
	fi
done

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"
rm -f ./CMakeCache.txt

PROCESSOR_COUNT=$(cmake -P ../cmake/processor-counter.cmake 2>&1 || echo "1")

NINJA_BIN="$(which ninja 2>&1)";
if [ $? -ne 0 ]; then
	NINJA_BIN="$(which ninja-build 2>&1)";
	if [ $? -ne 0 ]; then
		NINJA_BIN="";
	fi
fi

if [ -z $NINJA_BIN ]; then
	echo "Using make build system"
	cmake .. -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" ${@:2}
	cmake --build . -- -j ${PROCESSOR_COUNT} ${VERBOSE:+VERBOSE=1}
	STATUS=$?
else
	echo "Using Ninja build system"
	cmake .. -G Ninja -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" ${@:2}
	ninja -j ${PROCESSOR_COUNT} ${VERBOSE:+-v}
	STATUS=$?
fi

cd ..

exit $STATUS
