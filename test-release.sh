#!/bin/bash

cd build-Release
ctest -j N --output-on-failure 
