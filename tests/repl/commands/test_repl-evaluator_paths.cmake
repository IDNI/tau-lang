#
# Paths of the REPL evaluator and of `tau gen`/`tau compile` that the command
# suites leave out: a run with the benchmarks switched off (its warnings
# still print, its timings do not), `memory` while a run waits for input, a
# flag command on an option that does not exist, help on a word with no page,
# where a parse error of a `set` line points, the refusals of an artifact
# that cannot be written or built, what `tau compile` makes of the result of
# an SDK box's build script and where it finds an installed box, a spec path
# that cannot be read, and a synthesis tool that is missing, not executable or
# gives no verdict.
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
if(TEST test_repl-evaluator_paths-set_phi_option_error_points_at_the_terminator)
	set_tests_properties(
		test_repl-evaluator_paths-set_phi_option_error_points_at_the_terminator
		PROPERTIES WILL_FAIL TRUE)
endif()

# `tau gen` refuses a spec with a free variable.
add_raw_repl_test(evaluator_paths-gen_free_variable
	"d=$(mktemp -d) && printf 'o1[t]:sbf = x.\\n' > $d/s.tau && ${TAU_RUN} gen $d/s.tau -o $d/out; r=$?; rm -rf $d; exit $r"
	"the specification contains free variables" NO_FAIL_REGEX REQUIRES hostfs)

# `tau gen` names an output directory it cannot create or read. Windows
# makes /dev/null/out a directory of the current drive.
if(NOT WIN32)
	add_raw_repl_test(evaluator_paths-gen_output_dir_not_creatable
		"d=$(mktemp -d) && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && ${TAU_RUN} gen $d/s.tau -o /dev/null/out; r=$?; rm -rf $d; exit $r"
		"gen: cannot create output dir /dev/null/out" NO_FAIL_REGEX REQUIRES hostfs)
endif()
# skipped (exit 77) where the permission bits do not keep the reader out
add_raw_repl_test(evaluator_paths-gen_output_dir_not_readable
	"d=$(mktemp -d) && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && mkdir $d/out && chmod 000 $d/out && if ls $d/out >/dev/null 2>&1; then chmod 755 $d/out; rm -rf $d; exit 77; fi; ${TAU_RUN} gen $d/s.tau -o $d/out; r=$?; chmod 755 $d/out; rm -rf $d; exit $r"
	"gen: cannot read .*/out: " NO_FAIL_REGEX REQUIRES hostfs)
if(TEST test_repl-evaluator_paths-gen_output_dir_not_readable)
	set_tests_properties(test_repl-evaluator_paths-gen_output_dir_not_readable
		PROPERTIES SKIP_RETURN_CODE 77)
endif()

# `tau gen` into an artifact directory names the file it cannot write (the
# sources, the presets, the platform map, each toolchain file and the
# marker): each case puts a directory, or a plain file, where gen writes. A
# `.` matches the separator, which is `\` on Windows.
foreach(_case
		"main_cpp|mkdir $d/out/main.cpp|gen: cannot write main.cpp in .*.out"
		"cmakelists|mkdir $d/out/CMakeLists.txt|gen: cannot write CMakeLists.txt in .*.out"
		"presets|mkdir $d/out/CMakePresets.json|gen: cannot write .*.out.CMakePresets.json"
		"toolchains|touch $d/out/cmake|gen: cannot create .*.out.cmake.toolchains"
		"platforms|mkdir $d/out/platforms.json|gen: cannot write .*.out.platforms.json"
		"mingw_toolchain|mkdir -p $d/out/cmake/toolchains/mingw-w64-x86_64.cmake|gen: cannot write .*.out.cmake.toolchains.mingw-w64-x86_64.cmake"
		"arm64_toolchain|mkdir -p $d/out/cmake/toolchains/aarch64-linux-gnu.cmake|gen: cannot write .*.out.cmake.toolchains.aarch64-linux-gnu.cmake"
		"marker|rm $d/out/.tau-artifact && mkdir $d/out/.tau-artifact|gen: cannot write .*.out..tau-artifact")
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

# A duplicate pattern in a substitution into an input that does not infer
# reports the duplicate and the failed inference.
add_repl_test(evaluator_paths-subst_duplicate_pattern_uninferred_input
	"s a + b = c [a / d, a / e]"
	"Duplicate match pattern in substitution \\(value=a\\).*Type inference failed"
	NO_FAIL_REGEX)

