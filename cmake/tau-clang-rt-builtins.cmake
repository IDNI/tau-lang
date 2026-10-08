# To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

# The tau build and the installed TauConfig.cmake share this lookup.
include_guard(GLOBAL)

# Sets <out_var> to the clang_rt.builtins library of the clang-cl compiler
# <cxx>. clang-cl does not link the builtins that __int128 needs.
function(tau_find_clang_rt_builtins cxx out_var)
	execute_process(
		COMMAND "${cxx}" -print-resource-dir
		OUTPUT_VARIABLE _tau_resource_dir
		OUTPUT_STRIP_TRAILING_WHITESPACE
		RESULT_VARIABLE _tau_resource_rc
		ERROR_QUIET)
	set(_tau_builtins
		"${_tau_resource_dir}/lib/windows/clang_rt.builtins-x86_64.lib")
	if(NOT _tau_resource_rc EQUAL 0 OR NOT _tau_resource_dir
		OR NOT EXISTS "${_tau_builtins}")
		message(FATAL_ERROR
			"clang-cl needs clang_rt.builtins-x86_64.lib for __int128. "
			"The compiler resource folder does not hold it: "
			"'${_tau_resource_dir}'")
	endif()
	set(${out_var} "${_tau_builtins}" PARENT_SCOPE)
endfunction()
