#!/bin/bash

./dev debug -DTAU_BUILD_EXECUTABLE=ON && gdb --args ./build-Debug/tau $@
