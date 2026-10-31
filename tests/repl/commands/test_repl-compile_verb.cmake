# `tau compile` verb: emit + build + run the echo spec end-to-end.
include(add_compile_test)

add_compile_test(compile_verb-echo
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
	REQUIRES hostfs)

# Period-terminated spec: proves compile_spec parses via get_spec, not just
# the bare-formula get_formula fallback (echo.tau above has no period).
add_compile_test(compile_verb-echo_dot
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo_dot.tau"
	REQUIRES hostfs)

# A lookback atom under an eventuality drives a __step_ge<k> guard
# (ltl_aba_helpers.tmpl.h's step_guard_prop) into the synthesized strategy;
# this is the emit_main/table_step_provider path's own smoke test for it.
add_compile_test(compile_verb-ltl_lookback_under_eventuality
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/ltl_lookback_under_eventuality.tau"
	REQUIRES hostfs)

# The program prints its report only with --benchmarks.
add_compile_test(compile_verb-run_prints_no_report
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/ltl_lookback_under_eventuality.tau"
	ARGS "-DEXPECT_STDERR=^$"
	REQUIRES hostfs)
add_compile_test(compile_verb-benchmarks_print_the_report
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/ltl_lookback_under_eventuality.tau"
	ARGS "-DRUN_ARGS=--benchmarks"
		"-DEXPECT_STDERR=produce_step_solution:.*run: [0-9.]+ ms"
	REQUIRES hostfs)

# --cxx names the compiler the nested cmake configure uses: an unusable
# name must reach that configure (and fail it) instead of being replaced
# by whatever clang++ is on PATH. The spec names no BA so every pack gets
# as far as the configure; it is copied to a temp dir because the build
# tree is placed beside the spec file.
add_compile_test(compile_verb-cxx_override_is_used
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo_untyped.tau"
	CHECKER "${CMAKE_CURRENT_LIST_DIR}/../check_cxx_override.cmake"
	PASS_REGEX "nonexistent/c\\+\\+")

# A tautological literal still gives its clause a two-step warm-up, so the
# spec is realizable and compiles (o2 may be 0 at steps 0 and 1).
add_compile_test(compile_verb-warm_up_tautology
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/warm_up_tautology.tau"
	REQUIRES hostfs)

# The program plays the strategy `run` plays. Over each of these specs the
# data game decides, and the program used to play the abstraction's
# strategy instead: it stopped at step 2 finding no witness for its edge,
add_compile_run_test(compile_verb-plays_run_strategy 6
	"(always o2[t] = o1[t-1] && !(o1[t-1] = 0)) && (sometimes !(o2[t-1] = o1[t]))")
# printed nothing (its edge had no solution at step 0),
add_compile_run_test(compile_verb-plays_run_strategy_from_start 6
	"(always o2[t] = o1[t-1] && o3[t] = o2[t-1] && (!(o2[t-1] = o1[t]) || !(o2[t-1] = 1))) && (sometimes (o1[t] = 0 && o2[t-1] = o1[t]))")
# never met the awaited event,
add_compile_run_test(compile_verb-plays_run_strategy_goal_met 6
	"(always o2[t]:bv[1] = o1[t-1]:bv[1] && o1[t-1]:bv[1] = 0) && (sometimes !(o1[t]:bv[1] = o2[t-1]:bv[1]))")
# or was refused, its bitvector output given no witness.
add_compile_run_test(compile_verb-plays_run_strategy_witness 6
	"(always o1[t-1]:bv[1] = 0) && (sometimes !(o2[t-1]:bv[1] = 1))")
# An output no atom of the strategy reads is printed, as `run` prints it.
add_compile_run_test(compile_verb-prints_every_output 6
	"(always (o2[0]:bv[1] = 1 || o2[0]:bv[1] = 0) && o1[1]:bv[1] = 1 && !((o3[t-1]:bv[1] = 1 && o3[t]:bv[1] = 0))) && (sometimes (o3[t]:bv[1] = o1[t-1]:bv[1] && !(o3[t-2]:bv[1] = 1)))")
