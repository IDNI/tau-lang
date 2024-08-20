#!/bin/bash

cd build-RelWithDebInfo
ctest -j 8 --output-on-failure $@
