#
# IN-R1 / LA-N2 / IN-N13 regressions through the REPL (api-mediated path).
#
# Before the fix, normalize() rewrote every topmost non-always temporal
# operator into `sometimes(first operand)`: binary operators lost their right
# operand, A/E were erased (nesting a temporal quantifier and aborting the
# Debug REPL on an assert), and `F` lost its realizability semantics.
#

include(add_repl_test)

# the right operand of U survives normalization
add_repl_test(ctl_star-normalize_keeps_U_rhs
	"normalize (o1[t] = 1) until (o1[t] = 0)" "U")
add_repl_test(ctl_star-normalize_U_not_sometimes
	"normalize (o1[t] = 1) until (o1[t] = 0)"
	"%1" NO_TRACE FAIL_REGEX "sometimes")

# U's target is a contradiction: normalization folds it to F and φ U F = F
# (the target used to be dropped, answering T)
add_repl_test(ctl_star-sat_U_contradictory_target
	"sat (o1[t] = 1) until (o1[t] = 0 && o1[t] = 1)" ": F" REQUIRES ltlsynt)

# F over an input is unrealizable: the environment can keep i1 at 0.
add_repl_test(ctl_star-realizable_F_input_unrealizable "realizable F i1[t] = 1" ": F" REQUIRES ltlsynt)
add_repl_test(ctl_star-sat_F_output_realizable "sat F o1[t] = 1" ": T")

# crash regressions: these aborted the Debug REPL (normalizer
# !find_top(wff_always) / satisfiability st.size() < 2 asserts)
add_repl_test(ctl_star-sat_A_always_input_no_abort
	"fragment ctl_star. sat A (always i1[t] = 1)" ": F")
add_repl_test(ctl_star-sat_E_always_output_no_abort
	"fragment ctl_star. sat E (always o1[t] = 1)" ": T" REQUIRES ltlsynt)
add_repl_test(ctl_star-sat_A_sometimes_contradiction_no_abort
	"fragment ctl_star. sat A (sometimes (o1[t] = 1 && o1[t] = 0))" ": F" REQUIRES ltlsynt)
# validity over computation trees has no procedure: UNKNOWN, never a verdict
# (it used to answer F for the tautology A G p -> A F p)
add_repl_test(ctl_star-valid_A_always_no_abort
	"fragment ctl_star. valid A (always o1[t] = 1)" "cannot be decided" NO_FAIL_REGEX)
add_repl_test(ctl_star-valid_tautology_not_F
	"fragment ctl_star. valid (A (always o1[t] = 1)) -> (A (F o1[t] = 1))"
	"cannot be decided" NO_TRACE FAIL_REGEX "%1: F")

# CTL* verdicts through the api path (IN-R7): A constrains its body.
# realizable decides realizability; sat of A χ agrees with sat of χ.
add_repl_test(ctl_star-realizable_A_F_input
	"fragment ctl_star. realizable A (F i1[t] = 1)" ": F" REQUIRES ltlsynt)
add_repl_test(ctl_star-sat_A_F_input_like_its_body
	"fragment ctl_star. sat A (F i1[t] = 1)" ": F")
add_repl_test(ctl_star-sat_A_F_output "fragment ctl_star. sat A (F o1[t] = 1)" ": T" REQUIRES ltlsynt)
add_repl_test(ctl_star-realizable_A_always_output
	"fragment ctl_star. realizable A (always o1[t] = 1)" ": T")
add_repl_test(ctl_star-realizable_E_F_output
	"fragment ctl_star. realizable E (F o1[t] = 1)" ": T" REQUIRES ltlsynt)
# direction outputs pin the witness path, so E over inputs is decided
add_repl_test(ctl_star-realizable_E_F_input
	"fragment ctl_star. realizable E (F i1[t] = 1)" ": T" REQUIRES ltlsynt)
# the witness state has read its input: no branch from it keeps i1 at both
# 1 and 0; decided on codes within the round cap of a game over formulas
add_repl_test(ctl_star-realizable_E_always_input_both_values
	"fragment ctl_star. set ltlrefinementrounds 4. realizable (E (always i1[t] = 1)) && (E (always i1[t] = 0))" ": F" REQUIRES ltlsynt)
