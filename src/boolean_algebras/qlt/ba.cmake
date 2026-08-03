set(TAU_BA_ID qlt)
set(TAU_BA_TYPE qlt)
set(TAU_BA_HEADER boolean_algebras/qlt/qlt.h)
set(TAU_BA_SOURCES boolean_algebras/qlt/qlt.cpp)
set(TAU_BA_TESTS
	tests/test_integration-satisfiability-qlt.cpp
	tests/test_integration-solver-qlt.cpp
	tests/test_omcat_types.cpp
	tests/test_qlt.cpp
	tests/test_qlt_grammar.cpp
	tests/test_qlt_oracle.cpp
	tests/test_semantic_pwr.cpp
)
set(TAU_BA_TEST_REQUIRES_test_qlt_grammar qint)
