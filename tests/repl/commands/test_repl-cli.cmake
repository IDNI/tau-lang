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
add_raw_repl_test(cli-version_flag
	"${TAU_RUN} --version"
	"Tau Language Framework")

add_raw_repl_test(cli-version_flag_short
	"${TAU_RUN} -v"
	"Tau Language Framework")

# --- interactive REPL --------------------------------------------------------
# Piping a command plus `q` on stdin drives the interactive loop rather than the
# -e one-shot path, so welcome() and repl::run() execute. -X selects the legacy
# terminal REPL, which is the branch that works without a tty.
add_multiline_repl_test(cli-interactive_quit
	"Welcome to the Tau Language Framework" NO_FAIL_REGEX STDIN "q\\n")

add_multiline_repl_test(cli-interactive_command
	"Tau Language Framework"
	NO_FAIL_REGEX STDIN "version\\nq\\n")

# --- specification file ------------------------------------------------------
# `-` reads the specification from stdin, which exercises run_tau_spec()'s
# stdin branch without needing a fixture file on disk. The spec's `i` stream
# defaults to the console, i.e. the same stdin, which is already at EOF; with
# --quit that is the graceful "No more inputs provided" termination rather than
# an interactive prompt loop.
add_multiline_repl_test(cli-spec_from_stdin
	"No more inputs provided|Terminating"
	NO_FAIL_REGEX STDIN "o[t] = i[t].\\n" NO_X FLAGS "-" "-q")

# An empty specification on stdin returns success early (main.cpp:94) without
# constructing an interpreter at all.
add_raw_repl_test(cli-empty_spec_from_stdin
	"printf '' | ${TAU_RUN} - -q"
	"")

# A specification file that does not exist is rejected by the CLI ARGUMENT
# PARSER, before main() ever calls run_tau_spec. main.cpp:88's own
# "Cannot open file" branch is therefore unreachable for a plainly missing path
# (it would need a file that vanishes or becomes unreadable between
# parse_args() and the open), so it stays uncovered by design.
add_raw_repl_test(cli-missing_spec_file
	"${TAU_RUN} definitely_absent_spec.tau"
	"Invalid command, or no such file or URL" NO_FAIL_REGEX)

# --- a real specification file ----------------------------------------------
# Runs run_tau_spec() end to end: read the file, build an interpreter, step, and
# terminate via --quit when the console input stream is exhausted.
# A wasm module reaches host files through the CLI's node NODEFS mount
# (src/tau_stdin_node.pre.js); REQUIRES hostfs registers disabled where it does
# not exist instead of dropping the name.
add_raw_repl_test(cli-spec_file
	"printf 'o[t] = i[t].\\n' > cli_spec_fixture.tau && ${TAU_RUN} cli_spec_fixture.tau -q < /dev/null; r=$?; rm -f cli_spec_fixture.tau; exit $r"
	"No more inputs provided" NO_FAIL_REGEX REQUIRES hostfs)

# --- CLI argument ordering ---------------------------------------------------
# A boolean option takes an optional value such as on or off. A file placed
# after it must still reach the inputs and run, rather than being consumed as
# the flag's value and leaving the REPL open.
add_raw_repl_test(cli-option_before_file_runs_file
	"printf 'o[t] = i[t].\\n' > cli_order_fixture.tau && echo q | ${TAU_RUN} -q cli_order_fixture.tau; r=$?; rm -f cli_order_fixture.tau; exit $r"
	"Execution step: 0" FAIL_REGEX "Welcome to the Tau Language Framework"
	REQUIRES hostfs)

# --- spec file WITHOUT --quit ------------------------------------------------
# run_loop() prints "Press ENTER to continue" only for a step that needs no
# input, so the fixture below has none. At EOF the getline fails and the loop
# breaks on the eof/fail guard.
# Same REQUIRES hostfs gate as cli-spec_file above.
add_raw_repl_test(cli-spec_file_no_quit
	"printf 'o[t] = 0.\\n' > cli_noquit_fixture.tau && ${TAU_RUN} cli_noquit_fixture.tau < /dev/null; r=$?; rm -f cli_noquit_fixture.tau; exit $r"
	"Press ENTER to continue" NO_FAIL_REGEX REQUIRES hostfs)

