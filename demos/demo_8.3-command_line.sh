#!/bin/bash
# The command line of tau: -e, --error-quits, spec files and exit codes.
#
# Run from the demos folder with a Release build:  ./demo_8.3-command_line.sh
# Set TAU to use another binary.

TAU="${TAU:-$(pwd)/../build/release/tau}"
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

# -e evaluates one REPL command and exits. The last -e is the one evaluated.
"$TAU" --color false -e "n x | x'" </dev/null 2>&1 | grep -E '^%'

# A command that fails ends the process with a nonzero code when --error-quits
# is given. Here the formula lacks its closing parenthesis:
"$TAU" --color false --error-quits -e "sat G (o1[t] = " </dev/null >/dev/null 2>&1
echo "exit code with --error-quits: $?"

# A spec file is a specification, not a list of commands. It declares its
# streams and runs: this one mirrors a file of bits to the console.
printf '1\n0\n1\n' > "$work/in.txt"
cat > "$work/mirror.tau" <<SPEC
i1:bv[1] := in file("$work/in.txt").
o1:bv[1] := out console.
always o1[t]:bv[1] = i1[t]:bv[1].
SPEC
"$TAU" --color false "$work/mirror.tau" </dev/null 2>&1 | grep -E 'Execution step|:='
echo "exit code of a spec file: ${PIPESTATUS[0]}"

# A file that does not exist is an error.
"$TAU" --color false "$work/nofile.tau" </dev/null 2>&1 | tail -n 1
echo "exit code of a missing file: ${PIPESTATUS[0]}"

# The same switch exists in the REPL, where the first failing line ends the
# session with exit code 1:
printf 'set error-quits on\nn x\nsat G (o1[t] = \nn y\n' | "$TAU" -X >/dev/null 2>&1
echo "exit code of the REPL with error-quits: $?"
