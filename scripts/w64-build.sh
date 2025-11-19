#!/bin/bash

BUILD_TYPE="${1:-Release}"

if [ "$1" == "$BUILD_TYPE" ]; then
	shift
fi

./dev build "${BUILD_TYPE}" -DCMAKE_TOOLCHAIN_FILE=../cmake/mingw-w64-x86_64.cmake $@