# `run` executes each of these specs by solving each step, with no
# strategy, and so does the program. It used to play the abstraction's
# strategy, which broke the specification at step 1,
add_compile_run_test(compile_verb-solves_as_run 6
	"always o2[t]:bv[1] = o1[t-1]:bv[1] && o3[t]:bv[1] = o2[t-1]:bv[1] && o3[t-1]:bv[1] = 0")
# printed nothing,
add_compile_run_test(compile_verb-solves_as_run_from_start 6
	"(always o3[0] = 0 && (!(o2[t-1] = o1[t-1]) || o2[t-1] = 0) && o1[t-1] = o3[t-1]) && (sometimes o3[t-1] = o1[t-1])")
# or was refused, its bitvector output given no witness.
add_compile_run_test(compile_verb-solves_as_run_witness 6
	"always o1[t-1]:bv[1] = 1")

# --preset, -D and the platform SDK: the artifact configure is driven by the
# artifact's own CMakePresets.json, so these cases need an SDK to be present.
# An unknown --preset is refused before any configure runs.
add_compile_test(compile_verb-unknown_preset
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
	PRESET no-such-platform EXPECT_FAIL NEEDS_SDK "${CMAKE_BINARY_DIR}/sdk")

# A -D reaches the emitted project's configure: an unused cache variable
# is named in configure.log.
add_compile_test(compile_verb-define_reaches_configure
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
	NEEDS_SDK "${CMAKE_BINARY_DIR}/sdk"
	ARGS "-DEXTRA_ARGS=-DTAU_COMPILE_PROBE=1" "-DEXPECT_LOG=TAU_COMPILE_PROBE"
		"-DNO_RUN=ON")

# Without --preset the build is native: this build's SDK, cmake's compiler and
# the build type the -D names.
add_compile_test(compile_verb-native_build_type
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
	NO_PRESET NO_SDK NEEDS_SDK "${CMAKE_BINARY_DIR}/sdk"
	ARGS "-DEXTRA_ARGS=-DCMAKE_BUILD_TYPE=RelWithDebInfo"
		"-DEXPECT_LOG=RelWithDebInfo" "-DNO_RUN=ON")

# TAU_CXX names the compiler when --cxx is not given.
add_compile_test(compile_verb-tau_cxx_env
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
	NO_PRESET NEEDS_SDK "${CMAKE_BINARY_DIR}/sdk"
	ARGS "-DTAU_CXX_ENV=${CMAKE_CXX_COMPILER}"
		"-DEXPECT_LOG=${CMAKE_CXX_COMPILER}" "-DNO_RUN=ON")

# Output paths: the spec is never the output, an output equal to the spec
# is refused, and a relative -o resolves against the caller's directory.
foreach(_case same_spec default_out relative_out)
	add_compile_test(compile_verb-paths_${_case}
		"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
		CHECKER "${CMAKE_CURRENT_LIST_DIR}/../check_compile_paths.cmake"
		NEEDS_SDK "${CMAKE_BINARY_DIR}/sdk" ARGS "-DCASE=${_case}")
endforeach()

# The MinGW platform build is almost never present; the case proves the
# cross preset and its vendored toolchain resolve when it is.
get_filename_component(_tau_build_base "${CMAKE_BINARY_DIR}" DIRECTORY)
add_compile_test(compile_verb-release_w64
	"${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau"
	PRESET release-w64 NO_SDK NEEDS_SDK "${_tau_build_base}/release-w64/sdk"
	ARGS "-DNO_RUN=ON")
