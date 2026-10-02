// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#ifdef DEBUG // in release it is included with tau.h
#	include "solver.h"
#endif
#include "test_integration-solver_helper.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("find_solution") {

	TEST_CASE("one var: {(0, 1)}:qlt x = 0.") {
		CHECK( test_find_solution("{(0, 1)}:qlt x = 0.") );
	}

	TEST_CASE("two vars: {(0, 1)}:qlt x | {[1, 2)}:qlt y = 0.") {
		CHECK( test_find_solution("{(0, 1)}:qlt x | {[1, 2)}:qlt y = 0.") );
	}

	TEST_CASE("complement: {(0, 1)}:qlt x | {(0, 1)}:qlt' y = 0.") {
		CHECK( test_find_solution("{(0, 1)}:qlt x | {(0, 1)}:qlt' y = 0.") );
	}
}

TEST_SUITE("solve_inequality_system") {

	bool test_solve_inequality_system(const std::vector<std::string>& inequalities) {
		return ::test_solve_inequality_system(inequalities, node_t::ba::splitter_one(qlt_type<node_t>()));
	}

	TEST_CASE("{(0, 1)}:qlt x != 0 && {(0, 1)}:qlt' x != 0.") {
		CHECK( test_solve_inequality_system({
			"{(0, 1)}:qlt x != 0.",
			"{(0, 1)}:qlt' x != 0."
		}) );
	}

	TEST_CASE("{(0, 1)}:qlt x != 0 && {[1, 2)}:qlt y != 0.") {
		CHECK( test_solve_inequality_system({
			"{(0, 1)}:qlt x != 0.",
			"{[1, 2)}:qlt y != 0."
		}) );
	}

	TEST_CASE("x : qlt != {0}:qlt && x : qlt != {1}:qlt.") {
		CHECK( test_solve_inequality_system({
			"x : qlt != {0}:qlt.",
			"x : qlt != {1}:qlt."
		}) );
	}
}

TEST_SUITE("solve_system") {

	bool test_solve_system(const std::string& equality,
			const std::vector<std::string>& inequalities) {
		return ::test_solve_system(equality, inequalities, node_t::ba::splitter_one(qlt_type<node_t>()));
	}

	TEST_CASE("{(0, 1)}:qlt x = 0 && {(0, 1)}:qlt' x != 0.") {
		CHECK( test_solve_system(
			"{(0, 1)}:qlt x = 0.",
			{"{(0, 1)}:qlt' x != 0."}
		) );
	}

	// Both vars share the same qlt coefficient so the disjoint [1,2) inequality is not contradicted.
	TEST_CASE("{(0, 1)}:qlt x | {(0, 1)}:qlt y = 0 && {[1, 2)}:qlt y != 0.") {
		CHECK( test_solve_system(
			"{(0, 1)}:qlt x | {(0, 1)}:qlt y = 0.",
			{"{[1, 2)}:qlt y != 0."}
		) );
	}

	TEST_CASE("x : qlt < y : qlt") {
		CHECK( test_solve_system(
			"x : qlt & (y : qlt)' = 0.",
			{"x : qlt & (y : qlt)' | (x : qlt)' & y : qlt != 0."}
		) );
	}
}

TEST_SUITE("solve") {

	bool test_solve(const std::string& system, const solver_options& options) {
		return ::test_solve(system, options);
	}
	bool test_solve(const std::string& system, const tref type = qlt_type<node_t>()) {
		return ::test_solve(system, type);
	}
	bool test_solve_min(const std::string& system, const tref type = qlt_type<node_t>()) {
		return ::test_solve_min(system, type);
	}
	bool test_solve_max(const std::string& system, const tref type = qlt_type<node_t>()) {
		return ::test_solve_max(system, type);
	}

	// The typed 0 and 1 are the empty set and all of Q: a proper nonempty
	// subset satisfies both disequalities. No nonzero set is least, and no
	// proper one greatest.
	TEST_CASE("x : qlt != 0") {
		const char* system = "x : qlt != 0.";
		CHECK( test_solve(system) );
		CHECK( test_solve_max(system) );
	}

	TEST_CASE("x : qlt != 0 && x : qlt != 1") {
		CHECK( test_solve("x : qlt != 0 && x : qlt != 1.") );
	}

	TEST_CASE("{(0, 1)}:qlt x != 0 && {[1, 2)}:qlt y != 0") {
		const char* system = "{(0, 1)}:qlt x != 0 && {[1, 2)}:qlt y != 0.";
		CHECK( test_solve(system) );
	}

	TEST_CASE("unsatisfiable: x : qlt = 0 && x : qlt != 0") {
		const char* system = "x : qlt = 0 && x : qlt != 0.";
		CHECK( !test_solve(system) );
	}

	// <= is inclusion: x lies between {0} and [0,1]; no set lies between
	// {0} and {1}.
	TEST_CASE("interval via <=: {0}:qlt <= x:qlt && x:qlt <= {[0,1]}:qlt") {
		CHECK( test_solve("{0}:qlt <= x:qlt && x:qlt <= {[0,1]}:qlt.") );
		CHECK( !test_solve("{0}:qlt <= x:qlt && x:qlt <= {1}:qlt.") );
	}
}

