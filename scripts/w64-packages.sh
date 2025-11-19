#!/bin/bash

./dev w64-build Release -DTAU_WINDOWS_ZIP_PACKAGE=ON $@
cd ./build-Release
cpack -C Release
cd ..

./dev w64-build Release -DTAU_WINDOWS_PACKAGE=ON $@
cd ./build-Release
cpack -C Release
cd ..
