# The node that runs the wasm tests. emsdk's own node comes first: the code
# emcc emits needs the node version its emsdk ships, and a system node found
# first on PATH (an older one) fails every test at module load.
#
# Resolution: -DNODE_EXECUTABLE > $EMSDK_NODE > NODE_JS of the emsdk config
# ($EM_CONFIG, else <emsdk>/.emscripten) > <emsdk>/node/*/bin/node > PATH.
# The result is also the cross-compiling emulator, so Emscripten.cmake does
# not pick its own node from PATH. Call it after set_emscripten_toolchain()
# and before project().
function(tau_resolve_emsdk_node)
	if(NODE_EXECUTABLE)
		set(_node "${NODE_EXECUTABLE}")
	elseif(DEFINED ENV{EMSDK_NODE} AND EXISTS "$ENV{EMSDK_NODE}")
		set(_node "$ENV{EMSDK_NODE}")
	else()
		get_filename_component(_emsdk "${EMSCRIPTEN_DIR}/../.." ABSOLUTE)
		if(DEFINED ENV{EM_CONFIG} AND EXISTS "$ENV{EM_CONFIG}")
			set(_config "$ENV{EM_CONFIG}")
		else()
			set(_config "${_emsdk}/.emscripten")
		endif()
		if(EXISTS "${_config}")
			get_filename_component(_cfgdir "${_config}" DIRECTORY)
			file(STRINGS "${_config}" _line REGEX "^NODE_JS *=")
			if(_line MATCHES "^NODE_JS *= *['\"]([^'\"]+)['\"]")
				string(REPLACE "$CFGDIR" "${_cfgdir}" _cand
					"${CMAKE_MATCH_1}")
				if(EXISTS "${_cand}")
					set(_node "${_cand}")
				endif()
			endif()
		endif()
		if(NOT _node)
			file(GLOB _cands "${_emsdk}/node/*/bin/node")
			if(_cands)
				list(SORT _cands COMPARE NATURAL ORDER DESCENDING)
				list(GET _cands 0 _node)
			endif()
		endif()
		if(NOT _node)
			find_program(_node_on_path NAMES node nodejs)
			set(_node "${_node_on_path}")
		endif()
	endif()
	if(NOT _node)
		message(FATAL_ERROR "no node found for the wasm build: run "
			"'./dev dep-emsdk.sh' or pass -DNODE_EXECUTABLE=<node>")
	endif()
	set(NODE_EXECUTABLE "${_node}" CACHE FILEPATH
		"node that runs the wasm tests" FORCE)
	# The CLI links -sJSPI; a node that ships JSPI behind a flag (node 24)
	# fails its module at load without it.
	set(_jspi_probe "process.exit(typeof WebAssembly.Suspending==='function'?0:1)")
	set(_emulator "${_node}")
	execute_process(COMMAND "${_node}" -e "${_jspi_probe}"
		RESULT_VARIABLE _jspi_default OUTPUT_QUIET ERROR_QUIET)
	if(NOT _jspi_default EQUAL 0)
		execute_process(COMMAND "${_node}" --experimental-wasm-jspi
			-e "${_jspi_probe}"
			RESULT_VARIABLE _jspi_flag OUTPUT_QUIET ERROR_QUIET)
		if(_jspi_flag EQUAL 0)
			list(APPEND _emulator --experimental-wasm-jspi)
		endif()
	endif()
	set(CMAKE_CROSSCOMPILING_EMULATOR "${_emulator}" CACHE STRING
		"Path to the emulator for the target system." FORCE)
	message(STATUS "wasm tests run under ${_emulator}")
endfunction()