# --- limit options (2026-08-17 unified limit options) ------------------------
# One end-to-end round trip per wiring style: the CLI flag must land in the
# library global the REPL's `get` reads back. One cap, one gc knob (decimal
# value), and one of the pre-existing interpreter options now readable from
# the REPL cover the three distinct code paths in main.cpp's apply block.
add_repl_test(cli-max_fixpoint_steps_flag
	"get max-fixpoint-steps" "max-fixpoint-steps: *9" NO_TRACE
	FLAGS --max-fixpoint-steps 9)

# Without the flag the library's own default (or its TAU_* variable) applies.
add_repl_test(cli-max_fixpoint_steps_default
	"get max-fixpoint-steps" "max-fixpoint-steps: *500" NO_FAIL_REGEX NO_TRACE)
add_repl_test(cli-gc_growth_factor_flag
	"get gc-growth-factor" "gc-growth-factor: *2.5" NO_TRACE
	FLAGS --gc-growth-factor 2.5)

add_repl_test(cli-max_revision_alts_flag
	"get max-revision-alts" "max-revision-alts: *4" NO_TRACE
	FLAGS --max-revision-alts 4)

# -K is --ba-component-factoring alone; the closed-regions timeout has no
# short form.
add_raw_repl_test(cli-help_short_k_is_factoring
	"${TAU_RUN} --help"
	"--ba-component-factoring\t-K" FAIL_REGEX "--ltl-closed-regions-timeout\t-K")

# --help lists the new options.
add_raw_repl_test(cli-help_lists_limit_options
	"${TAU_RUN} --help"
	"max-fixpoint-steps" NO_FAIL_REGEX)

# A flag search that reaches its cap gives no verdict (GitHub #191).
add_raw_repl_test(cli-help_flag_search_giveup_is_no_verdict
	"${TAU_RUN} --help"
	"max-flag-search-steps[^\n]*a give-up reports an error, not a verdict"
	FAIL_REGEX "reports unsatisfiable")

