set(TAU_BA_ID qint)
set(TAU_BA_TYPE qint)
set(TAU_BA_HEADER boolean_algebras/qint/qint.h)
set(TAU_BA_SOURCES boolean_algebras/qint/qint.cpp)
# suites this algebra owns, relative to this manifest; registered when qint is
# in the pack, so no central list names them
set(TAU_BA_TESTS
	tests/test_qint.cpp
	tests/test_qint_grammar.cpp
	tests/test_integration-satisfiability-qint.cpp
	tests/test_integration-solver-qint.cpp
)
