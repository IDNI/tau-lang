#!/bin/bash

source "$(dirname "${BASH_SOURCE[0]}")/env"

dev_reject_build_type test-release Release "$@"
cd "${REPO_ROOT}"
test_entry Release -DTAU_BUILD_TESTS=ON "$@"