# The data game decides each of these on codes a table carries since the
# Mealy view covers them: the bits of an 8-bit output,
add_compile_run_test(compile_verb-plays_view_on_bits 6
	"(always ((o1[t]:bv[8] < (o1[t]:bv[8] - {3}:bv[8])) || ({0}:bv[8] = o1[t-1]:bv[8]))) && (sometimes ((({3}:bv[8] + o1[t]:bv[8]) <= o1[t-1]:bv[8]) || (o1[t]:bv[8] < {2}:bv[8])))")
# and the order of qlt values, whose output the program solves each step
# for the point run takes.
add_compile_run_test(compile_verb-plays_view_on_order_types 6
	"(always {0}:qlt <= o1[t]:qlt) && (sometimes ({1/2}:qlt > o1[t]:qlt || o1[t-1]:qlt != o1[t-1]:qlt))")
# With a node table of one node the codes of the same spec do not fit, and
# the data game decides it on formulas: its strategy has no Mealy view, and
# the program, reading the same table size from the environment, plays it
# as run does.
add_compile_run_test(compile_verb-plays_formula_strategy 6
	"(always ((o1[t]:bv[8] < (o1[t]:bv[8] - {3}:bv[8])) || ({0}:bv[8] = o1[t-1]:bv[8]))) && (sometimes ((({3}:bv[8] + o1[t]:bv[8]) <= o1[t-1]:bv[8]) || (o1[t]:bv[8] < {2}:bv[8])))")
if(TEST "test_repl-compile_verb-plays_formula_strategy")
	set_tests_properties("test_repl-compile_verb-plays_formula_strategy"
		PROPERTIES ENVIRONMENT "TAU_LTL_DATA_GAME_MAX_NODES=1")
endif()

include(add_repl_test)

# argument checks before any codegen
add_raw_repl_test(compile_verb-no_spec "${TAU_RUN} compile"
	"Usage: tau compile <spec.tau>" NO_FAIL_REGEX)
add_raw_repl_test(compile_verb-two_specs
	"d=$(mktemp -d) && printf 'o1[t] = 1.\\n' > $d/a.tau && printf 'o1[t] = 0.\\n' > $d/b.tau && ${TAU_RUN} compile $d/a.tau $d/b.tau; r=$?; rm -rf $d; exit $r"
	"tau compile: exactly one spec file is expected" NO_FAIL_REGEX REQUIRES hostfs)
add_raw_repl_test(compile_verb-empty_spec
	"d=$(mktemp -d) && : > $d/empty.tau && ${TAU_RUN} compile $d/empty.tau; r=$?; rm -rf $d; exit $r"
	"Spec file is empty: .*empty.tau" NO_FAIL_REGEX REQUIRES hostfs)
# `-` names stdin; an empty one is refused after the a.out/a.build defaults
# (with -o, after <out>.build) are chosen
add_raw_repl_test(compile_verb-stdin_empty "printf '' | ${TAU_RUN} compile -"
	"Spec file is empty: -" NO_FAIL_REGEX)
add_raw_repl_test(compile_verb-stdin_empty_with_output
	"printf '' | ${TAU_RUN} compile - -o prog"
	"Spec file is empty: -" NO_FAIL_REGEX)
# SDK resolution failures: none of these spawns a build
add_raw_repl_test(compile_verb-sdk_dir_env_missing
	"d=$(mktemp -d) && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && TAU_SDK_DIR=$d/no_sdk ${TAU_RUN} compile $d/s.tau -o $d/prog; r=$?; rm -rf $d; exit $r"
	"tau SDK not found at --sdk-dir / TAU_SDK_DIR: .*no_sdk" NO_FAIL_REGEX
	REQUIRES hostfs subprocess)
# the flag wins over the variable
add_raw_repl_test(compile_verb-sdk_dir_flag_wins
	"d=$(mktemp -d) && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && TAU_SDK_DIR=$d/env_sdk ${TAU_RUN} compile --sdk-dir $d/flag_sdk $d/s.tau -o $d/prog; r=$?; rm -rf $d; exit $r"
	"tau SDK not found at --sdk-dir / TAU_SDK_DIR: .*flag_sdk" NO_FAIL_REGEX
	REQUIRES hostfs subprocess)
