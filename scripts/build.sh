#!/bin/bash

printf 'argc=%d\n' "$#"
for a in "$@"; do printf '[%q]\n' "$a"; done

VERBOSE=""
BUILD_TYPE="${1:-Release}"
shift

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

TARGET=""
GENERATOR="Ninja" # default generator
CMAKE_ARGS=()
while [ $# -gt 0 ]; do
	case "$1" in
		-v)
			VERBOSE="1"
			shift
			;;
		--target)
			if [ $# -lt 2 ]; then
				echo "Missing value for --target"
				exit 1
			fi
			TARGET="$2"
			shift 2
			;;
		-G)
			if [ $# -lt 2 ]; then
				echo "Missing value for -G"
				exit 1
			fi
			GENERATOR="$2"
			shift 2
			;;
		*)
			CMAKE_ARGS+=("$1")
			shift
			;;
	esac
done

# Extract TAU_SHARED_PREFIX from CLI arguments
TAU_SHARED_PREFIX="${HOME}/.tau"
for arg in "${CMAKE_ARGS[@]}"; do
	if [[ $arg == -DTAU_SHARED_PREFIX=* ]]; then
		TAU_SHARED_PREFIX="${arg#-DTAU_SHARED_PREFIX=}"
		break
	fi
done

echo "TAU_SHARED_PREFIX: ${TAU_SHARED_PREFIX}"

# Set number of build jobs (0 to use half of available logical CPU cores)
TAU_BUILD_JOBS=0
for arg in "${CMAKE_ARGS[@]}"; do
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

if [[ $GENERATOR == Ninja ]]; then # Check if ninja is installed
	NINJA_BIN="$(which ninja 2>&1)";
	if [ $? -ne 0 ]; then
		NINJA_BIN="$(which ninja-build 2>&1)";
		if [ $? -ne 0 ]; then
			echo "Ninja is not available. Falling back to make."
			GENERATOR="Unix Makefiles"
		fi
	fi
fi

VERBOSE_FLAG=""
if [[ $VERBOSE == 1 ]]; then
	if [[ $GENERATOR == Ninja ]]; then
		VERBOSE_FLAG="-v"
	else
		VERBOSE_FLAG="VERBOSE=1"
	fi
fi

echo "Configuring with $GENERATOR build system"
echo "Config command: cmake .. -G \"$GENERATOR\" -DCMAKE_BUILD_TYPE=\"${BUILD_TYPE}\" \"${CMAKE_ARGS[@]}\""
cmake .. -G "$GENERATOR" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" "${CMAKE_ARGS[@]}"
echo "Building with $GENERATOR"
echo "Parallel jobs: ${TAU_BUILD_JOBS}"
echo "Target: ${TARGET}"
BUILD_TARGET=()
if [ -n "$TARGET" ]; then
	BUILD_TARGET=(--target "$TARGET")
fi
echo "Build command: cmake --build . ${BUILD_TARGET} -- -j ${TAU_BUILD_JOBS} ${VERBOSE_FLAG}"
cmake --build . "${BUILD_TARGET[@]}" -- -j ${TAU_BUILD_JOBS} ${VERBOSE_FLAG}
STATUS=$?

cd ..

exit $STATUS
