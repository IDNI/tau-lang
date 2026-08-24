cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# Resolve a NATIVE tgf executable for parser generation.
#
# generate_parser() (generate-parser.cmake) needs a tgf binary it can run at
# build time. Under a native toolchain that is simply the in-tree tgf target.
# Under a cross toolchain (Emscripten, and any future one) tgf is built for
# the target platform and the build host cannot execute it, so a second,
# native build of external/parser is driven on the side through
# ExternalProject_Add purely to get a runnable tgf out of it. Either way this
# module sets TAU_TGF_EXECUTABLE (the binary to run) and TAU_TGF_DEPEND (what
# to depend on to have it built).
if(NOT CMAKE_CROSSCOMPILING)
	set(TAU_TGF_EXECUTABLE "$<TARGET_FILE:tgf>")
	set(TAU_TGF_DEPEND tgf)
else()
	include(ExternalProject)

	set(TAU_HOST_TOOLS_DIR "${CMAKE_BINARY_DIR}/host-tools")
	set(TAU_HOST_TOOLS_BINARY_DIR "${TAU_HOST_TOOLS_DIR}/build")
	set(TAU_TGF_EXECUTABLE "${TAU_HOST_TOOLS_BINARY_DIR}/tgf")
	set(TAU_TGF_DEPEND tau_host_tgf)

	# Same submodule, configured fresh and without TAU_PARSER_BUILD_EMSCRIPTEN,
	# so external/parser/CMakeLists.txt never calls set_emscripten_toolchain()
	# and CMake picks up the build host's own compiler. tgf only: no tests, no
	# examples, no FTXUI (its REPL is unused here, and skipping it avoids
	# fetching it from source).
	set(TAU_HOST_TGF_CMAKE_ARGS
		-DCMAKE_BUILD_TYPE=Release
		-DCMAKE_TOOLCHAIN_FILE=
		-DTAU_PARSER_BUILD_STATIC_LIBRARY=ON
		-DTAU_PARSER_BUILD_TGF=ON
		-DTAU_PARSER_BUILD_TESTS=OFF
		-DTAU_PARSER_BUILD_EXAMPLES=OFF
		-DTAU_PARSER_BUILD_DOC=OFF
		-DTAU_PARSER_DONT_USE_FTXUI=ON
		-DTAU_PARSER_INSTALL=OFF
		-DTAU_SHARED_PREFIX=${TAU_SHARED_PREFIX}
		-DTAU_BUILD_JOBS=${TAU_BUILD_JOBS})
	# Reuse whatever compiler launcher (ccache, ...) the outer configure
	# already wired in, rather than forcing a choice of our own.
	if(CMAKE_C_COMPILER_LAUNCHER)
		list(APPEND TAU_HOST_TGF_CMAKE_ARGS
			-DCMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER})
	endif()
	if(CMAKE_CXX_COMPILER_LAUNCHER)
		list(APPEND TAU_HOST_TGF_CMAKE_ARGS
			-DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER})
	endif()

	set(TAU_HOST_TGF_BUILD_COMMAND
		${CMAKE_COMMAND} --build "${TAU_HOST_TOOLS_BINARY_DIR}" --target tgf)
	if(TAU_BUILD_JOBS MATCHES "^[1-9][0-9]*$")
		list(APPEND TAU_HOST_TGF_BUILD_COMMAND --parallel ${TAU_BUILD_JOBS})
	endif()

	# The parent configure's CC/CXX/*FLAGS, if any, belong to the cross
	# toolchain and must not reach this native sub-configure; they arrive
	# through the process environment rather than the cache, so listing
	# -DCMAKE_TOOLCHAIN_FILE= above does not stop them and they are stripped
	# here instead.
	ExternalProject_Add(${TAU_TGF_DEPEND}
		SOURCE_DIR "${PROJECT_SOURCE_DIR}/external/parser"
		PREFIX "${TAU_HOST_TOOLS_DIR}"
		BINARY_DIR "${TAU_HOST_TOOLS_BINARY_DIR}"
		CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env
			--unset=CC --unset=CXX
			--unset=CFLAGS --unset=CXXFLAGS --unset=LDFLAGS
			-- ${CMAKE_COMMAND} -S <SOURCE_DIR> -B <BINARY_DIR>
				-G "${CMAKE_GENERATOR}" ${TAU_HOST_TGF_CMAKE_ARGS}
		BUILD_COMMAND ${TAU_HOST_TGF_BUILD_COMMAND}
		# The generated parsers are only as current as the tgf that wrote
		# them, and ExternalProject otherwise stamps its build step and never
		# revisits it, so moving the submodule would leave the old generator
		# in place. The step itself is a no-op once the sub-build is settled.
		BUILD_ALWAYS TRUE
		INSTALL_COMMAND ""
		BUILD_BYPRODUCTS "${TAU_TGF_EXECUTABLE}"
		USES_TERMINAL_CONFIGURE TRUE
		USES_TERMINAL_BUILD TRUE)
endif()
