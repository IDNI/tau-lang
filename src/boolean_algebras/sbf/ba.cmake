set(TAU_BA_ID sbf)
set(TAU_BA_TYPE sbf_ba)
set(TAU_BA_HEADER boolean_algebras/sbf/sbf_ba.h)
set(TAU_BA_GRAMMAR parser/sbf.tgf)
set(TAU_BA_TESTS
	tests/test_integration-satisfiability-sbf.cpp
	tests/test_integration-sbf1.cpp
	tests/test_integration-sbf2.cpp
	tests/test_integration-solver-sbf.cpp
	tests/test_sbf.cpp
	tests/test_sbf_ba_parsing.cpp
	tests/test_sbf_ba_splitter.cpp
	tests/test_sbf_oracle.cpp
)
set(TAU_BA_TEST_REQUIRES_test_integration-sbf1 bv)
set(TAU_BA_TEST_REQUIRES_test_integration-sbf2 bv)
set(TAU_BA_TEST_REQUIRES_test_integration-solver-sbf bv)
set(TAU_BA_TEST_REQUIRES_test_sbf bv)
set(TAU_BA_TEST_REQUIRES_test_sbf_ba_parsing bv)
set(TAU_BA_TEST_REQUIRES_test_sbf_ba_splitter bv)
