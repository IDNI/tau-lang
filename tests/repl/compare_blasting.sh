#!/usr/bin/env bash
# Compares `normalize` output with blasting on vs off; the C++ case asserts
# only on_result == off_result, which no ctest regex can express.
# Usage: compare_blasting.sh <tau-binary> <formula>; exit code is the verdict.
set -u

TAU="${1:?usage: compare_blasting.sh <tau-binary> <formula>}"
FORMULA="${2:?usage: compare_blasting.sh <tau-binary> <formula>}"

# Resource guards, ported from the C++ helper's constants.
MEMORY_LIMIT_KB=$((16384 * 1024))
TIME_LIMIT_S=1200

ulimit -v "${MEMORY_LIMIT_KB}" 2>/dev/null || \
	echo "warning: could not apply ${MEMORY_LIMIT_KB}KB address-space limit" >&2

# Run one normalization and echo its result line.
run_normalize() {
	local mode="$1" out rc
	out="$(timeout "${TIME_LIMIT_S}" "${TAU}" \
		-e "set blasting ${mode}. normalize ${FORMULA}." 2>&1)"
	rc=$?
	if [ "${rc}" -eq 124 ]; then
		echo "__TIMEOUT__ after ${TIME_LIMIT_S}s [blasting ${mode}]" >&2
		return 1
	fi
	# An internal error is a failure, matching add_repl_test's Error pattern.
	if printf '%s' "${out}" | grep -q "Error"; then
		echo "__ERROR__ tau reported an error [blasting ${mode}]" >&2
		printf '%s\n' "${out}" >&2
		return 1
	fi
	printf '%s' "${out}" \
		| sed 's/\x1b\[[0-9;]*m//g' \
		| grep -E '^%[0-9]+:' \
		| tail -1
}

ON="$(run_normalize on)"   || exit 1
OFF="$(run_normalize off)" || exit 1

# A missing result line means the formula never normalized -- not an agreement.
if [ -z "${ON}" ] || [ -z "${OFF}" ]; then
	echo "FAIL: no '%N:' result line produced" >&2
	echo "  formula: ${FORMULA}" >&2
	echo "  blasting on : '${ON}'" >&2
	echo "  blasting off: '${OFF}'" >&2
	exit 1
fi

if [ "${ON}" != "${OFF}" ]; then
	echo "FAIL: blasting on/off disagree" >&2
	echo "  formula: ${FORMULA}" >&2
	echo "  blasting on : ${ON}" >&2
	echo "  blasting off: ${OFF}" >&2
	exit 1
fi

exit 0
