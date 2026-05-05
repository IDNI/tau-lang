#
# unsat command
#

include(add_repl_test)

# the unsat command checks if a Tau formula is unsatisfiable and prints T or F
add_repl_test(unsat_cmd-f "unsat F" ": T")
add_repl_test(unsat_cmd-t "unsat T" ": F")
add_repl_test(unsat_cmd-contradiction_neg "unsat x=0 && !x=0" ": T")

# unsatisfiable formulas
add_repl_test(unsat_cmd-F             "unsat F"                  ": T")
add_repl_test(unsat_cmd-contradiction "unsat X = 0 && X != 0"   ": T")

# satisfiable formulas
add_repl_test(unsat_cmd-T           "unsat T"          ": F")
add_repl_test(unsat_cmd-wff_sat     "unsat X = 0"      ": F")
add_repl_test(unsat_cmd-disjunction "unsat X = 0 || Y = 0" ": F")

# with rec-relation defs
add_repl_test(unsat_cmd-rr_pred "p(X) := X = 0 && X != 0. unsat p(t)" ": T")

# history references
add_repl_test(unsat_cmd-mem_T    "T.   unsat %"    "F")
add_repl_test(unsat_cmd-mem_F    "F.   unsat %"    "T")
add_repl_test(unsat_cmd-mem_rel  "F.   unsat %-0"  "T")
add_repl_test(unsat_cmd-mem_abs  "F.   unsat %1"   "T")