# --- every limit flag, long AND short form (2026-08-17 coverage plan) --------
# Each row: testname|longflag|shortflag|value|get-option|expected-value.
# The round trip proves flag -> given_count() -> api setter -> library global
# -> get.
# Values are distinct from the defaults so a silently-ignored flag fails.
# An empty shortflag field means the option has no short form.
set(TAU_CLI_LIMIT_ROWS
	"spec_size_warn|spec-size-warn|w|4096|spec-size-warn|4096"
	"max_revision_alts|max-revision-alts|a|4|max-revision-alts|4"
	"block_max_splits|block-max-splits|p|512|block-max-splits|512"
	"block_max_rounds|block-max-rounds|r|33|block-max-rounds|33"
	"ba_decision_pins|ba-decision-pins|N|77|ba-decision-pins|77"
	"max_fixpoint_steps|max-fixpoint-steps|f|9|max-fixpoint-steps|9"
	"max_flag_search_steps|max-flag-search-steps|F|12|max-flag-search-steps|12"
	"block_squeeze_cap|block-squeeze-cap|z|64|block-squeeze-cap|64"
	"max_simplify_rounds|max-simplify-rounds|m|1000|max-simplify-rounds|1000"
	"max_def_passes|max-def-passes|P|40|max-def-passes|40"
	"max_enum_steps|max-enum-steps|E|33|max-enum-steps|33"
	"max_probe_steps|max-probe-steps|M|44|max-probe-steps|44"
	"max_rewrite_rounds|max-rewrite-rounds|R|21|max-rewrite-rounds|21"
	"gc_min_size|gc-min-size|G|512|gc-min-size|512"
	"gc_growth_factor|gc-growth-factor|W|2.5|gc-growth-factor|2.5"
	"max_consistency_subsets|max-consistency-subsets|j|9|max-consistency-subsets|9"
	"cache_bound|cache-bound|A|123|cache-bound|123"
	"max_cover_products|max-cover-products|n|9|max-cover-products|9"
	"max_constant_size|max-constant-size|u|300|max-constant-size|300"
	"tref_budget|tref-budget|y|4096|tref-budget|4096"
	"tref_budget_soft|tref-budget-soft|C|50|tref-budget-soft|50"
	"ltl_data_game_max_nodes|ltl-data-game-max-nodes||4096|ltl-data-game-max-nodes|4096"
	"ltl_data_game_max_memo|ltl-data-game-max-memo||4096|ltl-data-game-max-memo|4096"
	"ltl_data_game_max_combinations|ltl-data-game-max-combinations||77|ltl-data-game-max-combinations|77"
	"ltl_max_observations|ltl-max-observations||5|ltl-max-observations|5"
	"ltl_mealy_max_states|ltl-mealy-max-states||77|ltl-mealy-max-states|77"
	"ltl_mealy_max_edges|ltl-mealy-max-edges||99|ltl-mealy-max-edges|99"
	"compile_max_table_edges|compile-max-table-edges||50|compile-max-table-edges|50"
	"compile_build_timeout|compile-build-timeout||50|compile-build-timeout|50"
	"bf_dependence_max_nodes|bf-dependence-max-nodes||1000|bf-dependence-max-nodes|1000"
)
foreach(row IN LISTS TAU_CLI_LIMIT_ROWS)
	string(REPLACE "|" ";" f "${row}")
	list(GET f 0 nm)
	list(GET f 1 lflag)
	list(GET f 2 sflag)
	list(GET f 3 val)
	list(GET f 4 opt)
	list(GET f 5 expect)
	add_repl_test(cli-limit_long-${nm}
		"get ${opt}" "${opt}: *${expect}" NO_TRACE
		FLAGS --${lflag} ${val})
	if(NOT sflag STREQUAL "")
		add_repl_test(cli-limit_short-${nm}
			"get ${opt}" "${opt}: *${expect}" NO_TRACE
			FLAGS -${sflag} ${val})
	endif()
	add_raw_repl_test(cli-help_lists-${nm}
		"${TAU_RUN} --help"
		"${lflag}" NO_FAIL_REGEX)
endforeach()

# --- bv-blastdepth CLI flag (BA-declared option) -----------------------------
# bv declares blastdepth as its own option, addressed bv-blastdepth, present
# when bv is in the configured pack -- hence gated by hand here rather than
# through the uniform TAU_CLI_LIMIT_ROWS loop.
add_repl_test(cli-bv_blastdepth_flag
	"get bv-blastdepth" "bv-blastdepth: *8" NO_TRACE
	FLAGS --bv-blastdepth 8)

# --- bv-case-split-max-tests CLI flag (BA-declared option) -------------------
# bv declares case-split-max-tests as its own option, addressed
# bv-case-split-max-tests, present when bv is in the configured pack -- hence
# gated by hand here rather than through the uniform TAU_CLI_LIMIT_ROWS loop.
add_repl_test(cli-bv_case_split_max_tests_flag
	"get bv-case-split-max-tests" "bv-case-split-max-tests: *5" NO_TRACE
	FLAGS --bv-case-split-max-tests 5)

# --- bv-solve-timeout CLI flag (BA-declared option) --------------------------
add_repl_test(cli-bv_solve_timeout_flag
	"get bv-solve-timeout" "bv-solve-timeout: *7" NO_TRACE
	FLAGS --bv-solve-timeout 7)

# --- preprocessing default (GitHub #74) --------------------------------------
# The CLI's own option table used to hardcode its own default of `true`, so
# every plain `tau` invocation silently overrode the library's
# `preprocessing` global, and the single-lookback bv accumulator from #74
# hung on the CLI while completing instantly through the API. This test
# checks that the CLI reports the library's value, whatever it is, instead
# of holding a second hardcoded default. Drives the plain CLI, no -B given:
# it must finish and produce the reporter's expected 5, 8, 8. The input
# prompt answers `q` with a parse Error (that is how the run is ended
# without a tty), so no FAIL regex here.
add_repl_test(cli-blasting_default_off
	"get preprocessing" "preprocessing: *false" NO_TRACE)

