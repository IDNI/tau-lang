#
# The routes the LTL pipeline takes when the data game over codes does not
# decide a specification: the refinement loop that checks the abstraction's
# strategy against the data and its caps, the abstraction solved again with
# observations, the `ltl` trace of an ABA-infeasible edge, the time
# constraints of the skeleton, the CTL* reduction of A and E under each
# connective and the tableau of an E asked at several steps, the answers
# of an ltlfilt that prints what Spot never does, the conjunctions
# ltl-compositional leaves undecided, and the strategy exports and the
# Algorithm D fallback when ltlsynt or autfilt answer what Spot never does.
#

include(add_repl_test)
include(tau_repl_pack)

# `ltl-data-game-max-nodes 1` leaves the data game over codes undecided, so
# the abstraction's strategy is refined. o1 would have to repeat every six
# steps and flip every two; the data game over formulas decides what each
# capped refinement leaves open.
set(_chain "(always o2[t]:bv[1] = o1[t-2]:bv[1] && o3[t]:bv[1] = o2[t-2]:bv[1] && !(o1[t]:bv[1] = o3[t]:bv[1]) && o3[t-2]:bv[1] = o1[t]:bv[1]) && (sometimes o1[t-1]:bv[1] = 1)")
add_repl_test(ltl_refinement-round_cap_is_unknown_then_data_game
	"set ltl-data-game-max-nodes 1. set ltl-refinement-rounds 1. realizable ${_chain}"
	"UNKNOWN \\(ABA refinement bound reached \\(--ltl-refinement-rounds.*UNREALIZABLE \\(data game\\).*: F"
	REQUIRES ltlsynt)
add_repl_test(ltl_refinement-window_path_cap_is_unknown_then_data_game
	"set ltl-data-game-max-nodes 1. set ltl-window-max-paths 1. realizable ${_chain}"
	"UNKNOWN \\(window oracle path cap\\).*UNREALIZABLE \\(data game\\).*: F"
	REQUIRES ltlsynt)
# one fixpoint round decides neither the strategy on the data nor the data
# game over formulas: no verdict
add_repl_test(ltl_refinement-data_fixpoint_cap_is_unknown
	"set ltl-data-game-max-nodes 1. set ltl-data-fixpoint-rounds 1. realizable ${_chain}"
	"UNKNOWN \\(the strategy could not be checked against the data\\)"
	FAIL_REGEX "%[0-9]+: [TF]" REQUIRES ltlsynt)
# the strategy of the default abstraction loses against the data and no path
# can be blocked; the abstraction with observations has one that wins
add_repl_test(ltl_refinement-observed_abstraction_after_a_lost_strategy
	"set ltl-data-game-max-nodes 1. realizable (always o2[0]:bv[1] = 1 && o2[t]:bv[1] = o1[t-1]:bv[1]) && (sometimes i1[t-1]:bv[1] = o1[t-1]:bv[1])"
	"UNKNOWN \\(the strategy loses against the data and no blocking clause was found\\).*\\[ltl_aba\\] REALIZABLE.*: T"
	REQUIRES ltlsynt)

# With one k-ary subset checked, the skeleton allows o1 != o2, o2 != o3 and
# o1 != o3 at once; `ltl` traces the strategy edge the oracle refuses.
add_repl_test(ltl_refinement-ltl_traces_an_infeasible_edge
	"set max-consistency-subsets 1. set ltl-data-game-max-nodes 1. ltl (G(F(o1[t]:bv[1] != o2[t]:bv[1]))) && (G(o1[t]:bv[1] = o2[t]:bv[1] || (o2[t]:bv[1] != o3[t]:bv[1] && o1[t]:bv[1] != o3[t]:bv[1])))"
	": INFEASIBLE.*\\(formula: .*First round: ABA-infeasible transition.*UNREALIZABLE"
	REQUIRES ltlsynt)
