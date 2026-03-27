#!/bin/bash

./dev release -DTAU_BUILD_TESTS=ON $@ && \
	cd tests && \
	ctest -j 8 --test-dir ../build-Release --output-on-failure
