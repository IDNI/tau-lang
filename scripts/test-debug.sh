#!/bin/bash

./dev debug -DTAU_BUILD_TESTS=ON $@ && \
	cd tests && \
	ctest -j 8 --test-dir ../build-Debug --output-on-failure
