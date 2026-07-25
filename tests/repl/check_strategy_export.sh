#!/usr/bin/env bash
# Runs a sat check with TAU_LTL_EXPORT_STRATEGY_FILE set and verifies the
# written file contains "HOA:" and "States:" (stricter than the C++ case,
# which skips both greps when the file is missing).
# Usage: check_strategy_export.sh <tau-binary> <command>; exit code is the verdict.
set -u

TAU="${1:?usage: check_strategy_export.sh <tau-binary> <command>}"
CMD="${2:?usage: check_strategy_export.sh <tau-binary> <command>}"

TMP="$(mktemp -t tau_strategy_export.XXXXXX.hoa)" || {
	echo "FAIL: could not create temp file" >&2; exit 1; }
# Remove on every exit path, unlike the C++ case.
# on the success path, which leaks a file per failed run.
trap 'rm -f "${TMP}"' EXIT

out="$(TAU_LTL_EXPORT_STRATEGY_FILE="${TMP}" timeout 300 "${TAU}" -e "${CMD}" 2>&1)"
rc=$?

if [ "${rc}" -eq 124 ]; then
	echo "FAIL: timed out" >&2; exit 1
fi
if printf '%s' "${out}" | grep -q "Error"; then
	echo "FAIL: tau reported an error" >&2
	printf '%s\n' "${out}" >&2
	exit 1
fi

# The C++ asserts the sat check itself succeeds (CHECK(result)) before looking
# at the file, so assert the verdict too rather than only the file.
if ! printf '%s' "${out}" | sed 's/\x1b\[[0-9;]*m//g' | grep -qE ': T'; then
	echo "FAIL: expected a satisfiable verdict (': T')" >&2
	printf '%s\n' "${out}" >&2
	exit 1
fi

if [ ! -s "${TMP}" ]; then
	echo "FAIL: ${TAU_LTL_EXPORT_STRATEGY_FILE:-strategy file} was not written" >&2
	exit 1
fi

# A missing file is a failure here; the C++ case would pass silently.
for marker in "HOA:" "States:"; do
	if ! grep -q "${marker}" "${TMP}"; then
		echo "FAIL: exported strategy file has no '${marker}'" >&2
		echo "--- file content ---" >&2
		cat "${TMP}" >&2
		exit 1
	fi
done

exit 0
