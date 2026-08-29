#
# valid command
#

include(add_repl_test)

# the valid command checks if a Tau formula is logically equivalent to T
# and prints T or F
add_repl_test(valid_cmd-t "valid T" ": T")
add_repl_test(valid_cmd-formula "valid x = 0" ": F")
add_repl_test(valid_cmd-excluded_middle "valid x=0 || !x=0" ": T")

# tautologies (always T)
add_repl_test(valid_cmd-T         "valid T"              ": T")
add_repl_test(valid_cmd-tautology "valid T || F"         ": T")

# non-tautologies
add_repl_test(valid_cmd-F         "valid F"              ": F")
add_repl_test(valid_cmd-not_valid "valid X = 0"          ": F")
add_repl_test(valid_cmd-not_valid2 "valid X != 0"        ": F")

# valid with rec-relation defs
add_repl_test(valid_cmd-rr_pred "p(X) := T. valid p(t)" ": T")

# history references
add_repl_test(valid_cmd-mem_rel  "T.  valid %-0"  "T")
add_repl_test(valid_cmd-mem_abs  "T.  valid %1"   "T")
add_repl_test(valid_cmd-mem_last "T.  valid %"    "T")
add_repl_test(valid_cmd-mem_F    "F.  valid %"    "F")

# satisfiability: bv (cvc5)
add_repl_test(valid_cmd-bv-cvc5-implicit_exists_neg   "valid ! (all x x + y = { 1 }:bv[4])"           ": T")
add_repl_test(valid_cmd-bv-cvc5-positive_neg          "valid ! (all x x > { 0 }:bv[4])"               ": T")
add_repl_test(valid_cmd-bv-cvc5-plus_one_eq_neg       "valid ! (all x x + { 1 } = { 1 }:bv[4])"       ": T")
add_repl_test(valid_cmd-bv-cvc5-plus_one_lt_neg       "valid ! (all x x + { 1 } < { 1 }:bv[4])"       ": T")
add_repl_test(valid_cmd-bv-cvc5-typed_plus_one_lt_neg "valid ! (all x x + { 1 }:bv[4] < { 1 }:bv[4])" ": T")

# hooks: two-sided `valid (A) <-> (B)` equivalence, migrated from tests/unit/test_hooks.cpp
add_repl_test(valid_cmd-hooks_wff_imply-02_t_imply_x_eq_x            "valid (T -> x = 0) <-> (x = 0)"                      ": T")
add_repl_test(valid_cmd-hooks_wff_imply-03_x_imply_f_eq_negx         "valid ((x = 0) -> F) <-> (!(x = 0))"                 ": T")
add_repl_test(valid_cmd-hooks_wff_imply-06_non_degenerate_not_t      "valid (x = 0) -> (y = 0)"                            ": F")
add_repl_test(valid_cmd-hooks_wff_imply-06_non_degenerate_not_f      "valid !((x = 0) -> (y = 0))"                         ": F")

add_repl_test(valid_cmd-hooks_wff_rimply-01_x_rimply_y_eq_y_imply_x  "valid ((x = 0) <- (y = 0)) <-> ((y = 0) -> (x = 0))" ": T")
add_repl_test(valid_cmd-hooks_wff_rimply-02_f_rimply_x_eq_negx       "valid (F <- (x = 0)) <-> (!(x = 0))"                 ": T")
add_repl_test(valid_cmd-hooks_wff_rimply-03_x_rimply_t_eq_x          "valid ((x = 0) <- T) <-> (x = 0)"                    ": T")

add_repl_test(valid_cmd-hooks_wff_equiv-01_f_equiv_x_eq_negx         "valid (F <-> (x = 0)) <-> (!(x = 0))"                ": T")
add_repl_test(valid_cmd-hooks_wff_equiv-02_t_equiv_x_eq_x            "valid (T <-> (x = 0)) <-> (x = 0)"                   ": T")
add_repl_test(valid_cmd-hooks_wff_equiv-03_x_equiv_f_eq_negx         "valid ((x = 0) <-> F) <-> (!(x = 0))"                ": T")
add_repl_test(valid_cmd-hooks_wff_equiv-04_x_equiv_t_eq_x            "valid ((x = 0) <-> T) <-> (x = 0)"                   ": T")

add_repl_test(valid_cmd-hooks_wff_conditional-01_f_cond_x_y_eq_y     "valid (F ? (x = 0) : (y = 0)) <-> (y = 0)"           ": T")
add_repl_test(valid_cmd-hooks_wff_conditional-02_t_cond_x_y_eq_x     "valid (T ? (x = 0) : (y = 0)) <-> (x = 0)"           ": T")
add_repl_test(valid_cmd-hooks_wff_conditional-03_z_cond_x_x_eq_x     "valid ((z = 0) ? (x = 0) : (x = 0)) <-> (x = 0)"     ": T")
add_repl_test(valid_cmd-hooks_wff_conditional-04_non_degenerate_ne_x "valid ((z = 0) ? (x = 0) : (y = 0)) <-> (x = 0)"     ": F")
add_repl_test(valid_cmd-hooks_wff_conditional-04_non_degenerate_ne_y "valid ((z = 0) ? (x = 0) : (y = 0)) <-> (y = 0)"     ": F")

add_repl_test(valid_cmd-hooks_wff_lt_lteq-01_lt_expansion            "valid (x < y) <-> ((x & y') = 0 && x != y)"          ": T")
add_repl_test(valid_cmd-hooks_wff_lt_lteq-02_lteq_expansion          "valid (x <= y) <-> ((x & y') = 0)"                   ": T")

# AP1-1: the same normalization failure, reached through valid_spec's
# is_tau_impl call instead of is_tau_formula_sat. `valid` reports the
# structured error instead of deciding a verdict.
add_repl_test_fail(valid_cmd-oscillating_definition
	"f(x) := f(x)'. valid f(1) = 0" "Definition expansion oscillates")
