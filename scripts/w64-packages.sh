#!/bin/bash

set -euo pipefail

source "$(dirname "${BASH_SOURCE[0]}")/env"

dev_reject_build_type w64-packages Release "$@"
for PACKAGE_FLAG in TAU_WINDOWS_ZIP_PACKAGE TAU_WINDOWS_PACKAGE; do
	./dev w64-build Release "-D${PACKAGE_FLAG}=ON" "$@"
	run_cpack ./build-Release
done
