#!/bin/bash
# measure-test.sh <dir> <command> [<arg>...]
#
# Runs the command under GNU time and appends one JSON line to
# <dir>/<TAU_TEST_NAME>.jsonl: wall, user and system seconds, the peak
# resident set in kB and the exit status. ctest's ENVIRONMENT names the
# test (TAU_TEST_MEASURE, cmake/tau-test-measure.cmake). A test that runs
# tau more than once appends one line per run. Without a name, or without
# GNU time, the command runs as it is. The exit status is the command's.
# Nothing here depends on PATH: a case may run tau with a PATH of its own.
dir="$1"
shift
if [ -z "${TAU_TEST_NAME:-}" ] || [ ! -x /usr/bin/time ]; then
	exec "$@"
fi
name="${TAU_TEST_NAME//\//_}"
exec /usr/bin/time -a -o "$dir/${name:-unnamed}.jsonl" \
	-f '{"wall": %e, "user": %U, "sys": %S, "rss_kb": %M, "status": %x}' "$@"
