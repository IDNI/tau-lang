cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# basic commands

include(test_repl-quit_cmd.cmake)
include(test_repl-version_cmd.cmake)
include(test_repl-help_cmd.cmake)
include(test_repl-help_extended_cmd.cmake)

# options commands

include(test_repl-get_cmd.cmake)
include(test_repl-get_set_extended_cmd.cmake)
include(test_repl-set_cmd.cmake)
include(test_repl-enable_disable_cmd.cmake)
include(test_repl-toggle_cmd.cmake)

# commands for specs, formulas and terms

include(test_repl-valid_cmd.cmake)
include(test_repl-sat_cmd.cmake)
include(test_repl-unsat_cmd.cmake)
include(test_repl-realizable_cmd.cmake)
include(test_repl-unrealizable_cmd.cmake)
include(test_repl-solver_cmd.cmake)
include(test_repl-normalize_cmd.cmake)
include(test_repl-qelim_cmd.cmake)
include(test_repl-run_cmd.cmake)

# substitution and instantiation commands
include(test_repl-instantiation_cmd.cmake)
include(test_repl-substitution_cmd.cmake)

# normal form commands
include(test_repl-cnf_cmd.cmake)
include(test_repl-dnf_cmd.cmake)
include(test_repl-nnf_cmd.cmake)
include(test_repl-mnf_cmd.cmake)
include(test_repl-anf_pnf_cmd.cmake)
# include(test_repl-onf_cmd.cmake)
include(test_repl-onf_cmd-crash_regression.cmake)

# history and definitions commands
include(test_repl-history_cmd.cmake)
include(test_repl-definition_cmd.cmake)
include(test_repl-io_definition_cmd.cmake)


# session management and type inspection
include(test_repl-reset_cmd.cmake)
include(test_repl-whatis_cmd.cmake)

# blasting on/off equivalence
include(test_repl-blasting_equiv.cmake)

# LTL strategy execution
include(test_repl-strategy_exec.cmake)

# `tau compile` verb
include(test_repl-compile_verb.cmake)

# TODO (HIGH) add tests for help definition,...
# TODO (VERY_HIGH) add tests for normalize command
# TODO (VERY_HIGH) add tests  definition command
# TODO (VERY_HIGH) add tests for configuration commands
# TODO (VERY_HIGH) add tests for examples command
# TODO (VERY_HIGH) add tests for outputs command

# sample for comparing output with a file
#add_test(NAME test_name COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> | diff - test_name.dump")