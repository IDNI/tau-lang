cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# In these tests cases we only test that the commands is executed, we don't check
# if the results are the proper ones.

include(test_repl-definition_cmd.cmake)
include(test_repl-qelim_cmd.cmake)
include(test_repl-help_cmd.cmake)
include(test_repl-memory_cmd.cmake)
include(test_repl-normal_forms_cmd.cmake)
include(test_repl-normalize_cmd.cmake)
include(test_repl-quit_cmd.cmake)
include(test_repl-version_cmd.cmake)

# TODO (HIGH) add tests for help definition,...
# TODO (VERY_HIGH) add tests for normalize command
# TODO (VERY_HIGH) add tests  definition command
# TODO (VERY_HIGH) add tests for sat/unsat/valid command
# TODO (VERY_HIGH) add tests for solving command
# TODO (VERY_HIGH) add tests for configuration commands
# TODO (VERY_HIGH) add tests for examples command
# TODO (VERY_HIGH) add tests for outputs command

# sample for comparing output with a file
#add_test(NAME test_name COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> | diff - test_name.dump")