#!/bin/bash

source "$(dirname "${BASH_SOURCE[0]}")/env"

dev_reject_build_type test-with-sanitizer Debug "$@"
cd "${REPO_ROOT}"
test_entry Debug -DTAU_BUILD_TESTS=ON -DTAU_ADDRESS_SANITIZER=ON "$@"
