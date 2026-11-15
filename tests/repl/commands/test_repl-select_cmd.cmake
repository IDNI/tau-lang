#
# select command
#

include(add_repl_test)

# the selection is stored in the history, with the numbering of subformulas
add_repl_test(select_cmd-wff "select 2 x = 0 && y = 0" "%1[^:]*: x = 0")
add_repl_test(select_cmd-term "select 3 x & (y | z)" "%1[^:]*: y\\|z")
add_repl_test(select_cmd-history "x = 0 && y = 0. select 5 %1" "%2[^:]*: y = 0")
add_repl_test(select_cmd-quantified "select 5 x = 0 && ex y (y = 0 || x = 1)"
	"%1[^:]*: ex b1 b1 = 0 \\|\\| x = 1")

# a position past the last subformula, or 0, is an error naming the count
add_repl_test(select_cmd-out_of_range "select 7 x = 0 && y = 0"
	"Subformula does not exist.*6" NO_FAIL_REGEX)
add_repl_test(select_cmd-zero "select 0 x = 0 && y = 0"
	"Subformula does not exist.*6" NO_FAIL_REGEX)
add_repl_test(select_cmd-no_result_on_error "select 7 x = 0 && y = 0. history"
	"history is empty" NO_FAIL_REGEX)

# a selection composes with the other commands: select the quantified part
# of a formula, eliminate its quantifier, and substitute the result back
add_repl_test(select_cmd-qelim_subst
	"x = 0 && ex y (y = 0 && x = 1). select 5 %1. qelim %. s %1 [%2 / %]"
	"%3[^:]*: x' = 0.*%4[^:]*: x = 0 && x' = 0")

add_repl_test(help_select_cmd "help select" "the select command stores")
