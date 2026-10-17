#
# Every LTL operator under every decision command, over bv[1] streams, where
# each verdict has one right answer; the runs of R, W, T and of a liveness
# guarantee under an assumption on varying inputs; the levels of
# ltl-simplification; the witness of ltl-witness; the UNKNOWN of a timed out
# ltlsynt; and `[t+k]` under `run`.
#

# ltl_matrix(<op> <realizable> <unrealizable> <tautology>): sat, unsat,
# realizable and unrealizable of a realizable and of an unrealizable formula
# (the inputs of `sat` are universal, so `sat` agrees with `realizable`), and
# valid of a tautology and of the realizable formula, which is no tautology.
function(ltl_matrix op real unreal taut)
	foreach(_cmd sat realizable)
		add_repl_test(ltl_matrix-${op}-${_cmd}_realizable
			"${_cmd} ${real}" ": T" REQUIRES ltlsynt)
		add_repl_test(ltl_matrix-${op}-${_cmd}_unrealizable
			"${_cmd} ${unreal}" ": F" REQUIRES ltlsynt)
	endforeach()
	foreach(_cmd unsat unrealizable)
		add_repl_test(ltl_matrix-${op}-${_cmd}_realizable
			"${_cmd} ${real}" ": F" REQUIRES ltlsynt)
		add_repl_test(ltl_matrix-${op}-${_cmd}_unrealizable
			"${_cmd} ${unreal}" ": T" REQUIRES ltlsynt)
	endforeach()
	add_repl_test(ltl_matrix-${op}-valid_tautology
		"valid ${taut}" ": T" REQUIRES ltlsynt)
	add_repl_test(ltl_matrix-${op}-valid_not_tautology
		"valid ${real}" ": F" REQUIRES ltlsynt)
endfunction()

set(_o "o1[t]:bv[1] = 1")
set(_i "i1[t]:bv[1] = 1")
set(_ni "i1[t]:bv[1] = 0")

# the system may discharge U at once; a U over an input needs the environment
ltl_matrix(until "(${_i}) U (${_o})" "(${_o}) U (${_i})"
	"((${_o}) U (${_i})) -> (F(${_i}))")
# R holds while its right side holds until the left one does
ltl_matrix(release "(${_i}) R (${_o})" "(${_o}) R (${_i})"
	"(G(${_i})) -> ((${_o}) R (${_i}))")
# W over an input: once i1 fails, it must stay off, and the environment
# turns it on again
ltl_matrix(weak_until "(${_o}) W (${_i})" "(${_i}) W (G(${_ni}))"
	"(G(${_o})) -> ((${_o}) W (${_i}))")
# S and T read the past: an input since step 0 is the environment's
ltl_matrix(since "G((${_i}) S (${_o}))" "G((${_o}) S (${_i}))"
	"G((${_i}) -> ((${_o}) S (${_i})))")
ltl_matrix(trigger "G((${_i}) T (${_o}))" "G((${_o}) T (${_i}))"
	"G(((${_i}) T (${_o})) -> (${_o}))")
ltl_matrix(always_sometimes "G(F(${_o}))" "G(F(${_i}))"
	"(G(F(${_i}))) -> (F(${_i}))")
# a liveness guarantee under a fairness assumption, and the same guarantee
# without it
ltl_matrix(assumption "(G(F(${_i}))) -> (G(F(${_o} && ${_i})))"
	"G(F(${_o} && ${_i}))" "(F(${_i})) || (G(${_ni}))")

# The runs read the inputs when the strategy asks for them; the input prompt
# takes no `q`, so a run that reads every step ends with its input.
# R: o1 holds up to the first step with i1, which is step 0.
add_multiline_repl_test(ltl_matrix-run_release
	"o1\\[0\\] := 1\n"
	STDIN "run (${_i}) R (${_o}).\\n1\\nq\\nq\\n"
	FAIL_REGEX "unsat|Error" REQUIRES ltlsynt)
# W: i1 at step 0 discharges it, so o1 is free from then on
add_multiline_repl_test(ltl_matrix-run_weak_until
	"o1\\[0\\] := [01]\n"
	STDIN "run (${_o}) W (${_i}).\\n1\\nq\\nq\\n"
	FAIL_REGEX "unsat|Error" REQUIRES ltlsynt)
# T: i1 cannot break a T whose right side holds at every step
add_repl_test(ltl_matrix-run_trigger
	"run 4 steps G((${_i}) T (${_o}))."
	"o1\\[0\\] := 1.*o1\\[1\\] := 1.*o1\\[2\\] := 1.*o1\\[3\\] := 1"
	FAIL_REGEX "unsat|Error" REQUIRES ltlsynt)
# the guarantee follows the input, which is what makes it hold under the
# assumption
add_multiline_repl_test(ltl_matrix-run_assumption_follows_input
	"o1\\[0\\] := 1.*o1\\[1\\] := 0.*o1\\[2\\] := 1.*o1\\[3\\] := 1"
	STDIN "run (G(F(${_i}))) -> (G(F(${_o} && ${_i}))).\\n1\\n0\\n1\\n1\\n"
	FAIL_REGEX "unsat|Error" REQUIRES ltlsynt)
