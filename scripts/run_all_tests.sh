#!/bin/bash
# Run all unit + integration tests with per-test watchdog.
# Every test runs inside safe_run_test.sh — kills the full process group on timeout.
# Usage: run_all_tests.sh [build_dir]
#
# The test list is *discovered* from the build directory (every `test_*`
# executable CMake produced) instead of being hard-coded here; a hard-coded
# list silently rots as suites are added, renamed or removed.
#
# Per-test timeouts mirror tests/CMakeLists.txt: heavy adversarial/fuzz suites
# get TAU_TEST_TIMEOUT_HEAVY, everything else TAU_TEST_TIMEOUT.
#
# Exit 0 if all pass/skip, non-zero if any fail or timeout.

BUILDDIR="${1:-./build}"
SAFE="$(cd "$(dirname "$0")" && pwd)/safe_run_test.sh"
cd "$BUILDDIR" || { echo "build dir not found: $BUILDDIR"; exit 1; }

# Same values tests/CMakeLists.txt gives ctest; overridable from the environment.
TAU_TEST_TIMEOUT="${TAU_TEST_TIMEOUT:-600}"
TAU_TEST_TIMEOUT_HEAVY="${TAU_TEST_TIMEOUT_HEAVY:-1500}"

# Tests matching this are run in parallel shards by the section below, so the
# serial loop must skip them.
SHARD_GLOB='test_ltl_nlang_deepseek_*'

PASS=0; FAIL=0
declare -a FAILURES

# Heavy adversarial / fuzz suites, same set tests/CMakeLists.txt gives a longer
# ctest TIMEOUT to.
HEAVY_TESTS="test_ltl_adversarial test_ltl_adversarial2 test_ltl_aba test_ltl_fuzz100 test_ltl_soundness_regression"

timeout_for() {
	local t
	for t in $HEAVY_TESTS; do
		if [ "$t" = "$1" ]; then
			echo "$TAU_TEST_TIMEOUT_HEAVY"
			return
		fi
	done
	echo "$TAU_TEST_TIMEOUT"
}

run() {
    local name="$1" binary="$2" timeout="${3:-$(timeout_for "$1")}"
    if [ ! -x "$binary" ]; then
        echo "[SKIP] $name (binary not found)"
        return
    fi
    SAFE_RUN_TIMEOUT="$timeout" "$SAFE" "$binary" 2>&1
    local rc=$?
    if [ $rc -eq 0 ]; then
        echo "[PASS] $name"
        ((PASS++)) || true
    else
        echo "[FAIL] $name (exit $rc)"
        FAILURES+=("$name")
        ((FAIL++)) || true
    fi
}

# ── discovered tests ──────────────────────────────────────────────────────────
# CMake puts every test binary straight into the build root (see the add_test()
# COMMAND in tests/CMakeLists.txt), so a single glob covers unit, integration,
# api, repl and benchmark suites at once.
echo "=== tests ==="
shopt -s nullglob
found_any=0
for binary in ./test_*; do
    [ -f "$binary" ] && [ -x "$binary" ] || continue
    name="${binary#./}"
    # shellcheck disable=SC2254
    case "$name" in $SHARD_GLOB) continue ;; esac
    found_any=1
    run "$name" "$binary"
done
if [ "$found_any" -eq 0 ]; then
    echo "no test binaries found in $BUILDDIR — was the tree built with tests enabled?"
    exit 1
fi

# ── parallel shards ───────────────────────────────────────────────────────────
# Long nlang suites, when present, are sharded and run concurrently.
shards=( $SHARD_GLOB )
if [ ${#shards[@]} -gt 0 ]; then
    echo "=== nlang parallel ==="
    declare -a SHARD_PIDS SHARD_NAMES
    for binary in "${shards[@]}"; do
        [ -x "$binary" ] || continue
        SAFE_RUN_TIMEOUT="$TAU_TEST_TIMEOUT" "$SAFE" "$binary" &
        SHARD_PIDS+=("$!")
        SHARD_NAMES+=("$binary")
    done
    for i in "${!SHARD_PIDS[@]}"; do
        if wait "${SHARD_PIDS[$i]}"; then
            echo "[PASS] nlang shard ${SHARD_NAMES[$i]}"; ((PASS++)) || true
        else
            echo "[FAIL] nlang shard ${SHARD_NAMES[$i]}"
            FAILURES+=("nlang:shard:${SHARD_NAMES[$i]}"); ((FAIL++)) || true
        fi
    done
fi

# ── summary ───────────────────────────────────────────────────────────────────
echo ""
echo "=== RESULTS: $PASS passed, $FAIL failed ==="
if [ ${#FAILURES[@]} -gt 0 ]; then
    echo "FAILED:"
    for f in "${FAILURES[@]}"; do echo "  $f"; done
fi
[ $FAIL -eq 0 ]
