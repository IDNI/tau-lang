#!/bin/bash

cd tests
ctest -j 8 --test-dir ../build-RelWithDebInfo --output-on-failure $@
