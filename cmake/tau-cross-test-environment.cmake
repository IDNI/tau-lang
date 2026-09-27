include_guard(GLOBAL)

# ctest prefixes a built target with the emulator, so a binary a test starts
# from a script reaches binfmt; qemu takes the loader path from QEMU_LD_PREFIX.
# Property TESTS covers this directory alone, so call after its last add_test().
function(tau_set_cross_test_environment)
	get_property(_tests DIRECTORY PROPERTY TESTS)
	if(NOT _tests)
		return()
	endif()
	if(NOT DEFINED TAU_AARCH64_SYSROOT OR NOT CMAKE_CROSSCOMPILING_EMULATOR)
		return()
	endif()
	foreach(_test ${_tests})
		set_property(TEST "${_test}" APPEND PROPERTY ENVIRONMENT
			"QEMU_LD_PREFIX=${TAU_AARCH64_SYSROOT}")
	endforeach()
endfunction()
