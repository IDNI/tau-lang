#
# sat command
#

include(add_repl_test)

# the sat command checks if a Tau formula is satisfiable and prints T or F
add_repl_test(sat_cmd-t "sat T" ": T")
add_repl_test(sat_cmd-f "sat F" ": F")
add_repl_test(sat_cmd-formula "sat x = 0" ": T")

# GitHub #72: a conjunction of N clauses with pairwise disjoint variable
# support used to be decided by a single Boole decomposition over the whole
# formula -- a 2^N Shannon expansion, since no branch ever simplified a
# sibling clause. N=15 took ~15 s and N=18 timed out before the per-component
# block split; each component is now decided on its own, so N=20 is instant.
add_test(NAME "test_repl-sat_cmd-issue72_disjoint_support_ladder"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"sat always ((o0[t]=1 -> o1[t]=1) && (o10[t]=1 -> o11[t]=1) && (o20[t]=1 -> o21[t]=1) && (o30[t]=1 -> o31[t]=1) && (o40[t]=1 -> o41[t]=1) && (o50[t]=1 -> o51[t]=1) && (o60[t]=1 -> o61[t]=1) && (o70[t]=1 -> o71[t]=1) && (o80[t]=1 -> o81[t]=1) && (o90[t]=1 -> o91[t]=1) && (o100[t]=1 -> o101[t]=1) && (o110[t]=1 -> o111[t]=1) && (o120[t]=1 -> o121[t]=1) && (o130[t]=1 -> o131[t]=1) && (o140[t]=1 -> o141[t]=1) && (o150[t]=1 -> o151[t]=1) && (o160[t]=1 -> o161[t]=1) && (o170[t]=1 -> o171[t]=1) && (o180[t]=1 -> o181[t]=1) && (o190[t]=1 -> o191[t]=1))\"")
set_tests_properties("test_repl-sat_cmd-issue72_disjoint_support_ladder" PROPERTIES
	PASS_REGULAR_EXPRESSION ": T"
	FAIL_REGULAR_EXPRESSION "Error"
	TIMEOUT 60)