# a past operator under E keeps the all-paths encoding: undecided, not F
add_repl_test(ctl_star-realizable_E_since_undecided
	"fragment ctl_star. realizable E ((i1[t] = 1) since (i1[t] = 0))"
	"could not be decided" NO_FAIL_REGEX)
# negative A / E are decided through their duals
add_repl_test(ctl_star-sat_neg_A_through_dual
	"fragment ctl_star. sat !(A (F o1[t] = 1))" ": T" REQUIRES ltlsynt)
# semantic negation in a Boolean context is the negated realizability
add_repl_test(ctl_star-sat_semneg_unrealizable_body
	"fragment ctl_star. sat -(F i1[t] = 1)" ": T" REQUIRES ltlsynt)
add_repl_test(ctl_star-sat_semneg_realizable_body
	"fragment ctl_star. sat -(always o1[t] = 1)" ": F")
# a past-free -ψ under a temporal operator is the same constant
add_repl_test(ctl_star-sat_semneg_under_G
	"fragment ctl_star. sat G (-(F i1[t] = 1))" ": T" REQUIRES ltlsynt)
add_repl_test(ctl_star-sat_always_A_output
	"fragment ctl_star. sat always (A (o1[t] = 1))" ": T")
# unsound placements are refused with a diagnostic, not answered
add_repl_test(ctl_star-sat_A_under_F_refused
	"fragment ctl_star. sat F (A (o1[t] = 1))" "not soundly encodable" NO_FAIL_REGEX)

# ── Batch 2: gates, skeleton refusal, exception safety ─────────────────────

# IN-N5: ltl / valid / qelim are fragment-gated like sat/normalize/run
add_repl_test(fragment_gate-ltl_blocks_A_by_default
	"ltl A (F o1[t] = 1)" "require the ctl_star" NO_FAIL_REGEX)
add_repl_test(fragment_gate-valid_blocks_A_by_default
	"valid A (always o1[t] = 1)" "require the ctl_star" NO_FAIL_REGEX)
add_repl_test(fragment_gate-qelim_blocks_A_by_default
	"qelim A (o1[t] = 1)" "require the ctl_star" NO_FAIL_REGEX)
# (definition bodies conjoined by `run` are gated in run_cmd as well, but a
# CTL*-bodied definition is already rejected at definition time, so that
# gate is defense-in-depth with no black-box reproducer.)

# IN-R3 / LA-M3: in the ctl_star fragment, `ltl` reduces A and reports an
# undecided E, instead of printing "skeleton: 1" REALIZABLE (P3 / P10)
add_repl_test(ltl_cmd-ctl_star_A_F_output_reduced
	"fragment ctl_star. ltl A (F o1[t] = 1)" "CTL\\* reduced to LTL" REQUIRES ltlsynt)
add_repl_test(ltl_cmd-ctl_star_A_F_input_unrealizable
	"fragment ctl_star. ltl A (F i1[t] = 1)" "UNREALIZABLE" NO_FAIL_REGEX REQUIRES ltlsynt)
add_repl_test(ltl_cmd-ctl_star_e_over_past_undecided
	"fragment ctl_star. ltl E ((i1[t] = 1) since (i1[t] = 0))" "UNKNOWN" NO_FAIL_REGEX)
add_repl_test(ltl_cmd-ctl_star_e_over_past_not_realizable
	"fragment ctl_star. ltl E ((i1[t] = 1) since (i1[t] = 0))"
	"UNKNOWN" NO_TRACE FAIL_REGEX "skeleton: 1|^REALIZABLE")

# IN-R4 / IN-RT7: a backend that fails (usage/internal error, exit 2) must
# produce a diagnostic and a live REPL, not a dead process; the second
# command proves the REPL survived.
add_raw_repl_test(ltl_cmd-backend_failure_is_unknown
	"TAU_SPOT_BIN= TAU_SHARED_PREFIX= USERPROFILE=/no-home HOME=/no-home PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs:$PATH ${TAU_RUN} -e \"ltl F o1[t] = 1. sat always o1[t] = 1\""
	"UNKNOWN[^\n]*\n.*: T" FAIL_REGEX "REALIZABLE|Aborted|core dumped")