# a platform with no SDK names the package to install
foreach(_case "debug-arm64|-linux-arm64" "debug-wasm-nothreads|-wasm32-emscripten"
		"debug-w64|-windows-x86_64-mingw" "debug-msvc|")
	string(REPLACE "|" ";" _case "${_case}")
	list(GET _case 0 _platform)
	list(LENGTH _case _n)
	set(_suffix "")
	if(_n GREATER 1)
		list(GET _case 1 _suffix)
	endif()
	add_raw_repl_test(compile_verb-no_sdk_for_${_platform}
		"d=$(mktemp -d) && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && ${TAU_RUN} compile $d/s.tau --preset ${_platform} -o $d/prog; r=$?; rm -rf $d; exit $r"
		"no SDK for ${_platform}. install tau-sdk${_suffix} or build it with ./dev preset ${_platform}"
		NO_FAIL_REGEX REQUIRES hostfs subprocess)
endforeach()
# a configure that fails reports the end of compile.log; -G/--generator and
# -D reach the configure (an unknown generator always fails it)
if(EXISTS "${CMAKE_BINARY_DIR}/sdk/TauConfig.cmake")
	add_raw_repl_test(compile_verb-configure_failure_is_reported
		"d=$(mktemp -d) && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && ${TAU_RUN} compile $d/s.tau -G NoSuchGenerator -D FOO=1 -o $d/prog; r=$?; rm -rf $d; exit $r"
		"compile: the cmake build failed" NO_FAIL_REGEX REQUIRES hostfs subprocess)
	add_raw_repl_test(compile_verb-generator_long_option
		"d=$(mktemp -d) && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && ${TAU_RUN} compile $d/s.tau --generator NoSuchGenerator --define FOO=1 -o $d/prog; r=$?; rm -rf $d; exit $r"
		"compile: the cmake build failed" NO_FAIL_REGEX REQUIRES hostfs subprocess)
	# with --preset and TAU_SDK_DIR, the SDK of the environment drives a
	# cross configure, which the unknown generator fails the same way
	add_raw_repl_test(compile_verb-preset_with_sdk_dir_env
		"d=$(mktemp -d) && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && TAU_SDK_DIR=${CMAKE_BINARY_DIR}/sdk ${TAU_RUN} compile $d/s.tau --preset debug-arm64 -G NoSuchGenerator -o $d/prog; r=$?; rm -rf $d; exit $r"
		"compile: the cmake build failed" NO_FAIL_REGEX REQUIRES hostfs subprocess)
endif()

# `tau gen --aiger` writes the verified circuit, its streams by name
add_raw_repl_test(compile_verb-aiger_arbiter
	"d=$(mktemp -d) && printf '((G !(i1[t]:bv[1] = 1 && i2[t]:bv[1] = 1))) -> ((G (i1[t]:bv[1] = 1 -> o1[t]:bv[1] = 1)) && (G (i2[t]:bv[1] = 1 -> o1[t]:bv[1] = 0)))\\n' > $d/arb.tau && ${TAU_RUN} gen --aiger $d/arb.tau && cat $d/arb.aag; r=$?; rm -rf $d; exit $r"
	"aag [0-9]+ 2 [0-9]+ 1 [0-9]+\n.*i[01] i1\n.*o0 o1" REQUIRES hostfs ltlsynt)
add_raw_repl_test(compile_verb-aiger_history_latch
	"d=$(mktemp -d) && printf 'G(o1[t]:bv[1] = i1[t-1]:bv[1])\\n' > $d/copy.tau && ${TAU_RUN} gen --aiger $d/copy.tau && cat $d/copy.aag; r=$?; rm -rf $d; exit $r"
	"aag [0-9]+ 1 [0-9]+ 1 [0-9]+\n" REQUIRES hostfs ltlsynt)
