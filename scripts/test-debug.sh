#!/bin/bash

source "$(dirname "${BASH_SOURCE[0]}")/env"

cd "${REPO_ROOT}"
test_entry Debug -DTAU_BUILD_TESTS=ON "$@"
