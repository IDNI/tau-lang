# `tau compile <spec.tau>` end-to-end test: gated like the REPL suite on the
# spec's BA content, since it drives the same binary.

# Resolved at include time (see add_strategy_export_test.cmake for why).
set(TAU_COMPILE_VERB_CHECKER "${CMAKE_CURRENT_LIST_DIR}/check_compile_verb.cmake")
set(TAU_COMPILE_RUN_CHECKER "${CMAKE_CURRENT_LIST_DIR}/check_compile_matches_run.sh")

# add_compile_test(<name> <spec_file> [CHECKER <script>] [PASS_REGEX <re>]
#     [REQUIRES ltlsynt|hostfs|<ba-id> ...])
#
# Runs the CHECKER script, check_compile_verb.cmake by default, with
# `cmake -P` over <spec_file>. The exit code of the script is the verdict,
# and PASS_REGEX adds a pattern the output must match.
function(add_compile_test test_name spec_file)
	cmake_parse_arguments(PARSE_ARGV 2 _tau "" "CHECKER;PASS_REGEX" REQUIRES)
	set(_test "test_repl-${test_name}")
	# `tau compile` emits a CMake project and spawns a host compiler
	# (src/tau_compile.tmpl.h), which a wasm node host cannot run.
	list(APPEND _tau_REQUIRES subprocess)
	file(READ "${spec_file}" _spec_src)
	tau_repl_gate_case("${_test}" _spec_src)
	if(NOT _tau_CHECKER)
		set(_tau_CHECKER "${TAU_COMPILE_VERB_CHECKER}")
	endif()
	add_test(NAME "${_test}"
		COMMAND ${CMAKE_COMMAND} "-DTAU=${TAU_LAUNCHER}" "-DSPEC=${spec_file}"
			"-DTMP=${CMAKE_BINARY_DIR}/${_test}.scratch"
			"-DEXE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}" -P "${_tau_CHECKER}")
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
# what `run <steps> steps` prints (check_compile_matches_run.sh).
function(add_compile_run_test test_name steps spec)
	tau_repl_unsupported(_tau_skip "${spec}")
	if(_tau_skip)
		tau_repl_record_skip("${test_name}")
		return()
	endif()
	add_test(NAME "test_repl-${test_name}"
		COMMAND bash "${TAU_COMPILE_RUN_CHECKER}"
			"$<TARGET_FILE:${TAU_EXECUTABLE_NAME}>" "${steps}" "${spec}")
	set_tests_properties("test_repl-${test_name}" PROPERTIES
		PASS_REGULAR_EXPRESSION "SAME: "
		TIMEOUT 900 RUN_SERIAL TRUE)
endfunction()
