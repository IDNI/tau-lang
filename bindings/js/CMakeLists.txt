cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# EXPORT_ES6 and CommonJS are mutually exclusive per link, so the two module
# formats are two targets. Their OUTPUT_NAMEs must differ: emscripten derives
# the .wasm name from the JS one, so sharing it races the two link steps.

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	message(FATAL_ERROR "run bindings cmake from root with -DTAU_BUILD_BINDING_JS=ON")
endif()

if(NOT EMSCRIPTEN)
	message(FATAL_ERROR "bindings/js requires the emscripten toolchain (-DTAU_BUILD_EMSCRIPTEN=ON)")
endif()

# Default 64KiB wasm stack overflows while the generated parser tables
# build their nested std::set<std::set<lits<char,char>>> grammar structures
# at static-init time; the overflow corrupts the heap and a later free()
# aborts. 16MiB is comfortably above what that init needs.
set(TAU_JS_LINK_OPTIONS_COMMON "--bind --no-entry -s STACK_SIZE=16777216")
string(APPEND TAU_JS_LINK_OPTIONS_COMMON " -s EXPORT_NAME='tau'")
string(APPEND TAU_JS_LINK_OPTIONS_COMMON " -s MODULARIZE=1")
string(APPEND TAU_JS_LINK_OPTIONS_COMMON " -s ALLOW_MEMORY_GROWTH=1")
string(APPEND TAU_JS_LINK_OPTIONS_COMMON " -s WASM=1")
string(APPEND TAU_JS_LINK_OPTIONS_COMMON " -s ERROR_ON_UNDEFINED_SYMBOLS=1")
string(APPEND TAU_JS_LINK_OPTIONS_COMMON " -s ASSERTIONS=1")

# target: cmake target name; output_name/suffix: OUTPUT_NAME/SUFFIX target
# properties, which is what emscripten derives the sibling .wasm name from;
# extra_link_options: format-specific -s flags; wrapper: smoke-test file
# copied beside the built module (it require()s/import()s by relative path).
function(tau_js_add_target target output_name suffix extra_link_options wrapper)
	add_executable(${target} embindings.cpp)
	target_setup(${target})
	set_target_properties(${target} PROPERTIES
		OUTPUT_NAME "${output_name}"
		SUFFIX "${suffix}"
		LINK_FLAGS "${TAU_JS_LINK_OPTIONS_COMMON} ${extra_link_options}")
	target_include_directories(${target} PRIVATE
		"${CMAKE_SOURCE_DIR}/src"
		"${CMAKE_SOURCE_DIR}/parser"
		"${CMAKE_SOURCE_DIR}/external/parser/src"
		"${TAU_PACK_INCLUDE_DIR}"
		"${TAU_GENERATED_PARSER_DIR}")
	target_link_libraries(${target} PRIVATE
		${TAU_STATIC_LIB_NAME} ${TAU_BA_LINK_LIBS} embind)
	add_dependencies(${target} tau_header)

	add_custom_command(TARGET ${target} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_CURRENT_SOURCE_DIR}/${wrapper}
			$<TARGET_FILE_DIR:${target}>)
endfunction()

tau_js_add_target(tau_js     tau     .js  ""                 tau.node.js)
tau_js_add_target(tau_js_esm tau.esm .mjs "-s EXPORT_ES6=1"  tau.node.mjs)

# npm package.json lands beside tau.js/tau.esm.mjs (RUNTIME_OUTPUT_DIRECTORY
# above, set by target_setup(), is CMAKE_BINARY_DIR) so the build directory
# itself is the installable package; TAU_VERSION is the top-level CMakeLists.txt
# read of VERSION, reused rather than re-read here.
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/package.json.in
	${CMAKE_BINARY_DIR}/package.json
	@ONLY)

# LICENSE.md and the smoke/parity test scripts the package.json "files" list
# and "test" scripts reference, copied flat alongside package.json.
add_custom_target(tau_js_package_assets ALL
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_SOURCE_DIR}/LICENSE.md
		${CMAKE_BINARY_DIR}/LICENSE.md
	COMMAND ${CMAKE_COMMAND} -E make_directory
		${CMAKE_BINARY_DIR}/tests
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_CURRENT_SOURCE_DIR}/tests/parity.js
		${CMAKE_CURRENT_SOURCE_DIR}/tests/native.js
		${CMAKE_BINARY_DIR}/tests/
	COMMENT "Copying JS bindings package assets (LICENSE, tests)")