// SO-1: the DLO diversion in solve(const equations&, ...) only fired for a
// system that is *entirely* ordering atoms with no equality.  A mixed system
// therefore fell through to the BA-level solve_system, where
// check_extreme_solution only rejects on equals_F() after
// bf_reduce_canonical -- which cannot evaluate an ordering atom, so an extreme
// solution violating it silently passed -- and, with no equality present,
// solve_inequality_system's minterm_inequality_system_iterator dereferenced an
// empty traverser on the bf_gt node.  solve() now solves the system WITHOUT
// its ordering atoms and then verifies each of them against the solution
// (ground qlt comparisons fold to T/F at construction), declining on anything
// that does not verify.
TEST_SUITE("SO-1 mixed ordering systems") {

	std::optional<solution<node_t>> solve_mixed(const std::string& system) {
		tref form = get_nso_rr<node_t>(tau::get(system).value_or(nullptr)).value().main->get();
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(qlt_type<node_t>()),
			.mode = solver_mode::general
		};
		auto r = solve<node_t>(form, options);
		return r.has_value() ? std::optional<solution<node_t>>(r.value()) : std::nullopt;
	}

	TEST_CASE("unsat equality/ordering mix is declined, not mis-solved") {
		CHECK( !solve_mixed(
			"x : qlt = {1/2}:qlt && x : qlt > {3/4}:qlt.")
			.has_value() );
	}

	TEST_CASE("sat disequality/ordering mix yields a verified solution") {
		auto sol = solve_mixed(
			"x : qlt != {0}:qlt && x : qlt > {3/4}:qlt.");
		REQUIRE( sol.has_value() );
		REQUIRE( sol.value().size() == 1 );
		// The solution must satisfy the ordering atom it was verified
		// against: substituting it into the system's atoms must not
		// produce F (ground qlt comparisons fold at construction).
		tref atoms = get_nso_rr<node_t>(tau::get(
			"x : qlt != {0}:qlt && x : qlt > {3/4}:qlt.").value_or(nullptr))
			.value().main->get();
		tref subst = rewriter::replace<node_t>(atoms, sol.value());
		CHECK( !tau::get(subst).find_top(is<node_t, tau::wff_f>) );
	}
}

// SO-14: systems of order atoms against constants, the order being inclusion.
TEST_SUITE("SO-14 qlt ordering systems") {

	std::optional<solution<node_t>> solve_ord(const std::string& system) {
		tref form = get_nso_rr<node_t>(tau::get(system).value_or(nullptr)).value().main->get();
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(qlt_type<node_t>()),
			.mode = solver_mode::general
		};
		auto r = solve<node_t>(form, options);
		return r.has_value() ? std::optional<solution<node_t>>(r.value()) : std::nullopt;
	}

	TEST_CASE("bounded interval yields a witness inside it") {
		auto sol = solve_ord(
			"x : qlt > {3/4}:qlt && x : qlt < {[3/4,7/8]}:qlt.");
		REQUIRE( sol.has_value() );
		REQUIRE( sol.value().size() == 1 );
	}

	TEST_CASE("one-sided bound is satisfiable") {
		CHECK( solve_ord("x : qlt >= {5}:qlt.").has_value() );
	}

	TEST_CASE("empty interval is declined") {
		CHECK( !solve_ord(
			"x : qlt > {7/8}:qlt && x : qlt < {3/4}:qlt.")
			.has_value() );
	}

	TEST_CASE("two independent variables both get witnesses") {
		auto sol = solve_ord(
			"x : qlt > {1}:qlt && y : qlt < {0}:qlt.");
		REQUIRE( sol.has_value() );
		CHECK( sol.value().size() == 2 );
	}
}

