#!/bin/bash
# The Boost package for linux-x86_64.
# scripts/dep/common/boost.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=linux-x86_64
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

source "$(dirname "${BASH_SOURCE[0]}")/../common/boost.sh"
