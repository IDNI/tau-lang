#
# lgrs command
#

include(add_repl_test)

# issue-60: `lgrs` shared solve_cmd's defect -- lgrs_cmd also passed the
# grammar nonterminal from get_type_and_arg to print_solver_cmd_solution as if
# it were a BA type id, so any lgrs whose answer is the constant 0 or 1 threw
# `ba_types::type_tree: invalid ba_type_id`.
#
# It was additionally unreachable in Debug: api<node>::lgrs indexed the wff
# (`tau::get(eq)[0]` / `[1]`) instead of the bf_eq when screening the equation
# for non-Boolean operations. A wff wrapping a bf_eq has a single child, so
# `[1]` tripped the `c != nullptr` assert in tree<node>::child_tree. Release
# only survived it because the assert compiles out.

add_repl_test(lgrs_cmd-sbf_zero "lgrs x:sbf = 0" "x := \\{ 0 \\}:sbf")

# two variables: both are reported, and the second side of the equation is
# what the fixed non-Boolean screen actually looks at
add_repl_test(lgrs_cmd-sbf_two_vars "lgrs x:sbf | y:sbf = 0"
	"x := \\{ 0 \\}:sbf")

# untyped falls back to node::ba::default_type(), which is `tau`
add_repl_test(lgrs_cmd-untyped_zero "lgrs x = 0" "x := \\{ F \\}:tau")

# the non-Boolean screen is what was indexing the wrong node, so pin the
# rejection it is there for (add_repl_test_fail: the expected output itself
# contains "Error")
add_repl_test_fail(lgrs_cmd-non_boolean
	"lgrs x:bv[16] + y:bv[16] = { 0 }:bv[16]"
	"Found non-Boolean operation in equation")
