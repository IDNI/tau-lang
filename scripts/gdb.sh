#!/bin/bash

# Script to compile and run a specific test with GDB debugger
# Usage: ./dev gdb <test_name> [gdb_options]
# Examples:
#   ./dev gdb test_bool
#   ./dev gdb test_integration-ba_types_inference
#   ./dev gdb test_repl-cnf_cmd
#   ./dev gdb test_api-tref_api

source "$(dirname "${BASH_SOURCE[0]}")/env"

# Initialize DEP_VARS (dep_shared_prefix needs it declared as an associative
# array); this script has no -D options of its own, so no args are passed.
dep_entry

TEST_CMD="gdb"
TEST_OPTS_LABEL="gdb_options"
test_common_usage_extra() {
	echo ""
}
source "$(dirname "${BASH_SOURCE[0]}")/test_common"

echo ""
echo "Running test with GDB: $TEST_NAME"
echo ""

# Set LD_LIBRARY_PATH for tests that require the CVC5 shared library
CVC5_DIST="$(dep_shared_prefix)/cvc5/dist"
export LD_LIBRARY_PATH="${CVC5_DIST}/lib:${CVC5_DIST}/lib64:${LD_LIBRARY_PATH:-}"

if [[ "$TEST_NAME" =~ ^test_repl- ]]; then
	# Run REPL test with GDB
	cmd=$(ctest --test-dir build-Debug --output-on-failure -N -V -R "^${TEST_NAME}$" | grep "Test command: " | sed 's/.*"-c"[[:space:]]*//; s/^"//; s/"$//')
	# $cmd is a shell command line from ctest and may contain quoted
	# arguments with spaces, so it stays inside the script text for the
	# inner shell to parse. The extra gdb options arrive as positional
	# parameters through "$@".
	bash -c "gdb --args $cmd \"\$@\"" bash "$@"
	exit $?
fi

# Find the test executable in build-Debug directory
TEST_EXECUTABLE="./build-Debug/$TEST_NAME"

if [ ! -f "$TEST_EXECUTABLE" ]; then
	echo "Error: Test executable not found at $TEST_EXECUTABLE"
	exit 1
fi

# Run the test with GDB
gdb --args "$TEST_EXECUTABLE" "$@"
exit $?
