#!/bin/bash

# Script to compile and run a specific test with GDB debugger
# Usage: ./dev gdb <test_name> [gdb_options]
# Examples:
#   ./dev gdb test_bool
#   ./dev gdb test_integration-ba_types_inference
#   ./dev gdb test_repl-cnf_cmd
#   ./dev gdb test_api-tref_api

# Helper function to print usage
usage() {
	echo "Usage: ./dev gdb <test_name> [gdb_options]"
	echo ""
	echo "Examples:"
	echo "  ./dev gdb test_bool                              # Unit test"
	echo "  ./dev gdb test_integration-ba_types_inference    # Integration test"
	echo "  ./dev gdb test_repl-cnf_cmd                      # REPL test"
	echo "  ./dev gdb test_api-tref_api                      # API test"
	echo ""
	echo "Build flags are automatically determined based on test name:"
	echo "  test_integration-*   → -DTAU_BUILD_INTEGRATION=ON"
	echo "  test_repl-*          → -DTAU_BUILD_REPL_TESTS=ON"
	echo "  test_api-*           → -DTAU_BUILD_API_TESTS=ON"
	echo "  other tests (unit)   → -DTAU_BUILD_UNIT_TESTS=ON"
	echo ""
}

# Check if test name is provided
if [ $# -eq 0 ]; then
	echo "Error: No test name provided"
	echo ""
	usage
	exit 1
fi

if [ "$1" = "help" ] || [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
	usage
	exit 0
fi

TEST_NAME="$1"
shift

# Determine build flags based on test name
BUILD_FLAGS=""

if [[ "$TEST_NAME" =~ ^test_integration- ]]; then
	BUILD_FLAGS="-DTAU_BUILD_INTEGRATION=ON"
	echo "Building integration test: $TEST_NAME"
elif [[ "$TEST_NAME" =~ ^test_repl- ]]; then
	BUILD_FLAGS="-DTAU_BUILD_REPL_TESTS=ON"
	echo "Building REPL test: $TEST_NAME"
elif [[ "$TEST_NAME" =~ ^test_api- ]]; then
	BUILD_FLAGS="-DTAU_BUILD_API_TESTS=ON"
	echo "Building API test: $TEST_NAME"
else
	BUILD_FLAGS="-DTAU_BUILD_UNIT_TESTS=ON"
	echo "Building unit test: $TEST_NAME"
fi

echo "Build flags: $BUILD_FLAGS"
echo ""

# Build the test
echo "Compiling..."
./dev debug --target $TEST_NAME $BUILD_FLAGS || {
	echo "Error: Build failed"
	exit 1
}

echo ""
echo "Running test with GDB: $TEST_NAME"
echo ""

# Set LD_LIBRARY_PATH for tests that require the CVC5 shared library
export LD_LIBRARY_PATH="./external/cvc5/build/src:${LD_LIBRARY_PATH}"

if [[ "$TEST_NAME" =~ ^test_repl- ]]; then
	# Run REPL test with GDB
	cmd=$(ctest --test-dir build-Debug --output-on-failure -N -V -R "^${TEST_NAME}$" | grep "Test command: " | sed 's/.*"-c"[[:space:]]*//; s/^"//; s/"$//')
	bash -c "gdb --args $cmd $@"
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
