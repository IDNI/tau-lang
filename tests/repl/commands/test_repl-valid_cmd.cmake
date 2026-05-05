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
