#!/bin/bash

cd build-Release
ctest -j 1 --output-on-failure $@
