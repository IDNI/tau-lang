// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// literal_incomplete: a truncated literal is "keep reading", a malformed one
// is not. One trio per algebra that declares the capability, spelled the way
// that algebra's own parser reads a stream value.

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("configuration") {
	TEST_CASE("bdd_init") { bdd_init<Bool>(); }
}

TEST_SUITE("literal_incomplete distinguishes truncation from malformation") {
#ifdef TAU_PACK_HAS_BA_BV
	TEST_CASE("bv") {
		using d = ba_descriptor<bv, node_t>;
		CHECK(d::literal_incomplete("#b"));         // prefix, digits still to come
		CHECK_FALSE(d::literal_incomplete("#b101"));
		CHECK_FALSE(d::literal_incomplete("}"));    // wrong from the first byte
	}
#endif
#ifdef TAU_PACK_HAS_BA_SBF
	TEST_CASE("sbf") {
		using d = ba_descriptor<sbf_ba, node_t>;
		CHECK(d::literal_incomplete("( a"));        // opened, not closed
		CHECK_FALSE(d::literal_incomplete("a"));
		CHECK_FALSE(d::literal_incomplete(")"));
	}
#endif
#ifdef TAU_PACK_HAS_BA_QINT
	TEST_CASE("qint") {
		using d = ba_descriptor<qint, node_t>;
		CHECK(d::literal_incomplete("[0, 1"));
		CHECK_FALSE(d::literal_incomplete("[0, 1)"));
		CHECK_FALSE(d::literal_incomplete("]"));
	}
#endif
#ifdef TAU_PACK_HAS_BA_QLT
	TEST_CASE("qlt") {
		using d = ba_descriptor<qlt, node_t>;
		CHECK(d::literal_incomplete("[1, 2"));
		CHECK_FALSE(d::literal_incomplete("[1, 2]"));
		CHECK_FALSE(d::literal_incomplete("]"));
	}
#endif
#ifdef TAU_PACK_HAS_BA_HSB
	TEST_CASE("hsb") {
		using d = ba_descriptor<hsb, node_t>;
		CHECK(d::literal_incomplete("x[0] <"));
		CHECK_FALSE(d::literal_incomplete("x[0] <= 0"));
		CHECK_FALSE(d::literal_incomplete("<"));
	}
#endif
}