# `valid` of a full-LTL formula is UNKNOWN when ltlsynt gives no verdict.
add_raw_repl_test(evaluator_paths-valid_full_ltl_without_verdict
	"TAU_SPOT_BIN= TAU_SHARED_PREFIX= USERPROFILE=/no-home HOME=/no-home PATH=${CMAKE_CURRENT_SOURCE_DIR}/../stubs:$PATH ${TAU_RUN} -e \"valid (o1[t] = 1) U (o1[t] = 0)\""
	"UNKNOWN: validity of this full-LTL formula could not be decided"
	FAIL_REGEX "Aborted|core dumped")

# A tool missing from PATH runs from the spot-bin directory, where the
# failing stub makes the verdict UNKNOWN; a tool on PATH that cannot be
# executed is a failed spawn, never a verdict.
if(NOT CMAKE_CROSSCOMPILING_EMULATOR)
	add_raw_repl_test(evaluator_paths-ltlsynt_from_spot_bin
		"TAU_SPOT_BIN=${CMAKE_CURRENT_SOURCE_DIR}/../stubs TAU_SHARED_PREFIX= USERPROFILE=/no-home HOME=/no-home PATH=/nonexistent ${TAU_RUN} -e \"sat (o1[t] = 1) U (o1[t] = 0)\""
		"UNKNOWN: the synthesis backend failed"
		FAIL_REGEX ": T|Aborted|core dumped")
	add_raw_repl_test(evaluator_paths-ltlsynt_not_executable
		"d=$(mktemp -d) && printf '#!/bin/sh\\nexit 0\\n' > $d/ltlsynt && chmod 644 $d/ltlsynt && TAU_SPOT_BIN= TAU_SHARED_PREFIX= USERPROFILE=/no-home HOME=/no-home PATH=$d ${TAU_RUN} -e \"sat (o1[t] = 1) U (o1[t] = 0)\"; r=$?; rm -rf $d; exit $r"
		"UNKNOWN: the synthesis backend failed"
		FAIL_REGEX ": T|Aborted|core dumped" REQUIRES hostfs)
endif()

# A spec path naming a directory cannot be read, by `tau <spec>` and by
# every verb that reads a spec. The node mount of a wasm build opens a
# directory as an empty file.
if(NOT EMSCRIPTEN)
	add_raw_repl_test(evaluator_paths-run_spec_is_a_directory
		"d=$(mktemp -d) && mkdir $d/s.tau && ${TAU_RUN} $d/s.tau; r=$?; rm -rf $d; exit $r"
		"Cannot open file .*/s.tau" NO_FAIL_REGEX REQUIRES hostfs)
endif()
foreach(_verb gen compile import-tlsf)
	add_raw_repl_test(evaluator_paths-${_verb}_spec_is_a_directory
		"d=$(mktemp -d) && mkdir $d/s.tau && ${TAU_RUN} ${_verb} $d/s.tau; r=$?; rm -rf $d; exit $r"
		"Cannot open file: .*/s.tau" NO_FAIL_REGEX REQUIRES hostfs subprocess)
endforeach()

# `tau gen --aiger` names the circuit file it cannot write.
add_raw_repl_test(evaluator_paths-gen_aiger_cannot_write
	"d=$(mktemp -d) && printf 'G(o1[t]:bv[1] = i1[t-1]:bv[1])\\n' > $d/s.tau && ${TAU_RUN} gen --aiger $d/s.tau -o $d/no_dir/c.aag; r=$?; rm -rf $d; exit $r"
	"Cannot write file: .*/no_dir/c.aag" NO_FAIL_REGEX REQUIRES hostfs bv ltlsynt)

# `ltl-export-strategy` takes `hoa`, `dot` or nothing.
add_repl_test(evaluator_paths-ltl_export_strategy_unknown_format
	"set ltl-export-strategy foo"
	"The option does not take the value \\(name=ltl-export-strategy value=foo\\)"
	NO_FAIL_REGEX)