# ── Batch 3: error ≠ verdict ───────────────────────────────────────────────

# IN-N1: no Spot on PATH is UNKNOWN, not "UNREALIZABLE (propositional)"
# tau-run starts wine from PATH. tau.exe under wine cannot start a host
# ltlsynt, so the directory of wine hides no Spot from the case.
set(_no_spot_path /nonexistent-dir-without-spot)
if(CMAKE_CROSSCOMPILING_EMULATOR AND WIN32 AND NOT CMAKE_HOST_WIN32)
	list(GET CMAKE_CROSSCOMPILING_EMULATOR 0 _emulator)
	find_program(_emulator_path "${_emulator}" NO_CACHE)
	if(_emulator_path)
		cmake_path(GET _emulator_path PARENT_PATH _emulator_dir)
		string(APPEND _no_spot_path ":${_emulator_dir}")
	endif()
endif()
add_raw_repl_test(ltl_cmd-no_spot_is_unknown
	"TAU_SPOT_BIN= TAU_SHARED_PREFIX= USERPROFILE=/no-home HOME=/no-home PATH=${_no_spot_path} ${TAU_RUN} -e \"ltl F o1[t] = 1\""
	"UNKNOWN" FAIL_REGEX "UNREALIZABLE|Aborted|core dumped")

# LA-8: a REALIZABLE line followed by a garbled HOA must not become a
# REALIZABLE verdict with the oracle skipped (P4)
add_raw_repl_test(sat-garbled_hoa_is_unknown
	"TAU_SPOT_BIN= TAU_SHARED_PREFIX= USERPROFILE=/no-home HOME=/no-home PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs/garbled:$PATH ${TAU_RUN} -e \"fragment ctl_star. sat E (F o1[t] = 1)\""
	"UNKNOWN" FAIL_REGEX ": T|Aborted|core dumped")

# SY-R4: exit 0 with no verdict line is UNKNOWN, not UNREALIZABLE
add_raw_repl_test(ltl_cmd-garbage_output_is_unknown
	"TAU_SPOT_BIN= TAU_SHARED_PREFIX= USERPROFILE=/no-home HOME=/no-home PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs/garbage0:$PATH ${TAU_RUN} -e \"ltl F o1[t] = 1\""
	"UNKNOWN" FAIL_REGEX "UNREALIZABLE|Aborted|core dumped")

# SY-R1: realizability classifies backend failures too. The environment
# prefix keeps the case on bash rather than the ENVIRONMENT property, so it
# registers through the raw helper; REQUIRES qlt carries the pack gate the
# command text used to name by hand.
add_raw_repl_test(realizable-no_verdict_is_unknown
	"TAU_SPOT_BIN= TAU_SHARED_PREFIX= USERPROFILE=/no-home HOME=/no-home PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs:$PATH ${TAU_RUN} -e \"realizable F o1[t]:qlt = {1/2}:qlt\""
	"UNKNOWN" FAIL_REGEX "Aborted|core dumped" REQUIRES qlt)

# IN-M9 (Batch 6): `run` of a root-positive A never reached the CTL*
# reducer -- the A node was handed to the solver as a G spec and the run
# died with a false "unsat".  It executes as its body now.
add_repl_test(ctl_star-run_A_always_executes
	"fragment ctl_star. run A (always o1[t] = 1)"
	"o1\\[0\\] := T" NO_TRACE FAIL_REGEX "unsat|Internal error")

# IN-R6: an executed E reduction registers its witness output internally and
# routes through the LTL pipeline; the run executes and the witness stream is
# never printed. (Needs a live ltlsynt on PATH like the other ltl run tests.)
add_multiline_repl_test(ctl_star-run_E_executes
	"o1\\[0\\] := T"
	STDIN "fragment ctl_star.\\nrun E (sometimes (o1[t] = 1)).\\nq\\nq\\n"
	FAIL_REGEX "w_0\\[0\\] :=" REQUIRES ltlsynt)
