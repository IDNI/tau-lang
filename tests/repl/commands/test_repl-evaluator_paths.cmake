#
# Paths of the REPL evaluator and of `tau gen`/`tau compile` that the command
# suites leave out: a run with the benchmarks switched off (its warnings
# still print, its timings do not), `memory` while a run waits for input, a
# flag command on an option that does not exist, help on a word with no page,
# where a parse error of a `set` line points, and the refusals of an artifact
# that cannot be written or built.
#

include(add_repl_test)
include(tau_repl_pack)

# With the benchmarks off a bounded run prints its steps and no timing.
add_repl_test(evaluator_paths-run_without_benchmarks
	"disable benchmarks. o1:tau := out console. run 2 steps G (o1[t]:tau = 0)."
	"o1\\[0\\] := F.*o1\\[1\\] := F"
	FAIL_REGEX "Error|read_step_inputs")

# With the benchmarks off the warning of a step still prints.
add_multiline_repl_test(evaluator_paths-run_without_benchmarks_warns
	"Warning.*the update stream u solved to 0"
	STDIN "disable benchmarks\\nrun u[t] = i1[t] && o1[t] = 0.\\no1[t] = 1 && o1[t] = 0.\\nq\\nq\\n"
	FAIL_REGEX "Error|read_step_inputs")

# A run waiting for its first input holds no binding yet.
add_repl_test(evaluator_paths-memory_before_the_first_step
	"run o1[t] = i1[t]. memory"
	"memory: \\{\\}\n0 bindings")

# enable, disable and toggle name an option the repository does not know.
foreach(_cmd enable disable toggle)
	add_repl_test(evaluator_paths-${_cmd}_unknown_option
		"${_cmd} nosuchopt" "Unknown option \\(name=nosuchopt\\)")
endforeach()

# `help` on a word that has no page of its own says so.
add_repl_test(evaluator_paths-help_without_a_page
	"help file" "no help available for this command; `help` lists the commands")

# The parse error of a `set` line ended by `;` instead of `.` points at the
# `;`. With a name starting with `phi-` it points inside the name instead.
add_repl_test(evaluator_paths-set_error_points_at_the_terminator
	"set pwr-semantic on;" "Unexpected ';' at 1:20" NO_FAIL_REGEX)
add_repl_test(evaluator_paths-set_phi_option_error_points_at_the_terminator
	"set phi-delta-crosscheck on;" "Unexpected ';' at 1:28" NO_FAIL_REGEX)
set_tests_properties(
	test_repl-evaluator_paths-set_phi_option_error_points_at_the_terminator
	PROPERTIES WILL_FAIL TRUE)

# `tau gen` refuses a spec with a free variable.
add_raw_repl_test(evaluator_paths-gen_free_variable
	"d=$(mktemp -d) && printf 'o1[t]:sbf = x.\\n' > $d/s.tau && ${TAU_RUN} gen $d/s.tau -o $d/out; r=$?; rm -rf $d; exit $r"
	"the specification contains free variables" NO_FAIL_REGEX REQUIRES hostfs)

# `tau gen` names an output directory it cannot create or read.
add_raw_repl_test(evaluator_paths-gen_output_dir_not_creatable
	"d=$(mktemp -d) && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && ${TAU_RUN} gen $d/s.tau -o /dev/null/out; r=$?; rm -rf $d; exit $r"
	"gen: cannot create output dir /dev/null/out" NO_FAIL_REGEX REQUIRES hostfs)
# skipped (exit 77) where the permission bits do not keep the reader out
add_raw_repl_test(evaluator_paths-gen_output_dir_not_readable
	"d=$(mktemp -d) && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && mkdir $d/out && chmod 000 $d/out && if ls $d/out >/dev/null 2>&1; then chmod 755 $d/out; rm -rf $d; exit 77; fi; ${TAU_RUN} gen $d/s.tau -o $d/out; r=$?; chmod 755 $d/out; rm -rf $d; exit $r"
	"gen: cannot read .*/out: " NO_FAIL_REGEX REQUIRES hostfs)
set_tests_properties(test_repl-evaluator_paths-gen_output_dir_not_readable
	PROPERTIES SKIP_RETURN_CODE 77)

# `tau gen` into an artifact directory names the file it cannot write: each
# case puts a directory, or a plain file, where gen writes.
foreach(_case
		"main_cpp|mkdir $d/out/main.cpp|gen: cannot write main.cpp in .*/out"
		"cmakelists|mkdir $d/out/CMakeLists.txt|gen: cannot write CMakeLists.txt in .*/out"
		"presets|mkdir $d/out/CMakePresets.json|gen: cannot write .*/out/CMakePresets.json"
		"toolchains|touch $d/out/cmake|gen: cannot create .*/out/cmake/toolchains")
	string(REPLACE "|" ";" _case "${_case}")
	list(GET _case 0 _what)
	list(GET _case 1 _block)
	list(GET _case 2 _regex)
	add_raw_repl_test(evaluator_paths-gen_cannot_write_${_what}
		"d=$(mktemp -d) && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && mkdir $d/out && touch $d/out/.tau-artifact && ${_block} && ${TAU_RUN} gen $d/s.tau -o $d/out; r=$?; rm -rf $d; exit $r"
		"${_regex}" NO_FAIL_REGEX REQUIRES hostfs)
endforeach()

# `tau compile` refuses an SDK box with no build script, with and without a
# preset, before it starts a build.
foreach(_preset "" "--preset release")
	if(_preset)
		set(_name compile_preset_sdk_without_script)
	else()
		set(_name compile_sdk_without_script)
	endif()
	add_raw_repl_test(evaluator_paths-${_name}
		"d=$(mktemp -d) && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && mkdir $d/sdk && touch $d/sdk/TauConfig.cmake && ${TAU_RUN} compile ${_preset} --sdk-dir $d/sdk $d/s.tau -o $d/prog; r=$?; rm -rf $d; exit $r"
		"compile: the SDK has no cmake/tau-compile.cmake at .*/sdk" NO_FAIL_REGEX
		REQUIRES hostfs subprocess)
endforeach()

# An always whose body is only always clauses splits into those clauses.
add_repl_test(evaluator_paths-always_of_always_clauses_sat
	"sat always (always o1[t] = 0 && always o2[t] = 0)" ": T")
add_repl_test(evaluator_paths-always_of_always_clauses_unsat
	"sat always (always o1[t] = 0 && always o1[t] != 0)" ": F")

# A sometimes or until clause reading only later steps is anchored at the
# start of the clauses that read ahead: o1 is 1 from step 1 on.
add_repl_test(evaluator_paths-forward_reference_sometimes_sat
	"sat (G(o1[t+1]:bv[1] = 1)) && (F(o2[t+1]:bv[1] = 1))" ": T"
	REQUIRES ltlsynt bv)
add_repl_test(evaluator_paths-forward_reference_sometimes_unsat
	"sat (G(o1[t+1]:bv[1] = 1)) && (F(o1[t+1]:bv[1] = 0))" ": F"
	REQUIRES ltlsynt bv)
add_repl_test(evaluator_paths-forward_reference_until_sat
	"sat (G(o1[t+1]:bv[1] = 1)) && ((o2[t+1]:bv[1] = 0) U (o3[t+1]:bv[1] = 1))" ": T"
	REQUIRES ltlsynt bv)
add_repl_test(evaluator_paths-forward_reference_until_unsat
	"sat (G(o1[t+1]:bv[1] = 1)) && ((o2[t+1]:bv[1] = 0) U (o1[t+1]:bv[1] = 0))" ": F"
	REQUIRES ltlsynt bv)
