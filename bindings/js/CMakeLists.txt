cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# Embind library for the WebAssembly build. Produces tau.js/tau.wasm
# from the tau static library, built with -DTAU_BUILD_BINDING_JS=ON under the
# emscripten toolchain.

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
set(TAU_JS_LINK_OPTIONS "--bind --no-entry -s STACK_SIZE=16777216")
string(APPEND TAU_JS_LINK_OPTIONS " -s EXPORT_NAME='tau'")
string(APPEND TAU_JS_LINK_OPTIONS " -s MODULARIZE=1")
string(APPEND TAU_JS_LINK_OPTIONS " -s ALLOW_MEMORY_GROWTH=1")
string(APPEND TAU_JS_LINK_OPTIONS " -s WASM=1")
string(APPEND TAU_JS_LINK_OPTIONS " -s ERROR_ON_UNDEFINED_SYMBOLS=1")
string(APPEND TAU_JS_LINK_OPTIONS " -s ASSERTIONS=1")

add_executable(tau_js embindings.cpp)
target_setup(tau_js)
set_target_properties(tau_js PROPERTIES
	OUTPUT_NAME tau
	LINK_FLAGS "${TAU_JS_LINK_OPTIONS}")
target_include_directories(tau_js PRIVATE
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/parser"
	"${CMAKE_SOURCE_DIR}/external/parser/src"
	"${TAU_PACK_INCLUDE_DIR}"
	"${TAU_GENERATED_PARSER_DIR}")
target_link_libraries(tau_js PRIVATE
	${TAU_STATIC_LIB_NAME} ${TAU_BA_LINK_LIBS} embind)
add_dependencies(tau_js tau_header)

# tau.node.js does require('./tau.js'), so the smoke test goes in the
# target's own output directory rather than this list file's binary dir.
add_custom_command(TARGET tau_js POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_CURRENT_SOURCE_DIR}/tau.node.js
		$<TARGET_FILE_DIR:tau_js>)
