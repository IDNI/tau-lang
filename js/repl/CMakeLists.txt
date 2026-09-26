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

# The page around tau_repl.js/.wasm: xterm.js glue, the COOP/COEP
# service-worker fallback and the terminal chrome, ported from
# ~/Projects/parser.devel/js/tgf/ (see .local/build-emscripten.md, D6/D11/B7).
# target_setup() (cmake/tau-common.cmake) points every target's
# RUNTIME_OUTPUT_DIRECTORY at CMAKE_BINARY_DIR, not this directory's own
# binary dir, so tau_repl.js/.wasm land there too -- the page must join them
# at CMAKE_BINARY_DIR for a same-directory "<script src=tau_repl.js>" to work.
set(TAU_REPL_PAGE_DIR "${CMAKE_BINARY_DIR}")
configure_file(index.html "${TAU_REPL_PAGE_DIR}/index.html" COPYONLY)
configure_file(index.mjs "${TAU_REPL_PAGE_DIR}/index.mjs" COPYONLY)
configure_file(sw.js "${TAU_REPL_PAGE_DIR}/sw.js" COPYONLY)

# xterm.js comes from npm (D6), not a vendored blob: bindings/js/package.json
# carries it as a devDependency and ./dev dep-js-test-deps.sh (or a plain
# "npm install --prefix bindings/js") installs it, same as puppeteer-core for
# the headless-Chrome test harness. Copy the built UMD/CSS files next to
# tau_repl.js at configure time rather than referencing node_modules/ from
# the served page, so the page's directory stays self-contained.
set(TAU_JS_TEST_DIR "${CMAKE_SOURCE_DIR}/bindings/js")
set(TAU_XTERM_JS "${TAU_JS_TEST_DIR}/node_modules/@xterm/xterm/lib/xterm.js")
set(TAU_XTERM_CSS "${TAU_JS_TEST_DIR}/node_modules/@xterm/xterm/css/xterm.css")
set(TAU_XTERM_FIT_JS "${TAU_JS_TEST_DIR}/node_modules/@xterm/addon-fit/lib/addon-fit.js")
# The top-level CMakeLists.txt only auto-runs dep-chrome.sh/dep-js-test-deps.sh
# for TAU_BUILD_BROWSER_TESTS, not TAU_BUILD_REPL_WASM -- left that way rather
# than widened, since a FATAL_ERROR naming the one command to run is the same
# idiom TAU_PARSER_HAS_FTXUI_DEP above already uses for a missing dependency.
if(NOT EXISTS "${TAU_XTERM_JS}" OR NOT EXISTS "${TAU_XTERM_CSS}" OR NOT EXISTS "${TAU_XTERM_FIT_JS}")
	message(FATAL_ERROR
		"js/repl: xterm.js not found under ${TAU_JS_TEST_DIR}/node_modules. "
		"Run './dev dep-js-test-deps.sh' first.")
endif()
file(COPY "${TAU_XTERM_JS}" "${TAU_XTERM_CSS}" "${TAU_XTERM_FIT_JS}"
	DESTINATION "${TAU_REPL_PAGE_DIR}/vendor")
