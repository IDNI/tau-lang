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

// SO-14: the DLO ordering path (is_qlt_ordering_atom gate,
// solve_qlt_ordering_system, qlt_pick_witness) had zero direct coverage.
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
		CHECK( solved_and_satisfied("x : qlt < y : qlt && y : qlt < {0}:qlt.") );
		CHECK( solved_and_satisfied("x : qlt < y : qlt && x : qlt > {3}:qlt.") );
		CHECK( solved_and_satisfied(
			"{0}:qlt < x : qlt && x : qlt < y : qlt && y : qlt < {1}:qlt.") );
	}

	TEST_CASE("disequalities") {
		CHECK( solved_and_satisfied("x : qlt < y : qlt && x : qlt != {0}:qlt.") );
		CHECK( solved_and_satisfied(
			"x : qlt <= y : qlt && x : qlt != y : qlt.") );
		CHECK( solved_and_satisfied(
			"{0}:qlt < x : qlt && x : qlt < {1}:qlt && x : qlt != {1/2}:qlt.") );
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
		CHECK( solved_and_satisfied("x : qlt > {3/4}:qlt && x : qlt < {7/8}:qlt.") );
	}
}

// The joint ordering solver called directly on hand-built atoms, so each atom
// reaches it in exactly the shape written: negated, in the nXX spelling,
// oriented with > / >=, or against the typed 0 / 1 ends of the order. A
// returned model must satisfy every atom; a declined system returns nullopt.
TEST_SUITE("qlt joint ordering solver: atom shapes") {

	tref atom(const char* src) {
		return get_nso_rr<node_t>(tau::get(src).value_or(nullptr))
			.value().main->get();
	}
	tref lhs(tref a) { return tau::get(a)[0].first(); }
	tref rhs(tref a) { return tau::get(a)[0].second(); }
	tref neg(tref a) { return tau::build_wff_neg(a); }

	// the bf nodes of `x : qlt`, `y : qlt` and the typed 0 / 1 of qlt
	tref x() { return lhs(atom("x : qlt < y : qlt.")); }
	tref y() { return rhs(atom("x : qlt < y : qlt.")); }
	tref q0() { return tau::_0(ba_types<node_t>::id(qlt_type<node_t>())); }
	tref q1() { return tau::_1(ba_types<node_t>::id(qlt_type<node_t>())); }

	std::optional<solution<node_t>> dlo(const trefs& atoms) {
		inequality_system<node_t> sys;
		for (tref a : atoms) sys.insert(a);
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(qlt_type<node_t>()),
			.mode = solver_mode::general,
			.type_id = ba_types<node_t>::id(qlt_type<node_t>())
		};
		return qlt_omcat_solve_inequality_system<node_t>(sys, options);
	}

	bool solved_and_satisfied(const trefs& atoms) {
		auto sol = dlo(atoms);
		if (!sol) return false;
		for (tref a : atoms)
			if (!check_solution<node_t>(a, sol.value())) return false;
		return true;
	}

	TEST_CASE("negated comparisons flip over the total order") {
		// !(x < y) is y <= x, !(x <= y) is y < x
		CHECK( solved_and_satisfied({ neg(atom("x : qlt < y : qlt.")) }) );
		CHECK( solved_and_satisfied({ neg(atom("x : qlt <= y : qlt.")) }) );
		CHECK( solved_and_satisfied({ neg(atom("x : qlt = y : qlt.")) }) );
		CHECK( solved_and_satisfied({ neg(atom("x : qlt != y : qlt.")) }) );
		// !(x < y) && !(y <= x) has no model
		CHECK( !dlo({ neg(atom("x : qlt < y : qlt.")),
			neg(atom("y : qlt <= x : qlt.")) }).has_value() );
	}

	TEST_CASE("nXX spellings read as negations") {
		CHECK( solved_and_satisfied({ atom("x : qlt !< y : qlt.") }) );
		CHECK( solved_and_satisfied({ atom("x : qlt !> y : qlt.") }) );
		CHECK( solved_and_satisfied({ atom("x : qlt !<= y : qlt.") }) );
		CHECK( solved_and_satisfied({ atom("x : qlt !>= y : qlt.") }) );
		// x !< y && x !> y forces x = y, so x != y has no model
		CHECK( !dlo({ atom("x : qlt !< y : qlt."),
			atom("x : qlt !> y : qlt."),
			atom("x : qlt != y : qlt.") }).has_value() );
	}

	TEST_CASE("> and >= are oriented as < and <=") {
		CHECK( solved_and_satisfied({ atom("x : qlt > y : qlt.") }) );
		CHECK( solved_and_satisfied({ atom("x : qlt >= y : qlt."),
			atom("y : qlt > {2}:qlt.") }) );
		CHECK( !dlo({ atom("x : qlt > y : qlt."),
			atom("y : qlt >= x : qlt.") }).has_value() );
	}

	TEST_CASE("equality between variables merges them into one class") {
		tref eq = tau::build_bf_eq(x(), y());
		CHECK( solved_and_satisfied({ eq, atom("y : qlt > {1}:qlt.") }) );
		CHECK( !dlo({ eq, atom("x : qlt != y : qlt.") }).has_value() );
	}

	TEST_CASE("atoms against the typed 0 / 1 are decided for every point") {
		// Construction folds `x <= 1`, `0 <= x`, `x > 1` and `x < 0`
		// to T / F, so the non-strict and violated forms are reached
		// through negations, which it keeps.
		// holding ones add no constraint
		CHECK( solved_and_satisfied({ tau::build_bf_lt(x(), q1()),
			atom("x : qlt > {5}:qlt.") }) );
		CHECK( solved_and_satisfied({ tau::build_bf_lt(q0(), x()) }) );
		CHECK( solved_and_satisfied({ tau::build_bf_nlteq(x(), q0()) }) );
		CHECK( solved_and_satisfied({ tau::build_bf_ngteq(x(), q1()) }) );
		CHECK( solved_and_satisfied({ neg(tau::build_bf_lteq(q1(), x())) }) );
		CHECK( solved_and_satisfied({ tau::build_bf_neq(x(), q1()) }) );
		// violated ones decline: the value would be the typed end itself
		CHECK( !dlo({ tau::build_bf_nlt(x(), q1()) }).has_value() );
		CHECK( !dlo({ tau::build_bf_nlt(q0(), x()) }).has_value() );
		CHECK( !dlo({ neg(tau::build_bf_lt(x(), q1())) }).has_value() );
		CHECK( !dlo({ tau::build_bf_eq(x(), q1()) }).has_value() );
	}

	TEST_CASE("constants other than finite singletons decline") {
		CHECK( !dlo({ atom("x : qlt < {(0, 1)}:qlt.") }).has_value() );
		CHECK( !dlo({ atom("x : qlt < {(0, 1) | (2, 3)}:qlt.") })
			.has_value() );
		// a named endpoint is a singleton, but not a finite one
		CHECK( !dlo({ atom("x : qlt < {a}:qlt.") }).has_value() );
	}

	TEST_CASE("an atom outside the ordering fragment declines") {
		CHECK( !dlo({ atom("x : qlt < y : qlt || y : qlt < x : qlt.") })
			.has_value() );
	}

