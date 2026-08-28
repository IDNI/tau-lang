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
include(tau_repl_pack)

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
# run_loop() prints "Press ENTER to continue" only for a step that needs no
# input, so the fixture below has none. At EOF the getline fails and the loop
# breaks on the eof/fail guard.
add_test(NAME "test_repl-cli-spec_file_no_quit"
	COMMAND bash -c "printf 'o[t] = 0.\\n' > cli_noquit_fixture.tau && $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> cli_noquit_fixture.tau < /dev/null; r=$?; rm -f cli_noquit_fixture.tau; exit $r")
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

# --- every limit flag, long AND short form (2026-08-17 coverage plan) --------
# Each row: testname|longflag|shortflag|value|get-option|expected-value.
# The round trip proves flag -> optnum() -> api setter -> library global -> get.
# Values are distinct from the defaults so a silently-ignored flag fails.
set(TAU_CLI_LIMIT_ROWS
	"spec_size_warn|spec-size-warn|w|4096|specsizewarn|4096"
	"max_revision_alts|max-revision-alts|a|4|revisionalts|4"
	"block_max_splits|block-max-splits|p|512|maxsplits|512"
	"block_max_rounds|block-max-rounds|r|33|maxrounds|33"
	"max_fixpoint_steps|max-fixpoint-steps|f|9|fixpointsteps|9"
	"max_flag_search_steps|max-flag-search-steps|F|12|flagsteps|12"
	"block_squeeze_cap|block-squeeze-cap|z|64|squeezecap|64"
	"max_simplify_rounds|max-simplify-rounds|m|1000|simplifyrounds|1000"
	"max_def_passes|max-def-passes|P|40|defpasses|40"
	"max_enum_steps|max-enum-steps|E|33|enumsteps|33"
	"max_rewrite_rounds|max-rewrite-rounds|R|21|rewriterounds|21"
	"gc_min_size|gc-min-size|G|512|gcminsize|512"
	"gc_growth_factor|gc-growth-factor|W|2.5|gcgrowth|2.5"
	"max_consistency_subsets|max-consistency-subsets|k|9|maxsubsets|9"
	"cache_bound|cache-bound|C|123|cachebound|123"
	"max_cover_products|max-cover-products|n|9|maxcoverproducts|9"
)
foreach(row IN LISTS TAU_CLI_LIMIT_ROWS)
	string(REPLACE "|" ";" f "${row}")
	list(GET f 0 nm)
	list(GET f 1 lflag)
	list(GET f 2 sflag)
	list(GET f 3 val)
	list(GET f 4 opt)
	list(GET f 5 expect)
	add_test(NAME "test_repl-cli-limit_long-${nm}"
		COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --${lflag} ${val} -e \"get ${opt}\"")
	set_tests_properties("test_repl-cli-limit_long-${nm}" PROPERTIES
		PASS_REGULAR_EXPRESSION "${opt}: *${expect}"
		FAIL_REGULAR_EXPRESSION "Error")
	add_test(NAME "test_repl-cli-limit_short-${nm}"
		COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -${sflag} ${val} -e \"get ${opt}\"")
	set_tests_properties("test_repl-cli-limit_short-${nm}" PROPERTIES
		PASS_REGULAR_EXPRESSION "${opt}: *${expect}"
		FAIL_REGULAR_EXPRESSION "Error")
	add_test(NAME "test_repl-cli-help_lists-${nm}"
		COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --help")
	set_tests_properties("test_repl-cli-help_lists-${nm}" PROPERTIES
		PASS_REGULAR_EXPRESSION "${lflag}")
endforeach()

# --- bv-blastdepth CLI flag (BA-declared option) -----------------------------
# bv declares blastdepth as its own option, addressed bv-blastdepth, present
# when bv is in the configured pack -- hence gated by hand here rather than
# through the uniform TAU_CLI_LIMIT_ROWS loop.
tau_repl_unsupported(_tau_skip "get bv-blastdepth")
if(_tau_skip)
	tau_repl_record_skip("test_repl-cli-bv_blastdepth_flag")
else()
	add_test(NAME "test_repl-cli-bv_blastdepth_flag"
		COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> --bv-blastdepth 8 -e \"get bv-blastdepth\"")
	set_tests_properties("test_repl-cli-bv_blastdepth_flag" PROPERTIES
		PASS_REGULAR_EXPRESSION "bv-blastdepth: *8"
		FAIL_REGULAR_EXPRESSION "Error")
endif()

# --- preprocessing default (GitHub #74) --------------------------------------
# The library default is `preprocessing = false`
# (heuristics/preprocess_placement.h), because predicate blasting hands cvc5
# thousands of auxiliary quantifiers it does not need. The
# CLI's own option table used to hardcode its own default of `true`, so every
# plain `tau` invocation silently overrode the library decision, and the
# single-lookback bv accumulator from #74 hung on the CLI while completing
# instantly with `-B false` (or through the C++/Python API, which never saw the
# CLI default). Drives the plain CLI, no -B given: it must finish and produce
# the reporter's expected 5, 8, 8. The input prompt answers `q` with a parse
# Error (that is how the run is ended without a tty), so no FAIL regex here.
add_test(NAME "test_repl-cli-blasting_default_off"
	COMMAND bash -c "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -e \"get preprocessing\"")
set_tests_properties("test_repl-cli-blasting_default_off" PROPERTIES
	PASS_REGULAR_EXPRESSION "preprocessing: *off"
	FAIL_REGULAR_EXPRESSION "Error")

tau_repl_unsupported(_tau_skip "i1:bv[8] := in console.")
if(_tau_skip)
	tau_repl_record_skip("test_repl-cli-issue74_bv_accumulator_default_flags")
else()
	add_test(NAME "test_repl-cli-issue74_bv_accumulator_default_flags"
		COMMAND bash -c "printf 'i1:bv[8] := in console.\\nrun (o0s[0]:bv[8] = {#x05}:bv[8]) && (o0s[t]:bv[8] = o0s[t-1]:bv[8] + i1[t]:bv[8]).\\n3\\n0\\nq\\nq\\n' | $<TARGET_FILE:${TAU_EXECUTABLE_NAME}> -X")
	set_tests_properties("test_repl-cli-issue74_bv_accumulator_default_flags" PROPERTIES
		PASS_REGULAR_EXPRESSION "o0s\\[2\\] := 8"
		TIMEOUT 120)
endif()