add_multiline_repl_test(cli-issue74_bv_accumulator_default_flags
	"o0s\\[2\\] := 8"
	NO_FAIL_REGEX STDIN "i1:bv[8] := in console.\\nrun (o0s[0]:bv[8] = {#x05}:bv[8]) && (o0s[t]:bv[8] = o0s[t-1]:bv[8] + i1[t]:bv[8]).\\n3\\n0\\nq\\nq\\n"
	TIMEOUT 120)

# --- bv widening flags -------------------------------------------------------
# --bv-widening and --bv-max-width reach the api before the REPL. main.cpp
# applies them unconditionally, unlike -B. Each command text names bv, so the
# helper pack gate covers them.
add_repl_test(cli-bv_widening_flag
	"get bv-widening" "bv-widening: *true" NO_TRACE
	FLAGS --bv-widening)
add_repl_test(cli-bv_widening_long_flag
	"get bv-widening" "bv-widening: *true" NO_TRACE
	FLAGS --bv-widening)
add_repl_test(cli-bv_max_width_flag
	"get bv-max-width" "bv-max-width: *64" NO_TRACE
	FLAGS --bv-max-width 64)
# The flag changes the answer: 16 * 16 = 0 holds at 8 bits only modularly.
add_repl_test(cli-bv_widening_flag_changes_semantics
	"sat {16}:bv[8] * {16}:bv[8] = {0}:bv[8]" "%1.*: F" NO_TRACE
	FLAGS --bv-widening)
# A cap too small for the formula is undecidable: the entry point reports
# the error and answers with no verdict at all. The error IS the expected
# output, so no FAIL regex.
# The cap/width numbers now travel as report attrs (limit=.. width=..)
# instead of being spliced into the sentence (bv_widening.tmpl.h).
add_repl_test(cli-bv_max_width_cap_exceeded
	"sat o:bv[8] = x * y" "required width exceeds bv-max-width" NO_FAIL_REGEX NO_TRACE
	FLAGS --bv-widening --bv-max-width 12)
# Spec-file mode gets the flags too (they are applied before the file
# runs): a one-step run of the guard-free saturating add stores 200, not 44.
add_multiline_repl_test(cli-bv_widening_spec_file_mode
	"o1\\[0\\] := 200"
	NO_FAIL_REGEX STDIN "i1:bv[8] := in console.\\ni2:bv[8] := in console.\\nrun always o1[t]:bv[8] = min(i1[t] + i2[t], {200}:bv[8]).\\n200\\n100\\nq\\nq\\n"
	FLAGS --bv-widening TIMEOUT 120)

# With multi-character names `xy` is one variable, so a printed conjunction
# must keep its `&` to read back as the same formula.
add_repl_test(cli-no_charvar_prints_conjunction
	"normalize (ab:sbf & c_:sbf) = 0:sbf." "(ab&c_|c_&ab) = 0"
	FLAGS --charvar=false --color=false -S error NO_TRACE
	FAIL_REGEX "Error|abc_|c_ab")

# make_cli drops ASCII 22 (Ctrl-V) from a line: "&\026&" reads as "&&"
add_multiline_repl_test(cli-ctrl_v_is_dropped "%1[^:]*: F"
	NO_FAIL_REGEX STDIN "sat x = 0 &\\026& x != 0\\nq\\n")
# --ltl-guard-max-cubes lands in the global `get ltlguardmaxcubes` reads
add_repl_test(cli-ltl_guard_max_cubes_flag "get ltl-guard-max-cubes"
	"ltl-guard-max-cubes: *3" FLAGS --ltl-guard-max-cubes 3)
# a spec file that does not parse is reported and the run ends
add_raw_repl_test(cli-spec_file_parse_error
	"printf 'o1[t] = = 1.\\n' > cli_bad_spec_fixture.tau && ${TAU_RUN} cli_bad_spec_fixture.tau -q < /dev/null; r=$?; rm -f cli_bad_spec_fixture.tau; exit $r"
	"Syntax Error: Unexpected '='" NO_FAIL_REGEX REQUIRES hostfs)
