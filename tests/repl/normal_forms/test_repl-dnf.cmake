#
# order normal form (dnf) tests
#

#
# wff
#

add_repl_test(normal_forms-dnf_wff_T "dnf T" ": T")
add_repl_test(normal_forms-dnf_wff_F "dnf F" ": F")
add_repl_test(normal_forms-dnf_wff_dist_right "dnf (X=0 && (Y=0 || Z=0))" ": X = 0 && Y = 0 \\|\\| X = 0 && Z = 0")
add_repl_test(normal_forms-dnf_wff_dist_left "dnf ((X=0 || Y=0) && Z=0)" ": X = 0 && Z = 0 \\|\\| Y = 0 && Z = 0")

#
# bf
#

add_repl_test(normal_forms-dnf_bf_1 "dnf 1" ": 1")
add_repl_test(normal_forms-dnf_bf_0 "dnf 0" ": 0")
add_repl_test(normal_forms-dnf_bf_dist_right "dnf (X & (Y | Z))" ": . & . \\| . & .")
add_repl_test(normal_forms-dnf_bf_dist_left "dnf ((X | Y) & Z)" ": . & . \\| . & .")
