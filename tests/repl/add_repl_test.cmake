include(tau_repl_pack)

# The options of every helper here that checks the output of tau.
set(TAU_REPL_CHECK_OPTIONS NO_FAIL_REGEX)
set(TAU_REPL_CHECK_ONE_VALUE TIMEOUT)
set(TAU_REPL_CHECK_MULTI_VALUE ENV FAIL_REGEX REQUIRES)

# The epilogue of a helper that checks the output of tau, over the _tau_<OPTION>
# values its cmake_parse_arguments sets. The fail pattern is "Error" unless
# FAIL_REGEX replaces it or NO_FAIL_REGEX drops it.
function(tau_repl_check_case test regex)
	if(_tau_FAIL_REGEX AND _tau_NO_FAIL_REGEX)
		message(FATAL_ERROR "${test}: FAIL_REGEX and NO_FAIL_REGEX exclude each other")
	endif()
	if(NOT "${regex}" STREQUAL "")
		set_tests_properties("${test}" PROPERTIES PASS_REGULAR_EXPRESSION "${regex}")
	endif()
	if(_tau_FAIL_REGEX)
		set_tests_properties("${test}" PROPERTIES FAIL_REGULAR_EXPRESSION "${_tau_FAIL_REGEX}")
	elseif(NOT _tau_NO_FAIL_REGEX)
		set_tests_properties("${test}" PROPERTIES FAIL_REGULAR_EXPRESSION "Error")
	endif()
	if(_tau_TIMEOUT)
		set_tests_properties("${test}" PROPERTIES TIMEOUT "${_tau_TIMEOUT}")
	endif()
	# The browser suite reads the node build's registration, where a ltlsynt
	# or hostfs case is present, so the property carries its skip over. It
	# cannot reproduce the process environment of an ENV case either.
	set(_browser_skip "")
	if(_tau_ENV)
		set_tests_properties("${test}" PROPERTIES ENVIRONMENT "${_tau_ENV}")
		list(APPEND _browser_skip env)
	endif()
	foreach(_need ${_tau_REQUIRES})
		if(_need MATCHES "^(ltlsynt|hostfs)$")
			list(APPEND _browser_skip ${_need})
		endif()
	endforeach()
	if(_browser_skip)
		set_tests_properties("${test}" PROPERTIES TAU_BROWSER_SKIP "${_browser_skip}")
	endif()
	tau_repl_disable_skipped("${test}")
endfunction()

# Case strings escape quotes for `bash -c "..."` (`\"`). A direct argv must
# see bare quotes, or Tau parses `file(\"` as a backslash.
function(tau_repl_unescape_quotes out cmd)
	string(REPLACE "\\\"" "\"" _cmd "${cmd}")
	set(${out} "${_cmd}" PARENT_SCOPE)
endfunction()

# The host picks the shell of a case: a Windows target built on Linux runs its
# cases through bash and TAU_RUN, so the emulator starts tau.exe.

# add_repl_test(<name> <cmd> <regex> [FLAGS <arg>...] [NO_TRACE]
#     [ENV <VAR=value>...] [TIMEOUT <sec>] [FAIL_REGEX <re>] [NO_FAIL_REGEX]
#     [REQUIRES ltlsynt|hostfs|<ba-id> ...])
#
# Runs `tau <flags> -e "<cmd>" -S trace`. NO_TRACE drops `-S trace`.
function(add_repl_test test_name test_cmd test_regex)
	cmake_parse_arguments(PARSE_ARGV 3 _tau "NO_TRACE;${TAU_REPL_CHECK_OPTIONS}"
		"${TAU_REPL_CHECK_ONE_VALUE}" "FLAGS;${TAU_REPL_CHECK_MULTI_VALUE}")
	tau_repl_gate_case("test_repl-${test_name}" test_cmd)
	set(_trace -S trace)
	if(_tau_NO_TRACE)
		set(_trace "")
	endif()
	if(CMAKE_HOST_WIN32)
		tau_repl_unescape_quotes(_cmd "${test_cmd}")
		add_test(NAME "test_repl-${test_name}"
			COMMAND ${TAU_LAUNCHER} ${_tau_FLAGS} -e "${_cmd}" ${_trace})
	else()
		string(JOIN " " _line "${TAU_RUN}" ${_tau_FLAGS} -e "\"${test_cmd}\"" ${_trace})
		add_test(NAME "test_repl-${test_name}" COMMAND bash -c "${_line}")
	endif()
	tau_repl_check_case("test_repl-${test_name}" "${test_regex}")
endfunction()

# `test_cmd` asks a bitvector question that neither the bits nor cvc5 decide
# within a budget of 3 seconds (--bv-solve-timeout): the answer is UNKNOWN,
# naming the budget, and never a verdict.
function(add_repl_budget_test test_name test_cmd)
	add_repl_test(${test_name} "${test_cmd}"
		"UNKNOWN: a bitvector question passed its time budget [(]bv-solve-timeout, 3 s"
		FLAGS --bv-solve-timeout 3 NO_TRACE
		FAIL_REGEX "%[0-9]+: [TF]" TIMEOUT 120)
endfunction()

