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
