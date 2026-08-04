// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "normal_forms.h"
#include "normalizer.h"

TEST_SUITE("TreatExQuantifiedClause") {
	TEST_CASE("surviving inner quantifier blocks elimination (B5)") {
		// The inner ex y survived elimination; its equations are not
		// top-level conjuncts of the outer clause, so the eliminator
		// must keep the whole quantified clause instead of squeezing
		// them and silently dropping the inner binder (leaking y free)
		const char* sample =
			"ex x (xw = 0 && (ex y (xy = 0 && f(y) != 0))).";
		tref fm = get_nso_rr(sample).value().main->get();
		bool quant_eliminated = true;
		tref res = treat_ex_quantified_clause<node_t>(fm, quant_eliminated);
		CHECK( !quant_eliminated );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) != nullptr );
	}

	TEST_CASE("plain clause still eliminated (B5 control)") {
		const char* sample = "ex x (xw = 0 && xz != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		bool quant_eliminated = true;
		tref res = treat_ex_quantified_clause<node_t>(fm, quant_eliminated);
		CHECK( quant_eliminated );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
		CHECK( res != fm );
	}
}

TEST_SUITE("AntiPrenexBlock") {
	// Helper: peel the leading ex-quantifier prefix into a block,
	// innermost variable gets the lowest order index
	static std::tuple<tref, trefs, term_handle<node_t>::order> peel_block(
		tref fm)
	{
		trefs block;
		term_handle<node_t>::order order;
		while (tau::get(fm)[0].is(tau::wff_ex)) {
			block.push_back(tau::get(fm)[0].first());
			fm = tau::get(fm)[0].second();
		}
		for (size_t i = 0; i < block.size(); ++i)
			order.emplace(block[i], block.size() - 1 - i);
		return {fm, block, order};
	}

	// Helper: run anti_prenex_block on a parsed "ex ... (...)" sample
	static std::pair<tref, size_t> run_apb(const char* sample) {
		tref fm = get_nso_rr(sample).value().main->get();
		auto [body, block, order] = peel_block(fm);
		subtree_unordered_set<node_t> used_atms;
		subtree_unordered_map<node_t, int_t> quant_pattern;
		for (size_t i = 0; i < block.size(); ++i)
			quant_pattern.emplace(block[i], i + 1);
		tref res = anti_prenex_block<node_t>(body, block,
			used_atms, quant_pattern, order, is_tref_bv_type_family<node_t>);
		return {res, used_atms.size()};
	}

	// Helper: like run_apb, but normalizes the matrix first, so negative
	// atoms reach the core as !(f = 0) rather than bf_neq -- the form the
	// production pipeline always produces (resolve_ex_block calls
	// normalize_atomic_formula_operators before the core).
	static std::pair<tref, size_t> run_apb_norm(const char* sample) {
		tref fm = normalize_atomic_formula_operators<node_t>(
			get_nso_rr(sample).value().main->get());
		auto [body, block, order] = peel_block(fm);
		subtree_unordered_set<node_t> used_atms;
		subtree_unordered_map<node_t, int_t> quant_pattern;
		for (size_t i = 0; i < block.size(); ++i)
			quant_pattern.emplace(block[i], i + 1);
		tref res = anti_prenex_block<node_t>(body, block,
			used_atms, quant_pattern, order,
			is_tref_bv_type_family<node_t>);
		return {res, used_atms.size()};
	}

	TEST_CASE("disjunction push (B11)") {
		// The disjunction-case recursions must compile and push the
		// block into each disjunct independently
		// Every atom positive, so chapter 5 step 2b squeezes and
		// distributes: ex x (xy = 0) || ex x (xw = 0), each resolved by
		// Schroeder to T (x := 0 zeroes either term). Before 2b the
		// disjunction case pushed the block into each disjunct and the
		// terminal fallback re-wrapped it unresolved, leaving
		// "(ex b1 b1 y = 0) || (ex b1 b1 w = 0)".
		auto [res, used] = run_apb("ex x (xy = 0 || xw = 0).");
		CHECK( used == 0 );
		CHECK( tau::get(res)[0].is(tau::wff_t) );
	}

	TEST_CASE("decomposition keeps independent conjuncts (B12/B13)") {
		// z = 0 must survive (B13) and the negative branch must be
		// built from formula[atm:=F], so the remaining equation gets
		// resolved by the clause eliminator (B12).
		//
		// Mixed-sign on purpose: the all-positive form this case used
		// to carry ("ex x (z = 0 && (xy = 0 || xw = 0))") is now
		// settled by chapter 5 step 2b before any decomposition runs,
		// and collapses to plain "z = 0" -- correct, but it no longer
		// exercises what this case was written for. Adding the negated
		// atom keeps it on the Boole-decomposition path.
		auto [res, used] = run_apb_norm(
			"ex x (z = 0 && (xy = 0 || xw = 0) && xk != 0).");
		CHECK( used == 0 );
		// B13: the independent conjunct survives untouched.
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_or>)
			!= nullptr );
		CHECK( tau::get(res).to_str().find("z = 0")
			!= std::string::npos );
		// B12: the block is fully resolved, nothing re-wrapped.
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}

	TEST_CASE("all-positive form of B12/B13 now collapses via step 2b") {
		// Companion to the case above, without the negated atom. The
		// independent conjunct z = 0 is separated out first, so step 2b
		// sees only (xy = 0 || xw = 0), squeezes it to two disjuncts,
		// and both resolve to T -- leaving just z = 0.
		auto [res, used] = run_apb("ex x (z = 0 && (xy = 0 || xw = 0)).");
		CHECK( used == 0 );
		CHECK( tau::get(res).to_str() == "z = 0" );
	}

	TEST_CASE("no decomposable atoms keeps block (B14)") {
		// The dependent part contains only bf_neq atoms (run_apb does
		// not normalize, so step 2a's profile counts them as "other"
		// and declines): the decomposition cannot use them, and the
		// block must be kept rather than dereferencing end().
		//
		// Since chapter 5 step 2d the block is kept *per disjunct*
		// rather than wrapped around the whole disjunction -- the
		// distribution ex X (A || B) == ex X A || ex X B, pushed one
		// level further in. Previously:
		//   "z = 0 && (ex b1 b1 y != 0 || b1 w != 0)"
		auto [res, used] = run_apb("ex x (z = 0 && (xy != 0 || xw != 0)).");
		CHECK( matches_to_str_to_any_of(res, {
			"z = 0 && ((ex b1 b1 w != 0) || (ex b1 b1 y != 0))",
			"z = 0 && ((ex b1 b1 y != 0) || (ex b1 b1 w != 0))",
		}) );
		CHECK( used == 0 );
	}

	TEST_CASE("T branch shortcut restores used_atms (B15)") {
		// The positive decomposition branch resolves to T (the clause
		// ex x (atm && xk = 0) is satisfiable by x := 0), triggering
		// the early return, which must leave used_atms balanced
		auto [res, used] = run_apb(
			"ex x (z = 0 && (xy = 0 || xw = 0) && xk = 0).");
		CHECK( tau::get(res).to_str() == "z = 0" );
		CHECK( used == 0 );
	}

	TEST_CASE("fallback: plain bf_neq atom re-wraps block (B16)") {
		// body is a single bf_neq atom (neither wff_or nor wff_and at
		// the top level): the catch-all fallback re-wraps the quantifier
		// block around the atom and returns it unchanged
		auto [res, used] = run_apb("ex x (xy != 0).");
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) != nullptr );
		CHECK( used == 0 );
	}

	TEST_CASE("Boole branches are path-simplified (assumption 2)") {
		// The dependent part carries a contradiction between two atoms
		// that are NOT the decomposition pivot: !(xz = 0) rules out the
		// xz = 0 disjunct of (xz = 0 || xk = 0). The node hooks cannot
		// see it -- they only fold constants and direct `$X && !$X`
		// siblings -- so without the path-simplification pass the
		// branches carry it into the recursion and the decomposition
		// splits on atoms that are already dead, emitting degenerate
		// terms (z'w'z, z'y'z -- both identically 0) in the result.
		//
		// Measured: 8 bf_eq atoms without the pass, 3 with it.
		auto [res, used] = run_apb(
			"ex x ((xy = 0 || xw = 0) && !(xz = 0) "
			"&& (xz = 0 || xk = 0)).");
		CHECK( used == 0 );
		CHECK( tau::get(res).select_all(is<node_t, tau::bf_eq>).size()
			== 3 );
	}

	TEST_CASE("paper 2a: all-negated formula distributes over every connective") {
		// Every atom negated -> Corollary 5.1 with J1 empty: the block
		// distributes over the disjunction (and would over a
		// conjunction) in one step, each atom becoming ex x (g != 0),
		// which resolve_quantifiers2 turns into the functional form.
		// Without 2a each disjunct is a lone negated atom that the
		// has_active_var fallback simply re-wraps, so a quantifier
		// survives.
		auto [res, used] = run_apb_norm("ex x (xy != 0 || xw != 0).");
		CHECK( used == 0 );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
		CHECK( matches_to_str_to_any_of(res, {
			"!y = 0 || !w = 0",
			"!w = 0 || !y = 0",
		}) );
	}

	TEST_CASE("paper 2a: distributes over a conjunction too") {
		// The half no other path reaches: ex X (A && B) == ex X A && ex X B
		// for negated atoms. Without 2a this is a pure clause and goes
		// through eliminate_block_over_clause, which reaches the same
		// answer; the point here is that 2a gets it in one step and
		// without consuming any decomposition atom.
		auto [res, used] = run_apb_norm("ex x (xy != 0 && xw != 0).");
		CHECK( used == 0 );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}

	TEST_CASE("paper 2a: does not fire on a mixed-sign formula") {
		// One positive atom present -> 2a must not fire; the existing
		// pipeline handles it and still eliminates x.
		auto [res, used] = run_apb_norm("ex x (xy = 0 && xw != 0).");
		CHECK( used == 0 );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}

	TEST_CASE("paper 2b: positives are squeezed across a disjunction") {
		// ex x ((xy = 0 || xw = 0) && xz = 0)
		//   squeeze (distributing && over || on terms via
		//   f1=0 && f2=0 == f1|f2=0):  (xy|xz = 0) || (xw|xz = 0)
		//   distribute the block, Schroeder each: both are satisfiable
		//   at x := 0, so the whole thing is T.
		// (used_atms is a balance check only -- it is inserted and
		// erased on every path, so it never witnesses which path ran.)
		auto [res, used] = run_apb_norm(
			"ex x ((xy = 0 || xw = 0) && xz = 0).");
		CHECK( used == 0 );
		CHECK( tau::get(res)[0].is(tau::wff_t) );
	}

	TEST_CASE("paper 2b: squeeze keeps a genuine constraint") {
		// z = 0 has no x, so the squeeze couples it into both disjuncts
		// and it survives elimination.
		auto [res, used] = run_apb_norm(
			"ex x ((xy = 0 || w = 0) && z = 0).");
		CHECK( used == 0 );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}

	TEST_CASE("paper 2b: does not fire when a negated atom is present") {
		auto [res, used] = run_apb_norm(
			"ex x ((xy = 0 || xw = 0) && xz != 0).");
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}

	TEST_CASE("paper 2e: a block-variable-free atom is never the pivot") {
		// v = 0 mentions no block variable, so splitting on it makes no
		// progress toward eliminating x. With the 2e filter the
		// decomposition must pick an atom that actually contains x.
		auto [res, used] = run_apb_norm(
			"ex x ((xy = 0 || v = 0) && xk != 0).");
		CHECK( used == 0 );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}

	TEST_CASE("paper 2e: an equation is preferred over an order atom") {
		// Both x < z and xy = 0 contain the block variable and rank
		// equally under atm_formula_order_for_quant_elim, which orders
		// by variable priority only. But only a bf_eq can ever let
		// eliminate_block_over_clause remove the block: splitting on the
		// bf_lt sends both branches to the unrecognized-conjunct path,
		// which re-wraps, so the work is wasted and the result carries
		// the debris.
		//
		// Measured: 7 bf_eq atoms when the order atom is picked,
		// 4 when the equation is.
		auto [res, used] = run_apb_norm(
			"ex x ((x < z || xy = 0) && xk != 0).");
		CHECK( used == 0 );
		CHECK( tau::get(res).select_all(is<node_t, tau::bf_eq>).size()
			== 4 );
	}

	TEST_CASE("gamma2 folds the pivot instead of splitting") {
		// (x|y)x'y' = 0 holds for every x, so step 2h folds it to T
		// rather than branching. The dependent part then vanishes and
		// only the independent conjunct is left.
		auto [res, used] = run_apb_norm(
			"ex x (((x|y)x'y' = 0 || xk = 0) && z = 0).");
		CHECK( used == 0 );
		CHECK( tau::get(res).to_str() == "z = 0" );
	}

	TEST_CASE("2b: an unsatisfiable squeeze collapses to F") {
		// The squeeze of an all-positive part can come out as the empty
		// disjunction, which is F. Reached here because the conjunction
		// forces x to be both 0 and 1.
		auto [res, used] = run_apb_norm("ex x (x = 0 && x' = 0).");
		CHECK( used == 0 );
		CHECK( tau::get(res)[0].is(tau::wff_f) );
	}

	TEST_CASE("paper 2e: an order atom is used when no equation qualifies") {
		// Every bf_eq present is either negated or block-variable-free,
		// so eq_atms comes out empty and the order atom is the only
		// candidate left. It must be used rather than the whole
		// decomposition giving up.
		auto [res, used] = run_apb_norm(
			"ex x ((!(x = y) || v = 0) && !(x < w)).");
		CHECK( used == 0 );
		CHECK( res != nullptr );
	}

	TEST_CASE("gamma1 declines on a multi-variable block") {
		// The unique-zero substitution is restricted to blocks whose
		// only active variable is the pivot; with two active variables
		// it must fall through to the general decomposition and still
		// resolve correctly.
		auto [res, used] = run_apb_norm(
			"ex x ex y ((x ^ y = 0 || z = 0) && xw != 0).");
		CHECK( used == 0 );
		CHECK( res != nullptr );
	}

	TEST_CASE("budget: the split cap re-wraps instead of recursing") {
		// 30 conjunct pairs. Under a *depth* bound this shape is
		// unbounded work -- the recursion branches in two, so bounding
		// the path length to d still admits 2^d leaves, and this input
		// ran for minutes. block_boole_max_splits counts total splits
		// instead, so the graceful re-wrap path is reached and the call
		// returns promptly.
		std::string sample = "ex x (";
		for (size_t i = 0; i < 30; ++i) {
			if (i) sample += " && ";
			sample += "(x a" + std::to_string(i) + " = 0 || x b"
				+ std::to_string(i) + " != 0)";
		}
		sample += ").";
		auto parsed = get_nso_rr(sample.c_str());
		REQUIRE( parsed.has_value() );
		auto [res, used] = run_apb_norm(sample.c_str());
		CHECK( used == 0 );
		CHECK( res != nullptr );
	}

	TEST_CASE("budget: a deeply decomposable formula still terminates") {
		// Eight independent disjunctive pairs under one block. Without
		// the fast paths this is 2^8 branches; the budget makes it
		// impossible for any input to recurse unboundedly. The only
		// thing asserted is that it returns.
		auto [res, used] = run_apb_norm(
			"ex x ((xa = 0 || xb != 0) && (xc = 0 || xd != 0) && "
			"(xe = 0 || xf != 0) && (xg = 0 || xh != 0)).");
		CHECK( used == 0 );
		CHECK( res != nullptr );
	}

	TEST_CASE("paper 2d: a disjunctive dependent part is re-dispatched") {
		// After z = 0 is scoped out the dependent part is a bare
		// disjunction, whose two disjuncts are each a single clause that
		// the clause eliminator resolves. Before the re-dispatch this
		// fell through to Boole decomposition instead.
		auto [res, used] = run_apb_norm(
			"ex x (z = 0 && (xy != 0 || (xw = 0 && xk != 0))).");
		CHECK( used == 0 );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}

	TEST_CASE("gamma1: unique zero substitutes the witness") {
		// The pivot x ^ y = 0 has exactly one zero, x := y. The
		// T-branch is therefore the body with x substituted away, and
		// since x is the block's only active variable that branch comes
		// out quantifier-free without any Schroeder step.
		// (The classifier itself is covered by the BooleAtomAnalysis
		// suite; this locks in the wiring. On this input the general
		// gamma5 path reaches an equivalent result of the same size --
		// gamma1's gain is skipping the Schroeder elimination on the
		// T-branch, not a smaller answer.)
		auto [res, used] = run_apb_norm(
			"ex x ((x ^ y = 0 || z = 0) && xw != 0).");
		CHECK( used == 0 );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
		CHECK( tau::get(res).to_str() == "!yw = 0 || z = 0 && !w = 0" );
	}

	TEST_CASE("gamma3 folds the pivot to F instead of splitting") {
		// (x|y')|(x'|y) = 0 holds for no x, so the disjunct dies and
		// only xk = 0 can satisfy the left conjunct.
		auto [res, used] = run_apb_norm(
			"ex x (((x|y')|(x'|y) = 0 || xk = 0) && z = 0).");
		CHECK( used == 0 );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
		CHECK( tau::get(res).to_str() == "z = 0" );
	}

	TEST_CASE("paper 2e: a negated atom is never the pivot") {
		// The pivot must come from a non-negated position. Only xw = 0
		// qualifies; the equation inside !(xy = 0) must not be reached
		// by the candidate scan.
		auto [res, used] = run_apb_norm(
			"ex x ((xw = 0 || v = 0) && xy != 0).");
		CHECK( used == 0 );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}
}

