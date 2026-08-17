#
# CLI entry points of the tau executable (src/main.cpp)
#
# Coverage-driven additions (2026-08-01). main.cpp measured 50.7% line coverage,
# the lowest of any file in src/. Every pre-existing repl test invokes
# `tau -e "<command>"`, so main() itself was well covered up to the point where
# it dispatches on `-e`, and three whole branches below that never ran:
#
#   * `--version` (main.cpp:163),
#   * the interactive REPL path -- welcome() and repl::run() (main.cpp:206-217),
#     for which tests/repl/add_repl_test.cmake already provided an unused
#     add_echo_repl_test helper that pipes commands in on stdin,
#   * the specification-file path (main.cpp:181-186) and with it the whole of
#     run_tau_spec() (main.cpp:73-134), roughly 60 uncovered lines including
#     reading a spec from "-" (stdin) and the --quit termination branch.
#

include(add_repl_test)

# --- --version ---------------------------------------------------------------
# The `version` REPL command is already covered by test_repl-version_cmd; this
# is the distinct CLI flag, handled before the REPL is ever constructed.
add_test(NAME "test_repl-cli-version_flag"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --version")
set_tests_properties("test_repl-cli-version_flag" PROPERTIES
	PASS_REGULAR_EXPRESSION "Tau Language Framework"
	FAIL_REGULAR_EXPRESSION "Error")

add_test(NAME "test_repl-cli-version_flag_short"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -v")
set_tests_properties("test_repl-cli-version_flag_short" PROPERTIES
	PASS_REGULAR_EXPRESSION "Tau Language Framework"
	FAIL_REGULAR_EXPRESSION "Error")

# --- interactive REPL --------------------------------------------------------
# Piping a command plus `q` on stdin drives the interactive loop rather than the
# -e one-shot path, so welcome() and repl::run() execute. -X selects the legacy
# terminal REPL, which is the branch that works without a tty.
add_test(NAME "test_repl-cli-interactive_quit"
	COMMAND bash -c "echo q | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-cli-interactive_quit" PROPERTIES
	PASS_REGULAR_EXPRESSION "Welcome to the Tau Language Framework")

add_test(NAME "test_repl-cli-interactive_command"
	COMMAND bash -c "printf 'version\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
set_tests_properties("test_repl-cli-interactive_command" PROPERTIES
	PASS_REGULAR_EXPRESSION "Tau Language Framework")

# --- specification file ------------------------------------------------------
# `-` reads the specification from stdin, which exercises run_tau_spec()'s
# stdin branch without needing a fixture file on disk. The spec's `i` stream
# defaults to the console, i.e. the same stdin, which is already at EOF; with
# --quit that is the graceful "No more inputs provided" termination rather than
# an interactive prompt loop.
add_test(NAME "test_repl-cli-spec_from_stdin"
	COMMAND bash -c "printf 'o[t] = i[t].\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> - -q")
set_tests_properties("test_repl-cli-spec_from_stdin" PROPERTIES
	PASS_REGULAR_EXPRESSION "No more inputs provided|Terminating")

# An empty specification on stdin returns success early (main.cpp:94) without
# constructing an interpreter at all.
add_test(NAME "test_repl-cli-empty_spec_from_stdin"
	COMMAND bash -c "printf '' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> - -q")
set_tests_properties("test_repl-cli-empty_spec_from_stdin" PROPERTIES
	FAIL_REGULAR_EXPRESSION "Error")

# A specification file that does not exist is rejected by the CLI ARGUMENT
# PARSER, before main() ever calls run_tau_spec. main.cpp:88's own
# "Cannot open file" branch is therefore unreachable for a plainly missing path
# (it would need a file that vanishes or becomes unreadable between
# process_args() and the open), so it stays uncovered by design.
add_test(NAME "test_repl-cli-missing_spec_file"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> definitely_absent_spec.tau")
set_tests_properties("test_repl-cli-missing_spec_file" PROPERTIES
	PASS_REGULAR_EXPRESSION "Invalid command or file not exists")

# --- a real specification file ----------------------------------------------
# Runs run_tau_spec() end to end: read the file, build an interpreter, step, and
# terminate via --quit when the console input stream is exhausted.
add_test(NAME "test_repl-cli-spec_file"
	COMMAND bash -c "printf 'o[t] = i[t].\\n' > cli_spec_fixture.tau && $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> cli_spec_fixture.tau -q < /dev/null; r=$?; rm -f cli_spec_fixture.tau; exit $r")
set_tests_properties("test_repl-cli-spec_file" PROPERTIES
	PASS_REGULAR_EXPRESSION "No more inputs provided")

# --- CLI ARGUMENT ORDERING BUG (characterized, not endorsed) -----------------
# `tau <spec> -q` runs the specification, but `tau -q <spec>` SILENTLY IGNORES
# the specification and drops into the interactive REPL instead -- no error, no
# warning. The boolean options are declared with an optional value
# (cli::option(name, short, <bool default>)), so the following argument is
# consumed as the flag's value and never reaches cl.get_files().
#
# Any boolean flag placed before the file reproduces it (-q, -X, ...), and
# options-before-operands is the ordering most users would reach for, so this
# silently does the wrong thing. Pinned here so the current behaviour is
# visible; if it is fixed, this test should fail and be inverted.
add_test(NAME "test_repl-cli-option_before_file_ignores_file"
	COMMAND bash -c "printf 'o[t] = i[t].\\n' > cli_order_fixture.tau && echo q | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -q cli_order_fixture.tau; r=$?; rm -f cli_order_fixture.tau; exit $r")
set_tests_properties("test_repl-cli-option_before_file_ignores_file" PROPERTIES
	PASS_REGULAR_EXPRESSION "Welcome to the Tau Language Framework")

# --- spec file WITHOUT --quit ------------------------------------------------
# Without --quit, run_tau_spec() prints "Press ENTER to continue" and reads a
# line rather than terminating (main.cpp:107-121). With stdin at EOF the
# getline fails and the loop breaks on the eof/fail guard -- the same guard
# repl_evaluator::run_cmd uses to avoid spinning forever on closed stdin.
add_test(NAME "test_repl-cli-spec_file_no_quit"
	COMMAND bash -c "printf 'o[t] = i[t].\\n' > cli_noquit_fixture.tau && $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> cli_noquit_fixture.tau < /dev/null; r=$?; rm -f cli_noquit_fixture.tau; exit $r")
set_tests_properties("test_repl-cli-spec_file_no_quit" PROPERTIES
	PASS_REGULAR_EXPRESSION "Press ENTER to continue")

# --- limit options (2026-08-17 unified limit options) ------------------------
# One end-to-end round trip per wiring style: the CLI flag must land in the
# library global the REPL's `get` reads back. One cap, one gc knob (decimal
# value), and one of the pre-existing interpreter options now readable from
# the REPL cover the three distinct code paths in main.cpp's apply block.
add_test(NAME "test_repl-cli-max_fixpoint_steps_flag"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --max-fixpoint-steps 9 -e \"get fixpointsteps\"")
set_tests_properties("test_repl-cli-max_fixpoint_steps_flag" PROPERTIES
	PASS_REGULAR_EXPRESSION "fixpointsteps: *9"
	FAIL_REGULAR_EXPRESSION "Error")

add_test(NAME "test_repl-cli-gc_growth_factor_flag"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --gc-growth-factor 2.5 -e \"get gcgrowth\"")
set_tests_properties("test_repl-cli-gc_growth_factor_flag" PROPERTIES
	PASS_REGULAR_EXPRESSION "gcgrowth: *2.5"
	FAIL_REGULAR_EXPRESSION "Error")

add_test(NAME "test_repl-cli-max_revision_alts_flag"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --max-revision-alts 4 -e \"get revisionalts\"")
set_tests_properties("test_repl-cli-max_revision_alts_flag" PROPERTIES
	PASS_REGULAR_EXPRESSION "revisionalts: *4"
	FAIL_REGULAR_EXPRESSION "Error")

# --help lists the new options.
add_test(NAME "test_repl-cli-help_lists_limit_options"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --help")
set_tests_properties("test_repl-cli-help_lists_limit_options" PROPERTIES
	PASS_REGULAR_EXPRESSION "max-fixpoint-steps")
