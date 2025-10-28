#!/bin/bash

./dev build Debug \
        -DTAU_BUILD_TESTS=ON \
        -DTAU_ADDRESS_SANITIZER=ON \
        $@

./dev test-debug
