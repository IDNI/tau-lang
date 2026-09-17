# `tau compile <spec.tau>` end-to-end test: gated like the REPL suite on the
# spec's BA content, since it drives the same binary.

# Resolved at include time (see add_strategy_export_test.cmake for why).
set(TAU_COMPILE_VERB_CHECKER "${CMAKE_CURRENT_LIST_DIR}/check_compile_verb.sh")

function(add_compile_test test_name spec_file)
	file(READ "${spec_file}" _spec_src)
	tau_repl_unsupported(_tau_skip "${_spec_src}")
	if(_tau_skip)
		tau_repl_record_skip("${test_name}")
		return()
	endif()
	add_test(NAME "test_repl-${test_name}"
		COMMAND bash "${TAU_COMPILE_VERB_CHECKER}"
			"$<TARGET_FILE:${TAU_EXECUTABLE_NAME}>" "${spec_file}")
	# Compiling a nested cmake project takes real time beyond a REPL
	# round-trip: 150-175 s alone on a 32-thread machine. Under `ctest -j`
	# three of these nested builds ran at once beside a dozen other tests
	# and not only exceeded their own cap but starved the ltlsynt-heavy
	# suites into watchdog kills. RUN_SERIAL runs each compile test alone,
	# which costs a few sequential minutes per gate and makes both the
	# compile tests and their neighbours deterministic.
	set_tests_properties("test_repl-${test_name}" PROPERTIES
		TIMEOUT 900 RUN_SERIAL TRUE)
endfunction()
