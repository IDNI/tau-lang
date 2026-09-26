# resolve TAU_SHARED_PREFIX from -D, env, or default
include("${CMAKE_CURRENT_LIST_DIR}/tau-shared-resolve.cmake")

if(NOT EMSCRIPTEN_DIR)
	set(EMSCRIPTEN_DIR "${TAU_SHARED_PREFIX_RESOLVED}/emsdk/upstream/emscripten")
endif()

set(EMSCRIPTEN_CMAKE "${EMSCRIPTEN_DIR}/cmake/Modules/Platform/Emscripten.cmake")

# points CMAKE_TOOLCHAIN_FILE at emsdk's own Emscripten.cmake; call before
# project()
function(set_emscripten_toolchain)
	if(NOT EXISTS "${EMSCRIPTEN_CMAKE}")
		message(SEND_ERROR
			"emscripten not found in ${EMSCRIPTEN_DIR}. "
			"Use -DEMSCRIPTEN_DIR=<emscripten_install_directory>.")
		return()
	endif()
	set(EMSCRIPTEN_ROOT_PATH "${EMSCRIPTEN_DIR}" PARENT_SCOPE)
	set(CMAKE_TOOLCHAIN_FILE "${EMSCRIPTEN_CMAKE}" PARENT_SCOPE)
endfunction()
