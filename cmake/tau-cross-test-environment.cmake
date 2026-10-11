include_guard(GLOBAL)

# The Z: form of a build host path, the form a PE under wine reads.
function(tau_wine_path out path)
	string(REPLACE "/" "\\" _path "${path}")
	set(${out} "Z:${_path}" PARENT_SCOPE)
endfunction()

# The DLL folders a test PE loads from under wine: the store packages and the
# MinGW runtime. wine has no LD_LIBRARY_PATH, so they travel in WINEPATH.
function(_tau_wine_dll_path out)
	get_property(_known GLOBAL PROPERTY TAU_WINE_DLL_PATH SET)
	if(_known)
		get_property(_winepath GLOBAL PROPERTY TAU_WINE_DLL_PATH)
		set(${out} "${_winepath}" PARENT_SCOPE)
		return()
	endif()
	set(_dirs "")
	foreach(_prefix IN ITEMS "${CVC5_STORE_PREFIX}" "${BOOST_STORE_PREFIX}"
			"${TAU_CURL_PREFIX}")
		if(NOT _prefix)
			continue()
		endif()
		foreach(_sub bin lib)
			file(GLOB _dlls "${_prefix}/${_sub}/*.dll")
			if(_dlls)
				list(APPEND _dirs "${_prefix}/${_sub}")
			endif()
		endforeach()
	endforeach()
	foreach(_dll libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll)
		execute_process(COMMAND "${CMAKE_CXX_COMPILER}" "-print-file-name=${_dll}"
			OUTPUT_VARIABLE _dll_path OUTPUT_STRIP_TRAILING_WHITESPACE)
		if(IS_ABSOLUTE "${_dll_path}" AND EXISTS "${_dll_path}")
			get_filename_component(_dir "${_dll_path}" DIRECTORY)
			list(APPEND _dirs "${_dir}")
		endif()
	endforeach()
	list(REMOVE_DUPLICATES _dirs)
	set(_winepath "")
	foreach(_dir ${_dirs})
		tau_wine_path(_wine_dir "${_dir}")
		list(APPEND _winepath "${_wine_dir}")
	endforeach()
	set_property(GLOBAL PROPERTY TAU_WINE_DLL_PATH "${_winepath}")
	set(${out} "${_winepath}" PARENT_SCOPE)
endfunction()

# ctest prefixes a built target with the emulator, so a binary a test starts
# from a script reaches binfmt; qemu takes the loader path from QEMU_LD_PREFIX.
# Property TESTS covers this directory alone, so call after its last add_test().
function(tau_set_cross_test_environment)
	get_property(_tests DIRECTORY PROPERTY TESTS)
	if(NOT _tests OR NOT CMAKE_CROSSCOMPILING_EMULATOR)
		return()
	endif()
	if(DEFINED TAU_AARCH64_SYSROOT)
		foreach(_test ${_tests})
			set_property(TEST "${_test}" APPEND PROPERTY ENVIRONMENT
				"QEMU_LD_PREFIX=${TAU_AARCH64_SYSROOT}")
		endforeach()
	endif()
	if(WIN32)
		_tau_wine_dll_path(_winepath)
		if(_winepath)
			# ENVIRONMENT is a list, so the separators of WINEPATH stay escaped.
			string(REPLACE ";" "\\;" _winepath "${_winepath}")
			foreach(_test ${_tests})
				set_property(TEST "${_test}" APPEND PROPERTY ENVIRONMENT
					"WINEPATH=${_winepath}")
			endforeach()
		endif()
	endif()
endfunction()
