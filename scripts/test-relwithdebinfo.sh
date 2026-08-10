#!/bin/bash

source "$(dirname "${BASH_SOURCE[0]}")/env"

cd "${REPO_ROOT}"
test_entry RelWithDebInfo -DTAU_BUILD_TESTS=ON "$@"
