// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit-wise decision of simp_tau_unsat_valid's per-path tests.
//
// normalize_tau feeds every embedded :tau constant through
// simp_tau_unsat_valid, which decides validity of the element and then the
// satisfiability of each DNF path. Where the element is a conjunction of
// units with pairwise disjoint free supports, validity distributes over the
// conjunction and satisfiability factors over the supports, so the unit-wise
// verdicts are exact; everywhere else the monolithic checks stay. With the
// component factoring enabled the normal form must therefore be byte-identical
// to the stock one. The cases cover: a factorable element (fast path), an
// element with one unsat unit (the F path), a coupled element (no split), and
// an element that is a disjunction of conjunctions (path enumeration must
// still see every path).

#include "test_integration-satisfiability_helper.h"

using tau_api = api<node_t>;

namespace {

std::string api_normalize(const std::string& s) {
	auto f = tau_api::get_formula_or_term(s.c_str());
	REQUIRE(f.has_value());
	auto n = tau_api::normalize_formula(f.value());
	REQUIRE(n.has_value());
	return tau::get(n.value()).to_str();
}

struct factoring_config {
	factoring_config(bool on) { ba_component_factoring = on; }
	~factoring_config() { ba_component_factoring = false; }
};

// Independent units, all satisfiable: the element stays as it is.
const char* FACTORABLE =
	"o9[t]:tau = { (o1[t] = 1 -> o2[t] = 1) && (o3[t] = 1 -> o4[t] = 1)"
	" && (o5[t] = 0) }";
// One unit unsat (o1 pinned both ways): the element collapses to F.
const char* UNIT_UNSAT =
	"o9[t]:tau = { (o1[t] = 1) && (o1[t] = 0) && (o3[t] = 1 -> o4[t] = 1) }";
// Coupled units share o1: no split is possible, monolithic path.
const char* COUPLED =
	"o9[t]:tau = { (o1[t] = 1 -> o2[t] = 1) && (o1[t] = 1 -> o4[t] = 1) }";
// Two paths, one of them unsat: the unsat path must still be dropped.
const char* TWO_PATHS =
	"o9[t]:tau = { ((o1[t] = 1) && (o1[t] = 0)) || ((o3[t] = 1) && (o5[t] = 0)) }";

} // namespace

TEST_SUITE("simp_tau_unsat_valid unit-wise") {

	TEST_CASE("normal forms are byte-identical with factoring enabled") {
		std::string s1, s2, s3, s4;
		{
			factoring_config off(false);
			s1 = api_normalize(FACTORABLE);
			s2 = api_normalize(UNIT_UNSAT);
			s3 = api_normalize(COUPLED);
			s4 = api_normalize(TWO_PATHS);
		}
		{
			factoring_config on(true);
			CHECK(api_normalize(FACTORABLE) == s1);
			CHECK(api_normalize(UNIT_UNSAT) == s2);
			CHECK(api_normalize(COUPLED) == s3);
			CHECK(api_normalize(TWO_PATHS) == s4);
			// Warm caches: repeat the factorable case.
			CHECK(api_normalize(FACTORABLE) == s1);
		}
	}
}
