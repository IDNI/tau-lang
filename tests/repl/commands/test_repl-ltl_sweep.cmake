include(add_repl_test)

#
# Every formula of one and two operators against Spot, with a run check on a
# sample. An optimized binary only: at -O0 the sweep takes hours. The nightly
# workflow ltl-sweep.yml runs the temporal triples.
#

find_package(Python3 COMPONENTS Interpreter QUIET)
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND TAU_REPL_LTLSYNT_USABLE
		AND Python3_Interpreter_FOUND AND NOT CMAKE_HOST_WIN32
		AND bv IN_LIST TAU_PACK_BA_IDS)
	get_filename_component(_spot_bin "${TAU_LTLSYNT_PROGRAM}" DIRECTORY)
	set(_sweep_options "")
	if(TAU_ADDRESS_SANITIZER)
		list(APPEND _sweep_options --no-address-limit)
	endif()
	add_test(NAME test_repl-ltl_sweep
		COMMAND "${Python3_EXECUTABLE}"
			"${CMAKE_SOURCE_DIR}/tests/sweep/ltl_sweep.py"
			--tau "$<TARGET_FILE:tau>" --spot-bin "${_spot_bin}"
			--mode n1 n2 --jobs 8 --run 100 ${_sweep_options})
	set_tests_properties(test_repl-ltl_sweep PROPERTIES
		PASS_REGULAR_EXPRESSION "\n0 defects"
		PROCESSORS 8 TIMEOUT 1800 LABELS browser_skip)
endif()
