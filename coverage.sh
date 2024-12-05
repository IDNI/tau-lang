#!/bin/bash

./build.sh Coverage $@
cd build-Coverage
make coverage