add_raw_repl_test(compile_verb-aiger_refuses_wide_streams
	"d=$(mktemp -d) && printf 'G(o1[t]:bv[2] = { 2 }:bv[2])\\n' > $d/w.tau && ${TAU_RUN} gen --aiger $d/w.tau; r=$?; rm -rf $d; exit $r"
	"takes bv\\[1\\] streams only" NO_FAIL_REGEX REQUIRES hostfs)
add_raw_repl_test(compile_verb-aiger_refuses_update_stream
	"d=$(mktemp -d) && printf 'G(o1[t]:bv[1] = u[t]:bv[1])\\n' > $d/s.tau && ${TAU_RUN} gen --aiger $d/s.tau; r=$?; rm -rf $d; exit $r"
	"does not take the update stream u" NO_FAIL_REGEX REQUIRES hostfs)
add_raw_repl_test(compile_verb-aiger_refuses_product
	"d=$(mktemp -d) && printf 'G(o1[t]:bv[1] = i1[t]:bv[1] * i2[t]:bv[1])\\n' > $d/s.tau && ${TAU_RUN} gen --aiger $d/s.tau; r=$?; rm -rf $d; exit $r"
	"takes atoms over bits only" NO_FAIL_REGEX REQUIRES hostfs ltlsynt)
# -o names the circuit, and a spec from stdin writes a.aag
add_raw_repl_test(compile_verb-aiger_output_path
	"d=$(mktemp -d) && printf 'G(o1[t]:bv[1] = i1[t-1]:bv[1])\\n' > $d/s.tau && ${TAU_RUN} gen --aiger $d/s.tau -o $d/c.aag && head -1 $d/c.aag; r=$?; rm -rf $d; exit $r"
	"\naag [0-9]+ 1 [0-9]+ 1 [0-9]+\n" REQUIRES hostfs ltlsynt)
add_raw_repl_test(compile_verb-aiger_stdin
	"d=$(mktemp -d) && cd $d && printf 'G(o1[t]:bv[1] = i1[t-1]:bv[1])\\n' | ${TAU_RUN} gen --aiger - && head -1 a.aag; r=$?; cd / && rm -rf $d; exit $r"
	"generated: a.aag.*\naag [0-9]+ 1 [0-9]+ 1 [0-9]+\n" REQUIRES hostfs ltlsynt)
add_raw_repl_test(compile_verb-aiger_refuses_unrealizable
	"d=$(mktemp -d) && printf '(G(o1[t]:bv[1] = 1)) && (F(o1[t]:bv[1] = 0))\\n' > $d/u.tau && ${TAU_RUN} gen --aiger $d/u.tau; r=$?; rm -rf $d; exit $r"
	"unrealizable: there is no circuit" NO_FAIL_REGEX REQUIRES hostfs ltlsynt)

# `--machine`: the program plays ltlsynt's verified machine of the bit game
# as a table and runs with no ltlsynt reachable. A delay keeps its history
# in the machine's state,
add_compile_machine_test(compile_verb-machine_delay
	"G(o1[t]:bv[1] = i1[t-1]:bv[1])"
	INPUT "1|0|1|1|0" EXPECT "o1[1] := 1|o1[2] := 0|o1[3] := 1|o1[4] := 1")
# an arbiter grants the request it sees,
add_compile_machine_test(compile_verb-machine_arbiter
	"((G !(i1[t]:bv[1] = 1 && i2[t]:bv[1] = 1))) -> ((G (i1[t]:bv[1] = 1 -> o1[t]:bv[1] = 1)) && (G (i2[t]:bv[1] = 1 -> o1[t]:bv[1] = 0)))"
	INPUT "1|0|0|1|1|0" EXPECT "o1[0] := 1|o1[1] := 0|o1[2] := 1")
