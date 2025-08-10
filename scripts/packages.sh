#!/bin/bash

./dev release $@
cd ./build-Release
cpack -C Release
cd ..
