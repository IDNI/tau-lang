// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for Spec { transient; invariant; reactive } decomposition (#5).

#include "test_init.h"
#include "test_tau_helpers.h"
#include "spec.h"
#include <sstream>

using namespace idni::tau_lang;

static tref parse(const char* s) {
	auto nso = get_nso_rr<node_t>(tau::get(s));
	if (!nso.has_value()) return nullptr;
	return nso.value().main->get();
}

TEST_SUITE("Spec decomposition") {

	TEST_CASE("pure invariant: G(o=1)") {
		tref fm = parse("G ((o1[t] = 1)).");
		REQUIRE(fm);
		auto s = decompose_spec<node_t>(fm);
		CHECK(s.transient == nullptr);
		CHECK(s.invariant != nullptr);
		CHECK(s.reactive  == nullptr);
	}

	TEST_CASE("pure reactive: F(o=1)") {
		tref fm = parse("F ((o1[t] = 1)).");
		REQUIRE(fm);
		auto s = decompose_spec<node_t>(fm);
		CHECK(s.transient == nullptr);
		CHECK(s.invariant == nullptr);
		CHECK(s.reactive  != nullptr);
	}

	TEST_CASE("pure reactive: GF(o=1) — G with F inside") {
		tref fm = parse("G (F ((o1[t] = 1))).");
		REQUIRE(fm);
		auto s = decompose_spec<node_t>(fm);
		CHECK(s.reactive != nullptr);
		// GF goes to reactive (has_F detects the inner F).
		CHECK(s.invariant == nullptr);
	}

	TEST_CASE("invariant + reactive: (G(o1=1)) && (F(o2=1))") {
		// Explicit parenthesisation to get a clean wff_and at the top.
		tref fm = parse("(G ((o1[t] = 1))) && (F ((o2[t] = 1))).");
		REQUIRE(fm);
		auto s = decompose_spec<node_t>(fm);
		// G(o1=1) should decompose to invariant, F(o2=1) to reactive
		CHECK(s.invariant != nullptr);
		CHECK(s.reactive != nullptr);
		// Verify the reactive component contains the F(o2) clause
		std::stringstream ss;
		ss << tau::get(s.reactive);
		CHECK(ss.str().find("o2") != std::string::npos);
	}

	TEST_CASE("U reactive") {
		tref fm = parse("((o1[t] = 1) U (o1[t] = 0)).");
		REQUIRE(fm);
		auto s = decompose_spec<node_t>(fm);
		CHECK(s.reactive  != nullptr);
		CHECK(s.invariant == nullptr);
	}
}
