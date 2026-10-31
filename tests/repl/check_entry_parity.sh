#!/usr/bin/env bash
# The outputs `tau <spec file>` prints on the lines of an input file, against
# those of `run` of the same spec in the REPL. Ends with "SAME: <n> outputs"
# when both print the same lines, and with a diff otherwise.
# Usage: check_entry_parity.sh <tau> <spec.tau> <spec.in>
set -u

TAU="${1:?usage: check_entry_parity.sh <tau> <spec.tau> <spec.in>}"
SPEC="${2:?}"
IN="${3:?}"
outputs() { grep -ao 'o[0-9]*\[[0-9]*\] := .*'; }

file=$("$TAU" --color false "$SPEC" < "$IN" 2>/dev/null | outputs)
text=$(tr '\n' ' ' < "$SPEC" | sed 's/[. ]*$//')
repl=$( { printf 'run %s.\n' "$text"; cat "$IN"; printf 'q\nq\n'; } \
	| "$TAU" -X --color false 2>&1 | outputs)

if [ -n "$file" ] && [ "$file" = "$repl" ]; then
	echo "SAME: $(printf '%s\n' "$file" | wc -l) outputs"
else
	diff <(printf '%s\n' "$file") <(printf '%s\n' "$repl")
	echo "DIFFERENT"
fi
