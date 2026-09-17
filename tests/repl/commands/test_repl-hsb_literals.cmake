#
# hsb half-space literals with a linear expression on both sides: the parser
# moves the right-hand side to the left and the normalised element prints in
# the `linexpr < 0` form.
#
add_repl_test(hsb_literal-constant_rhs  "normalize o1[t]:hsb = {x[0] < 1}:hsb"            "x\\[0\\] - 1 < 0")
add_repl_test(hsb_literal-variable_rhs  "normalize o1[t]:hsb = {x[0] < x[1]}:hsb"         "x\\[0\\] - x\\[1\\] < 0")
add_repl_test(hsb_literal-both_sides    "normalize o1[t]:hsb = {2*x[0] < x[1] - 1}:hsb"   "2\\*x\\[0\\] - x\\[1\\] \\+ 1 < 0")
add_repl_test(hsb_literal-cancelling    "normalize o1[t]:hsb = {x[0] + x[1] < x[1] + 3}:hsb" "x\\[0\\] - 3 < 0")
add_repl_test_fail(hsb_literal-all_cancel "normalize o1[t]:hsb = {x[0] < x[0]}:hsb"        "failed for type")
add_repl_test(hsb_literal-closed_bound   "normalize o1[t]:hsb = {1 <= x[0]}:hsb"           "-x\\[0\\] \\+ 1 <= 0")
add_repl_test(hsb_literal-variable_closed "normalize o1[t]:hsb = {x[1] <= x[0]}:hsb"       "-x\\[0\\] \\+ x\\[1\\] <= 0")
# `x[0] <= 1` and `1 < x[0]` are not elements: rejected, naming the set that is.
add_repl_test_fail(hsb_literal-noncanonical_closed "normalize o1[t]:hsb = {x[0] <= 1}:hsb" "not an element of the lex-half-open algebra")
add_repl_test_fail(hsb_literal-noncanonical_open   "normalize o1[t]:hsb = {1 < x[0]}:hsb"  "not an element of the lex-half-open algebra")
