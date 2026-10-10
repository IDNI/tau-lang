#!/bin/bash
# The Boost package for windows-arm64-msvc.
# scripts/dep/common/boost-msvc.sh holds the MSVC body and
# scripts/dep/common/boost.sh the shared recipe.

set -u

DEP_FILE_TARGET=windows-arm64-msvc
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

DEP_BOOST_MSVC_VCVARS=vcvarsarm64.bat
DEP_BOOST_MSVC_B2_ARCH=arm

source "$(dirname "${BASH_SOURCE[0]}")/../common/boost-msvc.sh"
source "$(dirname "${BASH_SOURCE[0]}")/../common/boost.sh"
