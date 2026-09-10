#
# word spellings of the binary temporal operators: until/U, release/R,
# weak_until/W, since/S and trigger/T.
#
# Each pair below must parse to the identical formula: entering a formula
# that already sits in the REPL history reuses its slot, so a repeat prints
# the same %N and the "%1[^%]*%1" regex only matches when both spellings
# landed in slot 1.
#

include(add_repl_test)

add_repl_test(word_synonym-until_equals_U
	"(X = 0) U (Y = 0). (X = 0) until (Y = 0)"
	"%1[^%]*%1")
add_repl_test(word_synonym-release_equals_R
	"(X = 0) R (Y = 0). (X = 0) release (Y = 0)"
	"%1[^%]*%1")
add_repl_test(word_synonym-weak_until_equals_W
	"(X = 0) W (Y = 0). (X = 0) weak_until (Y = 0)"
	"%1[^%]*%1")
add_repl_test(word_synonym-since_equals_S
	"(X = 0) S (Y = 0). (X = 0) since (Y = 0)"
	"%1[^%]*%1")
add_repl_test(word_synonym-trigger_equals_T
	"(X = 0) T (Y = 0). (X = 0) trigger (Y = 0)"
	"%1[^%]*%1")

# an io_var name that begins with a synonym word but keeps going past it
# ("_x" cannot itself start an identifier) must still parse as that one
# whole variable, not the operator plus a dangling remainder.
add_repl_test(word_synonym-identifier_prefix_until
	"until_x := out console. sat until_x[t] = 1" ": T")
add_repl_test(word_synonym-identifier_prefix_release
	"release_x := out console. sat release_x[t] = 1" ": T")
add_repl_test(word_synonym-identifier_prefix_weak_until
	"weak_until_x := out console. sat weak_until_x[t] = 1" ": T")
add_repl_test(word_synonym-identifier_prefix_since
	"since_x := out console. sat since_x[t] = 1" ": T")
add_repl_test(word_synonym-identifier_prefix_trigger
	"trigger_x := out console. sat trigger_x[t] = 1" ": T")
