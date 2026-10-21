#
# The routes the LTL pipeline takes when the data game over codes does not
# decide a specification: the refinement loop that checks the abstraction's
# strategy against the data and its caps, the abstraction solved again with
# observations, the `ltl` trace of an ABA-infeasible edge, the time
# constraints of the skeleton, the CTL* reduction of A and E under each
# connective and the tableau of an E asked at several steps, and the
# answers of an ltlfilt that prints what Spot never does.
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
endif()
