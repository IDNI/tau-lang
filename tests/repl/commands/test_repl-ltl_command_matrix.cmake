#
# The LTL verdicts test_repl-ltl_operator_matrix.cmake leaves out: specs of
# other shapes than its one formula per operator, over the default type (a
# response, an input-only trigger, U against W, contradictions under sat),
# runs fed inputs that change at every step, and the verdict of a spec whose
# abstraction is decided but whose data game times out. Tau quantifies the
# inputs universally.
#

include(add_repl_test)

function(add_ltl_matrix_test name cmd expected)
	add_repl_test(ltl_matrix-${name} "${cmd}" "${expected}" REQUIRES ltlsynt)
endfunction()

# unrealizable
add_ltl_matrix_test(unrealizable_weak_until_kept_forever
	"unrealizable (o1[t] = 0) weak_until (i1[t] = 0 && o1[t] != 0)" ": F")
add_ltl_matrix_test(unrealizable_trigger_of_input
	"unrealizable always ((i1[t] = 0) trigger (i1[t] != 0))" ": T")
add_ltl_matrix_test(unrealizable_response
	"unrealizable always ((i1[t] = 0) -> (sometimes (o1[t] = 0)))" ": F")
add_ltl_matrix_test(unrealizable_response_on_input
	"unrealizable always ((i1[t] = 0) -> (sometimes (i1[t] != 0)))" ": T")

# valid
add_ltl_matrix_test(valid_weak_until
	"valid (o1[t] != 0) weak_until (o1[t] = 0)" ": T")
add_ltl_matrix_test(valid_until_is_strong
	"valid (o1[t] != 0) until (o1[t] = 0)" ": F")
add_ltl_matrix_test(valid_sometimes_not_always
	"valid (sometimes (o1[t] = 0)) -> (always (o1[t] = 0))" ": F")
add_ltl_matrix_test(valid_since_needs_no_right_side_now
	"valid always (((o1[t] = 0) since (i1[t] = 0)) -> (i1[t] = 0))" ": F")
add_ltl_matrix_test(valid_trigger_needs_the_past
	"valid always ((o1[t] = 0) -> ((i1[t] = 0) trigger (o1[t] = 0)))" ": F")
add_ltl_matrix_test(valid_sometimes_not_always_sometimes
	"valid (sometimes (o1[t] = 0)) -> (always (sometimes (o1[t] = 0)))" ": F")

# sat
add_ltl_matrix_test(sat_both_values_recur
	"sat (always (sometimes (o1[t] = 0))) && (always (sometimes (o1[t] != 0)))" ": T")
add_ltl_matrix_test(sat_recurrence_against_persistence
	"sat (always (sometimes (o1[t] = 0))) && (sometimes (always (o1[t] != 0)))" ": F")
add_ltl_matrix_test(sat_release_against_always
	"sat (always (o1[t] = 0)) && ((o1[t] != 0) release (o1[t] != 0))" ": F")
add_ltl_matrix_test(sat_weak_until_kept_forever
	"sat (always (o1[t] != 0)) && ((o1[t] != 0) weak_until (o1[t] = 0))" ": T")
add_ltl_matrix_test(sat_weak_until_left_side_broken
	"sat (always (o1[t] = 0)) && (always (o2[t] != 0)) && ((o1[t] != 0) weak_until (o2[t] = 0))"
	": F")

# run, fed inputs that change
function(add_ltl_matrix_run_test name input pass fail)
	add_multiline_repl_test(ltl_matrix-${name} "${pass}"
		STDIN "${input}" FAIL_REGEX "${fail}" REQUIRES ltlsynt)
endfunction()

# o1 holds through the step where i1 releases it
add_ltl_matrix_run_test(run_release
	"run (i1[t]:bv[1] = 1) release (o1[t]:bv[1] = 1).\\n0\\n0\\n1\\n0\\nq\\nq\\n"
	"o1\\[0\\] := 1\n.*o1\\[1\\] := 1\n.*o1\\[2\\] := 1"
	"o1\\[[012]\\] := 0|unsat|not executable")
add_ltl_matrix_run_test(run_weak_until
	"run (o1[t]:bv[1] = 1) weak_until (i1[t]:bv[1] = 1).\\n0\\n0\\n1\\n0\\nq\\nq\\n"
	"o1\\[0\\] := 1\n.*o1\\[1\\] := 1"
	"o1\\[[01]\\] := 0|unsat|not executable")
add_ltl_matrix_run_test(run_trigger
	"run G((i1[t]:bv[1] = 1) trigger (o1[t]:bv[1] = 1)).\\na\\na\\na\\nq\\nq\\n"
	"o1\\[2\\] := 1"
	"o1\\[[0-9]+\\] := 0|unsat|not executable")
# the response can only be given at the step that asks it
add_ltl_matrix_run_test(run_response_follows_input
	"run G((i1[t]:bv[1] = 1) -> F(o1[t]:bv[1] = 1 && i1[t]:bv[1] = 1)).\\n1\\n0\\n0\\n1\\nq\\nq\\n"
	"o1\\[0\\] := 1\n.*o1\\[3\\] := 1"
	"o1\\[[03]\\] := 0|unsat|not executable")

# A spec no check decides within the ltl-timeout is UNKNOWN, not F: the stub
# answers UNREALIZABLE for the abstraction and outlasts the timeout on the
# data game (tests/repl/stubs/slow_game/ltlsynt.cpp), and the input atom
# reads two steps. `run` refuses it without a verdict.
if(TARGET tau_stub_slow_game_ltlsynt)
	set(_slow "TAU_LTL_TIMEOUT=1 TAU_SPOT_BIN= TAU_SHARED_PREFIX= USERPROFILE=/no-home HOME=/no-home PATH=${TAU_SLOW_GAME_STUB_DIR}:$PATH")
	foreach(cmd realizable unrealizable)
		add_raw_repl_test(ltl_matrix-${cmd}_undecided_is_unknown
			"${_slow} ${TAU_RUN} -e \"${cmd} F (o1[t] = 1 && i1[t] != i1[t-1])\""
			"UNKNOWN" FAIL_REGEX "%1: [TF]|Aborted|core dumped" TIMEOUT 60)
	endforeach()
	add_raw_repl_test(ltl_matrix-run_undecided_gives_no_verdict
		"printf 'run F (o1[t] = 1 && i1[t] != i1[t-1]).\\n0\\n1\\nq\\nq\\n' | ${_slow} ${TAU_RUN}"
		"not executable: no strategy"
		FAIL_REGEX "unsat|unrealizable|:= |Aborted|core dumped" TIMEOUT 60)
endif()
