#!/bin/bash

./release.sh $@
cd ./build-Release
cpack -C Release
cd ..
