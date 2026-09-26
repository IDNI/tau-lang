// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "tau.h"

#include <optional>
#include <vector>

using namespace idni::tau_lang;
using node_t = tau_pack::node_t;
using tau = tree<node_t>;
using tau_t = tau_ba<TAU_PACK_BASE_BAS>;

namespace {

// A stream variable gets its input/output bit and its type from the spec
// parse only (transform_io_var asserts on an unclassified stream in a Debug
// build), so the formulas are parsed as a specification and its main taken.
tref wff(const char* src) {
	std::string spec = std::string(src) + ".";
	tref t = tau::get(spec.c_str()).value_or(nullptr);
	REQUIRE(t != nullptr);
	auto nso = get_nso_rr<node_t>(t);
	REQUIRE(nso.has_value());
	tref fm = nso->main->get();
	REQUIRE(fm != nullptr);
	return fm;
}

// `ba_normalized_without_mode` lets the environment variable
// TAU_BA_NORMALIZED_WITHOUT override the flag; run with it unset.
struct without_mode {
	int saved = ba_normalized_without;
	explicit without_mode(int mode) {
		ba_normalized_without = mode;
		tau_ba_normalized_without_hits = 0;
		tau_ba_normalized_without_shaped = 0;
		tau_ba_normalized_without_mismatches = 0;
	}
	~without_mode() { ba_normalized_without = saved; }
};

tau_t clause(const char* src) { return normalize_tau(tau_t(wff(src))); }

// the normal form of the clauses accumulated behind one another
tau_t accumulated(const std::vector<const char*>& srcs) {
	std::vector<tau_t> steps{ clause(srcs[0]) };
	for (size_t i = 1; i < srcs.size(); ++i)
		steps.push_back(normalize_tau(steps.back() & clause(srcs[i])));
	return steps.back();
}

// the normal form of the clauses accumulated in front of one another
tau_t accumulated_in_front(const std::vector<const char*>& srcs) {
	std::vector<tau_t> steps{ clause(srcs[0]) };
	for (size_t i = 1; i < srcs.size(); ++i)
		steps.push_back(normalize_tau(clause(srcs[i]) & steps.back()));
	return steps.back();
}

} // namespace

