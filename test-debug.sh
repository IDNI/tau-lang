#!/bin/bash

cd build-Debug
ctest -j N --output-on-failure $@
