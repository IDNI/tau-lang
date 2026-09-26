include_guard(GLOBAL)

# ctest runs the binaries the build tree holds and never builds them. The
# build_tree test builds the tree first, so no test runs a stale binary.
# Call it after the last add_test() of a directory.
function(tau_require_build_tree)
	get_property(_added GLOBAL PROPERTY TAU_BUILD_TREE_TEST)
	if(NOT _added)
		add_test(NAME build_tree
			COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
				--parallel ${TAU_BUILD_JOBS_RESOLVED})
		set_tests_properties(build_tree PROPERTIES FIXTURES_SETUP build_tree)
		set_property(GLOBAL PROPERTY TAU_BUILD_TREE_TEST ON)
	endif()
	get_property(_tests DIRECTORY PROPERTY TESTS)
	list(REMOVE_ITEM _tests build_tree)
	if(_tests)
		set_property(TEST ${_tests} APPEND PROPERTY FIXTURES_REQUIRED build_tree)
	endif()
endfunction()
