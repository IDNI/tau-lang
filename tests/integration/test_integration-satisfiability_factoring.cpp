// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Connected-support-component factoring on the sat path (issue #72).
// The factoring hook lives in api<node>::realizable, so these cases go
// through the api sat entry (is_tau_formula_sat directly would bypass it).
// Off by default; each case that exercises it toggles the runtime switch
// and restores the default afterwards.

#include "test_integration-satisfiability_helper.h"

using tau_api = api<node_t>;

// Drive sat through the tref route (get_formula_or_term + sat(tref)) - the
// path API clients use. The sat(std::string) convenience overload parses
// formula-first and diverges from the REPL on some shapes (e.g. quantified
// conjuncts), which is a separate pre-existing matter.
static bool api_sat(const char* s) {
	tref f = tau_api::get_formula_or_term(s);
	if (!f) return false;
	return tau_api::sat(f);
}

TEST_SUITE("component factoring") {
	TEST_CASE("off by default: stock behavior") {
		bdd_init<Bool>();
		CHECK(api_sat("always ((o10[t] = 1 -> o11[t] = 1) && (o20[t] = 1 -> o21[t] = 1))"));
	}
	TEST_CASE("disjoint supports factor and stay sat") {
		tau_api::set_component_factoring(true);
		CHECK(api_sat("always ((o10[t] = 1 -> o11[t] = 1) && (o20[t] = 1 -> o21[t] = 1) && (o30[t] = 1 -> o31[t] = 1))"));
		tau_api::set_component_factoring(false);
	}
	TEST_CASE("one dead component makes the whole unsat") {
		tau_api::set_component_factoring(true);
		CHECK(!api_sat("always ((o5[t] = 1) && (o5[t] = 0) && (o20[t] = 1 -> o21[t] = 1))"));
		tau_api::set_component_factoring(false);
	}
	TEST_CASE("shared supports form one component: chain unsat detected") {
		tau_api::set_component_factoring(true);
		CHECK(!api_sat("always ((o1[t] = 1) && (o1[t] = 1 -> o2[t] = 1) && (o2[t] = 0))"));
		tau_api::set_component_factoring(false);
	}
	TEST_CASE("quantifier above the conjunction suppresses the split") {
		tau_api::set_component_factoring(true);
		CHECK(api_sat("always ((all x (x = x)) && (o10[t] = 1 -> o11[t] = 1))"));
		tau_api::set_component_factoring(false);
	}
	TEST_CASE("answers agree with stock on the same formulas") {
		const char* fms[] = {
			"always ((o10[t] = 1 -> o11[t] = 1) && (o20[t] = 1 -> o21[t] = 1))",
			"always ((o5[t] = 1) && (o5[t] = 0) && (o20[t] = 1 -> o21[t] = 1))",
			"always ((o1[t] = 1) && (o1[t] = 1 -> o2[t] = 1) && (o2[t] = 0))",
		};
		for (const char* fm : fms) {
			bool stock = api_sat(fm);
			tau_api::set_component_factoring(true);
			bool factored = api_sat(fm);
			tau_api::set_component_factoring(false);
			CHECK(stock == factored);
		}
	}
}
