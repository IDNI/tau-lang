cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# basic commands

include(test_repl-quit_cmd.cmake)
include(test_repl-version_cmd.cmake)
#include(test_repl-clear_cmd.cmake)
include(test_repl-help_cmd.cmake)

# options commands

include(test_repl-get_cmd.cmake)
include(test_repl-set_cmd.cmake)
#include(test_repl-enable_cmd.cmake)
#include(test_repl-disable_cmd.cmake)
include(test_repl-toggle_cmd.cmake)

# commands for specs, formulas and terms

include(test_repl-valid_cmd.cmake)
include(test_repl-sat_cmd.cmake)
include(test_repl-unsat_cmd.cmake)
# AP-35: solver_cmd.cmake (not solve_cmd.cmake -- no such file ever existed,
# which is why the commented-out include below never worked) already
# covers the `solve` command's happy path with a bitvector/cvc5 equation.
# A plain, non-bv equation (e.g. `solve x | y = 0`, `solve x = 0`) currently
# aborts the REPL (std::logic_error "ba_types::type_tree: invalid
# ba_type_id" inside print_solver_cmd_solution), reproducible independent
# of any change here, so no additional `solve` test is added for that case.
include(test_repl-solver_cmd.cmake)
# `lgrs` is not wired in: every equation tried (bf, bv, with/without cvc5)
# currently aborts the REPL (assertion failure in tree<node>::child_tree,
# hit inside api<node>::lgrs(tref)'s norm_all_equations()/
# apply_all_xor_def() preprocessing -- see also the "lgrs" test in
# tests/api/test_api-tref_api.cpp for the same finding at the API level).
# That is a pre-existing bug in src/, out of scope for a tests-only change,
# so there is currently no working `lgrs` REPL command to test.
include(test_repl-normalize_cmd.cmake)
include(test_repl-qelim_cmd.cmake)

# substitution and instantiation commands
include(test_repl-instantiation_cmd.cmake)
include(test_repl-substitution_cmd.cmake)

# normal form commands
include(test_repl-cnf_cmd.cmake)
include(test_repl-dnf_cmd.cmake)
include(test_repl-nnf_cmd.cmake)
include(test_repl-mnf_cmd.cmake)
# include(test_repl-onf_cmd.cmake)
include(test_repl-onf_cmd-crash_regression.cmake)

# history and definitions commands
include(test_repl-history_cmd.cmake)
include(test_repl-definition_cmd.cmake)
include(test_repl-io_definition_cmd.cmake)


# TODO (HIGH) add tests for help definition,...
# TODO (VERY_HIGH) add tests for normalize command
# TODO (VERY_HIGH) add tests  definition command
# TODO (VERY_HIGH) add tests for solving command for non-bv equations and
#                  for the lgrs command, once the crashes documented above
#                  are fixed in src/
# TODO (VERY_HIGH) add tests for configuration commands
# TODO (VERY_HIGH) add tests for examples command
# TODO (VERY_HIGH) add tests for outputs command

# sample for comparing output with a file
#add_test(NAME test_name COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> | diff - test_name.dump")