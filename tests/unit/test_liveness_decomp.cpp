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

	// GR-RT1: two G(safety) conjuncts fold into ONE safety_part.
	TEST_CASE("[GR-RT1] multi-safety fold") {
		tref fm = parse("(G ((o1[t] = 1))) && (G ((o2[t] = 1))) && (G (F ((o3[t] = 1)))).");
		REQUIRE(fm);
		auto d = decompose_liveness<node_t>(fm);
		CHECK(d.is_gr1);
		REQUIRE(d.safety_part != nullptr);
		CHECK(d.liveness_parts.size() == 1);
		// The fold is G(a && b): exactly one always at the top.
		const auto& t = tau::get(d.safety_part);
		REQUIRE(t.has_child());
		CHECK(t[0].value.nt == tau::wff_always);
		CHECK(tau::get(t[0].first())[0].value.nt == tau::wff_and);
	}

	// GR-RT2 / LG-8: G(F(temporal)) is rejected, matching is_gr1_fragment.
	TEST_CASE("[GR-RT2] G(F(G p)) is not GR(1)") {
		tref fm = parse("G (F (G ((o1[t] = 1)))).");
		REQUIRE(fm);
		auto d = decompose_liveness<node_t>(fm);
		CHECK_FALSE(d.is_gr1);
		CHECK(d.liveness_parts.empty());
	}

	// GR-3: the sometimes spelling is accepted.
	TEST_CASE("[GR-3] G(sometimes p) is a liveness obligation") {
		tref fm = parse("G (sometimes ((o1[t] = 1))).");
		REQUIRE(fm);
		auto d = decompose_liveness<node_t>(fm);
		CHECK(d.is_gr1);
		CHECK(d.liveness_parts.size() == 1);
	}

	// GR-R1: G(A p) is not a safety part.
	TEST_CASE("[GR-R1] G(A p) is not GR(1)") {
		tref fm = parse("G (A ((o1[t] = 1))).");
		REQUIRE(fm);
		auto d = decompose_liveness<node_t>(fm);
		CHECK_FALSE(d.is_gr1);
	}

	// GR-R2 / GR-RT5: no formula is not a solved GR(1) instance.
	TEST_CASE("[GR-R2] nullptr yields is_gr1 == false") {
		auto d = decompose_liveness<node_t>(nullptr);
		CHECK_FALSE(d.is_gr1);
		CHECK(d.safety_part == nullptr);
		CHECK(d.liveness_parts.empty());
	}

}
