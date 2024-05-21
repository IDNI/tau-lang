#
# order normal form tests
#

# T
add_repl_test(normal_forms-onf_T "onf X T" "T")

# F
add_repl_test(normal_forms-onf_F "onf X F" "F")

# X=0
add_repl_test(normal_forms-onf_X_eq_0 "onf X (X = 0)" "F")

# Y=0
add_repl_test(normal_forms-onf_Y_eq_0 "onf X (Y = 0)" "Y")

# X!=0
add_repl_test(normal_forms-onf_X_neq_0 "onf X (X != 0)" "F")

# Y!=0
add_repl_test(normal_forms-onf_Y_neq_0 "onf X (Y != 0)" "Y")
