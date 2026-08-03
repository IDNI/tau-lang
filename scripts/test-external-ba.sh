#!/usr/bin/env bash
# Proves a Boolean algebra living outside src/ can join the pack.
#
# Configures a build whose pack is tau,sbf,ext -- where "ext" is registered
# from tests/external_ba/register.cmake -- builds the tau CLI, and checks the
# binary reports the algebra and parses one of its constants.
#
# Usage: scripts/test-external-ba.sh [build-dir] [jobs]

set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build="${1:-$root/build/external-ba}"
jobs="${2:-4}"
register="$root/tests/external_ba/register.cmake"

generator="Unix Makefiles"
if command -v ninja > /dev/null 2>&1 \
		|| command -v ninja-build > /dev/null 2>&1; then
	generator="Ninja"
fi

compiler_args=()
if command -v clang++ > /dev/null 2>&1 \
		&& command -v clang > /dev/null 2>&1; then
	compiler_args=(-DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++)
fi

echo "== configuring $build with the out-of-tree BA using $generator"
cmake --fresh -S "$root" -B "$build" -G "$generator" \
	"${compiler_args[@]}" \
	-DCMAKE_BUILD_TYPE=Devel \
	-DTAU_BUILD_EXECUTABLE=ON \
	-DTAU_BUILD_UNIT_TESTS=ON \
	-DTAU_EXTERNAL_BAS="$register" \
	-DTAU_BAS=tau,sbf,ext

pack_header="$build/generated/include/tau_pack.h"
echo "== checking the generated pack"
grep -q 'TAU_PACK_HAS_BA_EXT' "$pack_header" \
	|| { echo "FAIL: no TAU_PACK_HAS_BA_EXT in $pack_header"; exit 1; }
grep -q 'ext_ba' "$pack_header" \
	|| { echo "FAIL: ext_ba missing from the pack node type"; exit 1; }

echo "== building the tau CLI and the unit suite"
cmake --build "$build" -j"$jobs"

echo "== checking the algebra against the descriptor contract"
"$build/test_ba_conformance" \
	|| { echo "FAIL: the out-of-tree BA breaks the descriptor contract"; exit 1; }

echo "== smoke-running the binary"
out="$(printf 'sat {1}:ext = {1}:ext.\nq\n' | "$build/tau" 2>&1)"
echo "$out" | sed 's/\x1b\[[0-9;]*m//g' | grep -q ': T' \
	|| { echo "FAIL: external BA constant did not evaluate"; echo "$out"; exit 1; }

echo "PASS: out-of-tree BA 'ext' built into the pack and answered a query"