TEST_SUITE("AntiPrenexBlock0Arg") {
	// Tests for the zero-arg anti_prenex_block (the full pipeline:
	// NNF+simplify → subs_elim → normalize_atomic → process_block post-order).

	static tref run_apb0(const char* sample) {
		return anti_prenex_block<node_t>(
			get_nso_rr(sample).value().main->get());
	}

	TEST_CASE("quantifier-free formula is returned unchanged") {
		// Short-circuit: no quantifiers → original tref returned as-is.
		tref fm = get_nso_rr("xy = 0 && wz = 0.").value().main->get();
		CHECK( anti_prenex_block<node_t>(fm) == fm );
	}

	TEST_CASE("subs_elim: ex x (xy=0 && x=w) → wy=0") {
		// Step 2 (subs_elim): ex x (x=w && xy=0) → (xy=0)[x:=w] = wy=0
		tref res = run_apb0("ex x (xy = 0 && x = w).");
		CHECK( matches_to_str_to_any_of(res, {"wy = 0", "yw = 0"}) );
	}

	TEST_CASE("subs_elim: ex x (x=w) → T") {
		// ex x (x=w): body after subs_elim is empty (T), since the only
		// conjunct was the substitution witness x=w itself.
		tref res = run_apb0("ex x (x = w).");
		CHECK( tau::get(res)[0].is(tau::wff_t) );
	}

	TEST_CASE("all-block dualization: all x (x=0 || x!=0) → T") {
		// ∀x. (x=0 ∨ x≠0): law of excluded middle in BA → T.
		tref res = run_apb0("all x (x = 0 || x != 0).");
		CHECK( tau::get(res)[0].is(tau::wff_t) );
	}

	TEST_CASE("all-block dualization: all x (xy!=0) → F") {
		// ∀x. xy≠0: pick x=0 → 0·y=0=0, contradiction → F.
		tref res = run_apb0("all x xy != 0.");
		CHECK( tau::get(res)[0].is(tau::wff_f) );
	}

	TEST_CASE("ex block conjunction decomposition") {
		// ∃x. (xy=0 ∧ wz=0): wz=0 is independent of x → factor out;
		// ∃x. xy=0 → T (pick x=0).  Result: T ∧ wz=0 = wz=0.
		tref res = run_apb0("ex x (xy = 0 && wz = 0).");
		CHECK( matches_to_str_to_any_of(res, {"wz = 0", "zw = 0"}) );
	}

	TEST_CASE("trivial_skolem wiring: ex x (x=w || z=0) resolves via the block hook") {
		// x's only occurrence is `x=w`, reachable under wff_or. subs_elim
		// (Step 2) bails on any wff_or in scope, so only the trivial_skolem
		// wiring in process_quantifier_block can remove x here.
		// ex x.(x=w || z=0) is a tautology (x:=w always witnesses the left
		// disjunct), so it folds all the way to T.
		tref res = run_apb0("ex x (x = w || z = 0).");
		CHECK( tau::get(res)[0].is(tau::wff_t) );
	}

	TEST_CASE("trivial_skolem wiring: mixed block falls back safely when not fully eliminable") {
		// x occurs once, in `x=c` under an or; y occurs twice (yz=0 and
		// yw=0), so trivial_skolem_ex keeps y (occurrence count != 1).
		// Since not every block variable is eliminated, the wiring in
		// process_quantifier_block discards trivial_skolem_ex's result
		// (it only adopts full-block eliminations) and falls through to
		// the existing Boole-decomposition pipeline, which must still
		// resolve the whole block correctly on its own.
		tref res = run_apb0("ex x ex y ((x = c || z = 0) && (yz = 0 || yw = 0)).");
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}

	TEST_CASE("nested alternation: every block is processed") {
		// The inner ex-block is innermost; the enclosing all-block must
		// be processed too, and no quantifier may survive.
		tref res = run_apb0("all a ex b (ab = 0 && bc != 0).");
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}

	TEST_CASE("no quantifier survives a constant scope") {
		// ex x ((x|y)x'y' != 0) is F for every y, so `all y` ends up
		// wrapped around a bare constant. Nothing downstream folds a
		// binder, so the driver must.
		tref res = run_apb0("all y ex x ((x|y)x'y' != 0).");
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}
}

