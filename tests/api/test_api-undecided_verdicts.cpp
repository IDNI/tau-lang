// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Issue #141: sat / unsat / valid of a formula that normalization cannot
// decide must give no verdict (an UNKNOWN error), never T or F -- a negative
// fallback made `sat` of a satisfiable formula F and `valid` of its
// falsifiable negation T. The qlt order chains reported there are decided
// outright.

#include "test_init.h"
#include "test_tau_helpers.h"

using tau_api = api<node_t>;

namespace {

// The verdict of @p cmd applied to @p fm: T, F, or nullopt for no verdict.
std::optional<bool> verdict(const std::string& cmd, const std::string& fm) {
	auto r = cmd == "sat" ? tau_api::sat(fm)
		: cmd == "unsat" ? tau_api::unsat(fm)
		: tau_api::valid(fm);
	if (!r.has_value()) return std::nullopt;
	return r.value();
}

void check_verdict(const std::string& cmd, const std::string& fm,
	bool expected)
{
	CAPTURE(cmd);
	CAPTURE(fm);
	auto v = verdict(cmd, fm);
	REQUIRE(v.has_value());
	CHECK(*v == expected);
}

} // namespace

#ifdef TAU_PACK_HAS_BA_BV
TEST_SUITE("an undecided formula gives no verdict (issue #141)") {

	// A functional quantifier over a bv arithmetic body stays undecided.
	TEST_CASE("sat, unsat and valid report UNKNOWN") {
		const std::string fm = "(fex x (x + y:bv[4])) = 0";
		CHECK(!verdict("sat", fm).has_value());
		CHECK(!verdict("unsat", fm).has_value());
		CHECK(!verdict("valid", "!(" + fm + ")").has_value());
	}

	// One satisfiable disjunct decides the disjunction.
	TEST_CASE("a satisfiable disjunct outweighs an undecided one") {
		check_verdict("sat", "(fex x (x + y:bv[4])) = 0 || z = 0", true);
	}
}
#endif // TAU_PACK_HAS_BA_BV

#ifdef TAU_PACK_HAS_BA_QLT
TEST_SUITE("qlt order chains are decided (issue #141)") {

	const std::string chain = "x2:qlt < x1:qlt && x1:qlt < x3:qlt"
		" && x2:qlt < x3:qlt";

	TEST_CASE("the reported chain and its negation") {
		check_verdict("sat", chain, true);
		check_verdict("unsat", chain, false);
		check_verdict("valid", "!(" + chain + ")", false);
	}

	TEST_CASE("the reported chain under its witness") {
		check_verdict("sat", chain + " && x2:qlt = {0}:qlt"
			" && x1:qlt = {1}:qlt && x3:qlt = {2}:qlt", true);
	}

	TEST_CASE("a seven-inequality chain") {
		check_verdict("sat", "a:qlt < b:qlt && b:qlt < c:qlt"
			" && c:qlt < d:qlt && d:qlt < e:qlt && e:qlt < f:qlt"
			" && f:qlt < g:qlt && g:qlt < h:qlt", true);
	}

	TEST_CASE("renamed chain (control)") {
		check_verdict("sat", "x:qlt < y:qlt && y:qlt < z:qlt"
			" && x:qlt < z:qlt", true);
	}

	// A cycle is unsatisfiable: Fourier-Motzkin on the middle variable
	// leaves the contradictory pair.
	TEST_CASE("inconsistent cycles are unsatisfiable") {
		check_verdict("sat", "x:qlt < y:qlt && y:qlt < z:qlt"
			" && z:qlt < x:qlt", false);
		check_verdict("sat", "x2:qlt < x1:qlt && x1:qlt < x3:qlt"
			" && x3:qlt < x2:qlt", false);
		check_verdict("sat", "x:qlt < y:qlt && y:qlt < z:qlt"
			" && z:qlt <= x:qlt", false);
		// control: a non-strict cycle holds with all three equal
		check_verdict("sat", "x:qlt <= y:qlt && y:qlt <= z:qlt"
			" && z:qlt <= x:qlt", true);
	}

	// Density: `ex z (x < z && z < y)` is `x < y`.
	TEST_CASE("a two-sided symbolic bound is eliminated") {
		check_verdict("valid", "ex z (x:qlt < z && z < y:qlt)", false);
		check_verdict("sat", "!(ex z (x:qlt < z && z < y:qlt))", true);
		check_verdict("valid", "all x all y (x:qlt < y:qlt"
			" -> ex z (x < z && z < y))", true);
		check_verdict("valid", "all x all y (x:qlt <= y:qlt"
			" -> ex z (x < z && z < y))", false);
		check_verdict("sat", "ex z (x:qlt < z && z < y:qlt)"
			" && y:qlt < x:qlt", false);
	}

	// A symbolic disequality can empty a variable the other constraints pin
	// to one point.
	TEST_CASE("a disequality can empty a pinned variable") {
		auto v = verdict("sat", "a:qlt <= v:qlt && v:qlt <= a:qlt"
			" && v:qlt != a:qlt");
		CHECK(!(v.has_value() && *v));
		v = verdict("sat", "all a (ex v ({1}:qlt <= v && v <= {1}:qlt"
			" && v != a:qlt))");
		CHECK(!(v.has_value() && *v));
		// control: an open interval survives one removed point
		check_verdict("sat", "all a (ex v (v:qlt > {1}:qlt"
			" && v != a:qlt))", true);
	}
}
#endif // TAU_PACK_HAS_BA_QLT
