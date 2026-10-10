#!/bin/bash
# The cvc5 package for windows-x86_64-msvc.
# scripts/dep/common/cvc5-msvc.sh holds the MSVC body and
# scripts/dep/common/cvc5.sh the shared recipe.

set -u

DEP_FILE_TARGET=windows-x86_64-msvc
DEP_RECIPE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"

DEP_CVC5_MSVC_LLVM_ARCH=x64
DEP_CVC5_MSVC_GMP_TRIPLET=x64-windows-release

source "$(dirname "${BASH_SOURCE[0]}")/../common/cvc5-msvc.sh"
source "$(dirname "${BASH_SOURCE[0]}")/../common/cvc5.sh"
