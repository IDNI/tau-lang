cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# tau_repl: the FTXUI REPL (src/main.cpp) built as its own wasm executable,
# separate from bindings/js's embind library (tau.js). FTXUI's
# ScreenInteractive::Install() unconditionally spawns two std::threads, so
# this target is built -pthread -- carrying SharedArrayBuffer and requiring
# COOP/COEP on whatever page embeds it. That requirement is scoped to this
# preset (TAU_BUILD_REPL_WASM): it does not reach bindings/js's tau.js, which
# stays pthread-free (D11, .local/build-emscripten.md).
# Output name is "tau_repl", distinct from the binding's "tau" -- both land
# in the same build directory (see .local/build-emscripten.md, Phase 5).

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	message(FATAL_ERROR "run js/repl cmake from root with -DTAU_BUILD_REPL_WASM=ON")
endif()

if(NOT EMSCRIPTEN)
	message(FATAL_ERROR "js/repl requires the emscripten toolchain (-DTAU_BUILD_EMSCRIPTEN=ON)")
endif()

include(ftxui)
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
# read) onto a worker pthread, off the browser main thread. -pthread itself
# is a global flag (top-level CMakeLists.txt), applied to every linked
# object including FTXUI's own FetchContent build, since emcc requires them
# all to agree on it. -fwasm-exceptions comes from target_setup() above
# (cmake/tau-common.cmake); STACK_SIZE matches bindings/js/CMakeLists.txt's
# -- the same grammar-table static-init overflow (16MiB default-stack
# overflow) applies to this executable too. INITIAL_MEMORY is explicit
# because the default (16 MiB) is smaller than STACK_SIZE alone, leaving no
# room for static data; wasm-ld caught this at link time ("initial memory
# too small").
#
# JSPI is still needed despite PROXY_TO_PTHREAD: FTXUI's own Emscripten
# input-listener loop (screen_interactive.cpp's __EMSCRIPTEN__ branch) calls
# emscripten_sleep() unconditionally, which aborts at runtime
# ("compile your program with async support") without it, regardless of
# which thread runs the loop. Confirmed by running under Node before adding
# this flag. JSPI coexists with -fwasm-exceptions (unlike plain ASYNCIFY).
set_target_properties(tau_repl PROPERTIES
	OUTPUT_NAME "tau_repl"
	SUFFIX ".js"
	LINK_FLAGS "-s PROXY_TO_PTHREAD -s JSPI -s STACK_SIZE=16777216 -s INITIAL_MEMORY=33554432 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1")

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
