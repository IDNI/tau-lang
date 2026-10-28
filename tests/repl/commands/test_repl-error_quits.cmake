#
# error-quits: a line whose command fails ends tau with exit code 1
#

include(add_repl_test)

# add_error_quits_test(<name> <cmd> <error regex>)
#
# Runs `tau --error-quits -e "<cmd>"` and passes when tau exits with 1 after
# printing the error. POSIX only.
function(add_error_quits_test test_name test_cmd error_regex)
	add_raw_repl_test(error_quits-${test_name}
		"out=$(${TAU_RUN} --error-quits -e \"${test_cmd}\" 2>&1); echo \"exit=$? $out\""
		"^exit=1 .*${error_regex}" NO_FAIL_REGEX ${ARGN})
endfunction()

# A definition that never reaches a normal form fails each verdict command.
set(_osc "f(x) := f(x)'.")
set(_osc_error "definition expansion oscillates")
add_error_quits_test(sat "${_osc} sat f(1) = 0" "${_osc_error}")
add_error_quits_test(valid "${_osc} valid f(1) = 0" "${_osc_error}")
add_error_quits_test(unsat "${_osc} unsat f(1) = 0" "${_osc_error}")
add_error_quits_test(realizable "${_osc} realizable f(1) = 0" "${_osc_error}")
add_error_quits_test(unrealizable "${_osc} unrealizable f(1) = 0" "${_osc_error}")
add_error_quits_test(normalize "${_osc} n f(1) = 0" "${_osc_error}")
add_error_quits_test(unrealizable_core "${_osc} unrealizable_core f(1) = 0"
	"has no core")
add_error_quits_test(explain "${_osc} explain f(1) = 0" "has no core")
add_error_quits_test(run_setup "${_osc} run f(1) = 0" "${_osc_error}")
add_error_quits_test(run_no_session "run" "no run to continue")

# A normal form command that fails, here onf of a term.
add_error_quits_test(onf "onf x x" "Invalid formula")

add_error_quits_test(qelim "qelim x" "Invalid formula")
add_error_quits_test(solve "solve x" "Invalid formula")
add_error_quits_test(lgrs "lgrs x" "Invalid formula")
add_error_quits_test(subst_duplicate "s x | y [x / a, x / b]" "Duplicate match")
add_error_quits_test(subst_ill_typed "s x:sbf & y:sbf = 0 [x:sbf / z:bv[16]]"
	"not well-typed")
add_error_quits_test(history_missing "sat %5" "History location does not exist")

# A line that succeeds exits with 0, and without error-quits a failing line
# does too.
add_raw_repl_test(error_quits-success
	"out=$(${TAU_RUN} --error-quits -e \"sat T\" 2>&1); echo \"exit=$? $out\""
	"^exit=0 " NO_FAIL_REGEX)
add_raw_repl_test(error_quits-off
	"out=$(${TAU_RUN} -e \"qelim x\" 2>&1); echo \"exit=$? $out\""
	"^exit=0 .*Invalid formula" NO_FAIL_REGEX)

# In a session, the failing line ends the REPL before the next one runs.
add_multiline_repl_test(error_quits-session "Invalid formula"
	"set error-quits true" "qelim x" "sat T"
	FAIL_REGEX "%1[^:]*: T")
