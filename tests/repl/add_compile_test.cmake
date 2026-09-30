# `tau compile <spec.tau>` end-to-end test: gated like the REPL suite on the
# spec's BA content, since it drives the same binary.

# Resolved at include time (see add_strategy_export_test.cmake for why).
set(TAU_COMPILE_VERB_CHECKER "${CMAKE_CURRENT_LIST_DIR}/check_compile_verb.cmake")
set(TAU_COMPILE_RUN_CHECKER "${CMAKE_CURRENT_LIST_DIR}/check_compile_matches_run.cmake")

# The host platform's artifact preset: the build folder name when it is one of
# the platform names the SDK carries, else the build type's base folder (the
# pack variants configure the same way, only the SDK differs).
if(NOT DEFINED TAU_TEST_PLATFORM)
	get_filename_component(_tau_build_name "${CMAKE_BINARY_DIR}" NAME)
	if(EXISTS "${CMAKE_BINARY_DIR}/sdk/cmake/tau-platforms.cmake")
		include("${CMAKE_BINARY_DIR}/sdk/cmake/tau-platforms.cmake")
	endif()
	if(DEFINED TAU_PLATFORM_NAMES
			AND _tau_build_name IN_LIST TAU_PLATFORM_NAMES)
		set(TAU_TEST_PLATFORM "${_tau_build_name}")
	else()
		string(TOLOWER "${CMAKE_BUILD_TYPE}" TAU_TEST_PLATFORM)
	endif()
endif()

# add_compile_test(<name> <spec_file> [CHECKER <script>] [PRESET <platform>]
#     [NO_PRESET] [NO_SDK] [NEEDS_SDK <dir>] [EXPECT_FAIL] [ARGS <-DNAME=VALUE>...]
#     [PASS_REGEX <re>] [REQUIRES ltlsynt|hostfs|<ba-id> ...])
#
# Runs the CHECKER script, check_compile_verb.cmake by default, with
# `cmake -P` over <spec_file>, for the PRESET platform (the host one by
# default) and this build's SDK. NO_PRESET runs `tau compile` without
# --preset, so it builds for this machine. NO_SDK leaves the SDK to `tau
# compile`, as a cross case does. NEEDS_SDK skips the case unless <dir> holds
# an SDK. ARGS go to the script as they are. The exit code of the script is
# the verdict, and PASS_REGEX adds a pattern the output must match.
function(add_compile_test test_name spec_file)
	cmake_parse_arguments(PARSE_ARGV 2 _tau "NO_SDK;EXPECT_FAIL;NO_PRESET"
		"CHECKER;PRESET;NEEDS_SDK;PASS_REGEX" "ARGS;REQUIRES")
	set(_test "test_repl-${test_name}")
	# `tau compile` emits a CMake project and spawns a host compiler
	# (src/tau_compile.tmpl.h), which a wasm node host cannot run.
	list(APPEND _tau_REQUIRES subprocess)
	file(READ "${spec_file}" _spec_src)
	tau_repl_gate_case("${_test}" _spec_src)
	if(_tau_NEEDS_SDK AND NOT EXISTS "${_tau_NEEDS_SDK}/TauConfig.cmake")
		tau_repl_record_skip("${_test}" SDK)
		return()
	endif()
	if(NOT _tau_CHECKER)
		set(_tau_CHECKER "${TAU_COMPILE_VERB_CHECKER}")
	endif()
	if(NOT _tau_PRESET)
		set(_tau_PRESET "${TAU_TEST_PLATFORM}")
	endif()
	set(_args "-DTAU=${TAU_LAUNCHER}" "-DSPEC=${spec_file}"
		"-DTMP=${CMAKE_BINARY_DIR}/${_test}.scratch"
		"-DEXE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}")
	if(_tau_NO_PRESET)
		list(APPEND _args "-DPRESET=")
	else()
		list(APPEND _args "-DPRESET=${_tau_PRESET}")
	endif()
	if(NOT _tau_NO_SDK)
		list(APPEND _args "-DSDK=${CMAKE_BINARY_DIR}/sdk")
	endif()
	if(_tau_EXPECT_FAIL)
		list(APPEND _args "-DEXPECT_FAIL=ON")
	endif()
	add_test(NAME "${_test}"
		COMMAND ${CMAKE_COMMAND} ${_args} ${_tau_ARGS} -P "${_tau_CHECKER}")
	if(_tau_PASS_REGEX)
		set_tests_properties("${_test}" PROPERTIES
			PASS_REGULAR_EXPRESSION "${_tau_PASS_REGEX}")
	endif()
	# Compiling a nested cmake project takes real time beyond a REPL
	# round-trip: 150-175 s alone on a 32-thread machine. Under `ctest -j`
	# three of these nested builds ran at once beside a dozen other tests
	# and not only exceeded their own cap but starved the ltlsynt-heavy
	# suites into watchdog kills. RUN_SERIAL runs each compile test alone,
	# which costs a few sequential minutes per gate and makes both the
	# compile tests and their neighbours deterministic.
	set_tests_properties("${_test}" PROPERTIES TIMEOUT 900 RUN_SERIAL TRUE)
	tau_repl_disable_skipped("${_test}")
endfunction()

# `tau compile` against `run`: the program of a spec with no inputs prints
# what `run <steps> steps` prints (check_compile_matches_run.cmake).
function(add_compile_run_test test_name steps spec)
	cmake_parse_arguments(PARSE_ARGV 3 _tau "" "" "")
	set(_tau_REQUIRES subprocess)
	tau_repl_gate_case("test_repl-${test_name}" spec)
	add_test(NAME "test_repl-${test_name}"
		COMMAND ${CMAKE_COMMAND}
			"-DTAU=${TAU_LAUNCHER}" "-DSTEPS=${steps}" "-DSPEC=${spec}"
			"-DTMP=${CMAKE_BINARY_DIR}/test_repl-${test_name}.scratch"
			"-DEXE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}"
			-P "${TAU_COMPILE_RUN_CHECKER}")
	set_tests_properties("test_repl-${test_name}" PROPERTIES
		PASS_REGULAR_EXPRESSION "SAME: "
		TIMEOUT 900 RUN_SERIAL TRUE)
	tau_repl_disable_skipped("test_repl-${test_name}")
endfunction()
