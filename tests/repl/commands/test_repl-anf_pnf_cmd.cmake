#
# anf and pnf commands
# TODO: implement anf/pnf normal form commands (currently stubs)
#

include(add_repl_test)

# These tests use add_repl_test (not add_repl_test_fail) so they correctly
# FAIL until anf/pnf commands are properly implemented.
# Marked WILL_FAIL so CTest reports them as expected failures.
add_repl_test(anf_cmd-T         "anf T"       ".")
add_repl_test(anf_cmd-F         "anf F"       ".")
add_repl_test(anf_cmd-wff       "anf X = 0"   ".")
add_repl_test(anf_cmd-bf        "anf X & Y"   ".")

add_repl_test(pnf_cmd-T         "pnf T"       ".")
add_repl_test(pnf_cmd-F         "pnf F"       ".")
add_repl_test(pnf_cmd-wff       "pnf ex x x = 0" ".")

# TODO: remove WILL_FAIL once anf/pnf commands are implemented
set_tests_properties(
    test_repl-anf_cmd-T test_repl-anf_cmd-F test_repl-anf_cmd-wff test_repl-anf_cmd-bf
    test_repl-pnf_cmd-T test_repl-pnf_cmd-F test_repl-pnf_cmd-wff
    PROPERTIES WILL_FAIL TRUE
)
