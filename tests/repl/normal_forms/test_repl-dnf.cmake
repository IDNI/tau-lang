#
# order normal form (dnf) tests
#

#
# wff
#

# T
add_repl_test(normal_forms-dnf_wff_T "dnf T" ": T")

# F
add_repl_test(normal_forms-dnf_wff_F "dnf F" ": F")

# (X=0 || (Y=0 && Z=0))
add_repl_test(normal_forms-dnf_wff_dist_right "dnf (X=0 && (Y=0 || Z=0))" ": X = 0 && Y = 0 \\|\\| X = 0 && Z = 0")

# ((X=0 && Y=0) | Z=0)
add_repl_test(normal_forms-dnf_wff_dist_left "dnf ((X=0 || Y=0) && Z=0)" ": X = 0 && Z = 0 \\|\\| Y = 0 && Z = 0")

#
# bf
#

# 1
add_repl_test(normal_forms-dnf_bf_1 "dnf 1" ": 1")

# 0
add_repl_test(normal_forms-dnf_bf_0 "dnf 0" ": 0")

# TODO (HIGH) review the following tests (they are flaky due to the use of std::set)
# (X | (Y & Z))
#add_repl_test(normal_forms-dnf_bf_dist_right "dnf (X & (Y | Z))" ": X & Y \\| X & Z|: X & Z \\| X & Y|")

# ((X & Y) | Z)
#add_repl_test(normal_forms-dnf_bf_dist_left "dnf ((X | Y) & Z)" ": X & Z \\| Y & Z|: Z & X \\| Y & Z|: Y & Z \\| Z & X")
