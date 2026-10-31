include(add_repl_test)

#
# One spec through two entry points: the spec file and `run` in the REPL print
# the same outputs on the same inputs.
#

file(GLOB _inputs "${CMAKE_SOURCE_DIR}/tests/codegen_specs/*.in")
foreach(_in ${_inputs})
	get_filename_component(_name "${_in}" NAME_WE)
	set(_spec "${CMAKE_SOURCE_DIR}/tests/codegen_specs/${_name}.tau")
	file(READ "${_spec}" _text)
	add_raw_repl_test(entry_parity-${_name}
		"\"${CMAKE_CURRENT_SOURCE_DIR}/../check_entry_parity.sh\" ${TAU_RUN} \"${_spec}\" \"${_in}\" # ${_text}"
		"SAME: " NO_FAIL_REGEX REQUIRES ltlsynt hostfs)
endforeach()
