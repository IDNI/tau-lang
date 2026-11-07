include_guard(GLOBAL)

# The doctest listener (tests/benchmark_listener.h) writes the time and the
# peak memory of every case of a suite to <build>/benchmarks/<test>.json.
# TAU_TEST_MEASURE puts scripts/ci/measure-test.sh in front of every test
# process as well: GNU time appends the wall, user and system seconds and
# the peak resident set of the process to <build>/measure/<test>.jsonl,
# filed under the ctest name that TAU_TEST_NAME carries.
set(TAU_TEST_BENCHMARK_DIR "${PROJECT_BINARY_DIR}/benchmarks")
set(TAU_TEST_MEASURE_DIR "${PROJECT_BINARY_DIR}/measure")
file(MAKE_DIRECTORY "${TAU_TEST_BENCHMARK_DIR}")
set(TAU_TEST_MEASURE_LAUNCHER "")
if(TAU_TEST_MEASURE)
	# The launcher needs the -f, -a and %M of GNU time; the time of macOS
	# and the shell keyword have none of them.
	execute_process(COMMAND /usr/bin/time --version
		OUTPUT_VARIABLE _tau_time_version ERROR_VARIABLE _tau_time_version
		RESULT_VARIABLE _tau_time_result)
	if(NOT _tau_time_result EQUAL 0 OR NOT _tau_time_version MATCHES "GNU")
		message(FATAL_ERROR
			"TAU_TEST_MEASURE needs GNU time at /usr/bin/time (the time package)")
	endif()
	file(MAKE_DIRECTORY "${TAU_TEST_MEASURE_DIR}")
	set(TAU_TEST_MEASURE_LAUNCHER
		"${PROJECT_SOURCE_DIR}/scripts/ci/measure-test.sh" "${TAU_TEST_MEASURE_DIR}")
endif()

# The ENVIRONMENT entries of the suite registered as <test>: the file of its
# listener, and under TAU_TEST_MEASURE the name the launcher files it under.
function(tau_test_measure_environment out test)
	set(_env "TAU_BENCHMARK_FILE=${TAU_TEST_BENCHMARK_DIR}/${test}.json")
	if(TAU_TEST_MEASURE)
		list(APPEND _env "TAU_TEST_NAME=${test}")
	endif()
	set(${out} "${_env}" PARENT_SCOPE)
endfunction()
