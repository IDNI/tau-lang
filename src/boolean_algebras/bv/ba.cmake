set(TAU_BA_ID bv)
set(TAU_BA_TYPE bv)
set(TAU_BA_HEADER boolean_algebras/bv/bv_ba.h)
set(TAU_BA_GRAMMAR parser/bitvector.tgf)
set(TAU_BA_LINK_LIBS cvc5::cvc5)
set(TAU_BA_REQUIRES_PACKAGES cvc5)
set(TAU_BA_TESTS
	tests/test_bv_ba-cast.cpp
	tests/test_bv_ba-simplification.cpp
	tests/test_bv_ba_helpers.cpp
	tests/test_bv_bitblast.cpp
	tests/test_bv_bounded_child.cpp
	tests/test_bv_codegen.cpp
	tests/test_bv_widening.cpp
	tests/test_integration-blasting_correctness_check1.cpp
	tests/test_integration-blasting_correctness_check2.cpp
	tests/test_integration-blasting_correctness_check3.cpp
	tests/test_integration-blasting_correctness_check4.cpp
	tests/test_integration-blasting_correctness_check5.cpp
	tests/test_integration-bv_hooks.cpp
	tests/test_integration-heuristics-bv_ba_custom_simplification.cpp
	tests/test_integration-heuristics-bv_ba_custom_simplification_equivalence.cpp
	tests/test_integration-heuristics-bv_ba_cvc5_simplification.cpp
	tests/test_integration-heuristics-bv_predicate_blasting.cpp
	tests/test_integration-heuristics-bv_predicate_blasting_predicates.cpp
	tests/test_integration-interpreter-bv.cpp
	tests/test_integration-satisfiability-bv.cpp
	tests/test_integration-solver-bv.cpp
	tests/test_ltl_qlt_bv.cpp
)
set(TAU_BA_TEST_REQUIRES_test_ltl_qlt_bv qlt sbf)

# cvc5 links the host GMP, so a wasm build is not distributable.
set(TAU_BA_UNSUPPORTED_TARGETS wasm32-emscripten)
set(TAU_BA_UNSUPPORTED_REASON_wasm32-emscripten "nondistributable: cvc5 links GMP")
