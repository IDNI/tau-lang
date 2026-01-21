# To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# Install cvc5 shared library and all its runtime dependencies.
#   DESTINATION: path relative to CMAKE_INSTALL_PREFIX
#   COMPONENT:   install component name
#   SYSTEM_NAME: target platform
function(install_cvc5 DESTINATION COMPONENT SYSTEM_NAME)
	set(CVC5_DIST_DIR "${TAU_SHARED_PREFIX}/cvc5/dist")
	set(CVC5_LIB_DIR "${CVC5_DIST_DIR}/lib")
	set(CVC5_LIB_PATTERN "*.so*")
	set(CVC5_IS_CROSS_WIN FALSE)

	if(SYSTEM_NAME STREQUAL "Darwin")
		set(CVC5_LIB_PATTERN "*.dylib")
	endif()
	set(CVC5_TOOLCHAIN_DLLS "")

	if(SYSTEM_NAME STREQUAL "Windows")
		set(CVC5_DIST_DIR "${TAU_SHARED_PREFIX}/cvc5/dist-w64")
		set(CVC5_LIB_DIR "${CVC5_DIST_DIR}/bin")
		set(CVC5_LIB_PATTERN "*.dll")
		set(CVC5_IS_CROSS_WIN TRUE)

		set(CVC5_TOOLCHAIN_DLL_NAMES
			"libgcc_s_seh-1.dll"
			"libgcc_s_dw2-1.dll"
			"libstdc++-6.dll"
			"libwinpthread-1.dll")
		foreach(_dll ${CVC5_TOOLCHAIN_DLL_NAMES})
			execute_process(COMMAND "${CMAKE_CXX_COMPILER}"
				"-print-file-name=${_dll}"
				OUTPUT_VARIABLE _dll_path
				OUTPUT_STRIP_TRAILING_WHITESPACE)
			if(_dll_path AND NOT _dll_path STREQUAL "${_dll}")
				if(EXISTS "${_dll_path}")
					list(APPEND CVC5_TOOLCHAIN_DLLS
						"${_dll_path}")
				endif()
			endif()
		endforeach()
		list(REMOVE_DUPLICATES CVC5_TOOLCHAIN_DLLS)
	endif()

	set(_cvc5_install_code [[
cmake_policy(SET CMP0057 NEW)
set(DESTINATION "@DESTINATION@")
set(COMPONENT "@COMPONENT@")
set(SYSTEM_NAME "@SYSTEM_NAME@")
set(CVC5_DIST_DIR "@CVC5_DIST_DIR@")
set(CVC5_LIB_DIR "@CVC5_LIB_DIR@")
set(CVC5_LIB_PATTERN "@CVC5_LIB_PATTERN@")
set(CVC5_TOOLCHAIN_DLLS "@CVC5_TOOLCHAIN_DLLS@")
set(CVC5_IS_CROSS_WIN "@CVC5_IS_CROSS_WIN@")

message("=== Installing cvc5 runtime dependencies ===")
message("DESTINATION: ${DESTINATION}")
message("COMPONENT: ${COMPONENT}")
message("SYSTEM_NAME: ${SYSTEM_NAME}")
message("CVC5_DIST_DIR: ${CVC5_DIST_DIR}")
message("CVC5_LIB_DIR: ${CVC5_LIB_DIR}")
message("CVC5_LIB_PATTERN: ${CVC5_LIB_PATTERN}")
message("CVC5_TOOLCHAIN_DLLS: ${CVC5_TOOLCHAIN_DLLS}")
message("CVC5_IS_CROSS_WIN: ${CVC5_IS_CROSS_WIN}")

function(_cvc5_copy dest prefix)
	if(NOT ARGN)
		return()
	endif()
	foreach(_file ${ARGN})
		if(NOT EXISTS "${_file}")
			continue()
		endif()
		get_filename_component(_name "${_file}" NAME)
		if(prefix)
			message("${prefix}: ${_name}")
		endif()
		execute_process(COMMAND "${CMAKE_COMMAND}"
			-E copy_if_different "${_file}"
			"${dest}/${_name}")
	endforeach()
endfunction()

set(CVC5_LIB_PATH "$<TARGET_FILE:cvc5::cvc5>")
get_filename_component(CVC5_LIB_DIR "${CVC5_LIB_PATH}" DIRECTORY)
message("CVC5_LIB_DIR: ${CVC5_LIB_DIR}")

if(SYSTEM_NAME STREQUAL "Windows")
	if(NOT EXISTS "${CVC5_LIB_DIR}")
		message(FATAL_ERROR
			"cvc5 library directory not found: ${CVC5_LIB_DIR}")
	endif()

	file(GLOB cvc5_libs "${CVC5_LIB_DIR}/${CVC5_LIB_PATTERN}")
	if(NOT cvc5_libs)
		message(FATAL_ERROR
			"No cvc5 libraries found matching pattern: ${CVC5_LIB_PATTERN}")
	endif()

	set(CVC5_DEST_DIR "${CMAKE_INSTALL_PREFIX}/${DESTINATION}")
	file(MAKE_DIRECTORY "${CVC5_DEST_DIR}")

	foreach(lib ${cvc5_libs})
		get_filename_component(lib_name "${lib}" NAME)
		message("Installing cvc5 binary: ${lib} -> ${lib_name}")
		execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different
			"${lib}" "${CVC5_DEST_DIR}/${lib_name}")
	endforeach()

	if(CVC5_TOOLCHAIN_DLLS)
		list(REMOVE_DUPLICATES CVC5_TOOLCHAIN_DLLS)
		_cvc5_copy("${CVC5_DEST_DIR}"
			"Installing toolchain DLL"
			${CVC5_TOOLCHAIN_DLLS})
	endif()
else()
	if(NOT EXISTS "${CVC5_LIB_DIR}")
		message(FATAL_ERROR
			"cvc5 library directory not found: ${CVC5_LIB_DIR}")
	endif()
	file(GLOB cvc5_libs "${CVC5_LIB_DIR}/${CVC5_LIB_PATTERN}")
	if(NOT cvc5_libs)
		message(WARNING
			"No cvc5 libraries found matching pattern: ${CVC5_LIB_PATTERN}")
	else()
		foreach(lib ${cvc5_libs})
			get_filename_component(lib_name "${lib}" NAME)
			message("Installing: ${lib_name}")
			file(INSTALL "${lib}"
				DESTINATION "${CMAKE_INSTALL_PREFIX}/${DESTINATION}"
				FOLLOW_SYMLINK_CHAIN)
		endforeach()
	endif()
endif()
message("=== Finished installing cvc5 runtime dependencies ===")
]])
	string(CONFIGURE "${_cvc5_install_code}"
		_cvc5_install_code @ONLY)
	install(CODE "${_cvc5_install_code}"
		COMPONENT "${COMPONENT}"
		CONFIGURATIONS Release)
endfunction()
