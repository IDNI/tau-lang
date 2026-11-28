#!/usr/bin/env bash
# Produce the uninstrumented source-built reactor. Provision it separately.
set -euo pipefail
if [ "$#" != 5 ]; then
    echo "Usage: bash $0 TAU_SOURCE NEW_BUILD_DIR SOLVER_ROOT BOOST_INCLUDE TAU_DEPENDENCY_STORE" >&2
    exit 2
fi
: "${SDK:?missing Emscripten SDK root}" "${EM_CONFIG:?missing Emscripten configuration}" "${EM_CACHE:?missing Emscripten cache}"
source_root=$(realpath "$1") build=$(realpath -m "$2") solver=$(realpath "$3")
boost=$(realpath "$4") deps=$(realpath -m "$5")
test -f "$solver/build/src/libcvc5.a"
test -f "$boost/boost/version.hpp"
test ! -e "$build"
export TZ=UTC LC_ALL=C PYTHONHASHSEED=0 SOURCE_DATE_EPOCH=1791450180
# These parser gitlink revisions have identical source trees; keep the
# qualifier used by the reference producer for its dependency identity.
export TAU_PARSER_COMMIT=0bb513fbe3fba41aaa898b5a4b7acc7e051723e8
export TAU_Q_NODE
TAU_Q_NODE=$(command -v node)
cmake -S "$source_root" -B "$build" -G Ninja -DCMAKE_BUILD_TYPE=Release \
 -DCMAKE_CXX_FLAGS_RELEASE="-O1 -DNDEBUG -ffile-prefix-map=$source_root=tau-src -ffile-prefix-map=$build=tau-build" \
 -DCMAKE_C_FLAGS_RELEASE="-O1 -DNDEBUG" \
 -DTAU_BUILD_EMSCRIPTEN=ON -DEMSCRIPTEN_DIR="$SDK/emscripten" \
 -DTAU_WASM_PTHREADS=OFF -DTAU_BAS=sbf,tau,qint,bv,hsb,qlt -DTAU_CACHE=OFF \
 -DTAU_BUILD_STATIC_LIBRARY=ON -DTAU_BUILD_WORK_BUDGET_WORKER=ON -DTAU_LTO=OFF \
 -DTAU_PRIVATE_CVC5_ROOT="$solver" -DTAU_WORK_BUDGET_SPOT_V1=ON \
 -DTAU_OFFLINE_BOOST_HEADERS="$boost" \
 -DTAU_BUILD_CODEGEN_TESTS=OFF -DTAU_BUILD_TESTS=OFF -DTAU_DEPS_JOBS=1 \
 -DTAU_SHARED_PREFIX="$deps"
cmake --build "$build" --target tau_work_budget -j 1
printf '%s  %s\n' 6cda21644e326bac9b2108c7590f059585583339d42c93dee26dec0693012f50 "$build/tau_work_budget.wasm" | sha256sum -c -
