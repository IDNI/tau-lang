#!/bin/bash

cd build-Release
ctest -j 8 --output-on-failure $@
