#
# order normal form (dnf) tests
#

#
# wff
#

# T
add_test(NAME test_repl-normal_forms-dnf_wff_T
COMMAND bash -c "echo 'dnf T. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normal_forms-dnf_wff_T PROPERTIES
PASS_REGULAR_EXPRESSION ": T")

# F
add_test(NAME test_repl-normal_forms-dnf_wff_F
COMMAND bash -c "echo 'dnf F. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normal_forms-dnf_wff_F PROPERTIES
PASS_REGULAR_EXPRESSION ": F")

# (X=0 || (Y=0 && Z=0))
add_test(NAME test_repl-normal_forms-dnf_wff_dist_right
COMMAND bash -c "echo 'dnf (X=0 && (Y=0 || Z=0)). quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normal_forms-dnf_wff_dist_right PROPERTIES
PASS_REGULAR_EXPRESSION ": X = 0 && Y = 0 \\|\\| X = 0 && Z = 0")

# ((X=0 && Y=0) | Z=0)
add_test(NAME test_repl-normal_forms-dnf_wff_dist_left
COMMAND bash -c "echo 'dnf ((X=0 || Y=0) && Z=0). quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normal_forms-dnf_wff_dist_left PROPERTIES
PASS_REGULAR_EXPRESSION ": X = 0 && Z = 0 \\|\\| Y = 0 && Z = 0")

#
# bf
#

# 1
add_test(NAME test_repl-normal_forms-dnf_bf_1
COMMAND bash -c "echo 'dnf 1. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normal_forms-dnf_bf_1 PROPERTIES
PASS_REGULAR_EXPRESSION ": 1")

# 0
add_test(NAME test_repl-normal_forms-dnf_bf_0
COMMAND bash -c "echo 'dnf 0. quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-normal_forms-dnf_bf_0 PROPERTIES
PASS_REGULAR_EXPRESSION ": 0")

# TODO (HIGH) review the following tests (they are flaky due to the use of std::set)
# (X | (Y & Z))
#add_test(NAME test_repl-normal_forms-dnf_bf_dist_right
#COMMAND bash -c "echo 'dnf (X & (Y | Z)). quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-normal_forms-dnf_bf_dist_right PROPERTIES
#PASS_REGULAR_EXPRESSION ": X & Y \\| X & Z|: X & Z \\| X & Y|")

# ((X & Y) | Z)
#add_test(NAME test_repl-normal_forms-dnf_bf_dist_left
#COMMAND bash -c "echo 'dnf ((X | Y) & Z). quit' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
#set_tests_properties(test_repl-normal_forms-dnf_bf_dist_left PROPERTIES
#PASS_REGULAR_EXPRESSION ": X & Z \\| Y & Z|: Z & X \\| Y & Z|: Y & Z \\| Z & X")
