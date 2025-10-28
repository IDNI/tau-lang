#!/bin/bash

cd tests
ctest -j 8 --test-dir ../build-Release --output-on-failure $@
