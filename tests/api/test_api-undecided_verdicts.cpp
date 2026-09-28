// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Issue #141: sat / unsat / valid of a formula that normalization cannot
// decide must give no verdict (an UNKNOWN error), never T or F -- a negative
// fallback made `sat` of a satisfiable formula F and `valid` of its
// falsifiable negation T. The shapes reported there, a qlt order chain and
// the functional quantifiers fex / fall, are decided outright.

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

#ifdef TAU_PACK_HAS_BA_BV
// Two formulas normalization leaves undecided: a functional quantifier over
// a bv arithmetic body.
const std::string undecided_a = "(fex x (x + y:bv[4])) = 0";
const std::string undecided_b = "(fex x (x * w:bv[4])) = 1";
#endif // TAU_PACK_HAS_BA_BV

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

	// Two undecided disjuncts and no satisfiable one: the first no-verdict
	// is the result, the second does not replace it.
	TEST_CASE("two undecided disjuncts give no verdict") {
		CHECK(!verdict("sat", undecided_a + " || " + undecided_b)
			.has_value());
		CHECK(!verdict("sat", "(always (o1[t]:bv[4] = 0 && "
			+ undecided_a + ")) || (always (o2[t]:bv[4] = 0 && "
			+ undecided_b + "))").has_value());
	}

	// valid of a conjunction asks is_tau_impl about the negation, a
	// disjunction of two undecided disjuncts.
	TEST_CASE("valid of two undecided conjuncts gives no verdict") {
		auto r = tau_api::valid(undecided_a + " && " + undecided_b);
		CHECK(!r.has_value());
		CHECK(report_has_code(r.report(), code::solver_error));
	}

	// The negated equivalence splits into disjuncts that are all undecided.
	TEST_CASE("equivalence with an undecided side gives no verdict") {
		auto f1 = tau_api::get_formula(undecided_a);
		auto f2 = tau_api::get_formula("z = 0");
		REQUIRE(f1.has_value());
		REQUIRE(f2.has_value());
		auto r = are_tau_equivalent<node_t>(f1.value(), f2.value());
		CHECK(!r.has_value());
		CHECK(report_has_code(r.report(), code::solver_error));
		// control: the same formula is equivalent to itself
		auto same = are_tau_equivalent<node_t>(f1.value(), f1.value());
		REQUIRE(same.has_value());
		CHECK(same.value());
	}

	// An undecided step of an always run is no verdict, not a refutation.
	TEST_CASE("an undecided always run gives no verdict") {
		CHECK(!verdict("sat", "always (o1[t]:bv[4] = 0 && "
			+ undecided_a + ")").has_value());
		// control: the decidable run
		check_verdict("sat", "always o1[t]:bv[4] + o2[t] = 0", true);
	}

	// Lowering the log level to debug does not change the verdict of an
	// undecided formula.
	TEST_CASE("debug logging leaves the undecided verdict unchanged") {
		const auto level = logging::level();
		logging::debug();
		auto r = tau_api::sat(undecided_a);
		logging::set_filter(level);
		CHECK(!r.has_value());
	}
}

TEST_SUITE("unsat_core of undecided and edge-case inputs") {

	std::string core_str(const trefs& core) {
		std::string s;
		for (tref c : core) s += tau_api::to_str(c) + "; ";
		return s;
	}

	std::string core_str(const std::vector<std::string>& core) {
		std::string s;
		for (const auto& c : core) s += c + "; ";
		return s;
	}

	std::string report_str(const auto& r) {
		std::ostringstream os;
		os << r.report();
		return os.str();
	}

	TEST_CASE("a null or non-formula input is an invalid argument") {
		auto n = tau_api::unsat_core(tref(nullptr), false);
		CHECK(!n.has_value());
		CHECK(report_has_code(n.report(), code::invalid_argument));
		auto term = tau_api::get_formula_or_term("x & y");
		REQUIRE(term.has_value());
		auto t = tau_api::unsat_core(term.value(), false);
		CHECK(!t.has_value());
		CHECK(report_has_code(t.report(), code::invalid_argument));
		CHECK(report_str(t).find("Invalid formula") != std::string::npos);
	}

	// A bare formula, not a spec root, is its own main formula; with
	// realizability off the core is an unsatisfiable subset.
	TEST_CASE("the core of a bare unsatisfiable formula") {
		auto fm = tau_api::get_formula(
			"x & y = 1 && w = 0 && x | y = 0");
		REQUIRE(fm.has_value());
		auto core = tau_api::unsat_core(fm.value(), false);
		REQUIRE(core.has_value());
		CAPTURE(core_str(core.value()));
		CHECK(core.value().size() == 2);
		auto none = tau_api::unsat_core(
			tau_api::get_formula("x = 0 && y = 1").value(), false);
		REQUIRE(none.has_value());
		CHECK(none.value().empty());
	}

	// Parenthesized conjuncts and an always nested in an always split into
	// the same conjuncts as their flat form.
	TEST_CASE("parenthesized and nested always conjuncts") {
		auto core = tau_api::unsat_core(
			"(always o1[t] & o2[t] = 1 && always o3[t] = 0)"
			" && always (always o1[t] | o2[t] = 0 && o4[t] = 0).",
			false);
		REQUIRE(core.has_value());
		CAPTURE(core_str(core.value()));
		CHECK(core.value().size() == 2);
	}

	// A sub-check without a verdict keeps its conjunct and warns; the
	// conflicting pair is still found.
	TEST_CASE("an undecided sub-check keeps its conjunct and warns") {
		for (bool realizability : { false, true }) {
			CAPTURE(realizability);
			auto core = tau_api::unsat_core("a & b = 1 && a | b = 0"
				" && (fex x (x * y:bv[4])) = 0.", realizability);
			REQUIRE(core.has_value());
			CAPTURE(core_str(core.value()));
			CHECK(core.value() == std::vector<std::string>{
				"ab = 1", "a|b = 0" });
			CHECK(report_str(core).find("no verdict for a subset")
				!= std::string::npos);
		}
	}

	// No verdict for the whole spec is an error, not an empty core.
	TEST_CASE("an undecided spec has no core") {
		for (bool realizability : { false, true }) {
			CAPTURE(realizability);
			auto core = tau_api::unsat_core(undecided_a + ".",
				realizability);
			CHECK(!core.has_value());
			CHECK(report_has_code(core.report(), code::solver_error));
			CHECK(report_str(core).find("has no core")
				!= std::string::npos);
		}
	}
}
#endif // TAU_PACK_HAS_BA_BV

TEST_SUITE("functional quantifiers are evaluated (issue #141)") {

	TEST_CASE("fex is the join over the bound variable") {
		check_verdict("sat", "(fex x (y & x)) = 0", true);
		check_verdict("valid", "(fex x (y & x)) != 0", false);
		check_verdict("valid", "(fex x (y & x)) = y", true);
		check_verdict("valid", "(fex x x) = 1", true);
		check_verdict("valid", "(fex x (x & x')) = 0", true);
		// control: y = 0 witnesses the first query
		check_verdict("sat", "y = 0", true);
	}

	TEST_CASE("fall is the meet over the bound variable") {
		check_verdict("sat", "(fall x (y | x)) = 0", true);
		check_verdict("valid", "(fall x (y | x)) = y", true);
		check_verdict("valid", "(fall x x) = 0", true);
		check_verdict("unsat", "(fall x (y | x)) != y", true);
	}

	TEST_CASE("nested functional quantifiers") {
		check_verdict("valid", "(fex x (fall z (x | z))) = 1", true);
		check_verdict("valid", "(fall x (fex z (x & z))) = 0", true);
	}
}

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