TEST_SUITE("BlockAtomProfile") {
	// Chapter 5's steps 2a and 2b are guarded by whole-formula sign
	// predicates; this is the census that answers them.

	static block_atom_profile<node_t> profile(const char* sample) {
		tref fm = get_nso_rr(sample).value().main->get();
		// peel any quantifier prefix; we profile the matrix
		while (is_child_quantifier<node_t>(fm))
			fm = tau::get(fm)[0].second();
		fm = normalize_atomic_formula_operators<node_t>(fm);
		return profile_block_atoms<node_t>(fm,
			is_tref_bv_type_family<node_t>);
	}

	TEST_CASE("all negated") {
		auto p = profile("ex x (xy != 0 && (xw != 0 || xz != 0)).");
		CHECK( p.positives == 0 );
		CHECK( p.negatives == 3 );
		CHECK( p.others == 0 );
		CHECK( p.all_negated() );
		CHECK( !p.all_positive() );
	}

	TEST_CASE("all positive") {
		auto p = profile("ex x ((xy = 0 || xw = 0) && xz = 0).");
		CHECK( p.positives == 3 );
		CHECK( p.negatives == 0 );
		CHECK( p.others == 0 );
		CHECK( p.all_positive() );
		CHECK( !p.all_negated() );
	}

	TEST_CASE("mixed is neither") {
		auto p = profile("ex x (xy = 0 && xw != 0).");
		CHECK( p.positives == 1 );
		CHECK( p.negatives == 1 );
		CHECK( !p.all_positive() );
		CHECK( !p.all_negated() );
	}

	TEST_CASE("constants are not atoms") {
		auto p = profile("ex x (xy != 0 && T).");
		CHECK( p.negatives == 1 );
		CHECK( p.others == 0 );
		CHECK( p.all_negated() );
	}

	TEST_CASE("F is not an atom either") {
		auto p = profile("ex x (xy != 0 && (xw != 0 || F)).");
		CHECK( p.negatives == 2 );
		CHECK( p.positives == 0 );
		CHECK( p.others == 0 );
		CHECK( p.all_negated() );
	}

	TEST_CASE("an order relation normalizes into equations") {
		// normalize_atomic_formula_operators rewrites `x < w` into
		// `xw' = 0 && !(x = w)`, so a bare order relation never reaches
		// the census as an unhandled atom -- it arrives as one positive
		// and one negative. Both fast paths must still decline, on
		// mixed sign rather than on `others`.
		auto p = profile("ex x (xy = 0 && x < w).");
		CHECK( p.positives == 2 );
		CHECK( p.negatives == 1 );
		CHECK( p.others == 0 );
		CHECK( !p.all_positive() );
		CHECK( !p.all_negated() );
	}

	TEST_CASE("a negated order relation counts as other") {
		// A negated order relation does survive normalization as a
		// non-equation atom, which is the one shape that reaches the
		// `others` counter in ordinary input. Only !(f = 0) is a
		// "negative", so 2a must decline here -- distributing a
		// relation it has no rule for would be unjustified.
		auto p = profile("ex x (xy != 0 && !(x < w)).");
		CHECK( p.negatives == 1 );
		CHECK( p.others == 1 );
		CHECK( !p.all_negated() );
		CHECK( !p.all_positive() );
	}

	TEST_CASE("skip-matched content makes both fast paths decline") {
		// A bitvector atom is reserved for predicate blasting / the
		// solver. Squeezing it into a bf_or, or handing it its own
		// binder, is exactly the work skip exists to prevent, so the
		// census reports skip_content and both guards go false even
		// though the signs are uniform.
		// A single positive bv atom: without the skip_content guard the
		// census would read as all_positive and step 2b would squeeze it.
		auto p = profile("ex x:bv[8] (x = 0).");
		CHECK( p.positives == 1 );
		CHECK( p.skip_content );
		CHECK( !p.all_positive() );
		CHECK( !p.all_negated() );
	}

	TEST_CASE("a formula with no atoms at all is neither") {
		// Guards the > 0 conditions in all_negated()/all_positive():
		// zero atoms must not read as "all of them are negated".
		auto p = profile("ex x (T).");
		CHECK( p.positives == 0 );
		CHECK( p.negatives == 0 );
		CHECK( p.others == 0 );
		CHECK( !p.all_negated() );
		CHECK( !p.all_positive() );
	}

	TEST_CASE("a nested quantifier counts as other, and is not descended into") {
		auto p = profile("ex x (xy != 0 && (ex z (xz = 0))).");
		CHECK( p.negatives == 1 );
		CHECK( p.others == 1 );
		// the inner xz = 0 must NOT have been counted as a positive
		CHECK( p.positives == 0 );
		CHECK( !p.all_negated() );
	}
}

