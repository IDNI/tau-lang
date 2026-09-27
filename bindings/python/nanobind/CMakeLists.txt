cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(TAU_PYTHON_BINDING_MODULE "tau_nanobind")

message(STATUS "TAU_PYTHON_BINDING_MODULE: ${TAU_PYTHON_BINDING_MODULE}")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	message(FATAL_ERROR "run bindings cmake from root with -DTAU_BUILD_BINDING_PYTHON_NANOBIND=ON")
endif()

message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "TAU_SHARED_PREFIX: ${TAU_SHARED_PREFIX}")

if (CMAKE_VERSION VERSION_LESS 3.18)
	set(DEV_MODULE Development)
else()
	set(DEV_MODULE Development.Module)
endif()

# FindPython runs the target interpreter through CMAKE_CROSSCOMPILING_EMULATOR
# only under CMP0190 NEW; the project floor is 3.22, so raise it here.
if(POLICY CMP0190)
	cmake_policy(SET CMP0190 NEW)
endif()

find_package(Python 3.9 COMPONENTS Interpreter ${DEV_MODULE} REQUIRED)

# A cross build runs the target interpreter under the emulator; FindPython does
# this for its own queries, and the nanobind lookup and the tests must too.
set(TAU_PYTHON_EMULATOR "")
if(CMAKE_CROSSCOMPILING AND CMAKE_CROSSCOMPILING_EMULATOR)
	set(TAU_PYTHON_EMULATOR ${CMAKE_CROSSCOMPILING_EMULATOR})
endif()

execute_process(
	COMMAND ${TAU_PYTHON_EMULATOR} "${Python_EXECUTABLE}" -m nanobind --cmake_dir
	OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE nanobind_ROOT)
find_package(nanobind CONFIG REQUIRED)


nanobind_add_module(${TAU_PYTHON_BINDING_MODULE} tau_nanobind.cpp)

add_dependencies(${TAU_PYTHON_BINDING_MODULE} tau_header)
# The wheel bundles the cvc5 runtime beside the module under tau_libs, so the
# module searches its own directory rather than an absolute store path. macOS
# names the loader directory @loader_path. A Windows DLL has no search path, so
# delvewheel bundles and patches its imports instead.
if(WIN32)
	set(_tau_wheel_rpath "")
elseif(APPLE)
	set(_tau_wheel_rpath "@loader_path/tau_libs")
else()
	set(_tau_wheel_rpath "\$ORIGIN/tau_libs")
endif()
set_target_properties(${TAU_PYTHON_BINDING_MODULE} PROPERTIES
	INSTALL_RPATH "${_tau_wheel_rpath}"
	OUTPUT_NAME "tau"
)
target_compile_features(${TAU_PYTHON_BINDING_MODULE} PRIVATE cxx_std_23)
target_compile_definitions_if(${TAU_PYTHON_BINDING_MODULE} PRIVATE "${TAU_DEFINITIONS}")
target_link_libraries(${TAU_PYTHON_BINDING_MODULE} PRIVATE ${TAU_STATIC_LIB_NAME})
# tauparser, Boost, and cvc5 include dirs arrive through TAU_static's link
# interface; only the generated pack/parser dirs are private to the library.
target_include_directories(${TAU_PYTHON_BINDING_MODULE} PRIVATE
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/parser"
	"${TAU_PACK_INCLUDE_DIR}"
	"${TAU_GENERATED_PARSER_DIR}"
)
# Keep build, store, and home paths out of the module the wheel ships.
if(NOT MSVC)
	target_compile_options(${TAU_PYTHON_BINDING_MODULE} PRIVATE
		"-ffile-prefix-map=${CMAKE_SOURCE_DIR}="
		"-ffile-prefix-map=${CMAKE_BINARY_DIR}="
		"-ffile-prefix-map=${TAU_SHARED_PREFIX_RESOLVED}=")
	if(DEFINED CVC5_STORE_PREFIX AND NOT CVC5_STORE_PREFIX STREQUAL "")
		target_compile_options(${TAU_PYTHON_BINDING_MODULE} PRIVATE
			"-ffile-prefix-map=${CVC5_STORE_PREFIX}=")
	endif()
