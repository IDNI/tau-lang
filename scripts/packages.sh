#!/bin/bash

set -euo pipefail

source "$(dirname "${BASH_SOURCE[0]}")/env"

for PACKAGE_FLAG in TAU_DEB_PACKAGE TAU_RPM_PACKAGE; do
	./dev release "-D${PACKAGE_FLAG}=ON" "$@"
	run_cpack ./build-Release
done
