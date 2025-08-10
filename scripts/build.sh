#!/bin/bash

VERBOSE=""
BUILD_TYPE="${1:-Release}"

if [ "$2" = "-v" ]; then
	VERBOSE="1"
	shift
fi
VERBOSE="1"

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
	echo "Using make build system"
	cmake .. -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" ${@:2}
	cmake --build . -- -j5 ${VERBOSE:+VERBOSE=1}
	STATUS=$?
else
	echo "Using Ninja build system"
	cmake .. -G Ninja -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" ${@:2}
	ninja ${VERBOSE:+-v}
	STATUS=$?
fi

cd ..

exit $STATUS
