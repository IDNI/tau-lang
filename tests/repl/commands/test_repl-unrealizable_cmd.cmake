# unrealizable command

include(add_repl_test)

# the unrealizable command checks if a Tau specification is unrealizable and
# prints T or F
add_repl_test(unrealizable_cmd-f "unrealizable F" ": T")
add_repl_test(unrealizable_cmd-t "unrealizable T" ": F")

# unrealizable specs
add_repl_test(unrealizable_cmd-F               "unrealizable F"                ": T")
add_repl_test(unrealizable_cmd-contradiction   "unrealizable X = 0 && X != 0"  ": T")
# top-level F ≡ sometimes: trace existence, so F over an input is
# satisfiable; the unrealizable reactive form is the nested G (F ...)
add_repl_test(unrealizable_cmd-input_eventually "unrealizable F (i1[t] = 0)"   ": F")
add_repl_test(unrealizable_cmd-nested_input_gf  "unrealizable G (F (i1[t] = 0))" ": T")

# realizable specs
add_repl_test(unrealizable_cmd-T                "unrealizable T"               ": F")
add_repl_test(unrealizable_cmd-wff_sat          "unrealizable X = 0"           ": F")
add_repl_test(unrealizable_cmd-disjunction      "unrealizable X = 0 || Y = 0"  ": F")
add_repl_test(unrealizable_cmd-output_eventually "unrealizable F (o1[t] = 0)"  ": F")

# with rec-relation defs
add_repl_test(unrealizable_cmd-rr_pred "p(X) := X = 0 && X != 0. unrealizable p(t)" ": T")

# history references
add_repl_test(unrealizable_cmd-mem_T    "T.   unrealizable %"    "F")
add_repl_test(unrealizable_cmd-mem_F    "F.   unrealizable %"    "T")
add_repl_test(unrealizable_cmd-mem_rel  "F.   unrealizable %-0"  "T")
add_repl_test(unrealizable_cmd-mem_abs  "F.   unrealizable %1"   "T")

# satisfiability: bv (cvc5)
add_repl_test(unrealizable_cmd-bv-cvc5-implicit_exists    "unrealizable all x x + y = { 1 }:bv[4]"           ": T")
add_repl_test(unrealizable_cmd-bv-cvc5-positive           "unrealizable all x x > { 0 }:bv[4]"               ": T")
add_repl_test(unrealizable_cmd-bv-cvc5-plus_one_eq        "unrealizable all x x + { 1 } = { 1 }:bv[4]"       ": T")
add_repl_test(unrealizable_cmd-bv-cvc5-plus_one_lt        "unrealizable all x x + { 1 } < { 1 }:bv[4]"       ": T")
add_repl_test(unrealizable_cmd-bv-cvc5-typed_plus_one_lt  "unrealizable all x x + { 1 }:bv[4] < { 1 }:bv[4]" ": T")
