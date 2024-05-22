#!/bin/bash

cd build-Debug
ctest -j 1 --output-on-failure $@
