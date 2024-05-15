cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# In these tests cases we check the results obtained in the substitution command.
# The output of the tests cases must be validated against expected results.

# BF Substitution

add_test(NAME test_repl-substitution_cmd_bf1
        COMMAND bash -c "echo 'm x. m 1. s x @0 @1. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-substitution_cmd_bf1 PROPERTIES
        PASS_REGULAR_EXPRESSION ": 1")

add_test(NAME test_repl-substitution_cmd_bf2
        COMMAND bash -c "echo \"m ax + bx'. m y&z. s x @0 @1. q\" | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-substitution_cmd_bf2 PROPERTIES
        PASS_REGULAR_EXPRESSION ": \\(a & y & z\\) \\+ \\(b & \\(y & z\\)'\\)")

# TODO: Quantified variable is replaced by substitution.
add_test(NAME test_repl-substitution_cmd_bf3
        COMMAND bash -c "echo 'm fex k h & k. m v & w. s k @0 @1. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-substitution_cmd_bf3 PROPERTIES
        PASS_REGULAR_EXPRESSION ": fex k h & v & w")

# TODO: Variable is placed into scope of quantifier.
add_test(NAME test_repl-substitution_cmd_bf4
        COMMAND bash -c "echo 'm fex x x & y. m x. s y @0 @1. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-substitution_cmd_bf4 PROPERTIES
        PASS_REGULAR_EXPRESSION ": fex x x & x")

add_test(NAME test_repl-substitution_cmd_bf5
        COMMAND bash -c "echo 'm (x | (w & z)) & z. m b. s z @0 @1. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-substitution_cmd_bf5 PROPERTIES
        PASS_REGULAR_EXPRESSION ": \\(x | w & b\\) & b")

add_test(NAME test_repl-substitution_cmd_bf6
        COMMAND bash -c "echo 'm x & y. m T. s x @0 @1. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-substitution_cmd_bf6 PROPERTIES
        PASS_REGULAR_EXPRESSION "error: invalid argument")

# WFF Substitution

add_test(NAME test_repl-substitution_cmd_wff1
        COMMAND bash -c "echo 'm ?x && ?y. m T. s ?x @0 @1. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-substitution_cmd_wff1 PROPERTIES
        PASS_REGULAR_EXPRESSION ": T && \\?y")

# TODO: Negation symbol is not printed
add_test(NAME test_repl-substitution_cmd_wff2
        COMMAND bash -c "echo \"m ?a?x ^ ?b!?x. m ?y && ?z. s ?x @0 @1. q\" | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-substitution_cmd_wff2 PROPERTIES
        PASS_REGULAR_EXPRESSION ": \\(\\?a && \\?y && \\?z\\) \\^ \\(\\?b && \\(\\?y && \\?z\\)\\)")

# TODO: Quantified variable is replaced by substitution.
add_test(NAME test_repl-substitution_cmd_wff3
        COMMAND bash -c "echo 'm bool_ex ?k ?h && ?k. m ?v && ?w. s ?k @0 @1. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-substitution_cmd_wff3 PROPERTIES
        PASS_REGULAR_EXPRESSION ": bool_ex \\?k \\?h && \\?v && \\?w")

add_test(NAME test_repl-substitution_cmd_wff4
        COMMAND bash -c "echo 'm ex x x=0 && y=0. m ?x. s ?y @0 @1. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-substitution_cmd_wff4 PROPERTIES
        PASS_REGULAR_EXPRESSION ": ex x x = 0 && y = 0")

# TODO: Variable is placed into scope of quantifier.
add_test(NAME test_repl-substitution_cmd_wff5
        COMMAND bash -c "echo 'm bool_ex ?x ?x && ?y. m ?x. s ?y @0 @1. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-substitution_cmd_wff5 PROPERTIES
        PASS_REGULAR_EXPRESSION ": bool_ex \\?x \\?x && \\?x")

add_test(NAME test_repl-substitution_cmd_wff6
        COMMAND bash -c "echo 'm (?x || (?w && ?z)) && ?z. m ?b. s ?z @0 @1. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-substitution_cmd_wff6 PROPERTIES
        PASS_REGULAR_EXPRESSION ": \\(\\?x || \\?w && \\?b\\) && \\?b")

# TODO: We can substitute a bf into a wff
add_test(NAME test_repl-substitution_cmd_wff7
        COMMAND bash -c "echo 'm ?x && ?y. m 0. s ?x @0 @1. q' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
set_tests_properties(test_repl-substitution_cmd_wff7 PROPERTIES
        PASS_REGULAR_EXPRESSION ": 0 && \\?y")


