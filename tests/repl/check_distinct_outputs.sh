#!/usr/bin/env bash
# Reads the output of a run on stdin, passes it through, and checks the
# disequalities of the run's specification from step <start> on: a pair
# `a:b` or `a:b-k` asks that a[t] and b[t-k] print differently at every such
# step. Two values printed alike are equal, so the check never reports a
# false violation. It ends with "EQUAL OUTPUTS ..." for each violation, and
# with "distinct outputs at <n> steps" otherwise.
# Usage: check_distinct_outputs.sh <start> <pair>...
set -u

START="${1:?usage: check_distinct_outputs.sh <start> <pair>...}"
shift
ESC="$(printf '\033')"
sed "s/${ESC}\[[0-9;]*m//g" | awk -v start="${START}" -v pairs="$*" '
{ print }
match($0, /^[A-Za-z_][A-Za-z0-9_]*\[[0-9]+\] *:= /) {
	head = substr($0, 1, RLENGTH)
	value = substr($0, RLENGTH + 1)
	name = head; sub(/\[.*/, "", name)
	step = head; sub(/^[^[]*\[/, "", step); sub(/\].*/, "", step)
	val[name, step + 0] = value
	if (step + 0 > last) last = step + 0
}
END {
	n = split(pairs, ps, " ")
	checked = 0; bad = 0
	for (k = start; k <= last; ++k) {
		seen = 0
		for (i = 1; i <= n; ++i) {
			split(ps[i], ab, ":")
			b = ab[2]; lag = 0
			if (index(b, "-")) { lag = substr(b, index(b, "-") + 1) + 0; b = substr(b, 1, index(b, "-") - 1) }
			if (!((ab[1], k) in val) || !((b, k - lag) in val)) continue
			seen = 1
			if (val[ab[1], k] == val[b, k - lag]) {
				printf "EQUAL OUTPUTS %s[%d] = %s[%d] = %s\n", ab[1], k, b, k - lag, val[ab[1], k]
				bad = 1
			}
		}
		checked += seen
	}
	if (!bad) printf "distinct outputs at %d steps\n", checked
}'
