#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "$0")/cvc-environment.sh"
cd "$D"
M=$(command -v cmake)
"$M" -S src/cvc5 -B build -G 'Unix Makefiles' \
 -DCMAKE_TOOLCHAIN_FILE="$SDK/emscripten/cmake/Modules/Platform/Emscripten.cmake" \
 -DCMAKE_BUILD_TYPE=Production -DCMAKE_CXX_FLAGS="$EH_FLAGS $CPPFLAGS" -DCMAKE_C_FLAGS="$CPPFLAGS" \
 -DRT_LIBRARIES:STRING= \
 -DCMAKE_EXE_LINKER_FLAGS="$EH_FLAGS" \
 -DUSE_DEFAULT_LINKER=ON -DWASM=OFF -DBUILD_SHARED_LIBS=OFF -DENABLE_IPO=OFF -DENABLE_STATISTICS=OFF -DUSE_EDITLINE=OFF -DUSE_POLY=OFF -DUSE_PYTHON_VENV=OFF -DENABLE_AUTO_DOWNLOAD=OFF \
 "-DGMP_INCLUDE_DIR=$D/prefix/include" "-DGMPXX_INCLUDE_DIR=$D/prefix/include" \
 "-DGMP_LIBRARIES=$D/prefix/lib/libgmp.a" "-DGMPXX_LIBRARIES=$D/prefix/lib/libgmpxx.a" \
 "-DCaDiCaL_INCLUDE_DIR=$D/prefix/include" "-DCaDiCaL_LIBRARIES=$D/prefix/lib/libcadical.a" "-DSymFPU_INCLUDE_DIR=$D/prefix/include"
"${M}" --build build --target cvc5 --parallel "${1:-2}"
