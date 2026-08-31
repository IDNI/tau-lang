#
# history commands
#

include(add_repl_test)

# history list command
add_repl_test(history_list_cmd-empty "history" "history is empty")
add_repl_test(history_list_cmd-non_empty "1. history" ": 1")

# history store command
add_repl_test(history_store_cmd-bf "1" ": 1")
add_repl_test(history_store_cmd-wff "T" ": T")
add_repl_test(history_store_cmd-tau "{T.}" "1")

add_repl_test_fail(history_store_cmd-tau_fail "always o1[t]' = {x}:tau i1[t]" "Syntax Error")
add_repl_test_fail(history_store_cmd-sbf_fail "always o1[t]' = {x = 0}:sbf i1[t]" "Syntax Error")

# history print command
add_repl_test(history_print_cmd-empty_absolute "history %1" "history is empty")
add_repl_test(history_print_cmd-empty_relative "history %-1" "history is empty")
add_repl_test(history_print_cmd-non_empty_absolute "1. history %1" ": 1")
add_repl_test(history_print_cmd-non_empty_relative "1. history %" ": 1")
add_repl_test(history_print_cmd-non_empty_last "1. T. history %-1" ": 1")

# bare-statement pretty-printer coverage, migrated from test_tau_tree_printers.cpp

# "pretty print identical to input" (test_tau_tree_printers.cpp:24-35)
add_repl_test(history_store_cmd-printers-identical_01_x_eq_1          "x = 1."       ": x = 1")
add_repl_test(history_store_cmd-printers-identical_02_ab_eq_0         "ab = 0."      ": ab = 0")
add_repl_test(history_store_cmd-printers-identical_03_x_or_ba_ne_1    "x|ba != 1."   ": x\\|ba != 1")
add_repl_test(history_store_cmd-printers-identical_04_x_xor_ba_ne_0   "x^ba != 0."   ": x\\^ba != 0")
add_repl_test(history_store_cmd-printers-identical_05_paren_a_or_b_x_eq_0 "(a|b)x = 0." ": \\(a\\|b\\)x = 0")

# "pretty print different from input" (test_tau_tree_printers.cpp:37-45)
add_repl_test(history_store_cmd-printers-different_01_paren_x_eq_1       "(x) = 1."            ": x = 1")
add_repl_test(history_store_cmd-printers-different_02_all_x_all_y_xy_eq_0 "all x all y xy = 0." ": all b2, b1 b2 b1 = 0")

# quantifiers -- raw (unnormalized) printed form, assertion 1
add_repl_test(history_store_cmd-quantifiers-ex_01_ex_x_x_eq_0        "ex x x=0."             ": ex b1 b1 = 0")
add_repl_test(history_store_cmd-quantifiers-ex_02_ex_x_y_xy_eq_0     "ex x,y xy=0."          ": ex b2, b1 b2 b1 = 0")
add_repl_test(history_store_cmd-quantifiers-ex_03_ex_x_ex_y_xy_eq_0  "ex x ex y xy=0."       ": ex b2, b1 b2 b1 = 0")
# "all" TEST_CASE, all_cases (test_integration-quantifiers.cpp:18-22, 67)
add_repl_test(history_store_cmd-quantifiers-all_01_all_x_x_ne_0        "all x x!=0."           ": all b1 b1 != 0")
add_repl_test(history_store_cmd-quantifiers-all_02_all_x_y_xy_ne_0     "all x,y xy!=0."        ": all b2, b1 b2 b1 != 0")
add_repl_test(history_store_cmd-quantifiers-all_03_all_x_all_y_xy_ne_0 "all x all y xy!=0."    ": all b2, b1 b2 b1 != 0")
# "ex all" TEST_CASE, ex_all_cases (test_integration-quantifiers.cpp:24-28, 68)
add_repl_test(history_store_cmd-quantifiers-ex_all_01_ex_x_all_y_x_eq_y "ex x all y x=y."                ": ex b2 all b1 b2 = b1")
add_repl_test(history_store_cmd-quantifiers-ex_all_02_ex_xy_all_wz_x_eq_w_and_y_eq_z "ex x,y all w,z x=w&&y=z." ": ex b4, b3 all b2, b1 b4 = b2 && b3 = b1")
# "all ex" TEST_CASE, all_ex_cases (test_integration-quantifiers.cpp:30-34, 69)
add_repl_test(history_store_cmd-quantifiers-all_ex_01_all_x_ex_y_x_eq_y "all x ex y x=y."                ": all b2 ex b1 b2 = b1")
add_repl_test(history_store_cmd-quantifiers-all_ex_02_all_xy_ex_wz_x_eq_w_and_y_eq_z "all x,y ex w,z x=w && y=z." ": all b4, b3 ex b2, b1 b4 = b2 && b3 = b1")
# referencing a history location beyond the stored entries, absolute and
# relative (the "%N does not exist" report and its TAU_LOG_ERROR companion)
add_repl_test_fail(history_ref-absolute_missing "1. mnf %5" "does not exist")
add_repl_test_fail(history_ref-relative_missing "1. dnf %-5" "does not exist")