# A write the disk refuses fails `tau gen`, although the presets text fits
# in the stream buffer and the refusal comes only when the file closes. The
# node mount of a wasm build does not pass the refusal of /dev/full on.
if(NOT EMSCRIPTEN)
	add_raw_repl_test(evaluator_paths-gen_presets_on_a_full_disk
		"[ -w /dev/full ] || exit 77; d=$(mktemp -d) && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && mkdir $d/out && touch $d/out/.tau-artifact && ln -s /dev/full $d/out/CMakePresets.json && ${TAU_RUN} gen $d/s.tau -o $d/out; r=$?; rm -rf $d; exit $r"
		"gen: cannot write .*.out.CMakePresets.json" NO_FAIL_REGEX REQUIRES hostfs)
endif()
if(TEST test_repl-evaluator_paths-gen_presets_on_a_full_disk)
	set_tests_properties(test_repl-evaluator_paths-gen_presets_on_a_full_disk
		PROPERTIES SKIP_RETURN_CODE 77)
endif()

# Stand-in build scripts of an SDK box: each leaves what a build would, so
# what `tau compile` makes of the result runs without a compiler.
set(_ep_scripts "${CMAKE_CURRENT_BINARY_DIR}/evaluator_paths_sdk_scripts")
file(WRITE "${_ep_scripts}/nothing.cmake" "")
file(WRITE "${_ep_scripts}/native.cmake"
	"file(WRITE \"\${TAU_OUTPUT}\" \"x\")\nmessage(\"sdk=\${TAU_SDK_DIR}\")\n")
file(WRITE "${_ep_scripts}/js_only.cmake"
	"file(WRITE \"\${TAU_OUTPUT}.js\" \"x\")\n")
file(WRITE "${_ep_scripts}/wasm.cmake"
	"file(WRITE \"\${TAU_OUTPUT}.js\" \"x\")\nfile(WRITE \"\${TAU_OUTPUT}.wasm\" \"x\")\nmessage(\"deps=\${TAU_SDK_DEPS}\")\n")
file(WRITE "${_ep_scripts}/killed.cmake"
	"execute_process(COMMAND sh -c \"kill -9 \$PPID\")\n")

# `tau compile` over a box whose script builds nothing, only the js loader
# of a wasm program, a whole wasm program (the box's deps file reaches the
# script), or dies without a word. On Windows the script's sh cannot see
# cmake as its parent, so it cannot kill it.
set(_ep_cases
	"script_builds_nothing|nothing|the cmake build produced no [^\n]*.prog[^.]"
	"script_builds_js_only|js_only|the cmake build produced no [^\n]*.prog\\.wasm"
	"script_builds_wasm|wasm|compiled: [^\n]*.prog\\.js.*deps=[^\n]*.sdk.tau-sdk-deps\\.cmake")
if(NOT WIN32)
	list(APPEND _ep_cases
		"script_killed_silently|killed|compile: the cmake build failed \\(path=[^\n]*compile\\.log value=\\(no output\\)\\)")
endif()
foreach(_case IN LISTS _ep_cases)
	string(REPLACE "|" ";" _case "${_case}")
	list(GET _case 0 _what)
	list(GET _case 1 _script)
	list(GET _case 2 _regex)
	add_raw_repl_test(evaluator_paths-compile_${_what}
		"d=$(mktemp -d) && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && mkdir -p $d/sdk/cmake && touch $d/sdk/TauConfig.cmake $d/sdk/tau-sdk-deps.cmake && cp ${_ep_scripts}/${_script}.cmake $d/sdk/cmake/tau-compile.cmake && ${TAU_RUN} compile --sdk-dir $d/sdk $d/s.tau -o $d/prog; r=$?; cat $d/s.tau.build/compile.log; rm -rf $d; exit $r"
		"${_regex}" NO_FAIL_REGEX REQUIRES hostfs subprocess)
endforeach()

# A program that cannot replace the output (a directory with content there)
# stays beside it under its temporary name, and compile says so.
add_raw_repl_test(evaluator_paths-compile_output_is_a_directory
	"d=$(mktemp -d) && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && mkdir -p $d/sdk/cmake $d/prog/keep && touch $d/sdk/TauConfig.cmake && cp ${_ep_scripts}/native.cmake $d/sdk/cmake/tau-compile.cmake && ${TAU_RUN} compile --sdk-dir $d/sdk $d/s.tau -o $d/prog; [ -f $d/prog.tau-build ] && echo BUILT_LEFT_BESIDE; rm -rf $d"
	"compile: cannot move [^\n]*prog\\.tau-build to [^\n]*prog .*the program is left there.*BUILT_LEFT_BESIDE"
	FAIL_REGEX "produced no" REQUIRES hostfs subprocess)