endif()

# The wheel script reads the installed module from here. The relative search
# path above is applied by this install, so the copy carries it. A Windows
# extension is a DLL, which install(TARGETS) classifies as RUNTIME.
install(TARGETS ${TAU_PYTHON_BINDING_MODULE}
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/tau_wheel"
	RUNTIME DESTINATION "${CMAKE_INSTALL_LIBDIR}/tau_wheel"
	COMPONENT "Python")

# The wheel producer needs the built module, so it runs at build time. The
# command comes from tau-deps, so the compiler and flags are this configure's.
if(TAU_DEPS_FROM_STORE AND DEFINED CVC5_STORE_PREFIX)
	# Each platform repairs the wheel with its own bundler: auditwheel on Linux,
	# delocate on macOS, delvewheel on Windows. auditwheel may live in a
	# different interpreter than the one that builds the wheel (that one must
	# match the extension ABI), so only it is resolved through an interpreter.
	# delocate and delvewheel install as console scripts on PATH.
	set(TAU_WHEEL_REPAIR_PYTHON "")
	set(TAU_WHEEL_REPAIR_PROGRAM "")
	if(TAU_DEPS_TARGET STREQUAL "windows-x86_64-msvc")
		set(TAU_WHEEL_PLATFORM "windows")
		find_program(TAU_WHEEL_REPAIR_PROGRAM delvewheel)
		if(NOT TAU_WHEEL_REPAIR_PROGRAM)
			message(FATAL_ERROR
				"tau_wheel needs delvewheel to bundle the cvc5 DLLs, but no "
				"'delvewheel' is on PATH. Install it (e.g. "
				"'${Python_EXECUTABLE} -m pip install delvewheel') and "
				"reconfigure; this configure does not install it.")
		endif()
	elseif(TAU_DEPS_TARGET MATCHES "^darwin-")
		set(TAU_WHEEL_PLATFORM "macos")
		find_program(TAU_WHEEL_REPAIR_PROGRAM delocate-wheel)
		if(NOT TAU_WHEEL_REPAIR_PROGRAM)
			message(FATAL_ERROR
				"tau_wheel needs delocate to bundle the cvc5 dylibs, but no "
				"'delocate-wheel' is on PATH. Install it (e.g. "
				"'${Python_EXECUTABLE} -m pip install delocate') and "
				"reconfigure; this configure does not install it.")
		endif()
	else()
		set(TAU_WHEEL_PLATFORM "linux")
		foreach(_tau_aw_candidate "${Python_EXECUTABLE}" python3 python)
			if(TAU_WHEEL_REPAIR_PYTHON)
				break()
			endif()
			execute_process(COMMAND "${_tau_aw_candidate}" -m auditwheel --version
				RESULT_VARIABLE _tau_aw_rc OUTPUT_QUIET ERROR_QUIET)
			if(_tau_aw_rc EQUAL 0)
				set(TAU_WHEEL_REPAIR_PYTHON "${_tau_aw_candidate}")
			endif()
		endforeach()
		if(NOT TAU_WHEEL_REPAIR_PYTHON)
			message(FATAL_ERROR
				"tau_wheel needs auditwheel to produce a manylinux wheel, but no "
				"interpreter on this machine provides it. Install it (e.g. "
				"'${Python_EXECUTABLE} -m pip install auditwheel') and "
				"reconfigure; this configure does not install it.")
		endif()
	endif()

	tau_deps_producer_command(_tau_wheel_producer
		"${CMAKE_SOURCE_DIR}/scripts/dep-tau-wheel-package.sh")
	get_filename_component(_tau_parser_entry "${TAU_PARSER_SDK_PREFIX}" DIRECTORY)
	get_filename_component(_tau_parser_id "${_tau_parser_entry}" NAME)
	get_filename_component(_tau_cvc5_entry "${CVC5_STORE_PREFIX}" DIRECTORY)
	get_filename_component(_tau_cvc5_id "${_tau_cvc5_entry}" NAME)
	get_filename_component(_tau_boost_entry "${BOOST_STORE_PREFIX}" DIRECTORY)
	get_filename_component(_tau_boost_id "${_tau_boost_entry}" NAME)
	add_custom_target(tau_wheel
		COMMAND "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}"
			--component Python
		COMMAND "${CMAKE_COMMAND}" -E env
			"TAU_WHEEL_PYTHON=${Python_EXECUTABLE}"
			"TAU_WHEEL_PLATFORM=${TAU_WHEEL_PLATFORM}"
			"TAU_WHEEL_REPAIR_PYTHON=${TAU_WHEEL_REPAIR_PYTHON}"
			"TAU_WHEEL_REPAIR_PROGRAM=${TAU_WHEEL_REPAIR_PROGRAM}"
			bash "${CMAKE_CURRENT_SOURCE_DIR}/pack-and-publish-wheel.sh"
				"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/tau_wheel"
				"${CVC5_STORE_PREFIX}"
				"${CMAKE_CURRENT_BINARY_DIR}"
				"${PROJECT_VERSION}"
				"${nanobind_VERSION}"
				"${_tau_parser_id}" "${_tau_cvc5_id}" "${_tau_boost_id}"
				${_tau_wheel_producer}
		DEPENDS ${TAU_PYTHON_BINDING_MODULE}
		USES_TERMINAL
		COMMENT "Packing and publishing the tau wheel")
