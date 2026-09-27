#
# `tau gen <spec.tau>... [-o <dir>]` cases: the codegen half of `tau compile`
# emits the artifact and stops before any build. Each case is a CMake script,
# so it needs no shell.
#
include(tau_repl_pack)

set(TAU_GEN_VERB_CHECKER "${CMAKE_CURRENT_LIST_DIR}/../check_gen_verb.cmake")

# add_gen_test(<name> CASE <case> SPEC <spec> [SPEC2 <spec2>]
#     [REQUIRES ltlsynt|hostfs|<ba-id> ...])
function(add_gen_test test_name)
	cmake_parse_arguments(PARSE_ARGV 1 _tau "" "CASE;SPEC;SPEC2" REQUIRES)
	set(_test "test_repl-gen_verb-${test_name}")
	# `tau gen` reads a host spec file and writes beside it, which a wasm
	# node host cannot do.
	list(APPEND _tau_REQUIRES subprocess)
	file(READ "${_tau_SPEC}" _spec_src)
	if(_tau_SPEC2)
		file(READ "${_tau_SPEC2}" _spec2_src)
		string(APPEND _spec_src "${_spec2_src}")
	endif()
	tau_repl_gate_case("${_test}" _spec_src)
	set(_args "-DTAU=${TAU_LAUNCHER}" "-DSPEC=${_tau_SPEC}" "-DCASE=${_tau_CASE}"
		"-DTMP=${CMAKE_BINARY_DIR}/${_test}.scratch")
	if(_tau_SPEC2)
		list(APPEND _args "-DSPEC2=${_tau_SPEC2}")
	endif()
	add_test(NAME "${_test}"
		COMMAND ${CMAKE_COMMAND} ${_args} -P "${TAU_GEN_VERB_CHECKER}")
	set_tests_properties("${_test}" PROPERTIES TIMEOUT 300 RUN_SERIAL TRUE)
	tau_repl_disable_skipped("${_test}")
endfunction()

add_gen_test(one_spec CASE one
	SPEC "${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
	REQUIRES hostfs)

add_gen_test(two_specs CASE two
	SPEC "${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
	SPEC2 "${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo_dot.tau"
	REQUIRES hostfs)

# -o names one directory, so it cannot serve two specs.
add_gen_test(output_with_two_specs CASE two_with_output
	SPEC "${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
	SPEC2 "${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo_dot.tau"
	REQUIRES hostfs)

# The artifact resolves the SDK by package name, never by a baked path.
add_gen_test(cmake_names_tau_package CASE cmake_names
	SPEC "${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
	REQUIRES hostfs)

# `tau gen -` reads the spec from stdin and writes a.build/.
add_gen_test(stdin_default_dir CASE stdin
	SPEC "${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
	REQUIRES hostfs)

# '-' names stdin, which can be drained once; a second '-' is an error.
add_gen_test(stdin_twice_is_error CASE stdin_twice
	SPEC "${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
	REQUIRES hostfs)

# -o refuses a non-empty directory that is not an artifact, and accepts one
# that carries the marker a previous `tau gen` wrote.
add_gen_test(refuse_nonempty_dir CASE nonempty_dir
	SPEC "${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
	REQUIRES hostfs)

add_gen_test(reuse_artifact_dir CASE reuse_dir
	SPEC "${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
	REQUIRES hostfs)
