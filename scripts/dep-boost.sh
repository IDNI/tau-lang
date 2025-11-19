#!/bin/bash

BUILD_JOBS=4
BOOST_REPO=https://github.com/boostorg/boost
BOOST_TAG=65c1319   # Boost 1.86
TAU_SHARED_PREFIX="${HOME}/.tau"
WITH_MINGW64="no"
BUILD_IF_NOT_EXISTS="libboost_log.a"

BOOST_SOURCE_DIR="${TAU_SHARED_PREFIX}/boost"
BOOST_BUILD_DIR=$BOOST_SOURCE_DIR/build-w64
BOOST_PREFIX="${TAU_SHARED_PREFIX}/boost/dist-w64"

# Overwrite default values if provided in CLI arguments
for arg in "${@:1}"; do
	if [[ $arg == --w64 ]]; then
		WITH_MINGW64="yes"
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
	if [[ $arg == -DBOOST_TAG=* ]]; then
		BOOST_TAG="${arg#-DBOOST_TAG=}"
		continue
	fi
done

if [ "$WITH_MINGW64" = "no" ]; then
        echo "Using system boost-dev packages"
        exit 0
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

# create user-config.jam
cat > "$BOOST_SOURCE_DIR/user-config.jam" << EOF
using gcc : mingw64 : x86_64-w64-mingw32-g++
        :
        <rc>x86_64-w64-mingw32-windres
        <archiver>x86_64-w64-mingw32-ar
;
EOF
mkdir -p $BOOST_PREFIX

# build boost
./bootstrap.sh --with-libraries=log && \
        ./b2 --user-config=./user-config.jam --prefix=$BOOST_PREFIX \
                target-os=windows address-model=64 variant=release install