# `tau compile` finds an installed box by the path of its own binary: under
# <prefix>/lib/tau/sdk, under a multiarch lib dir, for a --preset by the
# box's own folder, by the platform's build tree beside the binary, or by
# the platform marker of a flat box. A marker naming another platform is no
# box for the preset. Each case runs a link of tau inside a fake prefix.
if(NOT CMAKE_CROSSCOMPILING_EMULATOR AND NOT CMAKE_HOST_WIN32)
	set(_ep_tau "$<TARGET_FILE:${TAU_EXECUTABLE_NAME}>")
	foreach(_case
			"installed_box||lib/tau/sdk/box/lib/cmake/Tau|true|compiled: "
			"multiarch_box||lib/x86_64-linux-gnu/tau/sdk/box/lib/cmake/Tau|true|compiled: "
			"preset_multiarch_box|--preset release|lib/x86_64-linux-gnu/tau/sdk/release/lib/cmake/Tau|true|compiled: "
			"preset_platform_tree|--preset release|release/sdk|true|compiled: .*sdk=[^\n]*/release/sdk"
			"preset_marked_box|--preset release|bin/cmake/Tau|printf 'release\\r\\n' > $d/bin/cmake/Tau/cmake/tau-this-platform.txt|compiled: "
			"preset_box_marked_for_another_platform|--preset release|bin/cmake/Tau|printf 'debug\\n' > $d/bin/cmake/Tau/cmake/tau-this-platform.txt|no SDK for release. install tau-sdk or build it with ./dev preset release")
		string(REPLACE "|" ";" _case "${_case}")
		list(GET _case 0 _what)
		list(GET _case 1 _preset)
		list(GET _case 2 _box)
		list(GET _case 3 _marker)
		list(GET _case 4 _regex)
		add_raw_repl_test(evaluator_paths-compile_${_what}
			"d=$(mktemp -d ${CMAKE_BINARY_DIR}/tau-prefix.XXXXXX) && mkdir $d/bin && { ln ${_ep_tau} $d/bin/tau 2>/dev/null || cp ${_ep_tau} $d/bin/tau; } && printf 'o1[t]:sbf = i1[t]:sbf.\\n' > $d/s.tau && mkdir -p $d/${_box}/cmake && touch $d/${_box}/TauConfig.cmake && cp ${_ep_scripts}/native.cmake $d/${_box}/cmake/tau-compile.cmake && ${_marker} && $d/bin/tau compile ${_preset} $d/s.tau -o $d/prog; r=$?; cat $d/s.tau.build/compile.log; rm -rf $d; exit $r"
			"${_regex}" NO_FAIL_REGEX REQUIRES hostfs subprocess)
	endforeach()
endif()

# A write the disk refuses fails `gen --aiger -o` and `import-tlsf -o`.
if(NOT EMSCRIPTEN)
	add_raw_repl_test(evaluator_paths-gen_aiger_on_a_full_disk
		"[ -w /dev/full ] || exit 77; d=$(mktemp -d) && printf 'G(o1[t]:bv[1] = i1[t-1]:bv[1])\\n' > $d/s.tau && ${TAU_RUN} gen --aiger -o /dev/full $d/s.tau; r=$?; rm -rf $d; echo exit=$r"
		"Cannot write file: /dev/full.*exit=1" NO_FAIL_REGEX REQUIRES hostfs bv ltlsynt)
	set_tests_properties(test_repl-evaluator_paths-gen_aiger_on_a_full_disk PROPERTIES SKIP_RETURN_CODE 77)
	add_raw_repl_test(evaluator_paths-import_tlsf_on_a_full_disk
		"[ -w /dev/full ] || exit 77; ${TAU_RUN} import-tlsf ${CMAKE_SOURCE_DIR}/tests/syntcomp/arbiter2.tlsf -o /dev/full; echo exit=$?"
		"Cannot write file: /dev/full.*exit=1" NO_FAIL_REGEX REQUIRES hostfs)
	set_tests_properties(test_repl-evaluator_paths-import_tlsf_on_a_full_disk PROPERTIES SKIP_RETURN_CODE 77)
endif()
