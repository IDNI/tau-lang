#!/bin/bash
# tau gen, tau compile, --aiger and --machine on a mirror specification.
#
# Run from the demos folder with a Release build:  ./demo_8.1-gen_compile.sh
# Set TAU to use another binary. The compile steps build a small C++ project
# against the SDK of the binary's build folder and take about a minute each.

set -eu
TAU="${TAU:-$(pwd)/../build/release/tau}"
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
cd "$work"

# A spec file holds a specification, here a mirror of one input bit.
echo 'G(o1[t]:bv[1] = i1[t]:bv[1]).' > spec.tau

# tau gen writes a CMake project next to the spec, <spec>.build, with main.cpp
# and a CMakeLists.txt that links the tau SDK. Nothing is built.
"$TAU" gen spec.tau
ls spec.tau.build

# --aiger writes the AIGER circuit of a bv[1] LTL spec instead (spec.aag),
# a controller checked by ltlsynt.
"$TAU" gen --aiger spec.tau
cat spec.aag

# tau compile does gen and builds the project into an executable.
"$TAU" compile spec.tau -o sim

# The program reads one input per step and prints the outputs, and it stops
# when its input closes.
printf '1\n0\n1\n1\n' | ./sim 2>&1 | grep -E 'Execution step|:='

# --machine builds the program from a Mealy machine that ltlsynt verifies and
# plays it as a table, so it neither synthesizes nor solves when it runs.
"$TAU" compile --machine spec.tau -o sim_machine
printf '1\n0\n1\n1\n' | ./sim_machine 2>&1 | grep -E 'Execution step|:='
