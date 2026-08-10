#!/bin/bash

set -euo pipefail

source "$(dirname "${BASH_SOURCE[0]}")/env"

for PACKAGE_FLAG in TAU_WINDOWS_ZIP_PACKAGE TAU_WINDOWS_PACKAGE; do
	./dev w64-build Release "-D${PACKAGE_FLAG}=ON" "$@"
	run_cpack ./build-Release
done
