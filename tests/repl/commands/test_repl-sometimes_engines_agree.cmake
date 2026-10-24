#
# `sat` keeps an un-nested `sometimes` in the safety pipeline (its flags and
# the χ fixpoint), while `realizable` sends every `sometimes` to the game.
# Each spec here is asked both ways and both answers must be the same.
#

function(add_sometimes_agree_test name spec verdict)
	add_repl_test(sometimes_agree-${name}_sat "sat ${spec}" ": ${verdict}"
		${ARGN})
	add_repl_test(sometimes_agree-${name}_realizable "realizable ${spec}"
		": ${verdict}" ${ARGN})
endfunction()

# bv[2] strictly increasing runs out of values before and after reaching 3
add_sometimes_agree_test(bv2_increasing
	"(always o1[t]:bv[2] > o1[t-1]:bv[2]) && (sometimes o1[t]:bv[2] = {3}:bv[2])"
	F REQUIRES ltlsynt bv)
add_sometimes_agree_test(bv2_nondecreasing
	"(always o1[t]:bv[2] >= o1[t-1]:bv[2]) && (sometimes o1[t]:bv[2] = {3}:bv[2])"
	T REQUIRES ltlsynt bv)
# the environment can avoid 3 forever
add_sometimes_agree_test(bv2_copies_input
	"(always o1[t]:bv[2] = i1[t]:bv[2]) && (sometimes o1[t]:bv[2] = {3}:bv[2])"
	F REQUIRES ltlsynt bv)
# the warm-up step o1[0] has no i1[-1] to copy, so it may be 3
add_sometimes_agree_test(bv2_copies_previous_input
	"(always o1[t]:bv[2] = i1[t-1]:bv[2]) && (sometimes o1[t]:bv[2] = {3}:bv[2])"
	T REQUIRES ltlsynt bv)
# x, x+2, x, ... never holds both 3 and 2
add_sometimes_agree_test(bv2_two_goals_by_two
	"(always o1[t]:bv[2] = o1[t-1]:bv[2] + {2}:bv[2]) && (sometimes o1[t]:bv[2] = {3}:bv[2]) && (sometimes o1[t]:bv[2] = {2}:bv[2])"
	F REQUIRES ltlsynt bv)
add_sometimes_agree_test(bv1_constant_two_goals
	"(always o1[t]:bv[1] = o1[t-1]:bv[1]) && (sometimes o1[t]:bv[1] = 1) && (sometimes o1[t]:bv[1] = 0)"
	F REQUIRES ltlsynt bv)
add_sometimes_agree_test(bv1_goal_on_other_output
	"(always o2[t]:bv[1] = o1[t-1]:bv[1]) && (sometimes o2[t]:bv[1] = 1) && (sometimes o1[t]:bv[1] = 0)"
	T REQUIRES ltlsynt bv)
add_sometimes_agree_test(bv1_masked_input
	"(always o1[t]:bv[1] = i1[t]:bv[1] & o2[t]:bv[1]) && (sometimes o1[t]:bv[1] = 1)"
	F REQUIRES ltlsynt bv)
# the environment plays the complement of the output forever
add_sometimes_agree_test(sbf_disjoint_from_input
	"(always o1[t]:sbf & i1[t]:sbf = 0) && (sometimes o1[t]:sbf != 0)"
	F REQUIRES ltlsynt)
add_sometimes_agree_test(sbf_alternating
	"(always o1[t]:sbf & o1[t-1]:sbf = 0) && (sometimes o1[t]:sbf = 1) && (sometimes o1[t]:sbf = 0)"
	T REQUIRES ltlsynt)
# the output is chosen after the input of step 0
add_sometimes_agree_test(tau_constant_meets_input
	"(always o1[t] = o1[t-1]) && (sometimes o1[t] = i1[t])"
	T REQUIRES ltlsynt)
add_sometimes_agree_test(qint_avoids_input
	"(always o1[t]:qint != i1[t]:qint) && (sometimes o1[t]:qint = {0}:qint)"
	F REQUIRES ltlsynt qint)
