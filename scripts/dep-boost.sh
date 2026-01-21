#!/bin/bash

BUILD_JOBS=4
BOOST_REPO=https://github.com/boostorg/boost
BOOST_TAG=65c1319   # Boost 1.86
TAU_SHARED_PREFIX="${HOME}/.tau"
WITH_MINGW64="no"
BUILD_IF_NOT_EXISTS="libboost_log.a"
TAU_BUILD_PIC="OFF"

# Overwrite default values if provided in CLI arguments
for arg in "${@:1}"; do
	if [[ $arg == --w64 ]]; then
		WITH_MINGW64="yes"
		continue
	fi
	if [[ $arg == --darwin ]]; then
		BUILD_IF_NOT_EXISTS="libboost_log.dylib"
		continue
	fi
	if [[ $arg == -DTAU_BUILD_JOBS=* ]]; then
		BUILD_JOBS="${arg#-DTAU_BUILD_JOBS=}"
		continue
	fi
	if [[ $arg == -DTAU_SHARED_PREFIX=* ]]; then
		TAU_SHARED_PREFIX="${arg#-DTAU_SHARED_PREFIX=}"
		continue
	fi
	if [[ $arg == -DTAU_BUILD_PIC=* ]]; then
		TAU_BUILD_PIC="${arg#-DTAU_BUILD_PIC=}"
		continue
	fi
	if [[ $arg == -DBOOST_TAG=* ]]; then
		BOOST_TAG="${arg#-DBOOST_TAG=}"
		continue
	fi
done

BOOST_SOURCE_DIR="${TAU_SHARED_PREFIX}/boost"
if [ "$WITH_MINGW64" = "yes" ]; then
	BOOST_BUILD_DIR=$BOOST_SOURCE_DIR/build-w64
	BOOST_PREFIX="${TAU_SHARED_PREFIX}/boost/dist-w64"
else
	BOOST_BUILD_DIR=$BOOST_SOURCE_DIR/build
	BOOST_PREFIX="${TAU_SHARED_PREFIX}/boost/dist"
fi


if [ ! -d "${BOOST_SOURCE_DIR}" ]; then
	git clone $BOOST_REPO --branch master --single-branch ${BOOST_SOURCE_DIR}
	cd ${BOOST_SOURCE_DIR}
	git checkout ${BOOST_TAG}
else
        cd ${BOOST_SOURCE_DIR}
	echo "boost source directory already exists, skipping clone and checkout"
fi

REBUILD="yes"

if [ -f "${BOOST_PREFIX}/lib/${BUILD_IF_NOT_EXISTS}" ]; then
	echo "${BOOST_PREFIX}/lib/${BUILD_IF_NOT_EXISTS} already exists, skipping boost building"
	REBUILD="no"
fi
if [ -f "${BOOST_PREFIX}/lib64/${BUILD_IF_NOT_EXISTS}" ]; then
	echo "${BOOST_PREFIX}/lib64/${BUILD_IF_NOT_EXISTS} already exists, skipping boost building"
	REBUILD="no"
fi

if [ "$REBUILD" = "no" ]; then
        exit 0
fi

# initialize needed dependencies
git submodule update --init tools/boostdep
git submodule update --init libs/log
python3 tools/boostdep/depinst/depinst.py log

mkdir -p $BOOST_PREFIX

B2_ARGS=()
B2_ARGS+=("address-model=64")
B2_ARGS+=("variant=release")
# if [ "$TAU_BUILD_PIC" = "ON" ]; then
	B2_ARGS+=("cxxflags=-fPIC")
# fi

USER_CONFIG_ARG=""
if [ "$WITH_MINGW64" = "yes" ]; then
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
else
	# remove user-config.jam if it exists from previous Windows build
	rm -f "$BOOST_SOURCE_DIR/user-config.jam"
	if [ "$(uname)" = "Darwin" ]; then
		B2_ARGS+=("target-os=darwin")
	else
		B2_ARGS+=("target-os=linux")
	fi
fi


# build boost
./bootstrap.sh --with-libraries=log && \
./b2 ${USER_CONFIG_ARG} --prefix=$BOOST_PREFIX "${B2_ARGS[@]}" install
