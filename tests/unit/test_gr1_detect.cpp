// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for the GR(1) fragment classifier (Opt-3).

#include "test_init.h"
#include "test_tau_helpers.h"
#include "gr1_detect.h"

using namespace idni::tau_lang;

static tref parse(const char* s) {
	auto nso = get_nso_rr<node_t>(tau::get(s));
	if (!nso.has_value()) return nullptr;
	return nso.value().main->get();
}

TEST_SUITE("GR(1) fragment classifier") {

	TEST_CASE("G(safety) is GR(1)") {
		tref fm = parse("G ((o1[t] = 1)).");
		REQUIRE(fm);
		int s = -1, l = -1;
		CHECK(is_gr1_fragment<node_t>(fm, s, l));
		CHECK(s == 1);
		CHECK(l == 0);
	}

	TEST_CASE("GF(liveness) is GR(1)") {
		tref fm = parse("G (F ((o1[t] = 1))).");
		REQUIRE(fm);
		int s = -1, l = -1;
		CHECK(is_gr1_fragment<node_t>(fm, s, l));
		CHECK(s == 0);
		CHECK(l == 1);
	}

	TEST_CASE("G(safe) && GF(live) is GR(1)") {
		tref fm = parse(
			"G ((o1[t] = 1)) && G (F ((o2[t] = 1))).");
		REQUIRE(fm);
		int s = -1, l = -1;
		CHECK(is_gr1_fragment<node_t>(fm, s, l));
		CHECK(s == 1);
		CHECK(l == 1);
	}

	TEST_CASE("F(phi) alone is NOT GR(1)") {
		tref fm = parse("F ((o1[t] = 1)).");
		REQUIRE(fm);
		CHECK_FALSE(is_gr1_fragment<node_t>(fm));
	}

	TEST_CASE("(phi U psi) alone is NOT GR(1)") {
		tref fm = parse("((o1[t] = 1) U (o1[t] = 0)).");
		REQUIRE(fm);
		CHECK_FALSE(is_gr1_fragment<node_t>(fm));
	}

	TEST_CASE("G(G(phi)) is nested G — classifier sees nested temporal, rejects") {
		// Nested G isn't valid grammar, but conceptually the classifier
		// requires G's body to be non-temporal.  G(safety with temporal)
		// should be rejected.
		tref fm = parse("G (F (G ((o1[t] = 1)))).");
		// G(F(G(...))) — outer G has F inside, which is temporal, so NOT
		// GR(1) in our definition (only GF is allowed, not GFG).
		if (fm) CHECK_FALSE(is_gr1_fragment<node_t>(fm));
	}

	TEST_CASE("Parenthesised three-way: two safety, one liveness") {
		// Explicit parenthesization to match left-associative && structure.
		tref fm = parse(
			"((G ((o1[t] = 1))) && (G ((o2[t] = 1)))) && G (F ((o3[t] = 1))).");
		REQUIRE(fm);
		int s = 0, l = 0;
		CHECK(is_gr1_fragment<node_t>(fm, s, l));
		CHECK(s == 2);
		CHECK(l == 1);
	}
}
