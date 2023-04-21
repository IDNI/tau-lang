set(DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE "true")

add_link_options("-flto")

set(TEST_SAMPLE test_sample.cpp)
add_executable(test_sample ${TEST_SAMPLE})
target_setup(test_sample)
target_link_libraries(test_sample TAUo ${TEST_FRAMEWORK})

set(TEST_BA test_ba.cpp)
add_executable(test_ba ${TEST_BA})
target_setup(test_ba)
target_link_libraries(test_ba TAUo ${TEST_FRAMEWORK})

# set(OTHER_TEST other_test.cpp)
# add_executable(other_test ${TEST_EARLEY})
# target_setup(other_test)
# target_link_libraries(other_test TAUo ${TEST_FRAMEWORK})

add_custom_target(tmltest
	COMMAND sample_test && test_ba # && other_test ...
	DEPENDS sample_test test_ba # other_test ...
)
