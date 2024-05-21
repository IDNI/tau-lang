#
# order normal form (cnf) tests
#

#
# wff
#

# T
add_repl_test(normal_forms-cnf_wff_T "cnf T" ": T")

# F
add_repl_test(normal_forms-cnf_wff_F "cnf F" ": F")

# (X=0 || (Y=0 && Z=0))
add_repl_test(normal_forms-cnf_wff_dist_right "cnf (X=0 || (Y=0 && Z=0))" ": \\(X = 0 \\|\\| Y = 0\\) && \\(X = 0 \\|\\| Z = 0\\)")

# ((X=0 && Y=0) | Z=0)
add_repl_test(normal_forms-cnf_wff_dist_left "cnf ((X=0 && Y=0) || Z=0)" ": \\(X = 0 \\|\\| Z = 0\\) && \\(Y = 0 \\|\\| Z = 0\\)")

#
# bf
#

# 0
add_repl_test(normal_forms-cnf_bf_1 "cnf 1" ": 1")

# 1
add_repl_test(normal_forms-cnf_bf_0 "cnf 0" ": 0")

# TODO (HIGH) review the following tests (they are flaky due to the use of std::set)
# (X | (Y & Z))
add_repl_test(normal_forms-cnf_bf_dist_right "cnf (X | (Y & Z))" ": \\(. \\| .\\) & \\(. \\| .\\)|")

# ((X & Y) | Z)
add_repl_test(normal_forms-cnf_bf_dist_left "cnf ((X & Y) | Z)" ": \\(. \\| .\\) & \\(. \\| .\\)")