# without the data game `ltl` shows the safety formula `run` executes
add_repl_test(ltl_refinement-ltl_prints_the_safety_formula
	"set ltl-data-game-max-nodes 1. ltl F(o1[t]:bv[1] = 1)"
	"Safety formula: always o1\\[t\\]:bv\\[1\\]' = 0.*REALIZABLE"
	REQUIRES ltlsynt)

# A time constraint reads the step: `[t = 2]` is the step 2 alone, and a
# number on the left reads the comparison the other way round.
add_repl_test(ltl_refinement-time_constraint_equal_step_sat
	"realizable (G(o1[t]:bv[1] = 0 || [t = 2])) && (F(o1[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-time_constraint_other_step_unsat
	"realizable (G(o1[t]:bv[1] = 0 || [t = 2])) && (F(o1[t]:bv[1] = 1 && [t != 2]))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-time_constraint_number_left_before_sat
	"realizable (G(o1[t]:bv[1] = 0 || [3 > t])) && (F(o1[t]:bv[1] = 1 && [2 <= t]))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-time_constraint_number_left_after_unsat
	"realizable (G(o1[t]:bv[1] = 0 || [3 < t])) && (F(o1[t]:bv[1] = 1 && [2 >= t]))"
	": F" REQUIRES ltlsynt)

# a fixed-step atom under an eventually has no recurring semantics yet
add_repl_test(ltl_refinement-positional_atom_under_eventually_refused
	"realizable (G(o1[0]:bv[1] = 1 || (F(o1[1]:bv[1] = 1)))) && (F(o2[t]:bv[1] = 1))"
	"positional atom under .* is not yet supported.*UNKNOWN"
	FAIL_REGEX "%[0-9]+: [TF]" REQUIRES ltlsynt)

# E under each connective of the reduction. Without inputs the tree is one
# path, so E reads as the formula itself.
add_repl_test(ltl_refinement-ctl_star_e_under_or_sat
	"fragment ctl_star. realizable (E (F o1[t]:bv[1] = 1)) || (G o2[t]:bv[1] = 1)"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-ctl_star_e_under_or_unsat
	"fragment ctl_star. realizable (G(o1[t]:bv[1] = 0)) && ((E (F o1[t]:bv[1] = 1)) || (G o2[t]:bv[1] = 1)) && (F o2[t]:bv[1] = 0)"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-ctl_star_e_under_eventually_sat
	"fragment ctl_star. realizable F(E (G o1[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-ctl_star_e_left_of_until_sat
	"fragment ctl_star. realizable (E (F o1[t]:bv[1] = 1)) U (o2[t]:bv[1] = 1)"
	": T" REQUIRES ltlsynt)
