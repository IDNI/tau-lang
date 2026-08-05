#
# sat command
#

include(add_repl_test)

# the sat command checks if a Tau formula is satisfiable and prints T or F
add_repl_test(sat_cmd-t "sat T" ": T")
add_repl_test(sat_cmd-f "sat F" ": F")
add_repl_test(sat_cmd-formula "sat x = 0" ": T")

# satisfiable formulas
add_repl_test(sat_cmd-T         "sat T"            ": T")
add_repl_test(sat_cmd-wff_sat   "sat X = 0"        ": T")
add_repl_test(sat_cmd-wff_sat2  "sat X != 0"       ": T")
add_repl_test(sat_cmd-disjunct  "sat X = 0 || Y = 0" ": T")

# unsatisfiable formulas
add_repl_test(sat_cmd-F           "sat F"                  ": F")
add_repl_test(sat_cmd-contradiction "sat X = 0 && X != 0" ": F")

# with rec-relation defs
add_repl_test(sat_cmd-rr_pred "p(X) := X = 0. sat p(t)" ": T")

# history references
add_repl_test(sat_cmd-mem_rel  "T.   sat %-0"  "T")
add_repl_test(sat_cmd-mem_abs  "T.   sat %1"   "T")
add_repl_test(sat_cmd-mem_last "T.   sat %"    "T")
add_repl_test(sat_cmd-mem_F    "F.   sat %"    "F")

# AP1-1: a definition set whose expansion oscillates (`f(x) := f(x)'`, the
# same shape as the normalize_cmd regressions above) makes normalize_formula
# return nullptr. `sat` used to hand that null straight to is_tau_formula_sat,
# which dereferences it. The expected output is the verdict, not the
# diagnostic: the "oscillates" message is printed *before* the crash point, so
# matching on it would not tell a crash apart from a clean rejection, whereas
# the ": F" line can only be reached after sat returns.
# add_repl_test_fail is used because the diagnostic contains "Error".
add_repl_test_fail(sat_cmd-oscillating_definition
	"f(x) := f(x)'. sat f(1) = 0" ": F")
