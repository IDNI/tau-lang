#!/bin/bash

./dev debug && gdb --args ./build-Debug/tau $@
