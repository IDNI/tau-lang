cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# tau_repl: the FTXUI REPL (src/main.cpp) built as its own wasm executable,
# separate from bindings/js's embind library (tau.js). FTXUI's
# ScreenInteractive::Install() unconditionally spawns two std::threads, so
# this target requires -pthread regardless of TAU_WASM_PTHREADS (enforced by
# the fatal check in the top-level CMakeLists.txt) -- carrying
# SharedArrayBuffer and requiring COOP/COEP on whatever page embeds it.
# Output name is "tau_repl_web": the CLI now emits tau_repl.js, so this
# target moves aside to keep the wasm CLI, the JS binding and this page
# in one build directory.

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	message(FATAL_ERROR "run js/repl cmake from root with -DTAU_BUILD_REPL_WASM=ON")
endif()

if(NOT EMSCRIPTEN)
	message(FATAL_ERROR "js/repl requires the emscripten toolchain (-DTAU_BUILD_EMSCRIPTEN=ON)")
endif()

# tau-ftxui resolves FTXUI from the store prefix on CMAKE_PREFIX_PATH when
# TAU_DEPS_FROM_STORE is ON, the parser's own module otherwise.
include(tau-ftxui)
if(NOT TAU_PARSER_HAS_FTXUI_DEP)
	message(FATAL_ERROR "js/repl: FTXUI not available -- required for the wasm REPL")
endif()

add_executable(tau_repl "${CMAKE_SOURCE_DIR}/src/main.cpp")
target_setup(tau_repl)
target_include_directories(tau_repl PRIVATE
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/parser"
	"${CMAKE_SOURCE_DIR}/external/parser/src")
target_link_libraries(tau_repl PRIVATE
	${TAU_STATIC_LIB_NAME} ${TAU_BA_LINK_LIBS}
	ftxui::screen ftxui::dom ftxui::component)
target_compile_definitions(tau_repl PRIVATE TAU_PARSER_HAS_FTXUI)
add_dependencies(tau_repl tau_header)

# PROXY_TO_PTHREAD moves main() (and so ScreenInteractive::Loop()'s blocking
# read) onto a worker pthread, off the browser main thread. -pthread is a
# global flag (top-level CMakeLists.txt), so every linked object agrees on it.
#
# STACK_SIZE and INITIAL_MEMORY match bindings/js/CMakeLists.txt: the
# generated grammar tables overflow the default stack at static-init, and
# the default INITIAL_MEMORY leaves no room for static data on top of it.
#
# JSPI follows TAU_WASM_REPL_MODEL: FTXUI's Emscripten input listener calls
# emscripten_sleep() unconditionally in the sleep model, which aborts at
# runtime without it. JSPI coexists with -fwasm-exceptions, unlike ASYNCIFY.
set(TAU_REPL_LINK_FLAGS "-s PROXY_TO_PTHREAD -s STACK_SIZE=16777216 -s INITIAL_MEMORY=33554432 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1")
if(TAU_WASM_REPL_MODEL STREQUAL "sleep")
	set(TAU_REPL_LINK_FLAGS "${TAU_REPL_LINK_FLAGS} -s JSPI")
endif()
set_target_properties(tau_repl PROPERTIES
	OUTPUT_NAME "tau_repl_web"
	SUFFIX ".js"
	LINK_FLAGS "${TAU_REPL_LINK_FLAGS}")

# Page glue and vendored xterm.js come from the parser's shared terminal
# package (external/parser/js/tau-wasm-terminal), the same one the TGF page
# uses. ADDON_NAMES keep writes the author's addon-fit.js / addon-webgl.js
# names, which js/repl/index.html loads. bindings/js/package.json carries the
# xterm deps as devDependencies; ./dev dep-js-test-deps.sh (or a plain
# "npm install --prefix bindings/js") installs them.
set(TAU_REPL_PAGE_DIR "${CMAKE_BINARY_DIR}")
include("${CMAKE_SOURCE_DIR}/external/parser/js/tau-wasm-terminal/tau-wasm-terminal.cmake")
tau_wasm_terminal_page("${TAU_REPL_PAGE_DIR}"
	PACKAGE_DIR "${CMAKE_SOURCE_DIR}/bindings/js/node_modules"
	ADDON_NAMES keep)
# Tau's own page glue stays Tau's; copied after the shared files so it wins.
configure_file(index.html "${TAU_REPL_PAGE_DIR}/index.html" COPYONLY)
configure_file(index.mjs "${TAU_REPL_PAGE_DIR}/index.mjs" COPYONLY)
configure_file(sw.js "${TAU_REPL_PAGE_DIR}/sw.js" COPYONLY)