#ifdef TAU_PACK_HAS_BA_SBF
	TEST_CASE("an unread disequality over another algebra is skipped") {
		auto sol = dlo({ atom("x : qlt > {1}:qlt."),
			atom("{a}:sbf w != 0.") });
		REQUIRE( sol.has_value() );
		CHECK( check_solution<node_t>(atom("x : qlt > {1}:qlt."),
			sol.value()) );
	}
#endif
}

// Fourier-Motzkin elimination of an existential qlt variable bounded on both
// sides by other terms: every lower bound meets every upper bound, strictly
// iff either bound is strict. A body outside the fragment gives nullptr.
TEST_SUITE("qlt residual elimination") {

	tref wff(const char* src) {
		return get_nso_rr<node_t>(tau::get(src).value_or(nullptr))
			.value().main->get();
	}
	// the variable node of `x : qlt`
	tref var_x() {
		return tau::get(tau::get(wff("x : qlt < y : qlt."))[0].first())
			.first();
	}
	tref residual(const char* body) {
		return qlt_dlo_fm_residual<node_t>(var_x(), wff(body));
	}
	bool eliminates_to(const char* body, const char* expected) {
		tref r = residual(body);
		return r && tau::get(r) == tau::get(wff(expected));
	}

	TEST_CASE("bounds with the variable on the right are normalised") {
		CHECK( eliminates_to("b : qlt > x : qlt && a : qlt < x : qlt.",
			"a : qlt < b : qlt.") );
		CHECK( eliminates_to("a : qlt <= x : qlt && x : qlt < b : qlt.",
			"a : qlt < b : qlt.") );
		CHECK( eliminates_to("b : qlt >= x : qlt && a : qlt <= x : qlt.",
			"a : qlt <= b : qlt.") );
	}

	TEST_CASE("negated bounds are flipped") {
		CHECK( eliminates_to(
			"!(x : qlt <= a : qlt) && !(x : qlt >= b : qlt).",
			"a : qlt < b : qlt.") );
		CHECK( eliminates_to(
			"!(x : qlt > b : qlt) && !(x : qlt < a : qlt).",
			"a : qlt <= b : qlt.") );
	}

	TEST_CASE("nXX spellings are read as their positive forms") {
		CHECK( eliminates_to("x : qlt !< a : qlt && x : qlt !> b : qlt.",
			"a : qlt <= b : qlt.") );
		CHECK( eliminates_to(
			"x : qlt !<= a : qlt && x : qlt !>= b : qlt.",
			"a : qlt < b : qlt.") );
	}

	TEST_CASE("bodies outside the fragment are left alone") {
		// an equality needs no elimination, a disequality a case split
		CHECK( residual("a : qlt < x : qlt && x : qlt = b : qlt.")
			== nullptr );
		CHECK( residual("a : qlt < x : qlt && x : qlt != b : qlt.")
			== nullptr );
		// a conjunct without the variable
		CHECK( residual(
			"a : qlt < x : qlt && x : qlt < b : qlt && a : qlt < c : qlt.")
			== nullptr );
		// the variable on both sides
		CHECK( residual(
			"a : qlt < x : qlt && x : qlt < x : qlt & b : qlt.")
			== nullptr );
		// the variable inside a compound term
		CHECK( residual("a : qlt < x : qlt' && x : qlt < b : qlt.")
			== nullptr );
	}
}
