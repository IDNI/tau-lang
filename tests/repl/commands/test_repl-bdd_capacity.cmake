#
# A full bdd node table (default ids are 18 bits wide, 2^18 nodes) must end
# the command in an error, never in a verdict. The formula is satisfiable:
# 17 equal pairs x_i = y_i whose variables the tautology prefix orders
# grouped (all x before all y), so its bdd needs ~2^17 nodes plus
# intermediates.
#

set(TAU_BDD_FULL_SAT "sat {((((((x0|x0')&(x1|x1'))&((x2|x2')&(x3|x3')))&(((x4|x4')&(x5|x5'))&((x6|x6')&(x7|x7'))))&((((x8|x8')&(x9|x9'))&((x10|x10')&(x11|x11')))&(((x12|x12')&(x13|x13'))&((x14|x14')&((x15|x15')&(x16|x16'))))))&(((((y0|y0')&(y1|y1'))&((y2|y2')&(y3|y3')))&(((y4|y4')&(y5|y5'))&((y6|y6')&(y7|y7'))))&((((y8|y8')&(y9|y9'))&((y10|y10')&(y11|y11')))&(((y12|y12')&(y13|y13'))&((y14|y14')&((y15|y15')&(y16|y16')))))))&(((((x0^y0)'&(x1^y1)')&((x2^y2)'&(x3^y3)'))&(((x4^y4)'&(x5^y5)')&((x6^y6)'&(x7^y7)')))&((((x8^y8)'&(x9^y9)')&((x10^y10)'&(x11^y11)'))&(((x12^y12)'&(x13^y13)')&((x14^y14)'&((x15^y15)'&(x16^y16)')))))}:sbf != 0")

add_test(NAME "test_repl-bdd_capacity-overflow_is_no_verdict"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"${TAU_BDD_FULL_SAT}\" 2>&1")
set_tests_properties("test_repl-bdd_capacity-overflow_is_no_verdict" PROPERTIES
	PASS_REGULAR_EXPRESSION "bdd node table exhausted"
	FAIL_REGULAR_EXPRESSION ": T|: F")

# The session goes on after an overflow and nothing computed on the full
# table is reused: the same query fails again the same way instead of being
# answered from a cache, and later queries that fit -- a constant already
# interned, or no sbf at all -- are answered correctly. (The overflow
# happens while its line is parsed, when sbf constants are built, so the
# queries go on lines of their own.)
set(_bdd_session "${CMAKE_CURRENT_BINARY_DIR}/bdd_capacity_session.tau")
file(WRITE "${_bdd_session}" "${TAU_BDD_FULL_SAT}\n${TAU_BDD_FULL_SAT}\nsat {x0}:sbf != 0\nsat x = 0 && x != 0\nquit\n")
add_test(NAME "test_repl-bdd_capacity-session_continues"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -S error < ${_bdd_session} 2>&1")
set_tests_properties("test_repl-bdd_capacity-session_continues" PROPERTIES
	PASS_REGULAR_EXPRESSION "bdd node table exhausted.*bdd node table exhausted.*%1[^:]*: T.*%2[^:]*: F"
	FAIL_REGULAR_EXPRESSION "%1[^:]*: F")
