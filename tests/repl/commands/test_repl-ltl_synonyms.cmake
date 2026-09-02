#
# Word synonyms of the temporal operators: U/until, R/release, W/weak_until,
# S/since and T/trigger (plus the pre-existing prefix aliases F/sometimes/<>
# and G/always/[]).
#
# Each pair of spellings must parse to the IDENTICAL formula.  The REPL makes
# that observable: entering a formula that already sits in the history reuses
# its slot, so both spellings print as %1 and no %2 ever appears.  The
# `%1[^%]*%1` regex fails if the second spelling lands in a fresh slot.
#

include(add_repl_test)

# binary infix operators, letter form followed by its word synonym
add_repl_test(ltl_synonyms-until_is_U
	"(o1[t] = 1) U (o2[t] = 1). (o1[t] = 1) until (o2[t] = 1)"
	"%1[^%]*%1")
add_repl_test(ltl_synonyms-release_is_R
	"(o1[t] = 1) R (o2[t] = 1). (o1[t] = 1) release (o2[t] = 1)"
	"%1[^%]*%1")
add_repl_test(ltl_synonyms-weak_until_is_W
	"(o1[t] = 1) W (o2[t] = 1). (o1[t] = 1) weak_until (o2[t] = 1)"
	"%1[^%]*%1")
add_repl_test(ltl_synonyms-since_is_S
	"(o1[t] = 1) S (o2[t] = 1). (o1[t] = 1) since (o2[t] = 1)"
	"%1[^%]*%1")
add_repl_test(ltl_synonyms-trigger_is_T
	"(o1[t] = 1) T (o2[t] = 1). (o1[t] = 1) trigger (o2[t] = 1)"
	"%1[^%]*%1")

# prefix aliases keep working and stay interchangeable
add_repl_test(ltl_synonyms-sometimes_is_F
	"F (o1[t] = 1). sometimes o1[t] = 1. <> o1[t] = 1"
	"%1[^%]*%1[^%]*%1")
add_repl_test(ltl_synonyms-always_is_G
	"G (o1[t] = 1). always o1[t] = 1. [] o1[t] = 1"
	"%1[^%]*%1[^%]*%1")

# word forms nest, mix with letter forms and flow through the commands
add_repl_test(ltl_synonyms-nested_mixed
	"G ((o1[t] = 1) U ((o2[t] = 1) S (o3[t] = 1))). G ((o1[t] = 1) until ((o2[t] = 1) since (o3[t] = 1)))"
	"%1[^%]*%1")
add_repl_test(ltl_synonyms-normalize_word_form
	"normalize (o1[t] = 1 && o1[t] = 1) until (o2[t] = 1)"
	"%[0-9]+")
add_repl_test(ltl_synonyms-sat_word_form
	"sat (o1[t] = 1) weak_until (o2[t] = 1)"
	": T")