TEST_SUITE("BlockSkipPaths") {
	// The skip-matched (bitvector) paths through block collection and the
	// chapter 5 fast paths.

	static quantifier_block<node_t> collect(const char* sample) {
		tref fm = get_nso_rr(sample).value().main->get();
		return collect_quantifier_block<node_t>(fm,
			is_tref_bv_type_family<node_t>);
	}

	TEST_CASE("an opposite-kind skipped quantifier ends the run") {
		// `ex a all x:bv[8] phi`. x is skip-matched, so this pass never
		// eliminates it -- but wrap_skipped re-emits everything in
		// blk.skipped *outermost*, so absorbing the `all` here would
		// hoist it out past the `ex` and produce all x ex a phi, which
		// is not equivalent (only the converse implication holds).
		// The run must therefore stop at it.
		auto blk = collect("ex a all x:bv[8] (a = 0 || x = 0).");
		CHECK( blk.is_ex );
		CHECK( blk.vars.size() == 1 );
		CHECK( blk.skipped.empty() );
		// The `all x` is left at the head of the matrix, where a later
		// round picks it up as an inner block.
		CHECK( is_child_quantifier<node_t>(blk.body) );
	}

	TEST_CASE("a leading skipped quantifier is absorbed and does not fix the kind") {
		// `all x:bv[8] ex a phi`. The skipped quantifier comes before
		// any active one, so it stays outermost either way and is safe
		// to defer. The block's kind must come from the first *active*
		// quantifier (ex), not from the leading skipped `all`.
		auto blk = collect("all x:bv[8] ex a (a = 0 || x = 0).");
		CHECK( blk.skipped.size() == 1 );
		CHECK( blk.skipped.front().second == false );   // it was an `all`
		CHECK( blk.is_ex );                             // kind from `ex a`
		CHECK( blk.vars.size() == 1 );
	}

	TEST_CASE("a same-kind skipped quantifier is transparent") {
		// `ex a ex x:bv[8] phi`: same kind, so it commutes with the
		// block and is deferred without ending the run.
		auto blk = collect("ex a ex x:bv[8] (a = 0 && x = 0).");
		CHECK( blk.is_ex );
		CHECK( blk.vars.size() == 1 );
		CHECK( blk.skipped.size() == 1 );
		CHECK( blk.skipped.front().second == true );    // it was an `ex`
	}

	TEST_CASE("a run of only skipped quantifiers has no active variable") {
		// Nothing for this pass to eliminate: vars is empty and the
		// whole run is deferred.
		auto blk = collect("ex x:bv[8] (x = 0).");
		CHECK( blk.vars.empty() );
		CHECK( blk.skipped.size() == 1 );
	}
}

TEST_SUITE("BlockSqueeze") {
	// Direct tests of the two chapter 5 fast-path helpers, so the constant,
	// cap and decline branches can be reached without having to find a
	// formula that drives the whole pipeline into them.

	// The matrix of a quantified sample, operator-normalized.
	static tref body_of(const char* sample) {
		tref fm = get_nso_rr(sample).value().main->get();
		while (is_child_quantifier<node_t>(fm))
			fm = tau::get(fm)[0].second();
		return normalize_atomic_formula_operators<node_t>(fm);
	}

	// The bound variables of a quantified sample, outermost first.
	static trefs block_of(const char* sample) {
		tref fm = get_nso_rr(sample).value().main->get();
		trefs block;
		while (is_child_quantifier<node_t>(fm)) {
			block.push_back(tau::trim2(fm));
			fm = tau::get(fm)[0].second();
		}
		return block;
	}

	static size_t ba_type_of(const char* sample) {
		return tau::get(block_of(sample).front()).get_ba_type();
	}

	// ---- distribute_block_over_atoms (step 2a) ----

	TEST_CASE("2a: an atom without a block variable gets no binder") {
		// A vacuous binder would be sound but pure noise downstream, so
		// only dependent atoms are wrapped.
		const char* s = "ex x (xy != 0 || vw != 0).";
		tref res = distribute_block_over_atoms<node_t>(
			body_of(s), block_of(s));
		// Exactly one of the two disjuncts acquired a quantifier.
		CHECK( tau::get(res).select_all(is_quantifier<node_t>).size()
			== 1 );
	}

	TEST_CASE("2a: constants are passed through untouched") {
		const char* s = "ex x (xy != 0 || T).";
		tref res = distribute_block_over_atoms<node_t>(
			body_of(s), block_of(s));
		// T || anything folds to T via the hooks, and no binder is
		// attached to a constant.
		CHECK( tau::get(res)[0].is(tau::wff_t) );
	}

	// ---- squeeze_positive_disjuncts (step 2b) ----

	TEST_CASE("2b: T squeezes to the single term 0") {
		// T is `0 = 0`, so the disjunct list is [0].
		const char* s = "ex x (xy = 0).";
		auto sq = squeeze_positive_disjuncts<node_t>(
			_T<node_t>(), ba_type_of(s));
		REQUIRE( sq.has_value() );
		CHECK( sq->size() == 1 );
		CHECK( tau::get((*sq)[0]).equals_0() );
	}

	TEST_CASE("2b: F squeezes to the empty disjunction") {
		const char* s = "ex x (xy = 0).";
		auto sq = squeeze_positive_disjuncts<node_t>(
			_F<node_t>(), ba_type_of(s));
		REQUIRE( sq.has_value() );
		CHECK( sq->empty() );
	}

	TEST_CASE("2b: a single equation squeezes to its own term") {
		const char* s = "ex x (xy = 0).";
		auto sq = squeeze_positive_disjuncts<node_t>(
			body_of(s), ba_type_of(s));
		REQUIRE( sq.has_value() );
		CHECK( sq->size() == 1 );
	}

	TEST_CASE("2b: a conjunction of ors is the cross product") {
		// (a=0 || b=0) && (c=0 || d=0) -> four disjuncts, each an OR of
		// one term from either side.
		const char* s = "ex x ((xa = 0 || xb = 0) && (xc = 0 || xd = 0)).";
		auto sq = squeeze_positive_disjuncts<node_t>(
			body_of(s), ba_type_of(s));
		REQUIRE( sq.has_value() );
		CHECK( sq->size() == 4 );
	}

	TEST_CASE("2b: declines on a non-equation atom") {
		// bf_lt has no squeeze rule, so the whole attempt is abandoned
		// rather than partially applied.
		const char* s = "ex x (xa = 0 && x < b).";
		CHECK( !squeeze_positive_disjuncts<node_t>(
			body_of(s), ba_type_of(s)).has_value() );
	}

	TEST_CASE("2b: declines on a negated atom") {
		const char* s = "ex x (xa = 0 && xb != 0).";
		CHECK( !squeeze_positive_disjuncts<node_t>(
			body_of(s), ba_type_of(s)).has_value() );
	}

	TEST_CASE("2b: declines when the cross product exceeds the cap") {
		// Four conjuncts of three disjuncts each: 3^4 = 81 > 64, so the
		// multiplicative blow-up the cap exists to stop is refused and
		// the general algorithm runs instead.
		std::string sample = "ex x (";
		const char* groups[] = {"abc", "def", "ghi", "jkl"};
		for (size_t g = 0; g < 4; ++g) {
			if (g) sample += " && ";
			sample += "(";
			for (size_t i = 0; i < 3; ++i) {
				if (i) sample += " || ";
				sample += std::string("x") + groups[g][i]
					+ " = 0";
			}
			sample += ")";
		}
		sample += ").";
		CHECK( !squeeze_positive_disjuncts<node_t>(
			body_of(sample.c_str()),
			ba_type_of(sample.c_str())).has_value() );
	}

	TEST_CASE("2b: declines when a disjunction exceeds the cap") {
		// 65 disjuncts: over the cap by addition rather than
		// multiplication, which is a separate guard.
		std::string sample = "ex x (";
		for (size_t i = 0; i < 65; ++i) {
			if (i) sample += " || ";
			sample += "x a" + std::to_string(i) + " = 0";
		}
		sample += ").";
		auto parsed = get_nso_rr(sample.c_str());
		REQUIRE( parsed.has_value() );
		CHECK( !squeeze_positive_disjuncts<node_t>(
			body_of(sample.c_str()),
			ba_type_of(sample.c_str())).has_value() );
	}

	TEST_CASE("2b: just under the cap still squeezes") {
		// 3*3*3 = 27 <= 64: the boundary case on the other side, so the
		// cap is not accidentally rejecting everything.
		const char* s = "ex x ((xa = 0 || xb = 0 || xc = 0) "
			"&& (xd = 0 || xe = 0 || xf = 0) "
			"&& (xg = 0 || xh = 0 || xi = 0)).";
		auto sq = squeeze_positive_disjuncts<node_t>(
			body_of(s), ba_type_of(s));
		REQUIRE( sq.has_value() );
		CHECK( sq->size() == 27 );
	}
}