# and a liveness goal is met beside a safety part.
add_compile_machine_test(compile_verb-machine_liveness
	"G(F(o1[t]:bv[1] = 1)) && G(o1[t]:bv[1] = 1 -> o1[t-1]:bv[1] = 0) && G(o2[t]:bv[1] = i1[t]:bv[1])"
	INPUT "0|1|0|1|0|1" EXPECT "o2[0] := 0|o2[1] := 1|o2[4] := 0|o2[5] := 1"
	EXPECT_REGEX "o1\\[[0-5]\\] := 1")
# `tau gen --machine` writes the table main, not the solving one,
add_raw_repl_test(compile_verb-machine_gen_table
	"d=$(mktemp -d) && printf 'G(o1[t]:bv[1] = i1[t-1]:bv[1])\\n' > $d/s.tau && ${TAU_RUN} gen --machine $d/s.tau -o $d/out && grep -q 'table_step_provider<node_t>>(' $d/out/main.cpp && ! grep -q get_interpreter $d/out/main.cpp && echo TABLE; r=$?; rm -rf $d; exit $r"
	"TABLE" REQUIRES hostfs bv ltlsynt)
# a spec outside the bit fragment follows `run`, with the reason,
add_raw_repl_test(compile_verb-machine_falls_back
	"d=$(mktemp -d) && printf 'G(o1[t]:bv[2] = i1[t]:bv[2])\\n' > $d/s.tau && ${TAU_RUN} gen --machine $d/s.tau -o $d/out && grep -q get_interpreter $d/out/main.cpp && echo SOLVING; r=$?; rm -rf $d; exit $r"
	"has no bit machine to play.*takes bv\\[1\\] streams only.*SOLVING"
	REQUIRES hostfs bv ltlsynt)
# a machine above compile-max-table-edges also follows `run`,
add_raw_repl_test(compile_verb-machine_above_table_edges
	"d=$(mktemp -d) && printf 'G(o1[t]:bv[1] = i1[t-1]:bv[1])\\n' > $d/s.tau && ${TAU_RUN} gen --machine $d/s.tau -o $d/out && grep -q get_interpreter $d/out/main.cpp && echo SOLVING; r=$?; rm -rf $d; exit $r"
	"more edges than compile-max-table-edges \\(value=[0-9]+\\).*SOLVING"
	ENV TAU_COMPILE_MAX_TABLE_EDGES=1 REQUIRES hostfs bv ltlsynt)
# and an unrealizable spec gets no program.
add_raw_repl_test(compile_verb-machine_unrealizable
	"d=$(mktemp -d) && printf '(G(o1[t]:bv[1] = 1)) && (F(o1[t]:bv[1] = 0))\\n' > $d/u.tau && ${TAU_RUN} gen --machine $d/u.tau -o $d/out; r=$?; rm -rf $d; exit $r"
	"spec is UNREALIZABLE" NO_FAIL_REGEX REQUIRES hostfs bv ltlsynt)
# Each machine program above also has its trace checked against the spec; on
# seeded random inputs too,
add_compile_machine_test(compile_verb-machine_delay_random
	"G(o1[t]:bv[1] = i1[t-1]:bv[1])" STEPS 16 SEED 7)
add_compile_machine_test(compile_verb-machine_arbiter_random
	"((G !(i1[t]:bv[1] = 1 && i2[t]:bv[1] = 1))) -> ((G (i1[t]:bv[1] = 1 -> o1[t]:bv[1] = 1)) && (G (i2[t]:bv[1] = 1 -> o1[t]:bv[1] = 0)))"
	STEPS 16 SEED 11)
add_compile_machine_test(compile_verb-machine_liveness_random
	"G(F(o1[t]:bv[1] = 1)) && G(o1[t]:bv[1] = 1 -> o1[t-1]:bv[1] = 0) && G(o2[t]:bv[1] = i1[t]:bv[1])"
	STEPS 16 SEED 13)