// Relations between qlt variables are solved jointly: every model returned
// must satisfy each atom of its input, and only unsatisfiable systems may be
// declined.
TEST_SUITE("relational qlt ordering systems") {

	std::optional<solution<node_t>> solve_rel(const std::string& system) {
		tref form = get_nso_rr<node_t>(tau::get(system).value_or(nullptr)).value().main->get();
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(qlt_type<node_t>()),
			.mode = solver_mode::general
		};
		auto r = solve<node_t>(form, options);
		return r.has_value() ? std::optional<solution<node_t>>(r.value()) : std::nullopt;
	}

	// Substituting the model folds every ground atom; all must be T.
	bool satisfies(const std::string& system, const solution<node_t>& sol) {
		tref form = get_nso_rr<node_t>(tau::get(system).value_or(nullptr)).value().main->get();
		for (tref atom : get_cnf_wff_clauses<node_t>(form)) {
			tref v = tau::traverser(rewriter::replace<node_t>(atom, sol))
				| bf_reduce_canonical<node_t>() | tau::traverser::ref;
			if (!tau::get(v).equals_T()) return false;
		}
		return true;
	}

	bool solved_and_satisfied(const std::string& system) {
		auto sol = solve_rel(system);
		return sol.has_value() && satisfies(system, sol.value());
	}

	TEST_CASE("strict pair") {
		CHECK( solved_and_satisfied("x : qlt < y : qlt.") );
	}

	TEST_CASE("strict pair, reversed names") {
		CHECK( solved_and_satisfied("y : qlt < x : qlt.") );
		CHECK( solved_and_satisfied("x : qlt > y : qlt.") );
	}

	TEST_CASE("increasing and decreasing chains") {
		CHECK( solved_and_satisfied("x : qlt < y : qlt && y : qlt < z : qlt.") );
		CHECK( solved_and_satisfied("z : qlt < y : qlt && y : qlt < x : qlt.") );
		CHECK( solved_and_satisfied(
			"x2 : qlt < x1 : qlt && x1 : qlt < x3 : qlt && x2 : qlt < x3 : qlt.") );
	}

	TEST_CASE("two variables below one") {
		CHECK( solved_and_satisfied("x : qlt < y : qlt && z : qlt < y : qlt.") );
	}

	TEST_CASE("relations bounded by constants") {
		CHECK( solved_and_satisfied("x : qlt < y : qlt && y : qlt < {[0,1]}:qlt.") );
		CHECK( !solve_rel("x : qlt < y : qlt && y : qlt < {0}:qlt.").has_value() );
		CHECK( solved_and_satisfied("x : qlt < y : qlt && x : qlt > {3}:qlt.") );
		CHECK( solved_and_satisfied(
			"{0}:qlt < x : qlt && x : qlt < y : qlt && y : qlt < {[0,1]}:qlt.") );
	}

	TEST_CASE("disequalities") {
		CHECK( solved_and_satisfied("x : qlt < y : qlt && x : qlt != {0}:qlt.") );
		CHECK( solved_and_satisfied(
			"x : qlt <= y : qlt && x : qlt != y : qlt.") );
		CHECK( solved_and_satisfied(
			"{0}:qlt < x : qlt && x : qlt < {[0,1]}:qlt && x : qlt != {1/2}:qlt.") );
	}

	TEST_CASE("non-strict cycle forces equality") {
		CHECK( solved_and_satisfied(
			"x : qlt <= y : qlt && y : qlt <= x : qlt && x : qlt > {2}:qlt.") );
	}

	TEST_CASE("inconsistent systems are declined") {
		CHECK( !solve_rel("x : qlt < y : qlt && y : qlt < x : qlt.").has_value() );
		CHECK( !solve_rel("x : qlt < y : qlt && y : qlt <= x : qlt.").has_value() );
		CHECK( !solve_rel(
			"x : qlt <= y : qlt && y : qlt <= x : qlt && x : qlt != y : qlt.")
			.has_value() );
		CHECK( !solve_rel(
			"{1}:qlt < x : qlt && x : qlt < y : qlt && y : qlt < {1}:qlt.")
			.has_value() );
	}

	TEST_CASE("constant-bound controls") {
		CHECK( solved_and_satisfied("x : qlt > {1}:qlt.") );
		CHECK( solved_and_satisfied("x : qlt > {3/4}:qlt && x : qlt < {[3/4,7/8]}:qlt.") );
		CHECK( !solve_rel("x : qlt > {3/4}:qlt && x : qlt < {7/8}:qlt.").has_value() );
	}
}