TEST_SUITE("BooleAtomAnalysis") {
	// Chapter 5 step 2h splits the Boole decomposition into gamma1..gamma5;
	// four of the five are decided by cofactoring the pivot.

	static boole_atom_analysis<node_t> analyze(const char* sample) {
		tref fm = get_nso_rr(sample).value().main->get();
		tref var = tau::trim2(fm);
		tref body = normalize_atomic_formula_operators<node_t>(
			tau::get(fm)[0].second());
		return analyze_boole_atom<node_t>(body, var);
	}

	TEST_CASE("gamma2: identically zero") {
		// (x|y)x'y' is 0 for x:=0 (y&y' = 0) and for x:=1 (x' = 0), so
		// `= 0` holds whatever x is. Note the simple spellings (xx' = 0)
		// are folded to T by the node hooks before an atom ever exists,
		// so a term the hooks cannot fold syntactically is needed here.
		auto a = analyze("ex x ((x|y)x'y' = 0).");
		CHECK( a.kind == boole_atom_case::identically_zero );
		CHECK( tau::get(a.cofactor_0).equals_0() );
		CHECK( tau::get(a.cofactor_1).equals_0() );
	}

	TEST_CASE("gamma3: identically one") {
		// (x|y')|(x'|y) is 1 for either value of x, so `= 0` holds for
		// none. Same remark as gamma2 about hook-folded spellings.
		auto a = analyze("ex x ((x|y')|(x'|y) = 0).");
		CHECK( a.kind == boole_atom_case::identically_one );
		CHECK( tau::get(a.cofactor_0).equals_1() );
		CHECK( tau::get(a.cofactor_1).equals_1() );
	}

	TEST_CASE("gamma1: unique zero") {
		// x ^ y is zero exactly at x := y, i.e. f[x<-0] == f[x<-1]'.
		auto a = analyze("ex x (x ^ y = 0).");
		CHECK( a.kind == boole_atom_case::unique_zero );
		CHECK( tau::get(a.cofactor_0).to_str() == "y" );
		CHECK( tau::get(a.cofactor_1).to_str() == "y'" );
	}

	TEST_CASE("gamma1: unique zero, xor written out") {
		// The expanded form of the same function must classify the same.
		CHECK( analyze("ex x (x'y | xy' = 0).").kind
			== boole_atom_case::unique_zero );
	}

	TEST_CASE("gamma4: independent of the variable") {
		CHECK( analyze("ex x (yz = 0).").kind
			== boole_atom_case::independent );
	}

	TEST_CASE("gamma5: general") {
		CHECK( analyze("ex x (xy = 0).").kind
			== boole_atom_case::general );
	}

	TEST_CASE("a non-equation atom is always general, with no cofactors") {
		auto a = analyze("ex x (x < y).");
		CHECK( a.kind == boole_atom_case::general );
		// Nothing was cofactored, so the caller must not read these.
		CHECK( a.cofactor_0 == nullptr );
		CHECK( a.cofactor_1 == nullptr );
	}
}

TEST_SUITE("ResolveQuantifiers2Binders") {
	TEST_CASE("a binder absent from the order is left in place") {
		// ex x ex y (xy = 0) with an order covering only the outer
		// variable. bdd_quant can only eliminate what it finds in the
		// order, so stripping the whole prefix leaves the uncovered
		// variable free in the resulting term. It must stay bound.
		tref fm = get_nso_rr("ex x ex y (xy = 0).").value().main->get();
		tref outer = tau::trim2(fm);
		term_handle<node_t>::order order;
		order.emplace(outer, 0);
		tref res = resolve_quantifiers2<node_t>(fm, order,
			is_tref_bv_type_family<node_t>);
		// Whatever was eliminated, no variable may have leaked free.
		CHECK( get_free_vars<node_t>(res).size()
			== get_free_vars<node_t>(fm).size() );
	}
}

TEST_SUITE("QuantBlockPush") {
	TEST_CASE("1") {
		const char* sample = "ex x ex y xy = 0 && yx = 0 && !(x|y = 0) && !(x = y).";
		tref fm = get_nso_rr(sample).value().main->get();
		trefs quant_block;
		quant_block.push_back(tau::get(fm)[0].first());
		fm = tau::get(fm)[0].second();
		quant_block.push_back(tau::get(fm)[0].first());
		fm = tau::get(fm)[0].second();
		term_handle<node_t>::order order;
		tref res = eliminate_block_over_clause<node_t>(fm, quant_block,
			block_eliminability<node_t>{}, order);
		// tau::get(res).print(std::cout << "res: ") << "\n";
		CHECK(tau::get(res).to_str() == "(ex b2, b1 b2 b1|b1 b2 = 0) && (ex b2, b1 !(b2 b1|b1 b2)'&(b2|b1) = 0) && (ex b2, b1 !(b2 b1|b1 b2)'&(b2^b1) = 0)");
	}
	TEST_CASE("2") {
		const char* sample = "all u ex v (u<v && v<x).";
		tref fm = get_nso_rr(sample).value().main->get();
		fm = unequal_to_not_equal<node_t>(fm);
		trefs quant_block;
		term_handle<node_t>::order order;
		tref uvar = tau::trim2(fm);
		order.emplace(uvar, 1);
		fm = tau::get(fm)[0].second();
		quant_block.push_back(tau::trim2(fm));
		order.emplace(tau::trim2(fm), 0);
		fm = tau::get(fm)[0].second();
		tref res = eliminate_block_over_clause<node_t>(fm, quant_block,
			block_eliminability<node_t>{}, order);
		// tau::get(res).print(std::cout << "ex: ") << "\n";
		res = tau::build_wff_all(uvar, res, false);
		res = push_quantifiers_in<node_t>(res);
		// tau::get(res).print(std::cout << "all: ") << "\n";
		res = resolve_quantifiers2<node_t>(res, order);
		// tau::get(res).print(std::cout << "res: ") << "\n";
		CHECK(tau::get(res).equals_F());
	}
}

TEST_SUITE("PushUniversalQuantifierOneOr") {
	TEST_CASE("mixed-variable disjunction: x-free clause factored out") {
		// all x (x = 0 || z = 0):
		//   x = 0 contains the bound variable x → kept under ∀x
		//   z = 0 is x-free                     → pulled out as a disjunct
		// Exercises the wff_or branch where both q_fm and no_q_fm are
		// non-empty (the partial-removal path that was never exercised).
		// Result: (all x (x=0 || _F())) || (_F() || z=0)
		const char* sample = "all x (x = 0 || z = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = push_universal_quantifier_one<node_t>(fm);
		// The formula must have changed (quantifier was restructured)
		CHECK( tau::get(res) != tau::get(fm) );
		// The quantifier must survive (x-dependent clause kept under ∀x)
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) != nullptr );
		// A top-level disjunction must appear (x-free clause factored out)
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_or>) != nullptr );
	}
}

// Coverage for the anti-prenexing functions the 2026-07-30 review found
// untested (report section 5.8), plus the AP-1 gamma4 guard added while fixing
// that finding.