# add_echo_repl_test(<name> <cmd> <regex> [ENV <VAR=value>...]
#     [TIMEOUT <sec>] [FAIL_REGEX <re>] [NO_FAIL_REGEX]
#     [REQUIRES ltlsynt|hostfs|<ba-id> ...])
#
# Pipes the single line "<cmd>. q" into the interactive REPL.
function(add_echo_repl_test test_name test_cmd test_regex)
	cmake_parse_arguments(PARSE_ARGV 3 _tau "${TAU_REPL_CHECK_OPTIONS}"
		"${TAU_REPL_CHECK_ONE_VALUE}" "${TAU_REPL_CHECK_MULTI_VALUE}")
	tau_repl_gate_case("test_repl-${test_name}" test_cmd)
	if(CMAKE_HOST_WIN32)
		tau_repl_unescape_quotes(_cmd "${test_cmd}")
		add_test(NAME "test_repl-${test_name}"
			COMMAND powershell -NoProfile -Command
				"& { '${_cmd}. q' | & '${TAU_LAUNCHER}' }")
	else()
		add_test(NAME "test_repl-${test_name}"
			COMMAND bash -c "echo \"${test_cmd}. q\" | ${TAU_RUN}")
	endif()
	tau_repl_check_case("test_repl-${test_name}" "${test_regex}")
endfunction()

# add_multiline_repl_test(<name> <regex> <line1> [<line2> ...]
#     [STDIN <printf-payload>] [FLAGS <arg>...] [NO_X] [X_FIRST]
#     [ENV <VAR=value>...] [TIMEOUT <sec>] [FAIL_REGEX <re>] [NO_FAIL_REGEX]
#     [REQUIRES ltlsynt|hostfs|<ba-id> ...])
#
# Pipes each line argument into `tau <flags> -X` as one REPL line, then a
# trailing `q`, so a later line sees the effect of an earlier one. A case
# that needs its exact stdin bytes passes STDIN instead of lines. NO_X drops
# the legacy-REPL `-X` for a case that reads a spec from stdin, and X_FIRST
# puts `-X` before the flags.
function(add_multiline_repl_test test_name test_regex)
	cmake_parse_arguments(PARSE_ARGV 2 _tau "NO_X;X_FIRST;${TAU_REPL_CHECK_OPTIONS}"
		"STDIN;${TAU_REPL_CHECK_ONE_VALUE}" "FLAGS;${TAU_REPL_CHECK_MULTI_VALUE}")
	if(_tau_STDIN)
		set(_payload "${_tau_STDIN}")
	else()
		string(REPLACE ";" "\\n" _payload "${_tau_UNPARSED_ARGUMENTS}\\nq\\n")
		set(_tau_UNPARSED_ARGUMENTS "")
	endif()
	tau_repl_gate_case("test_repl-${test_name}" _payload)
	set(_args ${_tau_FLAGS})
	if(NOT _tau_NO_X)
		if(_tau_X_FIRST)
			list(PREPEND _args -X)
		else()
			list(APPEND _args -X)
		endif()
	endif()
	if(CMAKE_HOST_WIN32)
		# PowerShell needs real newlines. It doubles a single quote inside a
		# single-quoted string.
		string(REPLACE "\\n" "\n" _ps_stdin "${_payload}")
		string(REPLACE "'" "''" _ps_stdin "${_ps_stdin}")
		string(JOIN " " _ps_args ${_args})
		add_test(NAME "test_repl-${test_name}"
			COMMAND powershell -NoProfile -Command
				"& { '${_ps_stdin}' | & '${TAU_LAUNCHER}' ${_ps_args} }")
	else()
		string(JOIN " " _line "printf '${_payload}' |" "${TAU_RUN}" ${_args})
		add_test(NAME "test_repl-${test_name}" COMMAND bash -c "${_line}")
	endif()
	tau_repl_check_case("test_repl-${test_name}" "${test_regex}")
endfunction()

# add_raw_repl_test(<name> <command> <regex> [ENV <VAR=value>...]
#     [TIMEOUT <sec>] [FAIL_REGEX <re>] [NO_FAIL_REGEX]
#     [REQUIRES ltlsynt|hostfs|<ba-id> ...])
#
# Runs `bash -c "<command>"`, for a case whose command line is not the shape
# the other helpers build. An empty <regex> sets no pass pattern. POSIX only.
function(add_raw_repl_test test_name command test_regex)
	cmake_parse_arguments(PARSE_ARGV 3 _tau "${TAU_REPL_CHECK_OPTIONS}"
		"${TAU_REPL_CHECK_ONE_VALUE}" "${TAU_REPL_CHECK_MULTI_VALUE}")
	tau_repl_gate_case("test_repl-${test_name}" command)
	add_test(NAME "test_repl-${test_name}" COMMAND bash -c "${command}")
	tau_repl_check_case("test_repl-${test_name}" "${test_regex}")
endfunction()

# add_env_error_test(<name> <VAR=value> [REQUIRES <ba-id> ...])
#
# A bad value of an option variable is an error: tau prints it and exits
# with 1 before it runs a command. The error names the variable. POSIX only.
function(add_env_error_test test_name env)
	string(FIND "${env}" "=" _eq)
	string(SUBSTRING "${env}" 0 ${_eq} _var)
	add_raw_repl_test(${test_name}
		"out=$(${TAU_RUN} -e 'get ltl-timeout' 2>&1); echo \"exit=$? $out\""
		"exit=1 .*The option does not take the value of the environment variable \\(name=${_var} value="
		NO_FAIL_REGEX ENV ${env} ${ARGN})
endfunction()
