#!/bin/bash

# Script to compile and run a specific test
# Usage: ./dev test <test_name> [ctest_options]
# Examples:
#   ./dev test test_bool
#   ./dev test test_integration-ba_types_inference
#   ./dev test test_repl-cnf_cmd
#   ./dev test test_api-tref_api

TEST_CMD="test"
TEST_OPTS_LABEL="ctest_options"
source "$(dirname "${BASH_SOURCE[0]}")/test_common"

echo ""
echo "Running test: $TEST_NAME"
echo ""

# Run the specific test from the project root
ctest --test-dir build-Debug --output-on-failure -R "^${TEST_NAME}$" "$@"
exit $?
