#
# fragment command + CTL* gate (AP2-2)
#

include(add_repl_test)

# switching fragments echoes the active fragment
add_repl_test(fragment_cmd-ltl "fragment ltl" "fragment: ltl")
add_repl_test(fragment_cmd-ctl_star "fragment ctl_star" "fragment: ctl_star")

# AP2-2: in the default (ltl) fragment, CTL* operators (A/E/-) must be
# rejected with the gate's diagnostic instead of being forwarded to the LTL
# pipeline.  add_repl_test_fail is used because the diagnostic contains
# "Error"; matching the gate's own message distinguishes the rejection from
# any other failure.
add_repl_test_fail(fragment_gate-sat_blocks_A_by_default
	"sat A (o1[t] = 1)" "require the ctl_star")
add_repl_test_fail(fragment_gate-normalize_blocks_A_by_default
	"normalize A (o1[t] = 1)" "require the ctl_star")
add_repl_test_fail(fragment_gate-run_blocks_A_by_default
	"run A (o1[t] = 1)" "require the ctl_star")
