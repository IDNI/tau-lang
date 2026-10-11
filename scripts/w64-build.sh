#!/bin/bash

./dev build \
	-DCMAKE_TOOLCHAIN_FILE=external/parser/cmake/mingw-w64-x86_64.cmake \
	"$@"
