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

	TEST_CASE("x : qlt != 0 && (x : qlt)' != 0.") {
		CHECK( test_solve_inequality_system({
			"x : qlt != 0.",
			"(x : qlt)' != 0."
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

	TEST_CASE("x : qlt != 0") {
		const char* system = "x : qlt != 0.";
		CHECK( test_solve(system) );
		CHECK( !test_solve_min(system) );
		CHECK( test_solve_max(system) );
	}

	TEST_CASE("x : qlt != 0 && x : qlt != 1") {
		const char* system = "x : qlt != 0 && x : qlt != 1.";
		CHECK( test_solve(system) );
		CHECK( !test_solve_min(system) );
		CHECK( !test_solve_max(system) );
	}

	TEST_CASE("{(0, 1)}:qlt x != 0 && {[1, 2)}:qlt y != 0") {
		const char* system = "{(0, 1)}:qlt x != 0 && {[1, 2)}:qlt y != 0.";
		CHECK( test_solve(system) );
	}

	TEST_CASE("unsatisfiable: x : qlt = 0 && x : qlt != 0") {
		const char* system = "x : qlt = 0 && x : qlt != 0.";
		CHECK( !test_solve(system) );
	}

	// Regression: normalization renders <= as a negated ordering atom
	// (bf_nlt/bf_nlteq). The omcat gate in solve() must still recognize
	// those as ordering atoms and route the system to qlt's own solver;
	// previously it fell through to the ABA solve_inequality_system, which
	// cannot handle ordering atoms and crashed.
	TEST_CASE("interval via <=: {0}:qlt <= x:qlt && x:qlt <= {1}:qlt") {
		const char* system = "{0}:qlt <= x:qlt && x:qlt <= {1}:qlt.";
		CHECK( test_solve(system) );
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
		tref form = get_nso_rr<node_t>(tau::get(system)).value().main->get();
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
			"x : qlt != {0}:qlt && x : qlt > {3/4}:qlt."))
			.value().main->get();
		tref subst = rewriter::replace<node_t>(atoms, sol.value());
		CHECK( !tau::get(subst).find_top(is<node_t, tau::wff_f>) );
	}
}

// SO-14: the DLO ordering path (is_qlt_ordering_atom gate,
// solve_qlt_ordering_system, qlt_pick_witness) had zero direct coverage.
TEST_SUITE("SO-14 qlt ordering systems") {

	std::optional<solution<node_t>> solve_ord(const std::string& system) {
		tref form = get_nso_rr<node_t>(tau::get(system)).value().main->get();
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(qlt_type<node_t>()),
			.mode = solver_mode::general
		};
		auto r = solve<node_t>(form, options);
		return r.has_value() ? std::optional<solution<node_t>>(r.value()) : std::nullopt;
	}

	TEST_CASE("bounded interval yields a witness inside it") {
		auto sol = solve_ord(
			"x : qlt > {3/4}:qlt && x : qlt < {7/8}:qlt.");
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
