#
# Every temporal spelling through the REPL: the prefix operators in their
# three spellings each (always/G/[], sometimes/F/<>), the binary operators
# in their word and letter spellings (until/U, release/R, weak_until/W,
# since/S, trigger/T) and the CTL* path quantifiers A and E.
#
# A parity case enters several spellings of one formula: the REPL reuses
# the history slot of a formula it already holds, so the "%1[^%]*%1" regex
# only matches when every spelling landed in slot 1.
#

include(add_repl_test)

# ── parity of the spellings ─────────────────────────────────────────────────

add_repl_test(temporal_ops-always_G_brackets_same_formula
	"G (o1[t] = 1). always o1[t] = 1. [] o1[t] = 1"
	"%1[^%]*%1[^%]*%1")
add_repl_test(temporal_ops-sometimes_F_diamond_same_formula
	"F (o1[t] = 1). sometimes o1[t] = 1. <> o1[t] = 1"
	"%1[^%]*%1[^%]*%1")
add_repl_test(temporal_ops-until_U_same_formula
	"(o1[t] = 0) U (o1[t] = 1). (o1[t] = 0) until (o1[t] = 1)"
	"%1[^%]*%1")
add_repl_test(temporal_ops-release_R_same_formula
	"(o1[t] = 0) R (o1[t] = 1). (o1[t] = 0) release (o1[t] = 1)"
	"%1[^%]*%1")
add_repl_test(temporal_ops-weak_until_W_same_formula
	"(o1[t] = 0) W (o1[t] = 1). (o1[t] = 0) weak_until (o1[t] = 1)"
	"%1[^%]*%1")
add_repl_test(temporal_ops-since_S_same_formula
	"(o1[t] = 0) S (o1[t] = 1). (o1[t] = 0) since (o1[t] = 1)"
	"%1[^%]*%1")
add_repl_test(temporal_ops-trigger_T_same_formula
	"(o1[t] = 0) T (o1[t] = 1). (o1[t] = 0) trigger (o1[t] = 1)"
	"%1[^%]*%1")
# the letter T is the trigger operator between operands and the constant
# true at an operand position
add_repl_test(temporal_ops-trigger_T_beside_constant_T
	"(o1[t] = 1) T T. (o1[t] = 1) trigger T"
	"%1[^%]*%1")

# ── the prefix operators through the deciding commands ──────────────────────

add_repl_test(temporal_ops-sat_always "sat always o1[t] = 1" ": T")
add_repl_test(temporal_ops-sat_G "sat G (o1[t] = 1)" ": T")
add_repl_test(temporal_ops-sat_brackets "sat [] o1[t] = 1" ": T")
add_repl_test(temporal_ops-sat_sometimes "sat sometimes o1[t] = 1" ": T")
add_repl_test(temporal_ops-sat_F "sat F (o1[t] = 1)" ": T")
add_repl_test(temporal_ops-sat_diamond "sat <> o1[t] = 1" ": T")
add_repl_test(temporal_ops-valid_brackets "valid [] (o1[t] | o1[t]' = 1)" ": T")
add_repl_test(temporal_ops-unsat_diamond "unsat <> (o1[t] = 1 && o1[t] = 0)" ": T")
add_repl_test(temporal_ops-realizable_G "realizable G (o1[t] = 1)" ": T")
add_repl_test(temporal_ops-unrealizable_F_input "unrealizable F (i1[t] = 1)" ": T")
add_repl_test(temporal_ops-normalize_brackets_prints_always
	"normalize [] (o1[t] = 1 && o1[t] = 1)" "always o1\\[t\\]:tau' = 0")
add_repl_test(temporal_ops-normalize_diamond_prints_F
	"normalize <> (o1[t] = 1 && o1[t] = 1)" "F o1\\[t\\]:tau' = 0")

# ── the binary operators through the deciding commands ──────────────────────

add_repl_test(temporal_ops-realizable_until "realizable (o1[t] = 0) until (o1[t] = 1)" ": T")
add_repl_test(temporal_ops-realizable_release "realizable (o1[t] = 1) release (o1[t] = 0)" ": T")
add_repl_test(temporal_ops-realizable_weak_until "realizable (o1[t] = 0) weak_until (o1[t] = 1)" ": T")
add_repl_test(temporal_ops-realizable_since "realizable (o1[t] = 1) since (o1[t] = 0)" ": T")
add_repl_test(temporal_ops-realizable_trigger "realizable (o1[t] = 1) trigger (o1[t] = 0)" ": T")
add_repl_test(temporal_ops-realizable_G_until "realizable G ((o1[t] = 0) until (o1[t] = 1))" ": T")

