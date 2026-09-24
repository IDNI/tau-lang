// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Issue #141: sat / unsat / valid of a formula that normalization cannot
// decide must give no verdict (an UNKNOWN error), never T or F -- a negative
// fallback made `sat` of a satisfiable formula F and `valid` of its
// falsifiable negation T.

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