endif()

set_source_files_properties(tau_nanobind.cpp PROPERTIES
	COMPILE_OPTIONS "-Wno-error")

# target_compile_options(${TAU_PYTHON_BINDING_MODULE} PRIVATE -Wfatal-errors)

# BUILD_TESTING mirrors the include(CTest) gate the root CMakeLists.txt
# uses, so these register only when ctest infra is actually active.
if(BUILD_TESTING)
	file(GLOB TAU_PYTHON_BINDING_TEST_SCRIPTS
		"${CMAKE_SOURCE_DIR}/tests/bindings/python/test_*.py")
	foreach(test_script ${TAU_PYTHON_BINDING_TEST_SCRIPTS})
		get_filename_component(test_stem "${test_script}" NAME_WE)
		# the wheel check takes a wheel path, so it registers separately below
		if(test_stem STREQUAL "test_wheel_contents")
			continue()
		endif()
		add_test(NAME "binding-python-${test_stem}"
			COMMAND ${TAU_PYTHON_EMULATOR} ${Python_EXECUTABLE} "${test_script}")
		set_tests_properties("binding-python-${test_stem}" PROPERTIES
			ENVIRONMENT "TAU_PYTHON_MODULE_DIR=$<TARGET_FILE_DIR:${TAU_PYTHON_BINDING_MODULE}>")
	endforeach()

	# Repeatable wheel check: verifies a built wheel's tag, module, bundled cvc5
	# runtime, relative RUNPATH, and that no wheel file carries an absolute home
	# path. Opt-in because it needs the built wheel, not this build tree.
	set(TAU_WHEEL_PATH "" CACHE FILEPATH
		"Path to a built tau nanobind wheel for binding-python-wheel-check")
	if(TAU_WHEEL_PATH)
		add_test(NAME binding-python-wheel-check
			COMMAND ${TAU_PYTHON_EMULATOR} ${Python_EXECUTABLE}
				"${CMAKE_SOURCE_DIR}/tests/bindings/python/test_wheel_contents.py"
				"${TAU_WHEEL_PATH}")
	endif()
endif()

include(tau-cross-test-environment)
tau_set_cross_test_environment()
