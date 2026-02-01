#
# order normal form tests
#

# TODO (HIGH) the results should be reviewed and checked for correctness
add_repl_test(normal_forms-onf_T "onf X T" "T")
add_repl_test(normal_forms-onf_F "onf X F" "F")
add_repl_test(normal_forms-onf_X_eq_0 "onf X (X = 0)" "F")
add_repl_test(normal_forms-onf_Y_eq_0 "onf X (Y = 0)" "Y")
add_repl_test(normal_forms-onf_X_neq_0 "onf X (X != 0)" "F")
add_repl_test(normal_forms-onf_Y_neq_0 "onf X (Y != 0)" "Y")
