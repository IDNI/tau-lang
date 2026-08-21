#
# IN-R1 / LA-N2 / IN-N13 regressions through the REPL (api-mediated path).
#
# Before the fix, normalize() rewrote every topmost non-always temporal
# operator into `sometimes(first operand)`: binary operators lost their right
# operand, A/E were erased (nesting a temporal quantifier and aborting the
# Debug REPL on an assert), and `F` lost its realizability semantics.
#

include(add_repl_test)

# the right operand of U survives normalization
add_repl_test(ctl_star-normalize_keeps_U_rhs
	"normalize (o1[t] = 1) U (o1[t] = 0)" "U")
add_test(NAME "test_repl-ctl_star-normalize_U_not_sometimes"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"normalize (o1[t] = 1) U (o1[t] = 0)\"")
set_tests_properties("test_repl-ctl_star-normalize_U_not_sometimes" PROPERTIES
	PASS_REGULAR_EXPRESSION "%1"
	FAIL_REGULAR_EXPRESSION "sometimes")

# U with a contradictory target is UNSAT (was T: the target was dropped)
add_repl_test(ctl_star-sat_U_contradictory_target
	"sat (o1[t] = 1) U (o1[t] = 0 && o1[t] = 1)" ": F")

# F over an input is realizability, not existential-trace semantics
add_repl_test(ctl_star-sat_F_input_unrealizable "sat F i1[t] = 1" ": F")
add_repl_test(ctl_star-sat_F_output_realizable "sat F o1[t] = 1" ": T")

# crash regressions: these aborted the Debug REPL (normalizer
# !find_top(wff_always) / satisfiability st.size() < 2 asserts)
add_repl_test(ctl_star-sat_A_always_input_no_abort
	"fragment ctl_star. sat A (always i1[t] = 1)" ": F")
add_repl_test(ctl_star-sat_E_always_output_no_abort
	"fragment ctl_star. sat E (always o1[t] = 1)" ": T")
add_repl_test(ctl_star-sat_A_sometimes_contradiction_no_abort
	"fragment ctl_star. sat A (sometimes (o1[t] = 1 && o1[t] = 0))" ": F")
# valid_cmd is not fragment-gated (IN-R5, Batch 2); it must at least not abort
add_repl_test_fail(ctl_star-valid_A_always_no_abort
	"valid A (always o1[t] = 1)" ": [TF]")

# CTL* verdicts through the api path (IN-R7): A constrains its body
add_repl_test(ctl_star-sat_A_F_input "fragment ctl_star. sat A (F i1[t] = 1)" ": F")
add_repl_test(ctl_star-sat_A_F_output "fragment ctl_star. sat A (F o1[t] = 1)" ": T")
add_repl_test(ctl_star-sat_always_A_output
	"fragment ctl_star. sat always (A (o1[t] = 1))" ": T")
# unsound placements are refused with a diagnostic, not answered
add_repl_test_fail(ctl_star-sat_A_under_F_refused
	"fragment ctl_star. sat F (A (o1[t] = 1))" "not soundly encodable")