// Closed formulas over qlt sets, decided by the cells of their constants.
TEST_SUITE("qlt closed decisions over sets") {

	tref wff(const char* src) {
		return get_nso_rr<node_t>(tau::get(src).value_or(nullptr))
			.value().main->get();
	}
	std::optional<bool> decide(const char* src) {
		return qlt_decide_closed<node_t>(wff(src));
	}
	// the variable node of `x : qlt`
	tref var_x() {
		return tau::get(tau::get(wff("x : qlt = y : qlt."))[0].first())
			.first();
	}

	TEST_CASE("an element exists with any nonempty, nonfull value") {
		CHECK( decide("ex x : qlt (x = {[0,1]}:qlt).") == true );
		CHECK( decide("ex x : qlt (x != 0 && x' != 0).") == true );
		CHECK( decide("ex x : qlt ex y : qlt (x & y' = 0 && x != y).")
			== true );
		CHECK( decide("ex x : qlt (x = 0).") == true );
		CHECK( decide("ex x : qlt (x = 1).") == true );
	}

	TEST_CASE("a point is an atom, an interval is not") {
		CHECK( decide("ex x : qlt ({3}:qlt & x != 0 && {3}:qlt & x' != 0).")
			== false );
		CHECK( decide("ex x : qlt ({3}:qlt & x != 0 && {5}:qlt & x != 0).")
			== true );
		CHECK( decide("ex x : qlt ({[0,1]}:qlt & x != 0 "
			"&& {[0,1]}:qlt & x' != 0).") == true );
		CHECK( decide("ex x : qlt ((({3}:qlt | {5}:qlt) & x) != 0 "
			"&& (({3}:qlt | {5}:qlt) & x') != 0).") == true );
		CHECK( decide("all x : qlt ({3}:qlt & x = 0 || {3}:qlt & x' = 0).")
			== true );
	}

	TEST_CASE("a region of n points splits into at most n parts") {
		// {3, 5} has two points: three variables cannot cut it into three
		// nonempty disjoint parts
		CHECK( decide("ex x : qlt ex y : qlt ex z : qlt ("
			"({3}:qlt|{5}:qlt) & x != 0 && ({3}:qlt|{5}:qlt) & y != 0 "
			"&& ({3}:qlt|{5}:qlt) & z != 0 && x & y = 0 && x & z = 0 "
			"&& y & z = 0).") == false );
		CHECK( decide("ex x : qlt ex y : qlt ex z : qlt ("
			"{(0,1)}:qlt & x != 0 && {(0,1)}:qlt & y != 0 "
			"&& {(0,1)}:qlt & z != 0 && x & y = 0 && x & z = 0 "
			"&& y & z = 0).") == true );
	}

	TEST_CASE("the order is inclusion") {
		CHECK( decide("ex x : qlt ({1}:qlt < x && x < {[0,3]}:qlt).")
			== true );
		CHECK( decide("ex x : qlt ({1}:qlt < x && x < {3}:qlt).")
			== false );
		CHECK( decide("all x : qlt (x <= 1).") == true );
		CHECK( decide("all x : qlt (x < 1).") == false );
		CHECK( decide("all x : qlt ex y : qlt (x < y).") == false );
		CHECK( decide("all x : qlt ex y : qlt (x <= y && x != y || x = 1).")
			== true );
	}

	TEST_CASE("alternating quantifiers") {
		// every set has a complement, and only the empty set is below all
		CHECK( decide("all x : qlt ex y : qlt (x & y = 0 && x | y = 1).")
			== true );
		CHECK( decide("ex x : qlt all y : qlt (x <= y).") == true );
		CHECK( decide("ex x : qlt all y : qlt (x < y).") == false );
		// an atom exists below every nonzero element
		CHECK( decide("all x : qlt (x = 0 || ex y : qlt (y <= x && y != 0 "
			"&& all z : qlt ((z <= y) -> (z = 0 || z = y)))).") == true );
	}

	TEST_CASE("a free variable declines") {
		CHECK( !decide("ex x : qlt (x < y : qlt).") );
	}

	TEST_CASE("omcat_qe decides the quantified body") {
		tref x = var_x();
		CHECK( qlt_omcat_qe<node_t>(x,
			wff("x : qlt != 0 && x : qlt' != 0.")) == true );
		CHECK( qlt_omcat_qe<node_t>(x,
			wff("{3}:qlt & x : qlt != 0 && {3}:qlt & x : qlt' != 0."))
			== false );
		// a body only excluding values holds whatever else is free
		CHECK( qlt_omcat_qe<node_t>(x,
			wff("x : qlt != a : qlt && x : qlt != b : qlt.")) == true );
		CHECK( !qlt_omcat_qe<node_t>(x, wff("x : qlt < a : qlt.")) );
	}

	TEST_CASE("x = 0 and x = 1 are ordinary equations") {
		CHECK( !tau::get(wff("x : qlt = 1.")).equals_F() );
		CHECK( !tau::get(wff("x : qlt = 0.")).equals_F() );
		CHECK( !tau::get(wff("x : qlt != 0.")).equals_T() );
	}
}
