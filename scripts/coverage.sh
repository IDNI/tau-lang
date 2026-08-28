#!/bin/bash

set -euo pipefail

source "$(dirname "${BASH_SOURCE[0]}")/env"

dev_reject_build_type coverage Coverage "$@"
./dev build Coverage "$@"
cd build-Coverage
make coverage
