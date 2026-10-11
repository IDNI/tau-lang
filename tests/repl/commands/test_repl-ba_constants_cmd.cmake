#
# BA constant syntaxes through the REPL surface (BA1-30)
#

include(add_repl_test)

# qlt: satisfiability verdicts and the interval printed form
add_repl_test(ba_qlt-sat_neq "sat x : qlt != {0}:qlt" ": T")
add_repl_test(ba_qlt-unsat "sat x : qlt = {0}:qlt && x : qlt != {0}:qlt" ": F")
add_repl_test(ba_qlt-interval_print "n {(0, 1)}:qlt x = 0" "\\(0, 1\\)")

# qint
add_repl_test(ba_qint-sat "sat x : qint != {0}:qint" ": T")
# GitHub #186: endpoints are exact rationals, so 1/3 and its 16-digit
# decimal rounding are distinct.
add_repl_test(ba_qint-exact_endpoints_differ
	"normalize {[0,1/3)}:qint = {[0,0.3333333333333333)}:qint" ": F")
add_repl_test(ba_qint-exact_gap_is_nonempty
	"normalize ({[0,1/3)}:qint & {[0.3333333333333333,1)}:qint) = 0" ": F")
add_repl_test(ba_qint-exact_witness
	"normalize ex x:qint ((x = {[0,1/3)}:qint) && (x != {[0,0.3333333333333333)}:qint))" ": T")
add_repl_test(ba_qint-exact_print "n {[0,1/3)}:qint x = 0" "\\[0, 1/3\\)")

# hsb
add_repl_test(ba_hsb-sat "sat x : hsb != 0" ": T")
