#
# fex / fall: functional quantifiers
#
# GitHub #149: a fex/fall is evaluated before any substitution can cross its
# binder, so neither a Boole decomposition nor a quantifier elimination
# substitutes into its body through a binder of the same name.
#

include(add_repl_test)

# explicit witnesses: fall b (a | b) is a
add_repl_test(functional_quantifiers-solve_fall_witness
	"solve b = 1 && (fall b (a | b)) != 1" "a := \\{ F \\}:tau")
add_repl_test(functional_quantifiers-sat_fall_same_name
	"sat b = 1 && (fall b (a | b)) != 1" ": T")

# the free y outside fall y is not substituted into its body
add_repl_test(functional_quantifiers-valid_fall_same_name
	"valid ((fall y (x | y)) & y) = x & y" ": T")

# cofactor identities: fex x f = f[x:=0] | f[x:=1], fall x f = f[x:=0] & f[x:=1]
add_repl_test(functional_quantifiers-cofactor_fex
	"valid (fex x (x & y)) = y" ": T")
add_repl_test(functional_quantifiers-cofactor_fall
	"valid (fall x (x | y)) = y" ": T")
add_repl_test(functional_quantifiers-cofactor_fex_ite
	"valid (fex x (x & a | x' & b)) = a | b" ": T")
add_repl_test(functional_quantifiers-cofactor_fall_ite
	"valid (fall x (x & a | x' & b)) = a & b" ": T")

# nested shadowing: the inner binder hides the outer one
add_repl_test(functional_quantifiers-nested_fex_fex
	"valid (fex x (fex x (x & y))) = y" ": T")
add_repl_test(functional_quantifiers-nested_fex_fall
	"valid (fex x (fall x (x | y))) = y" ": T")
add_repl_test(functional_quantifiers-nested_fex_fall_distinct
	"valid (fex x (fall y (x | y'))) = 1" ": T")

# renamed binders denote the same function
add_repl_test(functional_quantifiers-renamed_fex
	"valid (fex p (p & y)) = (fex q (q & y))" ": T")
add_repl_test(functional_quantifiers-renamed_fall
	"valid (fall p (p | y)) = (fall q (q | y))" ": T")

# logical binders around functional binders of the same name
add_repl_test(functional_quantifiers-mixed_all_fex
	"valid all y ((fex y (x & y)) & y) = x & y" ": T")
add_repl_test(functional_quantifiers-mixed_normalize_ex_fex
	"normalize ex x ((fex x (x' & a)) & x) != 0" ": a != 0")
add_repl_test(functional_quantifiers-mixed_qelim_ex_fex
	"qelim ex a (b = 0 && (fex b (a & b)) != 0)" ": b = 0")
add_repl_test(functional_quantifiers-mixed_qelim_ex_fall
	"qelim ex y ((fall y (x | y)) & y) = 0" ": T")

# false validity controls
add_repl_test(functional_quantifiers-invalid_fex
	"valid (fex x (x & y)) = 0" ": F")
add_repl_test(functional_quantifiers-invalid_fall
	"valid (fall x (x | y)) = 1" ": F")

# closed constants
add_repl_test(functional_quantifiers-closed_fex
	"valid (fex p p) = 1" ": T")
add_repl_test(functional_quantifiers-closed_fall
	"valid (fall p p) = 0" ": T")
add_repl_test(functional_quantifiers-closed_fex_fall
	"valid (fex x fall y (x | y)) = 1" ": T")

# the t of a stream offset is not taken for a binder named t
add_repl_test(functional_quantifiers-offset_not_renamed
	"normalize (fex t (o1[t] & t)) = 0 && o2[t] = 0"
	"always \\(o1\\[t\\]:tau = 0 && o2\\[t\\]:tau = 0\\)")
