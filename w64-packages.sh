#!/bin/bash

./build.sh Release -DTAU_WINDOWS_ZIP_PACKAGE=ON -DCMAKE_TOOLCHAIN_FILE=../cmake/mingw-w64-x86_64.cmake $@
cd ./build-Release
cpack -C Release
cd ..

./build.sh Release -DTAU_WINDOWS_PACKAGE=ON -DCMAKE_TOOLCHAIN_FILE=../cmake/mingw-w64-x86_64.cmake $@
cd ./build-Release
cpack -C Release
cd ..
