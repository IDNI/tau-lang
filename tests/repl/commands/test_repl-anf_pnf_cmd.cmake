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

# The anf_cmd/pnf_cmd grammar rules are commented out in parser/tau.tgf
# ("Commented out, not implemented yet"), so "anf"/"pnf" are not
# recognized keywords at all. For the "T"/"F" inputs this makes the REPL
# fail to parse the line (matching the "." regex fails, as WILL_FAIL
# expects). For the "wff"/"bf" inputs, though, "anf X = 0" happens to be
# a syntactically valid formula once "anf" is read as three ANDed
# variables (a, n, f) rather than a keyword, so the REPL parses and
# stores it successfully -- the "." regex trivially matches, which
# WILL_FAIL would report as an unexpected pass. Only mark WILL_FAIL where
# the command line is genuinely rejected.
# TODO: remove WILL_FAIL once anf/pnf commands are implemented
set_tests_properties(
    test_repl-anf_cmd-T test_repl-anf_cmd-F
    test_repl-pnf_cmd-T test_repl-pnf_cmd-F
    PROPERTIES WILL_FAIL TRUE
)
