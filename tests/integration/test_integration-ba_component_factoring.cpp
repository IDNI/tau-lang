// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Support-component factoring of the Tau-BA constant/valid tests.
//
// sat over a conjunction whose conjuncts share no free variables factors
// exactly (models over disjoint supports compose); validity distributes over
// conjunction unconditionally. Enabling the factoring must therefore not
// change any answer. The cases below cover: a factorable satisfiable corpus,
// a corpus that is unsat INSIDE one component (the early-exit path), a
// coupled corpus (shared stream => components merge => the identity is not
// even needed), and a unit containing an embedded constant, which must take
// the conservative fallback (exercise of the guard, not just of the fast
// path -- a miss-only test would prove nothing about the gate).

#include "test_integration-satisfiability_helper.h"

using tau_api = api<node_t>;

namespace {

bool api_sat(const std::string& s) {
	tref f = tau_api::get_formula_or_term(s.c_str());
	REQUIRE(f != nullptr);
	return tau_api::sat(f);
}

struct factoring_config {
	factoring_config(bool on) { ba_component_factoring = on; }
	~factoring_config() { ba_component_factoring = false; }
};

// Three independent rules over disjoint streams: satisfiable.
const char* DISJOINT_SAT =
	"{ (o1[t] = 1 -> o2[t] = 1) && (o3[t] = 1 -> o4[t] = 1)"
	" && (o5[t] = 0) }:tau != 0";
// Unsat inside one component (o1 pinned to both values), others untouched.
const char* COMPONENT_UNSAT =
	"{ (o1[t] = 1) && (o1[t] = 0) && (o3[t] = 1 -> o4[t] = 1) }:tau != 0";
// Coupled: o1 appears in two conjuncts, the components merge.
const char* COUPLED_SAT =
	"{ (o1[t] = 1 -> o2[t] = 1) && (o1[t] = 1 -> o4[t] = 1) }:tau != 0";
// A unit embedding another BA constant: support is invisible, the factoring
// must fall back to the monolithic path.
const char* EMBEDDED_FALLBACK =
	"{ (o1[t]:tau = { o6[t] = 1 }) && (o3[t] = 1 -> o4[t] = 1) }:tau != 0";

} // namespace

TEST_SUITE("tau ba component factoring") {

	TEST_CASE("answers are unchanged with factoring enabled") {
		bool r1, r2, r3, r4;
		{
			factoring_config off(false);
			r1 = api_sat(DISJOINT_SAT);
			r2 = api_sat(COMPONENT_UNSAT);
			r3 = api_sat(COUPLED_SAT);
			r4 = api_sat(EMBEDDED_FALLBACK);
		}
		CHECK(r1 == true);
		CHECK(r2 == false);
		{
			factoring_config on(true);
			CHECK(api_sat(DISJOINT_SAT) == r1);
			CHECK(api_sat(COMPONENT_UNSAT) == r2);
			CHECK(api_sat(COUPLED_SAT) == r3);
			CHECK(api_sat(EMBEDDED_FALLBACK) == r4);
			// Warm component caches: repeat.
			CHECK(api_sat(DISJOINT_SAT) == r1);
		}
	}
}
