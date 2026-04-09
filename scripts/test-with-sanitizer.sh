#!/bin/bash

./dev build Debug \
		-DTAU_BUILD_TESTS=ON \
		-DTAU_ADDRESS_SANITIZER=ON \
		$@ && \
	cd tests && \
	ctest -j 8 --test-dir ../build-Debug --output-on-failure