TEST_SUITE("PushQuantifierOne") {

	static tref peel(const char* sample) {
		return get_nso_rr(sample).value().main->get();
	}

	// push_existential_quantifier_one had no test at all: it has no reference
	// anywhere outside antiprenexing.tmpl.h and its three branches were only
	// exercised incidentally through anti_prenex.
	TEST_CASE("existential push distributes over a disjunction") {
		tref fm = peel("ex x (x y = 0 || x z = 0).");
		tref res = push_existential_quantifier_one<node_t>(fm);
		// Both disjuncts must end up separately quantified.
		trefs qs = tau::get(res).select_all(is<node_t, tau::wff_ex>);
		CHECK( qs.size() == 2 );
	}

	TEST_CASE("existential push splits off an independent conjunct") {
		tref fm = peel("ex x (x y = 0 && z = 0).");
		tref res = push_existential_quantifier_one<node_t>(fm);
		CHECK( res != fm );
		// z = 0 does not mention x, so it must leave the scope; exactly one
		// binder survives, around the dependent part.
		trefs qs = tau::get(res).select_all(is<node_t, tau::wff_ex>);
		CHECK( qs.size() == 1 );
	}

	TEST_CASE("existential push commutes with an inner existential") {
		tref fm = peel("ex x ex y (x y = 0).");
		tref res = push_existential_quantifier_one<node_t>(fm);
		CHECK( res != nullptr );
		trefs qs = tau::get(res).select_all(is<node_t, tau::wff_ex>);
		CHECK( qs.size() == 2 );
	}

	TEST_CASE("existential push leaves a fully dependent atom alone") {
		tref fm = peel("ex x (x y = 0).");
		CHECK( push_existential_quantifier_one<node_t>(fm) == fm );
	}

	// AP-14's Release guard (return the input unchanged on a shape mismatch)
	// is deliberately not unit-tested: the DBG assert documenting the same
	// contract fires first in a Debug build, which is the only build these
	// unit tests run in. The guard exists so that Release does not read the
	// wrong children instead of aborting.

	// push_universal_quantifier_one had one test (the wff_or branch); the
	// conjunction-distribute and commute branches had none.
	TEST_CASE("universal push distributes over a conjunction") {
		tref fm = peel("all x (x y = 0 && x z = 0).");
		tref res = push_universal_quantifier_one<node_t>(fm);
		trefs qs = tau::get(res).select_all(is<node_t, tau::wff_all>);
		CHECK( qs.size() == 2 );
	}

	TEST_CASE("universal push commutes with an inner universal") {
		tref fm = peel("all x all y (x y = 0).");
		tref res = push_universal_quantifier_one<node_t>(fm);
		CHECK( res != nullptr );
		trefs qs = tau::get(res).select_all(is<node_t, tau::wff_all>);
		CHECK( qs.size() == 2 );
	}
}

TEST_SUITE("ExQuantifiedBooleDecomposition") {
	// ex_quantified_boole_decomposition had zero test references. Its result
	// must stay equivalent to the input whatever branch it takes.

	static tref run_ebd(const char* sample, bool& no_atms) {
		tref fm = normalize_atomic_formula_operators<node_t>(
			get_nso_rr(sample).value().main->get());
		subtree_unordered_map<node_t, tref> pool;
		subtree_unordered_map<node_t, int_t> quant_pattern;
		tref var = tau::trim2(fm);
		quant_pattern.emplace(var, 1);
		no_atms = false;
		return ex_quantified_boole_decomposition<node_t>(fm, pool,
			quant_pattern, nullptr, no_atms);
	}

	TEST_CASE("decomposition preserves meaning") {
		for (const char* s : { "ex x (x y = 0 && x z != 0).",
					"ex x (x y = 0 || x z = 0).",
					"ex x (x y = 0).",
					"ex x (x y = 0 && z = 0)." }) {
			CAPTURE(s);
			bool no_atms = false;
			tref fm = normalize_atomic_formula_operators<node_t>(
				get_nso_rr(s).value().main->get());
			tref res = run_ebd(s, no_atms);
			REQUIRE( res != nullptr );
			CHECK( are_nso_equivalent<node_t>(res, fm) );
		}
	}

	TEST_CASE("no decomposable atom sets no_atms") {
		// A wff_ref body has no bf_eq/bf_lt/bf_lteq atom to pivot on.
		bool no_atms = false;
		tref res = run_ebd("ex x f(x).", no_atms);
		CHECK( no_atms );
		CHECK( res != nullptr );
	}
}

TEST_SUITE("ProcessQuantifierBlocks") {
	// process_quantifier_blocks and select_innermost_blocks had no direct
	// test; the multi-round re-collection and the `done` retirement that
	// AP-12's termination argument rests on were unpinned.

	TEST_CASE("select_innermost_blocks finds the innermost block first") {
		tref fm = get_nso_rr("ex a all b (a b = 0).").value().main->get();
		subtree_unordered_set<node_t> done;
		std::vector<quantifier_block<node_t>> blocks;
		select_innermost_blocks<node_t>(fm, is_tref_bv_type_family<node_t>,
			done, blocks);
		REQUIRE( blocks.size() == 1 );
		// The innermost run is the universal one.
		CHECK( !blocks[0].is_ex );
		CHECK( blocks[0].vars.size() == 1 );
	}

	TEST_CASE("select_innermost_blocks skips retired heads") {
		tref fm = get_nso_rr("ex a (a b = 0).").value().main->get();
		subtree_unordered_set<node_t> done;
		std::vector<quantifier_block<node_t>> blocks;
		select_innermost_blocks<node_t>(fm, is_tref_bv_type_family<node_t>,
			done, blocks);
		REQUIRE( blocks.size() == 1 );
		done.insert(blocks[0].head);
		blocks.clear();
		select_innermost_blocks<node_t>(fm, is_tref_bv_type_family<node_t>,
			done, blocks);
		CHECK( blocks.empty() );
	}

	TEST_CASE("the driver converges on nested alternating blocks") {
		// Two rounds at least: the inner block is processed first, and
		// retiring its head is what promotes the outer one.
		tref fm = get_nso_rr("ex a all b ex c (a b = 0 || c = 0).")
			.value().main->get();
		tref res = process_quantifier_blocks<node_t>(fm,
			is_tref_bv_type_family<node_t>);
		REQUIRE( res != nullptr );
		CHECK( are_nso_equivalent<node_t>(res, fm) );
	}

	TEST_CASE("no_skip skips nothing") {
		CHECK( !no_skip<node_t>(get_nso_rr("x = 0.").value().main->get()) );
		CHECK( !no_skip<node_t>(nullptr) );
	}
}

TEST_SUITE("Gamma4Guard") {
	// AP-1: gamma4 lifts the pivot atom out of the block's quantifier scope,
	// but the binders are only re-attached inside the recursive results, so an
	// atom still mentioning another block variable must NOT be lifted. The
	// guard added for AP-1 makes that exact rather than relying on the
	// construction hooks happening to fold such atoms away first.
	static bool no_block_var_escapes(const char* sample) {
		tref fm = normalize_atomic_formula_operators<node_t>(
			get_nso_rr(sample).value().main->get());
		trefs block;
		term_handle<node_t>::order order;
		tref body = fm;
		while (tau::get(body)[0].is(tau::wff_ex)) {
			block.push_back(tau::get(body)[0].first());
			body = tau::get(body)[0].second();
		}
		for (size_t i = 0; i < block.size(); ++i)
			order.emplace(block[i], block.size() - 1 - i);
		subtree_unordered_set<node_t> used_atms;
		subtree_unordered_map<node_t, int_t> quant_pattern;
		for (size_t i = 0; i < block.size(); ++i)
			quant_pattern.emplace(block[i], i + 1);
		tref res = anti_prenex_block<node_t>(body, block, used_atms,
			quant_pattern, order, is_tref_bv_type_family<node_t>);
		const trefs& fv = get_free_vars<node_t>(res);
		for (tref v : block) {
			tref tv = tau::trim_right_sibling(v);
			for (tref f : fv)
				if (tau::get(f) == tau::get(tv)) return false;
		}
		return true;
	}

	TEST_CASE("no block variable escapes its scope") {
		for (const char* s : {
			"ex x ex y (((x|y')(x'|y') = 0 || w = 0) && x y != 0).",
			"ex x ex y ((y|y')x = 0 && x y != 0).",
			"ex x ex y (((y|y')x = 0 || w = 0) && x y != 0).",
			"ex x ex y ((y|y')(x|z) = 0 && x y != 0).",
			"ex x ex y ex z ((z|z')(x y) = 0 && x y != 0)." }) {
			CAPTURE(s);
			CHECK( no_block_var_escapes(s) );
		}
	}
}

