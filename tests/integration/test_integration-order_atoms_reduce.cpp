// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The BDD-based DNF/CNF reduction (dnf_cnf_to_reduced) must keep the order
// atoms `<` and `<=`: they are BDD variables like the equalities. Without
// that, a bitvector variable pinned to one value by order atoms lost every
// clause that mentioned only the pinned value, and
// `ex x (x > 2 && x < 4 && (x = 3 -> a = 0))` normalized to T.

#include "test_init.h"
#include "test_tau_helpers.h"

namespace {

tref parse_wff(const std::string& sample) {
	static tree<node_t>::get_options opts{ .parse = { .start = tree<node_t>::wff }};
	auto src = tree<node_t>::get(sample, opts);
	if (src == nullptr) TAU_LOG_ERROR << "Parsing failed for: " << sample;
	return src;
}

std::string norm(const std::string& sample) {
	auto wff = parse_wff(sample);
	return wff ? tau::get(normalizer<node_t>(wff)).to_str() : "parse_error";
}

// Two samples normalize to the same formula, or to formulas whose
// equivalence normalizes to T.
bool same(const std::string& a, const std::string& b) {
	tref ra = parse_wff(a), rb = parse_wff(b);
	if (!ra || !rb) return false;
	ra = normalizer<node_t>(ra), rb = normalizer<node_t>(rb);
	if (tau::get(ra) == tau::get(rb)) return true;
	return tau::get(normalizer<node_t>(tau::build_wff_equiv(ra, rb))).equals_T();
}

} // namespace

TEST_SUITE("configuration") {
	TEST_CASE("bdd_init") { bdd_init<Bool>(); }
	TEST_CASE("logging") { logging::trace(); }
}

TEST_SUITE("order atoms survive the reduction") {

	TEST_CASE("a value pinned by order atoms keeps the guarded consequent") {
		// The interval (2, 4) of bv[4] holds exactly one value, 3, so the
		// implication fires and only `a = 0` remains.
		CHECK(same("ex x (x:bv[4] > { 2 }:bv[4] && x:bv[4] < { 4 }:bv[4] && (x:bv[4] = { 3 }:bv[4] -> a = 0))",
			"a = 0"));
		CHECK(same("ex x (x:bv[4] > { 2 }:bv[4] && x:bv[4] < { 4 }:bv[4] && (x:bv[4] = { 3 }:bv[4] -> y:bv[4] = { 0 }:bv[4]))",
			"y:bv[4] = { 0 }:bv[4]"));
		CHECK(same("ex x (x:bv[4] >= { 3 }:bv[4] && x:bv[4] <= { 3 }:bv[4] && (x:bv[4] = { 3 }:bv[4] -> a = 0))",
			"a = 0"));
		CHECK(same("ex x (x:bv[4] < { 1 }:bv[4] && (x:bv[4] = { 0 }:bv[4] -> a = 0))",
			"a = 0"));
		CHECK(same("ex x (x:bv[8] > { 2 }:bv[8] && x:bv[8] < { 4 }:bv[8] && (x:bv[8] = { 3 }:bv[8] -> a = 0))",
			"a = 0"));
	}

	TEST_CASE("the same shapes without a guard are unchanged") {
		CHECK(same("ex x (x:bv[4] > { 2 }:bv[4] && x:bv[4] < { 4 }:bv[4] && a = 0)", "a = 0"));
		CHECK(norm("ex x (x:bv[4] > { 2 }:bv[4] && x:bv[4] < { 4 }:bv[4] && x:bv[4] != { 3 }:bv[4])") == "F");
		CHECK(norm("ex x (x:bv[4] > { 2 }:bv[4] && x:bv[4] < { 4 }:bv[4] && (x:bv[4] != { 3 }:bv[4] || x:bv[4] = { 5 }:bv[4]))") == "F");
		CHECK(same("ex x (x:bv[4] = { 3 }:bv[4] && (x:bv[4] = { 3 }:bv[4] -> a = 0))", "a = 0"));
	}

	TEST_CASE("negated order atoms are literals of the same variables") {
		// `x !< 3` next to `x < 3` is a contradiction, not two unrelated atoms.
		CHECK(norm("ex x (x:bv[4] < { 3 }:bv[4] && !(x:bv[4] < { 3 }:bv[4]))") == "F");
		CHECK(norm("all x (x:bv[4] < { 3 }:bv[4] || !(x:bv[4] < { 3 }:bv[4]))") == "T");
		CHECK(norm("all x (x:bv[4] <= { 3 }:bv[4] || x:bv[4] > { 3 }:bv[4])") == "T");
		CHECK(same("ex x (!(x:bv[4] < { 1 }:bv[4]) && !(x:bv[4] > { 1 }:bv[4]) && (x:bv[4] = { 1 }:bv[4] -> a = 0))",
			"a = 0"));
	}

	TEST_CASE("universal form of the pinned value") {
		CHECK(same("all x ((x:bv[4] > { 2 }:bv[4] && x:bv[4] < { 4 }:bv[4]) -> (x:bv[4] = { 3 }:bv[4] -> a = 0))",
			"a = 0"));
	}
}