# the environment keeps i1 at 0
add_repl_test(ltl_refinement-ctl_star_e_left_of_until_input_unsat
	"fragment ctl_star. realizable (E (F o1[t]:bv[1] = 1)) U (i1[t]:bv[1] = 1)"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-ctl_star_e_right_of_release_sat
	"fragment ctl_star. realizable (o2[t]:bv[1] = 1) R (E (F o1[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-ctl_star_e_left_of_weak_until_sat
	"fragment ctl_star. realizable (E (F o1[t]:bv[1] = 1)) W (o2[t]:bv[1] = 1)"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-ctl_star_e_left_of_since_sat
	"fragment ctl_star. realizable G((E (F o1[t]:bv[1] = 1)) S (o2[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-ctl_star_e_left_of_trigger_sat
	"fragment ctl_star. realizable G((E (F o1[t]:bv[1] = 1)) T (o2[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)

# With an input the witness path of E is pinned by directions one step
# later; the input of the step E is asked at is already the environment's.
add_repl_test(ltl_refinement-ctl_star_e_until_input_sat
	"fragment ctl_star. realizable E((o1[t]:bv[1] = 1) U (i1[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-ctl_star_e_until_input_unsat
	"fragment ctl_star. realizable (G(o1[t]:bv[1] = 0)) && (E((o1[t]:bv[1] = 1) U (i1[t]:bv[1] = 1)))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-ctl_star_e_release_input_sat
	"fragment ctl_star. realizable E((i1[t]:bv[1] = 1) R (o1[t]:bv[1] = 1))"
	": T" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-ctl_star_e_release_input_unsat
	"fragment ctl_star. realizable (G(o1[t]:bv[1] = 0)) && (E((i1[t]:bv[1] = 1) R (o1[t]:bv[1] = 1)))"
	": F" REQUIRES ltlsynt)

# An E asked at every step whose shared directions lose is decided again
# through the tableau of its path formula: two eventualities take turns, and
# a release unfolds into both of its sides.
add_repl_test(ltl_refinement-ctl_star_tableau_two_eventualities_unsat
	"fragment ctl_star. realizable (G(E((F(i1[t]:bv[1] = 1)) && (F(o1[t]:bv[1] = 1))))) && (G(o1[t]:bv[1] = 0))"
	": F" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-ctl_star_tableau_release_unsat
	"fragment ctl_star. realizable (G(E((i1[t]:bv[1] = 1) R (o1[t]:bv[1] = 1)))) && (G(o1[t]:bv[1] = 0))"
	": F" REQUIRES ltlsynt)
# past the state cap the directions stay shared, and their loss is no proof
add_repl_test(ltl_refinement-ctl_star_tableau_state_cap_is_unknown
	"fragment ctl_star. set ltl-ctl-star-tableau-max-states 1. realizable (G(E((F(i1[t]:bv[1] = 1)) && (F(o1[t]:bv[1] = 1))))) && (G(o1[t]:bv[1] = 0))"
	"no tableau within --ltl-ctl-star-tableau-max-states.*UNKNOWN: the CTL\\* reduction is unrealizable, but an E witness is encoded more strictly than E"
	FAIL_REGEX "%[0-9]+: [TF]" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-ltl_ctl_star_tableau_state_cap_is_unknown
	"fragment ctl_star. set ltl-ctl-star-tableau-max-states 3. ltl (G(E((F(i1[t]:bv[1] = 1)) && (F(o1[t]:bv[1] = 1))))) && (G(o1[t]:bv[1] = 0))"
	"CTL\\* reduced to LTL: .*UNKNOWN: the CTL\\* reduction is unrealizable, but an E witness"
	NO_FAIL_REGEX REQUIRES ltlsynt)
# `ltl` explains the reduction it decides: the tableau one, once the shared
# directions of an E asked at every step lose
add_repl_test(ltl_refinement-ltl_ctl_star_explains_the_tableau_reduction
	"fragment ctl_star. ltl (always oc[t]:bv[2] = oc[t-1]:bv[2] + {1}:bv[2]) && (always E ((oc[t]:bv[2] = {0}:bv[2] -> always (oc[t]:bv[2] = {2}:bv[2] -> i1[t]:bv[1] = 1)) && (oc[t]:bv[2] = {1}:bv[2] -> always (oc[t]:bv[2] = {2}:bv[2] -> i1[t]:bv[1] = 0))))"
	"CTL\\* reduced to LTL: [^\n]*w_0_r0"
	FAIL_REGEX "CTL\\* reduced to LTL: [^\n]*w_0_d_i1|Error" REQUIRES ltlsynt)
# `ltl` refuses an A under an eventually, as `realizable` does
add_repl_test(ltl_refinement-ltl_a_under_eventually_is_unknown
	"fragment ctl_star. ltl F(A(G(o1[t]:bv[1] = 1)))"
	"A under an existential or eventual context.*UNKNOWN"
	NO_FAIL_REGEX REQUIRES ltlsynt)

# An ltlfilt that prints no LTLf translation, and answers each tautology
# question `0 ` with a trailing blank (tests/repl/stubs/odd_ltlfilt).
if(NOT WIN32)
	set(_odd_ltlfilt "PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs/odd_ltlfilt:$PATH")
	add_raw_repl_test(ltl_refinement-ltlfilt_without_ltlf_translation_is_unknown
		"${_odd_ltlfilt} ${TAU_RUN} -e \"set ltl-semantics finite. realizable F(o1[t]:bv[1] = 1)\""
		"ltlfilt printed no formula for an LTLf formula.*UNKNOWN"
		FAIL_REGEX "%[0-9]+: [TF]|Aborted|core dumped" REQUIRES ltlsynt)
	# no constant output is proven, so Algorithm B decides
	add_raw_repl_test(ltl_refinement-ltlfilt_without_tautology_leaves_algorithm_b
		"${_odd_ltlfilt} ${TAU_RUN} -e \"realizable F(o1[t]:qlt > {0}:qlt || i1[t]:qlt > {1}:qlt)\" -S trace"
		"REALIZABLE \\(propositional\\).*: T"
		FAIL_REGEX "Error|Aborted|core dumped" REQUIRES ltlsynt)
	# an ltlfilt answering one line for several formulas is an error, not a
	# verdict (tests/repl/stubs/short_ltlfilt)
	add_raw_repl_test(ltl_refinement-ltlfilt_line_count_mismatch_is_an_error
		"PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs/short_ltlfilt:$PATH ${TAU_RUN} -e \"realizable F(o1[t]:qlt > {0}:qlt || i1[t]:qlt > {1}:qlt)\""
		"ltlfilt printed a line count other than the number of formulas"
		FAIL_REGEX "Aborted|core dumped|Assertion" REQUIRES ltlsynt)
endif()

# A formula with no data atom drives the step guards of its time
# constraints, as the data path does.
add_repl_test(ltl_refinement-atomless_time_constraint_realizable
	"realizable F([t > 3])" ": T" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-atomless_time_constraints_unrealizable
	"realizable (G(F([t > 3]))) && (F(G([t < 2])))" ": F" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-atomless_time_constraints_sat
	"sat (F([t > 3])) && (G(F([t > 1])))" ": T" REQUIRES ltlsynt)

# A conjunct with a fixed-step atom whose temporal operator holds no atom:
# the step-counter encoding has no proposition for it.
add_repl_test(ltl_refinement-positional_conjunct_with_atomless_eventually_is_unknown
	"realizable (G(o1[0]:bv[1] = 1 || (F([t > 3])))) && (G(o2[t]:bv[1] = 1))"
	"step-counter encoding: a conjunct with a fixed-position atom holds a subformula that is no atom of it.*UNKNOWN"
	FAIL_REGEX "%[0-9]+: [TF]" REQUIRES ltlsynt)

# ltl-compositional with an undecided part: a realizable other part leaves
# the conjunction undecided, an unrealizable one decides it.
add_repl_test(ltl_refinement-compositional_undecided_part_is_unknown
	"set ltl-compositional on. realizable (G(o1[0]:bv[1] = 1 || (F(o1[1]:bv[1] = 1)))) && (G(F(o2[t]:bv[1] = 1)))"
	"ltl_compositional_parts:[^\n]*[^0-9]2[^0-9].*UNKNOWN|UNKNOWN.*ltl_compositional_parts:[^\n]*[^0-9]2[^0-9]"
	FAIL_REGEX "%[0-9]+: [TF]" REQUIRES ltlsynt)
add_repl_test(ltl_refinement-compositional_unrealizable_part_after_undecided_one
	"set ltl-compositional on. realizable (G(o6[0]:bv[1] = 1 || (F(o6[1]:bv[1] = 1)))) && (o5[t]:bv[1] = 1 U i5[t]:bv[1] = 1)"
	"UNKNOWN.*: F|: F.*UNKNOWN"
	FAIL_REGEX "%[0-9]+: T|Error"
	REQUIRES ltlsynt)

# With one k-ary subset checked the abstraction's strategy takes an edge the
# oracle refuses, and its refinement is unrealizable: `run` refuses the spec.
add_multiline_repl_test(ltl_refinement-run_refuses_a_strategy_refined_away
	"not executable: it is unrealizable"
	"set max-consistency-subsets 1"
	"set ltl-data-game-max-nodes 1"
	"run 2 steps (G(F(o1[t]:bv[1] != o2[t]:bv[1]))) && (G(o1[t]:bv[1] = o2[t]:bv[1] || (o2[t]:bv[1] != o3[t]:bv[1] && o1[t]:bv[1] != o3[t]:bv[1])))."
	FAIL_REGEX "o1\\[0\\] :=" REQUIRES ltlsynt)

# `ltl` of an invariant whose bitvector question passes its time budget
# gives no verdict.
add_repl_test(ltl_refinement-ltl_invariant_past_its_budget_is_unknown
	"ltl always ((o2[t]:bv[16] * o2[t]:bv[16]) <= i1[t-1]:bv[16] && (i1[t]:bv[16] * o2[t]:bv[16]) < (i1[t-1]:bv[16] + o2[t-1]:bv[16]))"
	"Formula has no LTL operators \\(treated as G\\(phi\\)\\).*UNKNOWN: the synthesis backend failed or produced no verdict|UNKNOWN: the synthesis backend failed or produced no verdict.*Formula has no LTL operators \\(treated as G\\(phi\\)\\)"
	FLAGS --bv-solve-timeout 3 FAIL_REGEX "\nREALIZABLE|\nUNREALIZABLE|%[0-9]+: [TF]" TIMEOUT 120)

# ltlsynt refusing the strategy Algorithm D asks for, after its game was won,
# leaves the qlt spec to Algorithm A (tests/repl/stubs/odd_ltlsynt)
if(NOT WIN32)
	add_raw_repl_test(ltl_refinement-algorithm_d_strategy_refused_falls_to_algorithm_a
		"TAU_STUB_LTLSYNT=d_unrealizable PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs/odd_ltlsynt:$PATH ${TAU_RUN} -e \"set ltl-alg D. realizable F(o1[t]:qlt > {0}:qlt)\""
		"\\[ltl_aba\\] REALIZABLE\n.*: T"
		FAIL_REGEX "Error|UNREALIZABLE|Aborted|core dumped" REQUIRES ltlsynt qlt)
endif()

# A qlt atom whose side joins two streams has no T3 encoding; over a dense
# order o1 below o2 | o3 is always reachable, so the spec is realizable.
# `a < b` holds when every point of a is below every point of b, so a join
# on either side is the conjunction of the comparisons with its parts.
add_repl_test(ltl_refinement-qlt_join_side_is_realizable
	"realizable F(o1[t]:qlt < (o2[t]:qlt | o3[t]:qlt))"
	": T" FAIL_REGEX "UNKNOWN" REQUIRES ltlsynt qlt)
add_repl_test(ltl_refinement-qlt_join_side_lteq_is_realizable
	"realizable F(o1[t]:qlt <= (o2[t]:qlt | o3[t]:qlt))"
	": T" FAIL_REGEX "UNKNOWN" REQUIRES ltlsynt qlt)
add_repl_test(ltl_refinement-qlt_join_left_side_is_realizable
	"realizable F((o1[t]:qlt | o2[t]:qlt) < o3[t]:qlt)"
	": T" FAIL_REGEX "UNKNOWN" REQUIRES ltlsynt qlt)
add_repl_test(ltl_refinement-qlt_join_below_its_part_is_unrealizable
	"realizable F((o1[t]:qlt | o2[t]:qlt) < o1[t]:qlt)"
	": F" FAIL_REGEX "UNKNOWN" REQUIRES ltlsynt qlt)
add_repl_test(ltl_refinement-qlt_join_side_sat
	"sat ex x ex y ex z x:qlt < y:qlt | z:qlt"
	": T" FAIL_REGEX "UNKNOWN" REQUIRES qlt)
add_repl_test(ltl_refinement-qlt_join_below_its_part_unsat
	"sat ex x ex y (x:qlt | y:qlt) < x"
	": F" FAIL_REGEX "UNKNOWN" REQUIRES qlt)
add_repl_test(ltl_refinement-qlt_join_side_is_a_conjunction
	"valid all x all y all z (x:qlt < (y:qlt | z:qlt)) <-> (x < y && x < z)"
	": T" FAIL_REGEX "UNKNOWN" REQUIRES qlt)

# `ltl-export-strategy dot` with an autfilt that fails prints the HOA text
# instead (tests/repl/stubs/odd_ltlsynt), and a strategy file that cannot
# be written, or not flushed, is a warning that keeps the verdict.
if(NOT WIN32)
	add_raw_repl_test(ltl_refinement-export_strategy_dot_without_autfilt
		"TAU_STUB_LTLSYNT=autfilt_fails PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs/odd_ltlsynt:$PATH ${TAU_RUN} -e \"set ltl-export-strategy dot. realizable G(F(o1[t]:bv[1] = i1[t]:bv[1]))\" 2>&1"
		"=== STRATEGY HOA \\(dot unavailable\\) ===\nHOA: v1.*: T|: T.*=== STRATEGY HOA \\(dot unavailable\\) ===\nHOA: v1"
		FAIL_REGEX "STRATEGY DOT|Error|Aborted|core dumped" REQUIRES ltlsynt)
	add_repl_test(ltl_refinement-export_strategy_file_flush_failure
		"set ltl-export-strategy-file \\\"/dev/full\\\". realizable G(F(o1[t]:bv[1] = i1[t]:bv[1]))"
		"failed to write the strategy export file \\(path=/dev/full\\).*: T|: T.*failed to write the strategy export file \\(path=/dev/full\\)"
		NO_TRACE REQUIRES ltlsynt)
endif()
add_repl_test(ltl_refinement-export_strategy_file_unwritable
	"set ltl-export-strategy-file \\\"/nonexistent-dir/s.hoa\\\". realizable G(F(o1[t]:bv[1] = i1[t]:bv[1]))"
	"failed to write the strategy export file.*: T|: T.*failed to write the strategy export file"
	NO_TRACE REQUIRES ltlsynt)

# `ltl` traces a strategy state that has no edge, which only a tool other
# than Spot prints (tests/repl/stubs/odd_ltlsynt).
if(NOT WIN32)
	add_raw_repl_test(ltl_refinement-ltl_traces_an_edgeless_strategy_state
		"TAU_STUB_LTLSYNT=edgeless PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs/odd_ltlsynt:$PATH ${TAU_RUN} -e \"set ltl-data-game-max-nodes 1. ltl F(o1[t]:bv[1] = 1)\""
		"Strategy: 1 state\\(s\\), initial state 0\n[^\n]*\n  state 0:\n    \\(no outgoing edges\\)"
		FAIL_REGEX "Aborted|core dumped" REQUIRES ltlsynt)
	# a strategy the run shows to violate the spec makes the verdict unknown,
	# and the report closes its self-check scope before it leaves; an input
	# the strategy never reads is drawn, so a clause over it is checked too
	add_raw_repl_test(ltl_refinement-self_check_catches_a_lying_game
		"TAU_STUB_LTLSYNT=lying PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs/odd_ltlsynt:$PATH ${TAU_RUN} -e \"realizable (G(o1[t]:bv[1] = 1)) && (F(o2[t]:bv[1] = 1))\""
		"UNKNOWN: the strategy found for this specification violates it when run"
		FAIL_REGEX "Assertion|Aborted|core dumped|%[0-9]+[^\n]*: T" REQUIRES ltlsynt bv)
	add_raw_repl_test(ltl_refinement-self_check_draws_unread_inputs
		"TAU_STUB_LTLSYNT=lying_input PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs/odd_ltlsynt:$PATH ${TAU_RUN} -e \"realizable (G(i1[t]:bv[1] = 1 -> o1[t]:bv[1] = 1)) && F(o2[t]:bv[1] = 1)\""
		"violates always !i1\\[t\\]:bv\\[1\\] = 1 \\|\\| o1"
		FAIL_REGEX "Assertion|Aborted|core dumped|%[0-9]+[^\n]*: T" REQUIRES ltlsynt bv)
endif()
