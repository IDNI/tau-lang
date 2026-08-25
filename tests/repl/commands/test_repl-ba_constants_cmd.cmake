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

# hsb
add_repl_test(ba_hsb-sat "sat x : hsb != 0" ": T")
