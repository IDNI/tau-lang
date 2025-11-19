#!/bin/bash

./dev release -DTAU_DEB_PACKAGE=ON $@
cd ./build-Release
cpack -C Release
cd ..

./dev release -DTAU_RPM_PACKAGE=ON $@
cd ./build-Release
cpack -C Release
cd ..
