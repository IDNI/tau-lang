set(DOCTEST_CONFIG_ASSERTION_PARAMETERS_BY_VALUE "true")

add_link_options("-flto")



set(SAMPLE_TESTS sample_test.cpp)
add_executable(sample_test ${SAMPLE_TESTS})
target_setup(sample_test)
target_link_libraries(sample_test TAUo ${TEST_FRAMEWORK})

# set(OTHER_TEST other_test.cpp)
# add_executable(other_test ${TEST_EARLEY})
# target_setup(other_test)
# target_link_libraries(other_test TAUo ${TEST_FRAMEWORK})

add_custom_target(tmltest
	COMMAND sample_test # && other_test ...
	DEPENDS sample_test # other_test ...
)