TEST_SUITE("DistributeBlockOverAtoms") {
	// The wff_and recursion branch of distribute_block_over_atoms
	// (block_squeeze.tmpl.h:9-12) was only reached end-to-end via step 2a.
	// The block variables must be the parsed ones (tau::trim2 of each
	// quantifier), not freshly built ones -- the parser renames and types
	// them, so a rebuilt `x` matches nothing and no binder is emitted.

	static tref body_of2(const char* sample) {
		tref fm = get_nso_rr(sample).value().main->get();
		while (is_child_quantifier<node_t>(fm))
			fm = tau::get(fm)[0].second();
		return normalize_atomic_formula_operators<node_t>(fm);
	}

	static trefs block_of2(const char* sample) {
		tref fm = get_nso_rr(sample).value().main->get();
		trefs block;
		while (is_child_quantifier<node_t>(fm)) {
			block.push_back(tau::trim2(fm));
			fm = tau::get(fm)[0].second();
		}
		return block;
	}

	TEST_CASE("distributes over a conjunction of negated atoms") {
		const char* sample = "ex x (x y != 0 && x z != 0).";
		tref res = distribute_block_over_atoms<node_t>(
			body_of2(sample), block_of2(sample));
		REQUIRE( res != nullptr );
		// One binder per conjunct after distribution.
		trefs qs = tau::get(res).select_all(is<node_t, tau::wff_ex>);
		CHECK( qs.size() == 2 );
	}

	TEST_CASE("a single atom gets a single binder") {
		const char* sample = "ex x (x y != 0).";
		tref res = distribute_block_over_atoms<node_t>(
			body_of2(sample), block_of2(sample));
		trefs qs = tau::get(res).select_all(is<node_t, tau::wff_ex>);
		CHECK( qs.size() == 1 );
	}
}

// Regression tests for the Medium findings fixed in this round.
TEST_SUITE("BlockAtomProfileAtomlessness") {

	// AP-11. distribute_block_over_atoms -- the body of step 2a, which
	// all_negated() guards -- is Corollary 5.1 with J1 empty and holds only in
	// an *atomless* Boolean algebra. Over bv[1], `ex x (x != 0 && x' != 0)` is
	// F while the distributed `ex x (x != 0) && ex x (x' != 0)` is T. The guard
	// used to encode sign uniformity plus !skip_content only, so atomlessness
	// rode entirely on the caller's choice of `skip` -- and blast_block
	// re-entering with no_skip is exactly how that failed. `no_skip` here
	// reproduces that caller.
	static block_atom_profile<node_t> profile_no_skip(const char* sample) {
		tref fm = get_nso_rr(sample).value().main->get();
		while (is_child_quantifier<node_t>(fm))
			fm = tau::get(fm)[0].second();
		fm = normalize_atomic_formula_operators<node_t>(fm);
		return profile_block_atoms<node_t>(fm, no_skip<node_t>);
	}

	TEST_CASE("all_negated declines on bv content even under no_skip") {
		auto p = profile_no_skip("ex x:bv[8] (x != { 0 }:bv[8]).");
		// no_skip means the old guard saw uniform signs and nothing else.
		CHECK( !p.skip_content );
		CHECK( p.negatives == 1 );
		CHECK( p.others == 0 );
		CHECK( p.finite_ba_content );
		CHECK( !p.all_negated() );
	}

	TEST_CASE("atomless content still qualifies for step 2a") {
		// Control: the same shape over the default (atomless) type must keep
		// firing, so the new guard is not simply blocking everything.
		auto p = profile_no_skip("ex x (x y != 0 && x z != 0).");
		CHECK( !p.finite_ba_content );
		CHECK( p.all_negated() );
	}

	TEST_CASE("step 2b is not gated on atomlessness") {
		// all_positive() needs no atomlessness: squeezing
		// `f1 = 0 && f2 = 0` into `f1|f2 = 0` and distributing `ex` over a
		// disjunction are valid in any Boolean algebra. finite_ba_content is
		// therefore not even computed for a positive census.
		auto p = profile_no_skip("ex x (x y = 0 && x z = 0).");
		CHECK( p.all_positive() );
		CHECK( !p.finite_ba_content );
	}
}

TEST_SUITE("TreatExQuantifiedClauseNegatives") {

	// AP-4. squeeze_positives selects with select_top(is<bf_eq>), and
	// select_top descends through wff_neg, so the equation inside a `!(g = 0)`
	// was folded into the *positive* squeeze while the `neqs` scan matched only
	// bf_neq and never re-added the negation: the disequation was inverted and
	// dropped, turning this clause into T. The precondition ("negatives appear
	// as bf_neq") held only by accident of call order; it is now established in
	// the function itself.
	TEST_CASE("a wff_neg(bf_eq) conjunct is not folded into the positives") {
		const char* sample = "ex x (x a = 0 && !(x b = 0)).";
		tref fm = get_nso_rr(sample).value().main->get();
		REQUIRE( fm != nullptr );
		bool quant_eliminated = true;
		tref res = treat_ex_quantified_clause<node_t>(fm, quant_eliminated);
		REQUIRE( res != nullptr );
		// `ex x (xa = 0 && xb != 0)` is satisfiable but not valid: for a = b
		// no x satisfies both, so T would be wrong.
		CHECK( !tau::get(res).equals_T() );
		CHECK( are_nso_equivalent<node_t>(res, fm) );
	}

	TEST_CASE("the bf_neq spelling of the same clause agrees") {
		// Control: the two spellings must now give the same answer.
		tref neg = get_nso_rr("ex x (x a = 0 && !(x b = 0)).")
			.value().main->get();
		tref neq = get_nso_rr("ex x (x a = 0 && x b != 0).")
			.value().main->get();
		bool e1 = true, e2 = true;
		tref r1 = treat_ex_quantified_clause<node_t>(neg, e1);
		tref r2 = treat_ex_quantified_clause<node_t>(neq, e2);
		CHECK( are_nso_equivalent<node_t>(r1, r2) );
	}
}

// AP-5. Neither fallback treat_ex_quantified_clause is reached through
// (resolve_quantifiers, anti_prenex) takes a `skip` predicate, so the
// reservation eliminate_bv_and_quantifiers makes for reference-entangled
// variables never arrived. blocks_elimination covered a reference sitting in
// the same conjunct as the quantified variable; a variable entangled with one
// across conjuncts, through a shared atom, was eliminated anyway.
TEST_SUITE("TreatExQuantifiedClauseRefEntanglement") {

	static tref treat(const char* sample, bool& eliminated) {
		tref fm = get_nso_rr(sample).value().main->get();
		REQUIRE( fm != nullptr );
		eliminated = true;
		return treat_ex_quantified_clause<node_t>(fm, eliminated);
	}

	TEST_CASE("a variable entangled with a reference across conjuncts is kept") {
		// `x` shares the atom `x y = 0` with `y`, and `y` is an argument of the
		// unresolved predicate reference `q(y)`. No single conjunct holds both
		// `x` and the reference, so blocks_elimination alone does not fire.
		// A wff_ref (a predicate used as a formula) is what
		// collect_used_ref_variables seeds on -- a bf_ref inside an atom, as
		// in `f(y) != 0`, is not a seed.
		bool eliminated = true;
		tref res = treat("ex x (x y = 0 && q(y)).", eliminated);
		CHECK( !eliminated );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) != nullptr );
	}

	TEST_CASE("a variable sharing its conjunct with a reference is kept") {
		// Control for the pre-existing path: here the reference is inside a
		// conjunct that mentions `x`, which blocks_elimination already caught.
		bool eliminated = true;
		tref res = treat("ex x (x y = 0 && q(x)).", eliminated);
		CHECK( !eliminated );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) != nullptr );
	}

	TEST_CASE("a reference elsewhere in the clause does not block an unrelated variable") {
		// `x` shares no atom with `z`, so it is not entangled with `q(z)` and
		// must still be eliminated -- otherwise the guard would be a blanket
		// "any reference anywhere blocks everything".
		bool eliminated = true;
		tref res = treat("ex x (x y = 0 && q(z)).", eliminated);
		CHECK( eliminated );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}

	TEST_CASE("a reference-free clause is unaffected") {
		bool eliminated = true;
		tref res = treat("ex x (x y = 0 && x z != 0).", eliminated);
		CHECK( eliminated );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}
}

