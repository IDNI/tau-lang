#
# subformulas command
#

include(add_repl_test)

# the numbering: the expression itself is 1, each formula comes as it
# appears and the terms under a formula come after it; a subformula that
# occurs twice is listed once, where it first appears. The fail pattern is
# a line numbered past the last subformula.
add_repl_test(subformulas_cmd-wff "subformulas x = 0 && ex y (y = 0 || x = 1)"
	"1[^:]*: x = 0 && \\(ex b1 b1 = 0 \\|\\| x = 1\\).*2[^:]*: x = 0.*3[^:]*: x.*4[^:]*: 0.*5[^:]*: ex b1 b1 = 0 \\|\\| x = 1.*6[^:]*: b1 = 0 \\|\\| x = 1.*7[^:]*: b1 = 0.*8[^:]*: b1.*9[^:]*: x = 1.*10[^:]*: 1"
	FAIL_REGEX "Error|\n[^\n:]*11[^\n:]*: ")

add_repl_test(subformulas_cmd-term "subformulas x & (y | z)"
	"1[^:]*: x&\\(y\\|z\\).*2[^:]*: x.*3[^:]*: y\\|z.*4[^:]*: y.*5[^:]*: z"
	FAIL_REGEX "Error|\n[^\n:]*6[^\n:]*: ")

add_repl_test(subformulas_cmd-history "x = 0 && y = 0. subformulas %"
	"1[^:]*: x = 0 && y = 0.*2[^:]*: x = 0.*3[^:]*: x.*4[^:]*: 0.*5[^:]*: y = 0.*6[^:]*: y"
	FAIL_REGEX "Error|\n[^\n:]*7[^\n:]*: ")

# the list goes to the history only through select
add_repl_test(subformulas_cmd-history_unchanged "x = 0 && y = 0. subformulas %. history"
	"%1[^:]*: x = 0 && y = 0" FAIL_REGEX "Error|%2")

add_repl_test(help_subformulas_cmd "help subformulas" "the subformulas command lists")
