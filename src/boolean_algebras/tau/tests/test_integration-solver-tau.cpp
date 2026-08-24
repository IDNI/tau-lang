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

TEST_SUITE("solve_inequality_system") {

	bool test_solve_inequality_system(const std::vector<std::string>& inequalities) {
		return ::test_solve_inequality_system(inequalities,
			node_t::ba::splitter_one(tau_type<node_t>()));
	}

	bool test_unsat_inequality_system(const std::vector<std::string>& inequalities) {
		auto built = build_inequality_system(inequalities);
		if (!built.has_value()) return true; // already unsat before reaching the solver
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(tau_type<node_t>()),
			.mode = solver_mode::general,
			.type_id = get_ba_type_id<node_t>(tau_type<node_t>())
		};
		return !solve_inequality_system<node_t>(built.value(), options).has_value();
	}

	// Shaped like atomless2's step-1 system: two free variables, disequal to
	// constants and to each other. Must solve without enumerating the
	// odometer's candidate space; only completion/correctness is asserted.
	TEST_CASE("two free vars, disequal to T/F and to each other") {
		CHECK( test_solve_inequality_system({
			"o1[t]:tau != {T.}:tau.",
			"o1[t]:tau != {F.}:tau.",
			"o2[t]:tau != {T.}:tau.",
			"o2[t]:tau != {F.}:tau.",
			"o1[t]:tau != o2[t]:tau.",
		}) );
	}

	// Extends "one free var, two c1 targets nested in a competing c0" (below)
	// to three competing c1 targets, stressing atomless_choose_value's ladder
	// so later rows are more likely to reach the witness pool. Durable
	// contract only: solves, and substitution verifies.
	TEST_CASE("one free var, three c1 targets nested in a competing c0") {
		CHECK( test_solve_inequality_system({
			"(o1[t]:tau & {o5[t] = o6[t].}:tau) != {F.}:tau.",
			"(o1[t]:tau & {o7[t] = o8[t].}:tau) != {F.}:tau.",
			"(o1[t]:tau & {o9[t] = o10[t].}:tau) != {F.}:tau.",
			"(o1[t]:tau & {o5[t] = o6[t] || o7[t] = o8[t] "
				"|| o9[t] = o10[t].}:tau) "
				"!= {o5[t] = o6[t] || o7[t] = o8[t] "
				"|| o9[t] = o10[t].}:tau.",
		}) );
	}

	TEST_CASE("one free var, disequal to both T and F") {
		CHECK( test_solve_inequality_system({
			"o1[t]:tau != {T.}:tau.",
			"o1[t]:tau != {F.}:tau.",
		}) );
	}

	// Nested-critical shape atomless_choose_value's disjoint-representatives
	// construction must split for: two rows need x to hit distinct halves of
	// a third row's forbidden territory, so a naive union leaves no room
	// without splitting.
	TEST_CASE("one free var, two c1 targets nested in a competing c0") {
		CHECK( test_solve_inequality_system({
			"(o1[t]:tau & {o5[t] = o6[t].}:tau) != {F.}:tau.",
			"(o1[t]:tau & {o7[t] = o8[t].}:tau) != {F.}:tau.",
			"(o1[t]:tau & {o5[t] = o6[t] || o7[t] = o8[t].}:tau) "
				"!= {o5[t] = o6[t] || o7[t] = o8[t].}:tau.",
		}) );
	}

	TEST_CASE("unsat: one inequality is identically zero") {
		CHECK( test_unsat_inequality_system({
			"o1[t]:tau != {T.}:tau.",
			"o1[t]:tau != o1[t]:tau.",
		}) );
	}

	// Regression: solve_inequality_system_atomless's row extraction must
	// canonicalize each bf_neq atom to g != 0, g = l + r -- a caller isn't
	// guaranteed to hand it that shape (table_step_provider.tmpl.h's
	// ocltl_direct_decode_edge feeds verbatim-orientation atoms). Built
	// with tree builders, not a spec-text round-trip.
	tref tau_var_operand(const std::string& name) {
		using tau = tree<node_t>;
		tref eq = get_nso_rr<node_t>(
			tau::get(name + " != {T.}:tau.")).value().main->get();
		tref neq_node = tau::get(eq).first();
		return tau::get(neq_node).first();
	}

	bool check_atomless_solution(const inequality_system<node_t>& sys,
			const solution<node_t>& sol) {
		bool ok = true;
		for (tref eq : sys) ok = ok && check_solution<node_t>(eq, sol);
		return ok;
	}

	TEST_CASE("verbatim atom, zero on the left: not spuriously unsat") {
		using tau = tree<node_t>;
		size_t type_id = get_ba_type_id<node_t>(tau_type<node_t>());
		tref x = tau_var_operand("o1[t]:tau");
		tref a = tau_var_operand("o2[t]:tau");
		tref b = tau_var_operand("o3[t]:tau");
		inequality_system<node_t> sys;
		sys.insert(tau::build_bf_neq(tau::_0(type_id), x)); // 0 != x
		sys.insert(tau::build_bf_neq(x, a));                // x != o2[t]
		sys.insert(tau::build_bf_neq(x, b));                // x != o3[t]
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(tau_type<node_t>()),
			.mode = solver_mode::general,
			.type_id = type_id
		};
		auto solution = solve_inequality_system_atomless<node_t>(sys, options);
		REQUIRE( solution.has_value() );
		CHECK( check_atomless_solution(sys, solution.value()) );
	}

	TEST_CASE("verbatim atom, x != 1: witness actually satisfies it") {
		using tau = tree<node_t>;
		size_t type_id = get_ba_type_id<node_t>(tau_type<node_t>());
		tref x = tau_var_operand("o1[t]:tau");
		inequality_system<node_t> sys;
		sys.insert(tau::build_bf_neq(x, tau::_1(type_id))); // x != 1
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(tau_type<node_t>()),
			.mode = solver_mode::general,
			.type_id = type_id
		};
		auto solution = solve_inequality_system_atomless<node_t>(sys, options);
		REQUIRE( solution.has_value() );
		CHECK( check_atomless_solution(sys, solution.value()) );
	}

	TEST_CASE("verbatim atom, general two-sided l != r: witness actually satisfies it") {
		using tau = tree<node_t>;
		size_t type_id = get_ba_type_id<node_t>(tau_type<node_t>());
		tref x = tau_var_operand("o1[t]:tau");
		tref y = tau_var_operand("o2[t]:tau");
		inequality_system<node_t> sys;
		sys.insert(tau::build_bf_neq(x, tau::_1(type_id))); // x != 1
		sys.insert(tau::build_bf_neq(x, tau::_0(type_id))); // x != 0
		sys.insert(tau::build_bf_neq(y, tau::_1(type_id))); // y != 1
		sys.insert(tau::build_bf_neq(y, tau::_0(type_id))); // y != 0
		sys.insert(tau::build_bf_neq(x, y));                // x != y, both non-trivial
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(tau_type<node_t>()),
			.mode = solver_mode::general,
			.type_id = type_id
		};
		auto solution = solve_inequality_system_atomless<node_t>(sys, options);
		REQUIRE( solution.has_value() );
		CHECK( check_atomless_solution(sys, solution.value()) );
	}

	// Regression: every solved value must be a genuine, serializable BA
	// constant, never a bare free uconst leaked from atomless_choose_
	// value's witness pool. check_solution alone wouldn't catch this (it
	// tolerates a witness-carrying value); the interpreter instead hands
	// solutions straight to serialize_constant, which fails silently on a
	// bare uconst.
	TEST_CASE("solved values are serializable ba constants, not bare witnesses") {
		auto built = build_inequality_system({
			"o1[t]:tau != {T.}:tau.",
			"o1[t]:tau != {F.}:tau.",
			"o2[t]:tau != {T.}:tau.",
			"o2[t]:tau != {F.}:tau.",
			"o1[t]:tau != o2[t]:tau.",
		});
		REQUIRE( built.has_value() );
		const inequality_system<node_t>& system = built.value();
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(tau_type<node_t>()),
			.mode = solver_mode::general,
			.type_id = find_ba_type<node_t>(*system.begin())
		};
		auto solution = solve_inequality_system<node_t>(system, options);
		REQUIRE( solution.has_value() );
		CHECK( solution.value().size() == 2 );
		for (const auto& [var, value] : solution.value()) {
			std::stringstream ss;
			CHECK( serialize_constant<node_t>(ss, value,
				find_ba_type<node_t>(var)) );
		}
	}

	// Regression: solve_inequality_system_atomless's returned solution keys
	// must compare subtree_equals to the vars as they appear in the input
	// system, not merely by node identity -- table_step_provider.tmpl.h's
	// ocltl_direct_decode_edge relies on this.
	TEST_CASE("solve_inequality_system_atomless: solution keys match the "
	          "input system's own variable nodes") {
		using tau = tree<node_t>;
		size_t type_id = get_ba_type_id<node_t>(tau_type<node_t>());
		tref x = tau_var_operand("o1[t]:tau");
		inequality_system<node_t> sys;
		sys.insert(tau::build_bf_neq(x, tau::_1(type_id)));
		sys.insert(tau::build_bf_neq(x, tau::_0(type_id)));
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(tau_type<node_t>()),
			.mode = solver_mode::general,
			.type_id = type_id
		};
		auto solution = solve_inequality_system_atomless<node_t>(sys, options);
		REQUIRE( solution.has_value() );
		REQUIRE( solution.value().size() == 1 );
		tref key = solution.value().begin()->first;
		CHECK( tau::subtree_equals(key, x) );
	}

	// A typed zero on the LEFT of a raw bf_xor must reduce like any other
	// zero: 0 XOR x = x. Built with build_bf_xor directly, bypassing
	// operator^'s trivial-case shortcut, to exercise the reducer itself.
	TEST_CASE("reduction: typed zero XOR var reduces to var") {
		using tau = tree<node_t>;
		using tt = tau::traverser;
		size_t type_id = get_ba_type_id<node_t>(tau_type<node_t>());
		tref x = tau_var_operand("o1[t]:tau");
		tref zero = tau::_0(type_id);
		tref raw_xor = tau::build_bf_xor(zero, x);
		tref reduced = tt(raw_xor) | bf_reduce_canonical<node_t>() | tt::ref;
		tref expected = tt(x) | bf_reduce_canonical<node_t>() | tt::ref;
		CHECK( tau::get(reduced) == tau::get(expected) );
	}

	// Ledger contract: a witness excluding a value ledger_commit_witness
	// pinned is found and satisfies the whole system.
	TEST_CASE("ledger: fast path witness avoids a committed value") {
		using tau = tree<node_t>;
		size_t type_id = get_ba_type_id<node_t>(tau_type<node_t>());
		fresh_element_ledger ledger;
		tref committed = tau_var_operand("{o5[t] = o6[t].}:tau");
		ledger_commit_witness<node_t>(ledger, committed, type_id);
		tref x = tau_var_operand("o1[t]:tau");
		inequality_system<node_t> sys;
		sys.insert(tau::build_bf_neq(x, tau::_1(type_id)));
		sys.insert(tau::build_bf_neq(x, tau::_0(type_id)));
		sys.insert(tau::build_bf_neq(x, committed));
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(tau_type<node_t>()),
			.mode = solver_mode::general,
			.type_id = type_id,
			.ledger = &ledger
		};
		auto solution = solve_inequality_system_atomless<node_t>(sys, options);
		REQUIRE( solution.has_value() );
		CHECK( check_atomless_solution(sys, solution.value()) );
	}

	// Ledger contract: three successive calls sharing a ledger each exclude
	// every earlier call's witness, pinning cross-call distinctness and its
	// transitivity across the third call.
	TEST_CASE("ledger: cross-call distinctness pins transitivity across "
	          "three calls") {
		using tau = tree<node_t>;
		size_t type_id = get_ba_type_id<node_t>(tau_type<node_t>());
		fresh_element_ledger ledger;
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(tau_type<node_t>()),
			.mode = solver_mode::general,
			.type_id = type_id,
			.ledger = &ledger
		};

		tref x1 = tau_var_operand("o1[t]:tau");
		inequality_system<node_t> sys1;
		sys1.insert(tau::build_bf_neq(x1, tau::_1(type_id)));
		sys1.insert(tau::build_bf_neq(x1, tau::_0(type_id)));
		auto sol1 = solve_inequality_system_atomless<node_t>(sys1, options);
		REQUIRE( sol1.has_value() );
		REQUIRE( sol1->size() == 1 );
		tref w1 = sol1->begin()->second;
		CHECK( check_atomless_solution(sys1, sol1.value()) );
		ledger_commit_witness<node_t>(ledger, w1, type_id);

		tref x2 = tau_var_operand("o2[t]:tau");
		inequality_system<node_t> sys2;
		sys2.insert(tau::build_bf_neq(x2, tau::_1(type_id)));
		sys2.insert(tau::build_bf_neq(x2, tau::_0(type_id)));
		sys2.insert(tau::build_bf_neq(x2, w1));
		auto sol2 = solve_inequality_system_atomless<node_t>(sys2, options);
		REQUIRE( sol2.has_value() );
		tref w2 = sol2->begin()->second;
		CHECK( check_atomless_solution(sys2, sol2.value()) );
		ledger_commit_witness<node_t>(ledger, w2, type_id);

		tref x3 = tau_var_operand("o3[t]:tau");
		inequality_system<node_t> sys3;
		sys3.insert(tau::build_bf_neq(x3, tau::_1(type_id)));
		sys3.insert(tau::build_bf_neq(x3, tau::_0(type_id)));
		sys3.insert(tau::build_bf_neq(x3, w1));
		sys3.insert(tau::build_bf_neq(x3, w2));
		auto sol3 = solve_inequality_system_atomless<node_t>(sys3, options);
		REQUIRE( sol3.has_value() );
		CHECK( check_atomless_solution(sys3, sol3.value()) );
	}

	// External (non-ledger) constants are still verified, and excluding a
	// constant and its complement -- a genuine region collapse, a & ~a = 0
	// -- forces a fallback to the ladder rather than an unsound witness.
	TEST_CASE("ledger: external exclusions verified; region collapse falls "
	          "back") {
		using tau = tree<node_t>;
		using tt = tau::traverser;
		size_t type_id = get_ba_type_id<node_t>(tau_type<node_t>());
		fresh_element_ledger ledger;
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(tau_type<node_t>()),
			.mode = solver_mode::general,
			.type_id = type_id,
			.ledger = &ledger
		};

		tref a = tau_var_operand("{o5[t] = o6[t].}:tau");
		tref x = tau_var_operand("o1[t]:tau");
		inequality_system<node_t> sys;
		sys.insert(tau::build_bf_neq(x, tau::_1(type_id)));
		sys.insert(tau::build_bf_neq(x, tau::_0(type_id)));
		sys.insert(tau::build_bf_neq(x, a));
		auto sol = solve_inequality_system_atomless<node_t>(sys, options);
		REQUIRE( sol.has_value() );
		CHECK( check_atomless_solution(sys, sol.value()) );

		tref not_a = tt(~tau::get(a)) | bf_reduce_canonical<node_t>() | tt::ref;
		tref y = tau_var_operand("o2[t]:tau");
		inequality_system<node_t> sys2;
		sys2.insert(tau::build_bf_neq(y, tau::_1(type_id)));
		sys2.insert(tau::build_bf_neq(y, tau::_0(type_id)));
		sys2.insert(tau::build_bf_neq(y, a));
		sys2.insert(tau::build_bf_neq(y, not_a));
		auto sol2 = solve_inequality_system_atomless<node_t>(sys2, options);
		REQUIRE( sol2.has_value() );
		CHECK( check_atomless_solution(sys2, sol2.value()) );
	}

	// Non-exclusion-shaped rows decline the ledger fast path (not this row
	// shape) and still solve correctly via the disjoint-representatives
	// ladder, ledger set or not.
	TEST_CASE("ledger set but system not exclusion-shaped: ladder still "
	          "solves") {
		auto built = build_inequality_system({
			"(o1[t]:tau & {o5[t] = o6[t].}:tau) != {F.}:tau.",
			"(o1[t]:tau & {o7[t] = o8[t].}:tau) != {F.}:tau.",
			"(o1[t]:tau & {o5[t] = o6[t] || o7[t] = o8[t].}:tau) "
				"!= {o5[t] = o6[t] || o7[t] = o8[t].}:tau.",
		});
		REQUIRE( built.has_value() );
		const inequality_system<node_t>& system = built.value();
		fresh_element_ledger ledger;
		solver_options options = {
			.splitter_one = node_t::ba::splitter_one(tau_type<node_t>()),
			.mode = solver_mode::general,
			.type_id = find_ba_type<node_t>(*system.begin()),
			.ledger = &ledger
		};
		auto solution = solve_inequality_system_atomless<node_t>(system, options);
		REQUIRE( solution.has_value() );
		for (tref eq : system) CHECK( check_solution<node_t>(eq, solution.value()) );
	}
}
