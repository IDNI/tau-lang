// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "tau.h"

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

// `ba_normalized_memo_mode` lets the environment variable
// TAU_BA_NORMALIZED_MEMO override the flag; run with it unset.
struct memo_mode {
	int saved = ba_normalized_memo;
	explicit memo_mode(int mode) {
		ba_normalized_memo = mode;
		tau_ba_normalized_memo_hits = 0;
		tau_ba_normalized_memo_mismatches = 0;
	}
	~memo_mode() { ba_normalized_memo = saved; }
};

} // namespace

// The normal form `normalize_tau` returns is a fixed point of the
// normalizer, so the decision of such a main may skip its renormalization.
// The shadow mode runs the normalizer anyway and counts every main it
// changes; that count must stay zero while the skips are counted as hits.
TEST_SUITE("normalized main memo") {

	TEST_CASE("a normalized constant is decided without renormalization") {
		memo_mode shadow(2);
		tau_t k(wff("(always o1[t] = 1) && (always o2[t] = 0)"));
		tau_t n = normalize_tau(k);
		auto zero = n.is_zero();
		REQUIRE(zero.has_value());
		CHECK_FALSE(zero.value());
		auto one = n.is_one();
		REQUIRE(one.has_value());
		CHECK_FALSE(one.value());
		CHECK(tau_ba_normalized_memo_hits >= 1);
		CHECK(tau_ba_normalized_memo_mismatches == 0);
	}

	TEST_CASE("conjoining a clause keeps the memo exact") {
		memo_mode shadow(2);
		tau_t k = normalize_tau(tau_t(wff(
			"(always o1[t] = 1) && (always o2[t] = 0)")));
		tau_t c = normalize_tau(tau_t(wff("always o3[t] = 1")));
		tau_t kc = normalize_tau(k & c);
		auto zero = kc.is_zero();
		REQUIRE(zero.has_value());
		CHECK_FALSE(zero.value());
		auto one = kc.is_one();
		REQUIRE(one.has_value());
		CHECK_FALSE(one.value());
		// a second normalization of a returned main is the main itself
		tau_t again = normalize_tau(kc);
		CHECK(again.nso_rr.main->get() == kc.nso_rr.main->get());
		CHECK(tau_ba_normalized_memo_hits >= 2);
		CHECK(tau_ba_normalized_memo_mismatches == 0);
	}

	TEST_CASE("an unsatisfiable normalized constant is still zero") {
		memo_mode shadow(2);
		tau_t k = normalize_tau(tau_t(wff(
			"(always o1[t] = 1) && (always o1[t] = 0)")));
		auto zero = k.is_zero();
		REQUIRE(zero.has_value());
		CHECK(zero.value());
		CHECK(tau_ba_normalized_memo_hits >= 1);
		CHECK(tau_ba_normalized_memo_mismatches == 0);
	}

	TEST_CASE("off keeps the old path") {
		memo_mode off(0);
		tau_t k = normalize_tau(tau_t(wff("always o4[t] = 1")));
		auto zero = k.is_zero();
		REQUIRE(zero.has_value());
		CHECK_FALSE(zero.value());
		CHECK(tau_ba_normalized_memo_hits == 0);
	}
}
