#!/bin/bash
# The Spot CLI package for darwin-x86_64.
# scripts/dep/common/spot.sh holds the shared recipe.

set -u

DEP_FILE_TARGET=darwin-x86_64
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

source "$(dirname "${BASH_SOURCE[0]}")/../common/spot.sh"
