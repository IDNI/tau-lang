#
# `tau import-tlsf` over the TLSF files of tests/syntcomp: each realizable
# file gives a verified circuit through `tau gen --aiger`, which also checks
# the verdict of `realizable`; each unrealizable file gives none.
#

function(add_tlsf_aiger_test name expected)
	add_raw_repl_test(tlsf_import-${name}
		"d=$(mktemp -d) && ${TAU_RUN} import-tlsf ${CMAKE_SOURCE_DIR}/tests/syntcomp/${name}.tlsf -o $d/s.tau && ${TAU_RUN} gen --aiger $d/s.tau && head -1 $d/s.aag; r=$?; rm -rf $d; exit $r"
		"${expected}" ${ARGN} REQUIRES hostfs bv ltlsynt)
endfunction()

foreach(_tlsf arbiter2 arbiter2_assumed delay toggle response_assumed
		until_handshake)
	add_tlsf_aiger_test(${_tlsf} "\naag [0-9]+ [0-9]+ [0-9]+ [0-9]+ [0-9]+\n")
endforeach()

foreach(_tlsf arbiter2_no_wait predict response_unfair)
	add_tlsf_aiger_test(${_tlsf} "unrealizable: there is no circuit"
		NO_FAIL_REGEX)
endforeach()

# a TLSF signal becomes a bv[1] console stream of its name
add_raw_repl_test(tlsf_import-streams_and_sections
	"${TAU_RUN} import-tlsf ${CMAKE_SOURCE_DIR}/tests/syntcomp/arbiter2_assumed.tlsf"
	"r0 : bv\\[1\\] := in console\\.\n.*g0 : bv\\[1\\] := out console\\.\n.*always" REQUIRES hostfs)

add_raw_repl_test(tlsf_import-moore_refused
	"${TAU_RUN} import-tlsf ${CMAKE_SOURCE_DIR}/tests/syntcomp/moore_refused.tlsf"
	"Mealy semantics only" NO_FAIL_REGEX REQUIRES hostfs)

add_raw_repl_test(tlsf_import-not_tlsf_refused
	"d=$(mktemp -d) && printf 'MAIN { }\\n' > $d/x.tlsf && ${TAU_RUN} import-tlsf $d/x.tlsf; r=$?; rm -rf $d; exit $r"
	"not a TLSF basic specification" NO_FAIL_REGEX REQUIRES hostfs)

# REQUIRE is read under always and INITIALLY guards the rest; R and W keep
# their operators
add_raw_repl_test(tlsf_import-require_initially_release_weak_until
	"d=$(mktemp -d) && printf 'INFO { SEMANTICS: Mealy TARGET: Mealy }\\nMAIN { INPUTS { r; } OUTPUTS { g; } INITIALLY { !r; } REQUIRE { r -> X !r; } GUARANTEE { g R r; r W g; } }\\n' > $d/s.tlsf && ${TAU_RUN} import-tlsf $d/s.tlsf; r=$?; rm -rf $d; exit $r"
	"\\(! \\(r\\[0\\]:bv\\[1\\] = 1\\)\\) -> \\(\\(\\(always \\(\\(r\\[t\\]:bv\\[1\\] = 1\\) -> \\(! \\(r\\[t\\+1\\]:bv\\[1\\] = 1\\)\\)\\)\\) -> [^\n]* R [^\n]* W "
	REQUIRES hostfs)

add_raw_repl_test(tlsf_import-operator_name_refused
	"d=$(mktemp -d) && printf 'INFO { SEMANTICS: Mealy TARGET: Mealy }\\nMAIN { INPUTS { u; } OUTPUTS { g; } GUARANTEE { G g; } }\\n' > $d/s.tlsf && ${TAU_RUN} import-tlsf $d/s.tlsf; r=$?; rm -rf $d; exit $r"
	"a TLSF signal is named like a Tau operator \\(name=u\\)" NO_FAIL_REGEX REQUIRES hostfs)

add_raw_repl_test(tlsf_import-undeclared_signal_refused
	"d=$(mktemp -d) && printf 'INFO { SEMANTICS: Mealy TARGET: Mealy }\\nMAIN { INPUTS { r; } OUTPUTS { g; } GUARANTEE { G z; } }\\n' > $d/s.tlsf && ${TAU_RUN} import-tlsf $d/s.tlsf; r=$?; rm -rf $d; exit $r"
	"names a signal that is neither an input nor an output \\(name=z\\)" NO_FAIL_REGEX REQUIRES hostfs)

add_raw_repl_test(tlsf_import-no_file_usage
	"${TAU_RUN} import-tlsf"
	"Usage: tau import-tlsf <spec.tlsf>" NO_FAIL_REGEX)

# MSYS bash on Windows hands tau the path under its own root, C:/Program Files/Git.
add_raw_repl_test(tlsf_import-unwritable_output
	"${TAU_RUN} import-tlsf ${CMAKE_SOURCE_DIR}/tests/syntcomp/delay.tlsf -o /nonexistent-tau-dir/s.tau"
	"Cannot write file: [^\n]*/nonexistent-tau-dir/s.tau" NO_FAIL_REGEX REQUIRES hostfs)

# `||`, `true` and `false` become Tau's ||, T and F; g = 1 meets the
# guarantee whatever r is
add_raw_repl_test(tlsf_import-or_true_false
	"d=$(mktemp -d) && printf 'INFO { SEMANTICS: Mealy TARGET: Mealy }\\nMAIN { INPUTS { r; } OUTPUTS { g; } GUARANTEE { G (r || g); true; !false; } }\\n' > $d/s.tlsf && ${TAU_RUN} import-tlsf $d/s.tlsf -o $d/s.tau && cat $d/s.tau && ${TAU_RUN} gen --aiger $d/s.tau && cat $d/s.aag; r=$?; rm -rf $d; exit $r"
	"\\(always \\(\\(r\\[t\\]:bv\\[1\\] = 1\\) \\|\\| \\(g\\[t\\]:bv\\[1\\] = 1\\)\\)\\) && T && \\(! F\\)\n.*\naag 1 1 0 1 0\n2\n1\n"
	REQUIRES hostfs bv ltlsynt)
