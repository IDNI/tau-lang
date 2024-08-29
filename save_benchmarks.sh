#!/bin/bash

mv *.measures tests/benchmark/data/  > /dev/null 2>&1
mv *.callgrind.out tests/benchmark/data/  > /dev/null 2>&1
mv build*/*.measures tests/benchmark/data/  > /dev/null 2>&1
mv build*/*.callgrind.out tests/benchmark/data/  > /dev/null 2>&1