# a check that rests on `sat` of the spec and the trace: one wrong output
# makes it F.
add_repl_test(compile_verb-machine_trace_check_detects_a_violation
	"sat G(o1[t]:bv[1] = oi1[t-1]:bv[1]) && oi1[0]:bv[1] = 1 && o1[1]:bv[1] = 0"
	": F" REQUIRES ltlsynt bv)

# A fixed position: an output's is the machine's to set, the clause beside
# it starts at its lookback,
add_compile_machine_test(compile_verb-machine_fixed_output
	"o1[0]:bv[1] = 1 && G(o1[t]:bv[1] = i1[t-1]:bv[1])"
	INPUT "0|1|1|0" EXPECT "o1[0] := 1|o1[1] := 0|o1[2] := 1|o1[3] := 1")
# an input's at step 0 is a copy the machine keeps,
add_compile_machine_test(compile_verb-machine_fixed_input
	"G(o1[t]:bv[1] = i1[0]:bv[1])"
	INPUT "1|0|0|1" EXPECT "o1[0] := 1|o1[1] := 1|o1[2] := 1|o1[3] := 1")
# a conjunct of fixed positions alone reads them at its own step,
add_compile_machine_test(compile_verb-machine_fixed_conjunct
	"o1[2]:bv[1] = i1[1]:bv[1]" INPUT "0|1|0" EXPECT "o1[2] := 1")
# and under a temporal operator an input's after step 0 would have to be
# guessed at step 0,
add_raw_repl_test(compile_verb-aiger_refuses_later_fixed_input
	"d=$(mktemp -d) && printf 'F(o1[t]:bv[1] = i1[1]:bv[1])\\n' > $d/s.tau && ${TAU_RUN} gen --aiger $d/s.tau; r=$?; rm -rf $d; exit $r"
	"does not take an input at a fixed position after step 0"
	NO_FAIL_REGEX REQUIRES hostfs bv ltlsynt)
# A CTL* path quantifier has no circuit.
add_raw_repl_test(compile_verb-aiger_refuses_ctl_star
	"d=$(mktemp -d) && printf 'A G(o1[t]:bv[1] = 1)\\n' > $d/s.tau && ${TAU_RUN} gen --aiger $d/s.tau; r=$?; rm -rf $d; exit $r"
	"does not take CTL. operators: a circuit is one linear-time controller"
	NO_FAIL_REGEX REQUIRES hostfs bv ltlsynt)

# The machine of a spec with once or historically.
add_compile_machine_test(compile_verb-machine_once
	"G(o1[t]:bv[1] = 1 <-> once i1[t]:bv[1] = 1)"
	INPUT "0|1|0|0" EXPECT "o1[0] := 0|o1[1] := 1|o1[2] := 1|o1[3] := 1")
add_compile_machine_test(compile_verb-machine_historically
	"G(o1[t]:bv[1] = 1 <-> historically i1[t]:bv[1] = 1)"
	INPUT "1|0|1" EXPECT "o1[0] := 1|o1[1] := 0|o1[2] := 0")

# A -D value holding a semicolon reaches the configure whole.
if(EXISTS "${CMAKE_BINARY_DIR}/sdk/TauConfig.cmake")
	add_raw_repl_test(compile_verb-define_keeps_semicolon
		"d=$(mktemp -d) && cp ${CMAKE_SOURCE_DIR}/tests/codegen_specs/echo.tau $d/s.tau && ${TAU_RUN} compile $d/s.tau -o $d/p -D \"TAU_COMPILE_PROBE=/a$(printf '\\073')/b\" > /dev/null 2>&1; grep -h TAU_COMPILE_PROBE $d/s.tau.build/build/*/CMakeCache.txt; rm -rf $d"
		"TAU_COMPILE_PROBE:[A-Z]*=/a./b\n" NO_FAIL_REGEX REQUIRES hostfs subprocess)
endif()
