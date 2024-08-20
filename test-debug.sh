#!/bin/bash

cd build-Debug
ctest -j 8 --output-on-failure $@
