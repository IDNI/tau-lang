#!/bin/bash

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
EXTERNAL_DIR=$SCRIPT_DIR/../external
BOOST_ROOT=$EXTERNAL_DIR/boost-source
PREFIX_DIR=$EXTERNAL_DIR/libboost-mingw-w64
BOOST_REPO=https://github.com/boostorg/boost
BOOST_COMMIT=65c1319   # Boost 1.86

# clone and checkout boost
git clone $BOOST_REPO --branch master --single-branch $BOOST_ROOT
cd $BOOST_ROOT
git checkout $BOOST_COMMIT

# initialize needed dependencies
git submodule update --init tools/boostdep
git submodule update --init libs/log
python3 tools/boostdep/depinst/depinst.py log

# create user-config.jam
cat > "$BOOST_ROOT/user-config.jam" << EOF
using gcc : mingw64 : x86_64-w64-mingw32-g++
        :
        <rc>x86_64-w64-mingw32-windres
        <archiver>x86_64-w64-mingw32-ar
;
EOF
mkdir -p $PREFIX_DIR

# build boost
./bootstrap.sh --with-libraries=log
./b2 --user-config=./user-config.jam --prefix=$PREFIX_DIR target-os=windows address-model=64 variant=release install
