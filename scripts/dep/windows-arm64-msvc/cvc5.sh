#!/bin/bash
# The cvc5 package for windows-arm64-msvc.
# scripts/dep/common/cvc5-msvc.sh holds the MSVC body and
# scripts/dep/common/cvc5.sh the shared recipe.

set -u

DEP_FILE_TARGET=windows-arm64-msvc
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

DEP_CVC5_MSVC_LLVM_ARCH=ARM64
DEP_CVC5_MSVC_GMP_TRIPLET=arm64-windows-release

source "$(dirname "${BASH_SOURCE[0]}")/../common/cvc5-msvc.sh"
source "$(dirname "${BASH_SOURCE[0]}")/../common/cvc5.sh"
