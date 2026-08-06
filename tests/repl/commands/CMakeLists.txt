cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# basic commands

include(test_repl-quit_cmd.cmake)
include(test_repl-version_cmd.cmake)
include(test_repl-clear_cmd.cmake)
include(test_repl-help_cmd.cmake)

# options commands

include(test_repl-get_cmd.cmake)
include(test_repl-set_cmd.cmake)
include(test_repl-enable_cmd.cmake)
include(test_repl-disable_cmd.cmake)
include(test_repl-toggle_cmd.cmake)
include(test_repl-option_aliases.cmake)

# commands for specs, formulas and terms

include(test_repl-valid_cmd.cmake)
include(test_repl-sat_cmd.cmake)
include(test_repl-unsat_cmd.cmake)
# AP-35: solver_cmd.cmake (not solve_cmd.cmake -- no such file ever existed,
# which is why the commented-out include below never worked) already
# covers the `solve` command's happy path with a bitvector/cvc5 equation.
# issue-60: plain, non-bv equations (`solve x:sbf = 1`, `solve x = 0`, ...)
# used to abort the REPL with std::logic_error "ba_types::type_tree: invalid
# ba_type_id" inside print_solver_cmd_solution; solve_cmd was passing it a
# grammar nonterminal instead of a BA type id. Fixed, and covered here.
include(test_repl-solver_cmd.cmake)
# issue-60: `lgrs` used to abort on every equation tried (bf, bv, with/without
# cvc5) -- an assertion failure in tree<node>::child_tree, hit inside
# api<node>::lgrs(tref), which indexed the wff rather than the bf_eq when
# screening for non-Boolean operations. On top of that it shared solve_cmd's
# BA-type-id defect. Both fixed; see also the "lgrs" test in
# tests/api/test_api-tref_api.cpp for the API-level view.
include(test_repl-lgrs_cmd.cmake)
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
# ADT (algebraic data type) definitions (def_type_cmd) and their effect on
# normalize/sat/unsat/solve/run once a `type` statement has been accepted.
include(test_repl-adt.cmake)


# the `run` command: interactive interpreter execution driven from stdin
include(test_repl-run_cmd.cmake)

# CLI entry points of the tau executable itself (not REPL commands):
# --version, the interactive REPL loop, and the specification-file path.
include(test_repl-cli.cmake)

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