# an input on the right of a strong operator cannot be forced; the weak
# one is satisfied by holding the left side forever
add_repl_test(temporal_ops-unrealizable_until_input "unrealizable (o1[t] = 0) until (i1[t] = 1)" ": T")
add_repl_test(temporal_ops-unrealizable_release_input "unrealizable (o1[t] = 1) release (i1[t] = 1)" ": T")
add_repl_test(temporal_ops-unrealizable_since_input "unrealizable (o1[t] = 1) since (i1[t] = 1)" ": T")
add_repl_test(temporal_ops-unrealizable_trigger_input "unrealizable (o1[t] = 1) trigger (i1[t] = 1)" ": T")
add_repl_test(temporal_ops-realizable_weak_until_input "realizable (o1[t] = 1) weak_until (i1[t] = 1)" ": T")

# normalize keeps each binary operator and prints its letter
add_repl_test(temporal_ops-normalize_until_prints_U
	"normalize (o1[t] = 1) until (o1[t] = 0)" "= 1 U o1")
add_repl_test(temporal_ops-normalize_release_prints_R
	"normalize (o1[t] = 1) release (o1[t] = 0)" "= 1 R o1")
add_repl_test(temporal_ops-normalize_weak_until_prints_W
	"normalize (o1[t] = 1) weak_until (o1[t] = 0)" "= 1 W o1")
add_repl_test(temporal_ops-normalize_since_prints_S
	"normalize (o1[t] = 1) since (o1[t] = 0)" "= 1 S o1")
add_repl_test(temporal_ops-normalize_trigger_prints_T
	"normalize (o1[t] = 1) trigger (o1[t] = 0)" "= 1 T o1")

# ── the CTL* path quantifiers A and E ───────────────────────────────────────

# the default fragment rejects them
add_repl_test_fail(temporal_ops-sat_A_rejected_by_default
	"sat A (G (o1[t] = 1))" "require the ctl_star")
add_repl_test_fail(temporal_ops-sat_E_rejected_by_default
	"sat E (F (o1[t] = 1))" "require the ctl_star")

# every prefix spelling under A and E
add_repl_test(temporal_ops-sat_A_brackets_output "fragment ctl_star. sat A ([] o1[t] = 1)" ": T")
add_repl_test(temporal_ops-sat_A_G_output "fragment ctl_star. sat A (G (o1[t] = 1))" ": T")
add_repl_test(temporal_ops-sat_A_sometimes_output "fragment ctl_star. sat A (sometimes o1[t] = 1)" ": T")
add_repl_test(temporal_ops-sat_A_diamond_input "fragment ctl_star. sat A (<> i1[t] = 1)" ": F")
add_repl_test(temporal_ops-sat_E_diamond_output "fragment ctl_star. sat E (<> o1[t] = 1)" ": T")
add_repl_test(temporal_ops-sat_E_F_output "fragment ctl_star. sat E (F (o1[t] = 1))" ": T")
add_repl_test(temporal_ops-sat_E_always_output "fragment ctl_star. sat E (always o1[t] = 1)" ": T")

# every binary operator under A and E
add_repl_test(temporal_ops-sat_E_until_output "fragment ctl_star. sat E ((o1[t] = 0) until (o1[t] = 1))" ": T")
add_repl_test(temporal_ops-sat_A_until_input "fragment ctl_star. sat A ((o1[t] = 0) until (i1[t] = 1))" ": F")
add_repl_test(temporal_ops-sat_A_release_output "fragment ctl_star. sat A ((o1[t] = 1) release (o1[t] = 0))" ": T")
add_repl_test(temporal_ops-sat_A_weak_until_input "fragment ctl_star. sat A ((o1[t] = 1) weak_until (i1[t] = 1))" ": T")
add_repl_test(temporal_ops-sat_E_since_output "fragment ctl_star. sat E ((o1[t] = 1) since (o1[t] = 0))" ": T")
add_repl_test(temporal_ops-sat_A_trigger_output "fragment ctl_star. sat A ((o1[t] = 1) trigger (o1[t] = 0))" ": T")

# normalize and whatis keep the path quantifier
add_repl_test(temporal_ops-normalize_A_prints_A_always
	"fragment ctl_star. normalize A (G (o1[t] = 1))" "A always o1")
add_repl_test(temporal_ops-normalize_E_prints_E_F
	"fragment ctl_star. normalize E (<> o1[t] = 1)" "E F o1")
add_repl_test(temporal_ops-whatis_A_is_wff
	"fragment ctl_star. whatis A (always o1[t] = 1)" "node type: wff")
add_repl_test(temporal_ops-whatis_E_is_wff
	"fragment ctl_star. whatis E (sometimes o1[t] = 1)" "node type: wff")
