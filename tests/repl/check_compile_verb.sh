#!/usr/bin/env bash
# `tau compile` end-to-end smoke: copies a spec to a scratch dir (so the
# emitted spec.build/ lands outside the source tree), compiles it, then runs
# the produced executable. Usage: check_compile_verb.sh <tau-binary> <spec-file>.
set -u

TAU="${1:?usage: check_compile_verb.sh <tau-binary> <spec-file>}"
SPEC="${2:?usage: check_compile_verb.sh <tau-binary> <spec-file>}"

TMPDIR="$(mktemp -d -t tau_compile_verb.XXXXXX)" || {
	echo "FAIL: could not create scratch dir" >&2; exit 1; }
trap 'rm -rf "${TMPDIR}"' EXIT

cp "${SPEC}" "${TMPDIR}/spec.tau"
EXE="${TMPDIR}/spec_exe"

out="$(timeout 300 "${TAU}" compile "${TMPDIR}/spec.tau" -o "${EXE}" 2>&1)"
rc=$?
if [ "${rc}" -eq 124 ]; then
	echo "FAIL: tau compile timed out" >&2; exit 1
fi
if [ "${rc}" -ne 0 ]; then
	echo "FAIL: tau compile exited ${rc}" >&2
	printf '%s\n' "${out}" >&2
	exit 1
fi
if [ ! -x "${EXE}" ]; then
	echo "FAIL: no executable produced at ${EXE}" >&2
	printf '%s\n' "${out}" >&2
	exit 1
fi

timeout 30 "${EXE}" >/dev/null 2>&1
rc=$?
if [ "${rc}" -ne 0 ]; then
	echo "FAIL: compiled executable exited ${rc}" >&2
	exit 1
fi

exit 0
