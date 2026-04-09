#!/bin/bash

./dev relwithdebinfo -DTAU_BUILD_TESTS=ON $@ && \
	cd tests && \
	ctest -j 8 --test-dir ../build-RelWithDebInfo --output-on-failure
