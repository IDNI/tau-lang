#!/bin/bash

cd tests
ctest -j 8 --test-dir ../build-Debug --output-on-failure $@
