#!/usr/bin/env bash
# Pipes a demo into tau the way demos/run.sh does, without its pauses. A demo
# writes its output streams under /tmp, which here is a folder of its own, so
# demos in parallel do not share a file. Prints DEMO-END when tau exits 0.
# Usage: run_demo.sh <tau-binary> <demo-file>
set -u
set -o pipefail

TAU="${1:?usage: run_demo.sh <tau-binary> <demo-file>}"
DEMO="${2:?usage: run_demo.sh <tau-binary> <demo-file>}"

SCRATCH="$(mktemp -d)"
trap 'rm -rf "${SCRATCH}"' EXIT

cd "$(dirname "${DEMO}")" || exit 1
# Comments are echoed by the REPL, and their words are no output of a demo.
{ grep -vE '##|^[[:space:]]*#' "$(basename "${DEMO}")" || true; } \
	| sed "s#/tmp/#${SCRATCH}/#g" \
	| "${TAU}" -X
rc=$?
[ "${rc}" -eq 0 ] || { echo "tau exited with status ${rc}" >&2; exit "${rc}"; }
echo DEMO-END
