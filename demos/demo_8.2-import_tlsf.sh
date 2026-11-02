#!/bin/bash
# tau import-tlsf: the specification of a TLSF file, and what it refuses.
#
# Run from the demos folder with a Release build:  ./demo_8.2-import_tlsf.sh
# Set TAU to use another binary.

TAU="${TAU:-$(pwd)/../build/release/tau}"
tlsf=../tests/syntcomp
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

# The Tau spec goes to stdout, or to the -o path. It declares the streams, as
# bv[1] console streams, and ends with the specification itself.
"$TAU" import-tlsf "$tlsf/delay.tlsf" -o "$work/delay.tau"
cat "$work/delay.tau"

# The last line is a formula the REPL decides. delay repeats the input one step
# later and is realizable; predict must announce the next input and is not.
for name in delay predict toggle; do
	"$TAU" import-tlsf "$tlsf/$name.tlsf" -o "$work/$name.tau"
	formula=$(tail -n 1 "$work/$name.tau")
	echo "$name: $formula"
	"$TAU" --color false -q -e "realizable $formula" 2>&1 </dev/null | grep -E '^%'
done

# Only Mealy semantics is imported. A Moore file is refused with exit code 1.
"$TAU" import-tlsf "$tlsf/moore_refused.tlsf" -o "$work/moore.tau"
echo "exit code: $?"