// The normal form of a constant without one of its conjuncts is built from
// the shape of the normal form; the shadow mode normalizes the remaining
// conjunction anyway and counts every result that differs from the built
// form, which must stay zero while the assembled removals are counted.
TEST_SUITE("normalized without") {

	TEST_CASE("a unit is removed from the middle of the unit chain") {
		without_mode shadow(2);
		tau_t k = accumulated({ "always o1[t] = 0", "always o2[t] != 0",
			"always o3[t] = 0" });
		tau_t r = normalize_tau_without(k, clause("always o2[t] != 0"));
		CHECK(tau_ba_normalized_without_hits == 1);
		CHECK(tau_ba_normalized_without_shaped == 1);
		CHECK(tau_ba_normalized_without_mismatches == 0);
		// the same tree as the accumulation of the remaining clauses
		tau_t fresh = accumulated({ "always o1[t] = 0", "always o3[t] = 0" });
		CHECK(r.nso_rr.main->get() == fresh.nso_rr.main->get());
	}

	TEST_CASE("a clause is removed from either end of the clause chain") {
		without_mode shadow(2);
		std::vector<const char*> srcs = { "always o1[t] = 0",
			"always (o2[t] = 0 || o3[t] != 0)", "always (o4[t] != 0 || o5[t] = 0)",
			"always (o6[t] = 0 || o7[t] = 0)" };
		tau_t k = accumulated(srcs);
		tau_t inner = normalize_tau_without(k, clause(srcs[1]));
		tau_t outer = normalize_tau_without(k, clause(srcs[3]));
		CHECK(tau_ba_normalized_without_shaped == 2);
		CHECK(tau_ba_normalized_without_mismatches == 0);
		CHECK(inner.nso_rr.main->get()
			== accumulated({ srcs[0], srcs[2], srcs[3] }).nso_rr.main->get());
		CHECK(outer.nso_rr.main->get()
			== accumulated({ srcs[0], srcs[1], srcs[2] }).nso_rr.main->get());
	}

	TEST_CASE("removing the only conjunct leaves the valid constant") {
		without_mode shadow(2);
		tau_t k = clause("always o1[t] = 0");
		tau_t r = normalize_tau_without(k, clause("always o1[t] = 0"));
		CHECK(tau_ba_normalized_without_hits == 1);
		CHECK(tau_ba_normalized_without_shaped == 0);
		auto one = r.is_one();
		REQUIRE(one.has_value());
		CHECK(one.value());
	}

	TEST_CASE("a clause that is no conjunct leaves the constant as it is") {
		without_mode shadow(2);
		tau_t k = accumulated({ "always o1[t] = 0", "always o2[t] != 0" });
		tau_t r = normalize_tau_without(k, clause("always o3[t] = 0"));
		CHECK(tau_ba_normalized_without_hits == 0);
		CHECK(r.nso_rr.main->get() == k.nso_rr.main->get());
	}

	TEST_CASE("a conjunct sharing a stream with another takes the pipeline") {
		without_mode shadow(2);
		// the two clauses mention o2 and o3 both, so the removed clause
		// shares its streams with a remaining one: found (one hit), not
		// assembled (no shaped removal), normalized by the pipeline
		tau_t k = normalize_tau(tau_t(wff(
			"always (o1[t] = 0 && (o2[t] = 0 || o3[t] != 0) && (o3[t] = 0 || o2[t] != 0))")));
		tau_t r = normalize_tau_without(k, clause("always (o2[t] = 0 || o3[t] != 0)"));
		CHECK(tau_ba_normalized_without_hits == 1);
		CHECK(tau_ba_normalized_without_shaped == 0);
		CHECK(tau_ba_normalized_without_mismatches == 0);
		auto zero = r.is_zero();
		REQUIRE(zero.has_value());
		CHECK_FALSE(zero.value());
	}

	TEST_CASE("a mixed sequence is taken apart in another order than it was built") {
		without_mode shadow(2);
		std::vector<const char*> srcs = { "always o1[t] != 0",
			"always (o2[t] = 0 || o3[t] != 0)", "always o4[t] = 1",
			"always o5[t] != 0", "always (o6[t] != 0 || o7[t] = 1)",
			"always o8[t] = 0", "always (o9[t] = 0 || o10[t] = 0)",
			"always o11[t] != 0" };
		std::vector<tau_t> steps{ accumulated(srcs) };
		std::vector<const char*> left(srcs);
		const size_t order[] = { 3, 0, 5, 1, 2, 0, 1, 0 };
		for (size_t i : order) {
			steps.push_back(normalize_tau_without(steps.back(), clause(left[i])));
			left.erase(left.begin() + i);
			if (!left.empty())
				CHECK(steps.back().nso_rr.main->get()
					== accumulated(left).nso_rr.main->get());
		}
		CHECK(tau_ba_normalized_without_hits == 8);
		CHECK(tau_ba_normalized_without_shaped == 7);
		CHECK(tau_ba_normalized_without_mismatches == 0);
		auto one = steps.back().is_one();
		REQUIRE(one.has_value());
		CHECK(one.value());
	}

	TEST_CASE("every single and every ordered pair of removals from a mixed set") {
		without_mode shadow(2);
		std::vector<const char*> srcs = { "always o1[t] = 0",
			"always (o2[t] != 0 || o3[t] = 0)", "always o4[t] != 0",
			"always (o5[t] = 0 || o6[t] = 1)", "always o7[t] = 1",
			"always (o8[t] != 0 || o9[t] != 0)" };
		tau_t k = accumulated(srcs);
		size_t removals = 0;
		for (size_t i = 0; i < srcs.size(); ++i) {
			std::vector<const char*> left(srcs); left.erase(left.begin() + i);
			tau_t r = normalize_tau_without(k, clause(srcs[i])); ++removals;
			CHECK(r.nso_rr.main->get() == accumulated(left).nso_rr.main->get());
			for (size_t j = 0; j < left.size(); ++j) {
				std::vector<const char*> rest(left); rest.erase(rest.begin() + j);
				tau_t rr = normalize_tau_without(r, clause(left[j])); ++removals;
				CHECK(rr.nso_rr.main->get() == accumulated(rest).nso_rr.main->get());
			}
		}
		CHECK(removals == 36);
		CHECK(tau_ba_normalized_without_hits == 36);
		CHECK(tau_ba_normalized_without_shaped == 36);
		CHECK(tau_ba_normalized_without_mismatches == 0);
	}

	TEST_CASE("each building order removes to the normal form of its own remaining chain") {
		without_mode shadow(2);
		std::vector<const char*> srcs = { "always (o1[t] = 0 || o2[t] != 0)",
			"always o3[t] = 0", "always (o4[t] != 0 || o5[t] = 0)", "always o6[t] != 0" };
		tau_t behind = accumulated(srcs);
		tau_t front = accumulated_in_front(srcs);
		tau_t rb = normalize_tau_without(behind, clause(srcs[1]));
		tau_t rf = normalize_tau_without(front, clause(srcs[1]));
		CHECK(tau_ba_normalized_without_mismatches == 0);
		// the clause chain keeps the order the building gave it, so each
		// result is the normal form of its own remaining chain
		CHECK(rb.nso_rr.main->get()
			== accumulated({ srcs[0], srcs[2], srcs[3] }).nso_rr.main->get());
		CHECK(rf.nso_rr.main->get()
			== accumulated_in_front({ srcs[0], srcs[2], srcs[3] }).nso_rr.main->get());
	}

	TEST_CASE("off keeps the pipeline and gives the same tree") {
		tau_t k = accumulated({ "always o1[t] = 0", "always o2[t] != 0",
			"always (o3[t] = 0 || o4[t] = 0)" });
		std::optional<tau_t> on, off;
		{
			without_mode m(1);
			on.emplace(normalize_tau_without(k, clause("always o2[t] != 0")));
			CHECK(tau_ba_normalized_without_shaped == 1);
		}
		{
			without_mode m(0);
			off.emplace(normalize_tau_without(k, clause("always o2[t] != 0")));
			CHECK(tau_ba_normalized_without_hits == 1);
			CHECK(tau_ba_normalized_without_shaped == 0);
		}
		CHECK(on->nso_rr.main->get() == off->nso_rr.main->get());
	}
}
