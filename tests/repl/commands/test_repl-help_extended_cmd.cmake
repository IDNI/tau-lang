#
# help command — extended coverage for previously untested topics
#

include(add_repl_test)

# run command
add_repl_test(help_run_cmd        "help run"     "run command")
add_repl_test(help_run_short_cmd  "h run"        "run command")
add_repl_test(help_r_cmd          "help r"       "run command")

# solve command
add_repl_test(help_solve_cmd      "help solve"   "solve command")
add_repl_test(help_s_solve_cmd    "h solve"      "solve command")

# lgrs command
add_repl_test(help_lgrs_cmd       "help lgrs"    "lgrs command")

# sat/valid/unsat
add_repl_test(help_sat_cmd        "help sat"     "sat command")
add_repl_test(help_valid_cmd      "help valid"   "valid command")
add_repl_test(help_unsat_cmd      "help unsat"   "unsat command")

# ltl command
add_repl_test(help_ltl_cmd        "help ltl"     "ltl command")

# subst command
add_repl_test(help_subst_cmd      "help subst"   "substitute command")
add_repl_test(help_sub_short_cmd  "help s"       "substitute command")

# inst command
add_repl_test(help_inst_cmd       "help inst"    "instantiate command")
add_repl_test(help_i_cmd          "help i"       "instantiate command")

# set/get/enable/disable/toggle
add_repl_test(help_set_cmd        "help set"     "set")
add_repl_test(help_get_cmd        "help get"     "get")
add_repl_test(help_enable_cmd     "help enable"  "enable")
add_repl_test(help_disable_cmd    "help disable" "disable")
add_repl_test(help_toggle_cmd     "help toggle"  "toggle")

# definitions command
add_repl_test(help_defs_cmd       "help defs"        "definitions command")
add_repl_test(help_definitions_cmd "help definitions" "definitions command")

# clear command
add_repl_test(help_clear_cmd      "help clear"   "clear")
add_repl_test(help_c_cmd          "help c"       "clear")

# anf/pnf: the anf_cmd/pnf_cmd grammar rules are commented out in
# parser/tau.tgf ("Commented out, not implemented yet"), so "anf"/"pnf"
# are not recognized symbols at all -- "help anf"/"help pnf" fall through
# to generic input instead of showing the command's help text. Marked
# WILL_FAIL for the same reason as test_repl-anf_pnf_cmd.cmake's tests.
add_repl_test(help_anf_cmd        "help anf"     "not yet implemented")
add_repl_test(help_pnf_cmd        "help pnf"     "not yet implemented")
set_tests_properties(
    test_repl-help_anf_cmd test_repl-help_pnf_cmd
    PROPERTIES WILL_FAIL TRUE
)

# new commands
add_repl_test(help_whatis_cmd     "help whatis"  "whatis command")
add_repl_test(help_reset_cmd      "help reset"   "reset command")

# examples
add_repl_test(help_examples_cmd   "help examples" "examples")
add_repl_test(help_example_cmd    "help example"  "examples")
