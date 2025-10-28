#!/bin/bash

./dev build Coverage $@
cd build-Coverage
make coverage
