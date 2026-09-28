#
# NF-N1 regression: onf_cmd used to feed the variable argument (n[1], per the
# grammar `"onf" __ variable __ onf_cmd_arg`) through get_any/apply_all_defs,
# which resolves through tau_spec::add and hits its default-case assert(false)
# on a bare `variable` node -- aborting in debug builds and returning nullptr
# in release. These only check that onf no longer crashes on well-formed
# input; see test_repl-onf_cmd.cmake (disabled) for the still-unverified
# semantic correctness of onf's output.
#

add_repl_test(onf_cmd-no_crash_on_conjunction "onf x (xy = 0)" ": xy = 0")
add_repl_test(onf_cmd-no_crash_on_variable_equality "onf x (x = y)" ": x = y")

# onf takes a formula: a term argument is rejected instead of the whole line
# being stored as a term (`onf x y` used to print `%1: onfxy`)
add_repl_test_fail(onf_cmd-term_rejected "onf x y" "Invalid formula")
