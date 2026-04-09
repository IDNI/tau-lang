#!/bin/bash

# Script to compile and run a specific test
# Usage: ./dev test <test_name> [ctest_options]
# Examples:
#   ./dev test test_bool
#   ./dev test test_integration-ba_types_inference
#   ./dev test test_repl-cnf_cmd
#   ./dev test test_api-tref_api

# Helper function to print usage
usage() {
	echo "Usage: ./dev test <test_name> [ctest_options]"
	echo ""
	echo "Examples:"
	echo "  ./dev test test_bool                              # Unit test"
	echo "  ./dev test test_integration-ba_types_inference    # Integration test"
	echo "  ./dev test test_repl-cnf_cmd                      # REPL test"
	echo "  ./dev test test_api-tref_api                      # API test"
	echo ""
	echo "Build flags are automatically determined based on test name:"
	echo "  test_integration-*   → -DTAU_BUILD_INTEGRATION=ON"
	echo "  test_repl-*          → -DTAU_BUILD_REPL_TESTS=ON"
	echo "  test_api-*           → -DTAU_BUILD_API_TESTS=ON"
	echo "  other tests (unit)   → -DTAU_BUILD_UNIT_TESTS=ON"
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
# NOTE: test_api-repl* must be checked before the generic test_api-* pattern
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
echo "Running test: $TEST_NAME"
echo ""

# Run the specific test from the project root
ctest --test-dir build-Debug --output-on-failure -R "^${TEST_NAME}$" "$@"
exit $?
