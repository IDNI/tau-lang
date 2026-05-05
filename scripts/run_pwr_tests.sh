#!/bin/bash
# Run pointwise revision tests, working around the pre-existing
# test-ordering crash bug in the interpreter test framework
# (heap state leakage between TEST_CASEs causes SIGSEGV on later tests).
# Each test is run in a separate process to avoid leakage.

set -u
BUILD_DIR="${1:-build}"
BIN="$BUILD_DIR/test_integration-pointwise_revision"

if [[ ! -x "$BIN" ]]; then
	echo "binary not found: $BIN" >&2
	exit 2
fi

TESTS=(
	"pwr/path1*"
	"pwr/path2*"
	"pwr/path3*"
	"pwr/multi-step*"
	"pwr/this-stream*"
	"pwr/trivial*"
	"pwr/idempotence*"
	"pwr/false*"
	"pwr/sanity*"
	"pwr/complex:*"
	"pwr/sbf-revision*"
	"pwr/agm-success*"
	"pwr/agm-vacuity*"
	"pwr/agm-consistency*"
)

pass=0
fail=0
for tc in "${TESTS[@]}"; do
	out=$(timeout 30 "$BIN" -tc="$tc" 2>&1)
	if echo "$out" | grep -q "Status: SUCCESS"; then
		echo "[PASS] $tc"
		((pass++))
	else
		echo "[FAIL] $tc"
		echo "$out" | tail -10
		((fail++))
	fi
done

echo "---"
echo "passed: $pass / $((pass + fail))"
[[ $fail -eq 0 ]]
