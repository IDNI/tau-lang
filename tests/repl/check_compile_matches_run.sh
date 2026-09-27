#!/usr/bin/env bash
# `tau compile` against `run`: compiles a spec with no inputs, runs the
# program and `run N steps` of the same spec, and requires both to print
# the same value for every output of the first N steps.
# Usage: check_compile_matches_run.sh <tau-binary> <steps> <spec>.
set -u

TAU="${1:?usage: check_compile_matches_run.sh <tau-binary> <steps> <spec>}"
STEPS="${2:?usage: check_compile_matches_run.sh <tau-binary> <steps> <spec>}"
SPEC="${3:?usage: check_compile_matches_run.sh <tau-binary> <steps> <spec>}"

source "${BASH_SOURCE[0]%/*}/../../scripts/resolve_timeout"

TMPDIR="$(mktemp -d -t tau_compile_run.XXXXXX)" || {
	echo "FAIL: could not create scratch dir" >&2; exit 1; }
trap 'rm -rf "${TMPDIR}"' EXIT

printf '%s' "${SPEC}" > "${TMPDIR}/spec.tau"
EXE="${TMPDIR}/spec_exe"

out="$(run_with_timeout 600 "${TAU}" compile "${TMPDIR}/spec.tau" -o "${EXE}" 2>&1)"
rc=$?
if [ "${rc}" -ne 0 ] || [ ! -x "${EXE}" ]; then
	echo "FAIL: tau compile exited ${rc}"
	printf '%s\n' "${out}"
	exit 1
fi

# the outputs of the first STEPS steps, one "name[t] := value" per line
outputs() {
	sed 's/\x1b\[[0-9;]*m//g' | grep -oE '\bo[0-9]+\[[0-9]+\] := .*' \
		| sed 's/[[:space:]]*$//' \
		| awk -v n="${STEPS}" '{ split($1, a, /[][]/); if (a[2] < n) print }' \
		| sort
}

# the program asks for ENTER at each step that reads no input
program="$(yes '' | head -n "${STEPS}" \
	| run_with_timeout 60 "${EXE}" 2>&1 | outputs)"
run="$(printf 'run %s steps %s.\nq\n' "${STEPS}" "${SPEC}" \
	| run_with_timeout 300 "${TAU}" -X 2>&1 | outputs)"

if [ -z "${program}" ]; then
	echo "FAIL: the program printed no output"
	exit 1
fi
if [ "${program}" != "${run}" ]; then
	echo "FAIL: the program and run differ"
	diff <(printf '%s\n' "${program}") <(printf '%s\n' "${run}")
	exit 1
fi
echo "SAME: $(printf '%s\n' "${program}" | wc -l) outputs"
exit 0
