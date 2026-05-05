// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for Opt-6 liveness decomposition.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "liveness_decomp.h"

using namespace idni::tau_lang;

static tref parse(const char* s) {
	auto nso = get_nso_rr<node_t>(tau::get(s));
	if (!nso.has_value()) return nullptr;
	return nso.value().main->get();
}

TEST_SUITE("liveness decomposition") {

	TEST_CASE("pure safety: G(o=1)") {
		tref fm = parse("G ((o1[t] = 1)).");
		REQUIRE(fm);
		auto d = decompose_liveness<node_t>(fm);
		CHECK(d.is_gr1);
		CHECK(d.safety_part != nullptr);
		CHECK(d.liveness_parts.empty());
	}

	TEST_CASE("pure liveness: GF(o=1)") {
		tref fm = parse("G (F ((o1[t] = 1))).");
		REQUIRE(fm);
		auto d = decompose_liveness<node_t>(fm);
		CHECK(d.is_gr1);
		CHECK(d.safety_part == nullptr);
		CHECK(d.liveness_parts.size() == 1u);
	}

	TEST_CASE("safety + liveness") {
		tref fm = parse(
			"(G ((o1[t] = 1))) && (G (F ((o2[t] = 1)))).");
		REQUIRE(fm);
		auto d = decompose_liveness<node_t>(fm);
		CHECK(d.is_gr1);
		CHECK(d.safety_part != nullptr);
		CHECK(d.liveness_parts.size() == 1u);
	}

	TEST_CASE("two liveness obligations") {
		tref fm = parse(
			"(G (F ((o1[t] = 1)))) && (G (F ((o2[t] = 1)))).");
		REQUIRE(fm);
		auto d = decompose_liveness<node_t>(fm);
		CHECK(d.is_gr1);
		CHECK(d.liveness_parts.size() == 2u);
	}

	TEST_CASE("non-GR(1): F alone") {
		tref fm = parse("F ((o1[t] = 1)).");
		REQUIRE(fm);
		auto d = decompose_liveness<node_t>(fm);
		CHECK_FALSE(d.is_gr1);
	}

	TEST_CASE("non-GR(1): U") {
		tref fm = parse("((o1[t] = 1) U (o1[t] = 0)).");
		REQUIRE(fm);
		auto d = decompose_liveness<node_t>(fm);
		CHECK_FALSE(d.is_gr1);
	}
}
