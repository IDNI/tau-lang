#!/bin/bash

source "$(dirname "${BASH_SOURCE[0]}")/env"

dev_reject_build_type test-relwithdebinfo RelWithDebInfo "$@"
cd "${REPO_ROOT}"
test_entry RelWithDebInfo -DTAU_BUILD_TESTS=ON "$@"
