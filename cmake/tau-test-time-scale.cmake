# The time limits of the tests in a debug build.
#
# The limits are set for an optimized build. A debug (-O0) build runs a test 10
# to 40 times slower, slower still under ASan, so TAU_TEST_TIME_SCALE (default
# 10 for CMAKE_BUILD_TYPE Debug, else 1) multiplies every ctest TIMEOUT. A REPL
# case also gets TAU_LTL_TIMEOUT, the wall-clock cap of each ltlsynt call and
# each data game, at the default of ltl_timeout_sec_default
# (src/ltl_aba_limits.h) times the scale, unless the case sets the variable
# itself.
if(NOT DEFINED TAU_TEST_TIME_SCALE)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(TAU_TEST_TIME_SCALE 10)
	else()
		set(TAU_TEST_TIME_SCALE 1)
	endif()
endif()

# The cases that read the default ltl-timeout back.
set(TAU_TEST_TIME_SCALE_KEEP_LTL_TIMEOUT test_repl-get_cmd-ltl_timeout)

# Scales the tests of the current directory. A directory may call it more
# than once, so each test is scaled once.
function(tau_scale_test_time_limits)
	if(TAU_TEST_TIME_SCALE EQUAL 1)
		return()
	endif()
	math(EXPR _ltl_timeout "60 * ${TAU_TEST_TIME_SCALE}")
	get_property(_tests DIRECTORY PROPERTY TESTS)
	foreach(_test ${_tests})
		get_test_property("${_test}" TAU_TIME_SCALED _done)
		if(_done)
			continue()
		endif()
		set_property(TEST "${_test}" PROPERTY TAU_TIME_SCALED TRUE)
		get_test_property("${_test}" TIMEOUT _timeout)
		if(_timeout)
			math(EXPR _timeout "${_timeout} * ${TAU_TEST_TIME_SCALE}")
			set_property(TEST "${_test}" PROPERTY TIMEOUT "${_timeout}")
		endif()
		if(NOT _test MATCHES "^test_repl-"
			OR _test IN_LIST TAU_TEST_TIME_SCALE_KEEP_LTL_TIMEOUT)
			continue()
		endif()
		get_test_property("${_test}" ENVIRONMENT _env)
		if(NOT _env MATCHES "TAU_LTL_TIMEOUT=")
			set_property(TEST "${_test}" APPEND PROPERTY ENVIRONMENT
				"TAU_LTL_TIMEOUT=${_ltl_timeout}")
		endif()
	endforeach()
endfunction()
