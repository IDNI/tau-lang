#
# order normal form (cnf) tests
#

#
# wff
#

add_repl_test(normal_forms-cnf_wff_T "cnf T" ": T")
add_repl_test(normal_forms-cnf_wff_F "cnf F" ": F")
add_repl_test(normal_forms-cnf_wff_dist_right "cnf (X=0 || (Y=0 && Z=0))" ": \\(X = 0 \\|\\| Y = 0\\) && \\(X = 0 \\|\\| Z = 0\\)")
add_repl_test(normal_forms-cnf_wff_dist_left "cnf ((X=0 && Y=0) || Z=0)" ": \\(X = 0 \\|\\| Z = 0\\) && \\(Z = 0 \\|\\| Y = 0\\)")

#
# bf
#

add_repl_test(normal_forms-cnf_bf_1 "cnf 1" ": 1")
add_repl_test(normal_forms-cnf_bf_0 "cnf 0" ": 0")
add_repl_test(normal_forms-cnf_bf_dist_right "cnf (X | (Y & Z))" ": \\(.\\|.\\)&\\(.\\|.\\)|")
add_repl_test(normal_forms-cnf_bf_dist_left "cnf ((X & Y) | Z)" ": \\(.\\|.\\)&\\(.\\|.\\)")
