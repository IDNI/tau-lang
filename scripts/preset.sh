#!/bin/bash

source "$(dirname "${BASH_SOURCE[0]}")/env"

PRESET_RUN_BIN="tau"
cd "${REPO_ROOT}"

# The build, heavy and test jobs, from the cores and the memory
# (cmake/tau-jobs.cmake); a -D on the command line or the environment variable
# keeps its value. The build jobs reach devrc through TAU_BUILD_JOBS and the
# configure computes the same heavy jobs from them; the test jobs run ctest
# below.
jobs_args=()
for arg in "$@"; do
	[[ $arg == -- ]] && break
	case "$arg" in
		-DTAU_BUILD_JOBS=*|-DTAU_HEAVY_JOBS=*|-DTAU_TEST_JOBS=*) jobs_args+=("$arg") ;;
	esac
done
read -r build_jobs heavy_jobs test_jobs < <(cmake ${jobs_args[@]+"${jobs_args[@]}"} \
	-P "${REPO_ROOT}/cmake/tau-jobs.cmake")
if [[ -z ${test_jobs:-} ]]; then
	echo "cmake/tau-jobs.cmake gave no job counts" >&2
	exit 1
fi
echo "Jobs: build ${build_jobs}, heavy ${heavy_jobs}, test ${test_jobs}"
export TAU_BUILD_JOBS="${build_jobs}"

# resolve_jobs drops -DTAU_BUILD_JOBS from the configure arguments and exports it
# instead, so the cache keeps its empty default and every later reconfigure
# re-resolves the count. The typed spelling survives that drop and is cached.
# A test preset's `run` is taken out here: the tests run below with the test
# jobs, where devrc would run them with the build jobs.
normalize_args "$@"
[[ -z $PRESET ]] && PRESET="release"
run_tests=0
if [[ $RUN == run ]] && { [[ $PRESET == *test* ]] || [[ $PRESET == *-all ]] \
		|| [[ $PRESET == *-all-* ]] || [[ $PRESET == all ]]; }; then
	run_tests=1
fi
preset_args=()
after_dashes=0
for arg in "$@"; do
	if [[ $arg == -- ]]; then
		after_dashes=1
	elif [[ $after_dashes == 0 && $run_tests == 1 && $arg == run ]]; then
		continue
	fi
	preset_args+=("$arg")
	case "$arg" in
		-DTAU_BUILD_JOBS=*)
			[[ $after_dashes == 0 ]] \
				&& preset_args+=("-DTAU_BUILD_JOBS:STRING=${arg#-DTAU_BUILD_JOBS=}")
			;;
	esac
done
preset_entry ${preset_args[@]+"${preset_args[@]}"} || exit $?

if [[ $run_tests == 1 ]]; then
	echo "Running tests (${test_jobs} jobs)"
	ctest --preset "${PRESET}" -j "${test_jobs}" --output-on-failure
	status=$?
	if [[ $status -ne 0 ]]; then
		echo "Running tests failed"
		exit $status
	fi
	echo "All tests passed"
fi
