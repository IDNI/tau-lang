#!/bin/bash
# The tau nanobind wheel package for windows-x86_64-msvc.
# scripts/dep/common/tau-wheel.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=windows-x86_64-msvc
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

source "$(dirname "${BASH_SOURCE[0]}")/../common/tau-wheel.sh"
