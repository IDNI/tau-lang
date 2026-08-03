set(TAU_BA_ID hsb)
set(TAU_BA_TYPE hsb)
set(TAU_BA_HEADER boolean_algebras/hsb/hsb.h)
set(TAU_BA_LINK_LIBS cvc5::cvc5)
set(TAU_BA_REQUIRES_PACKAGES cvc5)
set(TAU_BA_TESTS
	tests/test_hsb.cpp
	tests/test_hsb_grammar.cpp
	tests/test_hsb_normalizer.cpp
	tests/test_hsb_splitter.cpp
	tests/test_integration-satisfiability-hsb.cpp
	tests/test_integration-solver-hsb.cpp
)
set(TAU_BA_TEST_REQUIRES_test_hsb bv qint sbf)