// AP-20 / AP-21. anti_prenex's memo is Release-only (TAU_CACHE is OFF in the
// Debug preset), which is why AP-20 -- a cache keyed on the formula alone while
// the result also depends on the runtime-mutable `bv_blasting` global -- had no
// regression test at all. Guarding the case on TAU_CACHE rather than on the
// build type is what makes it testable: it is compiled out of the Debug run and
// exercised by the Release one, which is the configuration the cache exists in.
//
// The assertions deliberately avoid committing to a normal form for either
// setting. What is pinned is the relation between the three answers: blasting
// must change the result at all (otherwise the case is vacuous), and switching
// back must return the first answer rather than the second. With a single
// formula-keyed cache the third call is served the second call's entry.
#ifdef TAU_CACHE
TEST_SUITE("AntiPrenexBlastingCache") {

	TEST_CASE("the memo is keyed on bv_blasting") {
		// An *open* bv scope over blastable arithmetic. Both properties are
		// needed: `y` free makes treat_ex_quantified_clause's
		// closed-and-solvable test fail, so the solver does not decide it
		// first, and `+ { 1 }` is arithmetic bv_predicate_blasting can actually
		// rewrite -- with `&`, or with multiplication by a non-constant,
		// blasting is a no-op and both settings give the same answer, which
		// would make this case vacuous. Observed here: blasting off keeps
		// `ex b1 b1+1 = y`, blasting on returns the bit-level expansion.
		tref fm = get_nso_rr("ex x (x:bv[4] + { 1 }:bv[4] = y:bv[4]).")
			.value().main->get();
		REQUIRE( fm != nullptr );

		const bool saved = bv_blasting;
		bv_blasting = false;
		tref off1 = anti_prenex<node_t>(fm);
		bv_blasting = true;
		tref on = anti_prenex<node_t>(fm);
		bv_blasting = false;
		tref off2 = anti_prenex<node_t>(fm);
		bv_blasting = saved;

		REQUIRE( off1 != nullptr );
		REQUIRE( on != nullptr );
		REQUIRE( off2 != nullptr );
		// Not vacuous: the setting really does change the answer.
		CHECK( tau::get(off1) != tau::get(on) );
		// The regression: switching back must not be served the on-entry.
		CHECK( tau::get(off2) == tau::get(off1) );
	}
}
#endif // TAU_CACHE

// Quantifier ids are canonicalised once at pipeline entry and once at exit, and
// every binder the pass builds runs with calculate_quant_id = false -- which
// also stops find_biggest_quant_id, a full pre_order walk, from running once per
// binder built.
//
// These pin that the renaming is invisible from outside: a block that mixes a
// trivially eliminable variable with one frozen by a reference must come back
// with exactly the frozen binder, through both entry points.
//
// The design's other id hazard -- a free variable named like a canonical id --
// is not reachable from the surface syntax and so has no case here.
// `canonize_quantifier_ids` renames a bound variable to the decimal string of
// its id ("1", "2"), while the printer displays those as `b1`, `b2`; parsing
// `b1` yields the distinct internal name "b1". Checked by hand:
// `ex x (x a = 0) && b1 c = 0.` normalises to `b1 c = 0`, uncaptured.
TEST_SUITE("CanonicalQuantifierIds") {

	TEST_CASE("a frozen binder survives alone, under canonical ids") {
		// x is trivially eliminable; y is frozen by the reference f.
		// Exactly one binder survives, and the reference is intact.
		const char* sample = "ex x, y (x = 0 && y w = 0 && f(y)).";
		tref fm = get_nso_rr(sample).value().main->get();
		for (tref res : { anti_prenex<node_t>(fm),
			anti_prenex_block<node_t>(fm) })
		{
			CHECK( tau::get(res).select_top(
				is<node_t, tau::wff_ex>).size() == 1 );
			CHECK( tau::get(res).find_top(
				is<node_t, tau::wff_ref>) );
		}
	}
}

// The two resource limits the block algorithm charges. Both are runtime
// parameters rather than header constants, per the project's standing policy;
// `bv_blasting` (heuristics/bv_predicate_blasting.h) is the precedent.
TEST_SUITE("BlockLimits") {

	TEST_CASE("exhausting the split budget keeps quantifiers and does not hang") {
		// With a budget of 1 the core cannot finish the decomposition.
		// The contract is: keep the quantifiers, log, return -- never
		// hang, and never claim an answer it did not compute.
		const size_t saved = block_boole_max_splits;
		block_boole_max_splits = 1;
		const char* sample =
			"ex x, y, z ((x a = 0 || y b = 0) && (z c = 0 || x d = 0)).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		block_boole_max_splits = saved;
		CHECK( res != nullptr );
		CHECK( !tau::get(res).equals_F() );
	}

	TEST_CASE("the limits are at their documented defaults") {
		CHECK( block_boole_max_splits == 512 );
		CHECK( block_max_rounds == 1000 );
	}
}

// The γ1 (unique-zero) cofactor fold and the constraint its F-branch must
// carry. Driven through the 8-arg core directly: the full pipeline's
// simplification steps resolve this shape before the core sees it, so a
// pipeline-level test would pass even with the bug present.
TEST_SUITE("Gamma1NegatedBranch") {
	static tref g1_core(const char* sample) {
		tref fm = get_nso_rr(sample).value().main->get();
		trefs block;
		term_handle<node_t>::order order;
		tref body = fm;
		while (tau::get(body)[0].is(tau::wff_ex)) {
			block.push_back(tau::get(body)[0].first());
			body = tau::get(body)[0].second();
		}
		for (size_t i = 0; i < block.size(); ++i)
			order.emplace(block[i], block.size() - 1 - i);
		body = normalize_atomic_formula_operators<node_t>(body);
		subtree_unordered_set<node_t> used_atms;
		subtree_unordered_map<node_t, int_t> qp;
		for (size_t i = 0; i < block.size(); ++i)
			qp.emplace(block[i], i + 1);
		size_t sl = block_boole_max_splits;
		analysis_context<node_t> ac;
		const block_eliminability<node_t> elim = analyse_block<node_t>(
			block, get_cnf_wff_clauses<node_t>(body), ac);
		return anti_prenex_block<node_t>(body, block, used_atms, qp,
			order, is_tref_bv_type_family<node_t>, sl, elim);
	}

	// The named free variable of @p fm -- the PARSED node, since a node
	// built by hand would differ (type inference) and replace() would
	// silently miss it.
	static tref g1_var(tref fm, const char* name) {
		for (tref v : get_free_vars<node_t>(fm))
			if (tau::get(v).to_str() == name) return v;
		return nullptr;
	}

	// Ground truth of a one-free-variable result: substitute the constant
	// and read the normalized closed instance. T and F are both definite.
	static bool g1_holds_at(tref res, tref var, bool one) {
		const size_t ty = tau::get(var).get_ba_type();
		tref g = rewriter::replace<node_t>(res, var,
			one ? tau::_1_trimmed(ty) : tau::_0_trimmed(ty));
		return tau::get(normalize_non_temp<node_t>(tau::reget(g)))
			.equals_T();
	}

	TEST_CASE("the unique-zero fold keeps the pivot's negation") {
		// ∃q (q ≠ 0 ∧ (a q = 0 ∨ q = 0)) ≡ a' ≠ 0: T at a=0, F at a=1.
		// γ1 fires on the pivot `q = 0` (f = q has its unique zero at
		// q := 0). Before the fix the F-branch was φ[atm→F] with ¬atm
		// dropped, so the `q ≠ 0` constraint -- which lives only in
		// occurrences of the atom -- vanished, and the core answered T
		// at a=1 as well. This is the wrong answer the legacy fallback
		// had been silently repairing on the issue #70 interpreter spec.
		const char* sample = "ex q (!(q = 0) && (a q = 0 || q = 0)).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = g1_core(sample);
		tref var = g1_var(fm, "a");
		REQUIRE( var != nullptr );
		CHECK( g1_holds_at(res, var, false) );
		CHECK( !g1_holds_at(res, var, true) );
	}

	TEST_CASE("the five-conjunct interpreter shape stays sound") {
		// The shape the issue #70 step system actually produced, reduced
		// by tree-level delta debugging. Same truth table: T at a=0,
		// F at a=1 (s does not matter; check both values).
		const char* sample =
			"ex q (!(q = 0) && (a q = 0 || q = 0) && "
			"(!(q = a) || !(s' = 0) || a q = q || !(q = 0)) && "
			"(!(a' = 0) || !(s' = 0) || a q = a || a = q || !(q = 0)) && "
			"(!(a q = q) || !(s' = 0) || q = a || a q = 0 || q = 0)).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = g1_core(sample);
		tref a = g1_var(fm, "a");
		tref s = g1_var(fm, "s");
		REQUIRE( a != nullptr );
		REQUIRE( s != nullptr );
		const size_t st = tau::get(s).get_ba_type();
		tref r0 = rewriter::replace<node_t>(res, s, tau::_0_trimmed(st));
		tref r1 = rewriter::replace<node_t>(res, s, tau::_1_trimmed(st));
		CHECK( g1_holds_at(r0, a, false) );
		CHECK( !g1_holds_at(r0, a, true) );
		CHECK( g1_holds_at(r1, a, false) );
		CHECK( !g1_holds_at(r1, a, true) );
	}
}
