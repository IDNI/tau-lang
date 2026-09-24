// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "tau.h"

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

// `ba_normalized_conjunction_mode` lets the environment variable
// TAU_BA_NORMALIZED_CONJUNCTION override the flag; run with it unset.
struct conjunction_mode {
	int saved = ba_normalized_conjunction;
	explicit conjunction_mode(int mode) {
		ba_normalized_conjunction = mode;
		tau_ba_normalized_conjunction_hits = 0;
		tau_ba_normalized_conjunction_mismatches = 0;
	}
	~conjunction_mode() { ba_normalized_conjunction = saved; }
};

} // namespace

// The normal form of `K & c`, K a normal form normalize_tau returned and c
// a single clause, is built from the two bodies instead of renormalizing
// the whole conjunction. The shadow mode runs the pipeline anyway and
// counts every result that differs from the built form; that count must
// stay zero while the builds are counted as hits.
TEST_SUITE("normalized conjunction") {

	TEST_CASE("a clause conjoined to a normal form builds the pipeline's result") {
		conjunction_mode shadow(2);
		tau_t k = normalize_tau(tau_t(wff(
			"(always o1[t] = 1) && (always o2[t] = 0)")));
		tau_t c = normalize_tau(tau_t(wff("always o3[t] = 1")));
		tau_t kc = normalize_tau(k & c);
		CHECK(tau_ba_normalized_conjunction_hits >= 1);
		CHECK(tau_ba_normalized_conjunction_mismatches == 0);
		auto zero = kc.is_zero();
		REQUIRE(zero.has_value());
		CHECK_FALSE(zero.value());
	}

	TEST_CASE("clauses accumulated over several steps stay exact") {
		conjunction_mode shadow(2);
		std::vector<tau_t> steps;
		steps.push_back(normalize_tau(tau_t(wff("always o1[t] = 1"))));
		const char* clauses[] = { "always o2[t] = 0",
			"always (o3[t] = 1 || o4[t] = 0)", "always o1[t] != 0",
			"always (o2[t] = 0 || o3[t] = 1)" };
		for (const char* src : clauses)
			steps.push_back(normalize_tau(steps.back()
				& normalize_tau(tau_t(wff(src)))));
		CHECK(tau_ba_normalized_conjunction_hits >= 4);
		CHECK(tau_ba_normalized_conjunction_mismatches == 0);
		auto zero = steps.back().is_zero();
		REQUIRE(zero.has_value());
		CHECK_FALSE(zero.value());
	}

	TEST_CASE("the built normal form is a fixed point of the normalizer") {
		conjunction_mode on(1);
		tau_t k = normalize_tau(tau_t(wff(
			"(always o1[t] = 1) && (always o2[t] = 0)")));
		tau_t c = normalize_tau(tau_t(wff("always (o3[t] = 1 || o1[t] = 0)")));
		tau_t kc = normalize_tau(k & c);
		CHECK(tau_ba_normalized_conjunction_hits >= 1);
		auto again = normalizer<node_t>(kc.nso_rr);
		REQUIRE(again.has_value());
		CHECK(again.value() == kc.nso_rr.main->get());
	}

	TEST_CASE("an unsatisfiable conjunction is still zero") {
		conjunction_mode shadow(2);
		tau_t k = normalize_tau(tau_t(wff("always o1[t] = 1")));
		tau_t c = normalize_tau(tau_t(wff("always o1[t] = 0")));
		tau_t kc = normalize_tau(k & c);
		CHECK(tau_ba_normalized_conjunction_mismatches == 0);
		auto zero = kc.is_zero();
		REQUIRE(zero.has_value());
		CHECK(zero.value());
	}

	TEST_CASE("a side that is no always-hull takes the pipeline") {
		conjunction_mode on(1);
		tau_t k = normalize_tau(tau_t(wff("always o1[t] = 1")));
		tau_t c = normalize_tau(tau_t(wff("sometimes o2[t] = 1")));
		tau_t kc = normalize_tau(k & c);
		CHECK(tau_ba_normalized_conjunction_hits == 0);
		auto zero = kc.is_zero();
		REQUIRE(zero.has_value());
		CHECK_FALSE(zero.value());
	}
}