# the request of step 0 is answered by step 1
add_multiline_repl_test(ltl_matrix-run_response
	"o1\\[[01]\\] := 1\n"
	STDIN "run G((${_i}) -> F(${_o})).\\n1\\nq\\nq\\n"
	FAIL_REGEX "unsat|Error" REQUIRES ltlsynt)
add_multiline_repl_test(ltl_matrix-run_unrealizable_refused
	"not executable: it is unrealizable"
	STDIN "run (${_o}) U (${_i}).\\nq\\n"
	NO_FAIL_REGEX REQUIRES ltlsynt)

# Every level of ltl-simplification keeps a strategy that alternates o1
foreach(_level no bisim bwoa sat bisim-sat bwoa-sat)
	add_repl_test(ltl_matrix-simplification_${_level}_alternates
		"set ltl-simplification ${_level}. run 4 steps (G(F(${_o}))) && (G(F(o1[t]:bv[1] = 0)))."
		"o1\\[0\\] := ([01]).*o1\\[1\\] := ([01]).*o1\\[2\\] := ([01]).*o1\\[3\\] := ([01])"
		FAIL_REGEX "UNKNOWN|Error|o1\\[0\\] := 0.*o1\\[1\\] := 0.*o1\\[2\\] := 0.*o1\\[3\\] := 0|o1\\[0\\] := 1.*o1\\[1\\] := 1.*o1\\[2\\] := 1.*o1\\[3\\] := 1"
		REQUIRES ltlsynt)
endforeach()

# The witness of `G(o1 -> !i1) && F(o1)`: the environment keeps i1 at 1,
# that is, p0 (`i1 = 0`) false on every edge
add_repl_test(ltl_matrix-witness_keeps_input_up
	"set ltl-witness on. realizable (G (${_o} -> ${_ni})) && (F (${_o}))"
	"ENV COUNTER-STRATEGY.*\n  p0 := i1\\[t\\]:bv\\[1\\] = 0\n.*AP: [0-9]+ \"p0\".*\n\\[!0\\] 0\n--END--"
	REQUIRES ltlsynt)

# An ltlsynt that never answers: the ltl-timeout watchdog kills it, and every
# command reports UNKNOWN, none a verdict.
if(NOT CMAKE_HOST_WIN32 AND TAU_REPL_LTLSYNT_USABLE)
	set(_slow "${CMAKE_CURRENT_BINARY_DIR}/slow_ltlsynt")
	file(WRITE "${_slow}/ltlsynt"
		"#!/bin/sh\n"
		"case \"$1\" in --version) exec \"${TAU_LTLSYNT_PROGRAM}\" \"$@\";; esac\n"
		"sleep 30\n")
	file(CHMOD "${_slow}/ltlsynt" PERMISSIONS OWNER_READ OWNER_WRITE
		OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	set(_unknown "the realizability of this specification is UNKNOWN")
	foreach(_cmd realizable unrealizable)
		add_raw_repl_test(ltl_matrix-timeout_${_cmd}_unknown
			"PATH=${_slow}:$PATH ${TAU_RUN} -e \"set ltl-timeout 1. ${_cmd} F(${_o})\""
			"${_unknown}.*UNKNOWN: the synthesis backend"
			FAIL_REGEX ": [TF]\n" REQUIRES ltlsynt)
	endforeach()
	add_raw_repl_test(ltl_matrix-timeout_run_unknown
		"PATH=${_slow}:$PATH ${TAU_RUN} -e \"set ltl-timeout 1. run 2 steps F(${_o}).\""
		"${_unknown}.*no strategy was synthesised"
		FAIL_REGEX "is unrealizable|o1\\[0\\] :=" REQUIRES ltlsynt)
	foreach(_verb gen compile)
		add_raw_repl_test(ltl_matrix-timeout_${_verb}_unknown
			"d=$(mktemp -d) && printf 'G(F(${_o}))\\n' > $d/s.tau && cd $d && PATH=${_slow}:$PATH TAU_LTL_TIMEOUT=1 ${TAU_RUN} ${_verb} s.tau; r=$?; rm -rf $d; exit $r"
			"${_unknown}.*no strategy was synthesised"
			FAIL_REGEX "is unrealizable" REQUIRES ltlsynt hostfs)
	endforeach()
endif()

# `[t+k]` under `run`: G(o1[t+1] = 1) leaves o1[0] free, which the run fills
# with 0
add_repl_test(ltl_matrix-run_forward_reference_leaves_step_0_free
	"run 4 steps G(o1[t+1]:bv[1] = 1)."
	"o1\\[0\\] := 0.*o1\\[1\\] := 1.*o1\\[2\\] := 1.*o1\\[3\\] := 1"
	FAIL_REGEX "unsat|Error" REQUIRES ltlsynt)
add_repl_test(ltl_matrix-run_forward_reference_two_steps
	"run 4 steps always o1[t+2]:bv[1] = 1."
	"o1\\[0\\] := 0.*o1\\[1\\] := 0.*o1\\[2\\] := 1.*o1\\[3\\] := 1"
	FAIL_REGEX "unsat|Error")
