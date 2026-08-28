// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "normal_forms.h"
#include "normalizer.h"
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
			used_atms, quant_pattern, order, eliminability<node_t>::arith_only());
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
			eliminability<node_t>::arith_only());
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
		// Order flipped by the 8f1a74c1 parser regen (Debug's
		// matches_to_any_of only checks expected[0] -- see test_helpers.h).
		// Order flipped again by the 2026-08-27 regen (left-assoc
		// arithmetic + `(bv[N])` cast disambiguation in tau.tgf).
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
		// Measured: 7 bf_eq atoms when the order atom is picked, 4 when
		// the equation is (6 before the bare-atom matrix was routed
		// through eliminate_block_over_clause, which folds further).
		// The count is a proxy for the pivot choice, so
		// it tracks whatever the downstream simplifiers leave behind:
		// this branch's squeeze_absorb reaches further than the one the
		// original 4 was measured against, and the two disagree on how
		// much of the debris they fold. What identifies the equation as
		// the pivot either way is that no bf_lt survives -- when the
		// order atom is picked, both branches hit the unrecognized-
		// conjunct path and it re-wraps with the `<` still in place. The
		// equivalence check pins the part that must not drift.
		const char* sample = "ex x ((x < z || xy = 0) && xk != 0).";
		auto [res, used] = run_apb_norm(sample);
		CHECK( used == 0 );
		CHECK( tau::get(res).find_top(is<node_t, tau::bf_lt>) == nullptr );
		CHECK( tau::get(res).select_all(is<node_t, tau::bf_eq>).size()
			== 4 );
		CHECK( are_nso_equivalent<node_t>(res,
			normalize_atomic_formula_operators<node_t>(
				get_nso_rr(sample).value().main->get())) );
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
	// Tests for the one-arg anti_prenex pipeline (NNF+simplify → subs_elim →
	// normalize_atomic → process_block post-order → canonical ids).

	static tref run_apb0(const char* sample) {
		return anti_prenex<node_t>(
			get_nso_rr(sample).value().main->get());
	}

	TEST_CASE("quantifier-free formula is returned unchanged") {
		// Short-circuit: no quantifiers → original tref returned as-is.
		tref fm = get_nso_rr("xy = 0 && wz = 0.").value().main->get();
		CHECK( anti_prenex<node_t>(fm) == fm );
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
		// Order flipped by the 8f1a74c1 parser regen (Debug's
		// matches_to_any_of only checks expected[0] -- see test_helpers.h).
		CHECK( matches_to_str_to_any_of(res, {"zw = 0", "wz = 0"}) );
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
			eliminability<node_t>::arith_only());
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
			eliminability<node_t>::arith_only());
	}

	TEST_CASE("an opposite-kind skipped quantifier ends the run") {
		// `ex a all x:bv[8] phi`. x is skip-matched, so this pass never
		// eliminates it -- but wrap_skipped re-emits everything in
		// blk.displaced by category, frozen/arithmetic/blasteable
		// outermost, so absorbing the `all` here would hoist it out
		// past the `ex` and produce all x ex a phi, which is not
		// equivalent (only the converse implication holds). The run
		// must therefore stop at it.
		auto blk = collect("ex a all x:bv[8] (a = 0 || x = 0).");
		CHECK( blk.is_ex );
		CHECK( blk.vars.size() == 1 );
		CHECK( blk.displaced.empty() );
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
		CHECK( blk.displaced.size() == 1 );
		CHECK( std::get<1>(blk.displaced.front()) == false ); // it was an `all`
		CHECK( blk.is_ex );                             // kind from `ex a`
		CHECK( blk.vars.size() == 1 );
	}

	TEST_CASE("a same-kind skipped quantifier is transparent") {
		// `ex a ex x:bv[8] phi`: same kind, so it commutes with the
		// block and is deferred without ending the run.
		auto blk = collect("ex a ex x:bv[8] (a = 0 && x = 0).");
		CHECK( blk.is_ex );
		CHECK( blk.vars.size() == 1 );
		CHECK( blk.displaced.size() == 1 );
		CHECK( std::get<1>(blk.displaced.front()) == true );  // it was an `ex`
	}

	TEST_CASE("a run of only skipped quantifiers has no active variable") {
		// Nothing for this pass to eliminate: vars is empty and the
		// whole run is deferred.
		auto blk = collect("ex x:bv[8] (x = 0).");
		CHECK( blk.vars.empty() );
		CHECK( blk.displaced.size() == 1 );
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
		// the general algorithm runs instead. block_squeeze_cap defaults
		// to 0 (unlimited) since it became a runtime parameter, so the
		// cap under test is set explicitly for this case's duration.
		const size_t saved_cap = block_squeeze_cap;
		block_squeeze_cap = 64;
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
		block_squeeze_cap = saved_cap;
	}

	TEST_CASE("2b: declines when a disjunction exceeds the cap") {
		// 65 disjuncts: over the cap by addition rather than
		// multiplication, which is a separate guard. As above, the cap
		// under test is set explicitly (the default is unlimited).
		const size_t saved_cap = block_squeeze_cap;
		block_squeeze_cap = 64;
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
		block_squeeze_cap = saved_cap;
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
			eliminability<node_t>::arith_only());
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
}
// Coverage for the anti-prenexing functions the 2026-07-30 review found
// untested (report section 5.8), plus the AP-1 gamma4 guard added while fixing
TEST_SUITE("ProcessQuantifierBlocks") {
	// process_quantifier_blocks and select_innermost_blocks had no direct
	// test; the multi-round re-collection and the `done` retirement that
	// AP-12's termination argument rests on were unpinned.

	TEST_CASE("select_innermost_blocks finds the innermost block first") {
		tref fm = get_nso_rr("ex a all b (a b = 0).").value().main->get();
		subtree_unordered_set<node_t> done;
		std::vector<quantifier_block<node_t>> blocks;
		select_innermost_blocks<node_t>(fm, eliminability<node_t>::arith_only(),
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
		select_innermost_blocks<node_t>(fm, eliminability<node_t>::arith_only(),
			done, blocks);
		REQUIRE( blocks.size() == 1 );
		done.insert(blocks[0].head);
		blocks.clear();
		select_innermost_blocks<node_t>(fm, eliminability<node_t>::arith_only(),
			done, blocks);
		CHECK( blocks.empty() );
	}

	TEST_CASE("the driver converges on nested alternating blocks") {
		// Two rounds at least: the inner block is processed first, and
		// retiring its head is what promotes the outer one.
		tref fm = get_nso_rr("ex a all b ex c (a b = 0 || c = 0).")
			.value().main->get();
		tref res = process_quantifier_blocks<node_t>(fm,
			eliminability<node_t>::arith_only());
		REQUIRE( res != nullptr );
		CHECK( are_nso_equivalent<node_t>(res, fm) );
	}

	TEST_CASE("the empty analysis skips nothing") {
		const auto none = eliminability<node_t>::none();
		CHECK( !none.skip(get_nso_rr("x = 0.").value().main->get()) );
		CHECK( !none.skip(nullptr) );
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
			quant_pattern, order, eliminability<node_t>::arith_only());
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
	// rode entirely on the caller's choice of analysis -- and blast_block
	// re-entering with a skip-nothing one is exactly how that failed. The
	// empty analysis here reproduces that caller.
	static block_atom_profile<node_t> profile_skipping_nothing(const char* sample) {
		tref fm = get_nso_rr(sample).value().main->get();
		while (is_child_quantifier<node_t>(fm))
			fm = tau::get(fm)[0].second();
		fm = normalize_atomic_formula_operators<node_t>(fm);
		return profile_block_atoms<node_t>(fm,
			eliminability<node_t>::none());
	}

	TEST_CASE("all_negated declines on bv content even under the empty analysis") {
		auto p = profile_skipping_nothing("ex x:bv[8] (x != { 0 }:bv[8]).");
		// Skipping nothing means the old guard saw uniform signs and
		// nothing else.
		CHECK( !p.skip_content );
		CHECK( p.negatives == 1 );
		CHECK( p.others == 0 );
		CHECK( p.finite_ba_content );
		CHECK( !p.all_negated() );
	}

	TEST_CASE("atomless content still qualifies for step 2a") {
		// Control: the same shape over the default (atomless) type must keep
		// firing, so the new guard is not simply blocking everything.
		auto p = profile_skipping_nothing("ex x (x y != 0 && x z != 0).");
		CHECK( !p.finite_ba_content );
		CHECK( p.all_negated() );
	}

	TEST_CASE("step 2b is not gated on atomlessness") {
		// all_positive() needs no atomlessness: squeezing
		// `f1 = 0 && f2 = 0` into `f1|f2 = 0` and distributing `ex` over a
		// disjunction are valid in any Boolean algebra. finite_ba_content is
		// therefore not even computed for a positive census.
		auto p = profile_skipping_nothing("ex x (x y = 0 && x z = 0).");
		CHECK( p.all_positive() );
		CHECK( !p.finite_ba_content );
	}
}
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
		tref res = anti_prenex<node_t>(fm);
		CHECK( tau::get(res).select_top(
			is<node_t, tau::wff_ex>).size() == 1 );
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_ref>) );
	}
}

// The two resource limits the block algorithm charges. Both are runtime
// parameters rather than header constants, per the project's standing policy;
// `preprocessing` (heuristics/preprocess_placement.h) is the precedent.
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
		CHECK( block_boole_max_splits
			== std::numeric_limits<size_t>::max() );
		CHECK( block_max_rounds == std::numeric_limits<size_t>::max() );
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
			order, eliminability<node_t>::arith_only(), sl, elim);
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

// Task 11 (user directive 2026-08-14): "bv variables that appear only in atoms
// that are purely BA are also eliminable." The pair below pins the two halves
// of that -- what must now be eliminated in Tau, and what must NOT start being
// distributed once it is.
TEST_SUITE("PureBaBvEliminability") {

	TEST_CASE("pure-BA bv scope is eliminated in Tau, no quantifier survives") {
		// ex x (x|y = 0) over bv[2] == y = 0 by Boole's expansion, valid
		// in any BA. Before this task the blanket bv skip kept the binder.
		//
		// The analysis has to be the one `normalize_non_temp` builds: the
		// 1-argument `anti_prenex` hands down
		// `eliminability<node_t>::arith_only()`, a blanket floor with no
		// explicit verdicts at all, and it is exactly that floor the seed
		// flip has to beat. Calling the 2-argument entry point with
		// `analyse_formula`'s result is the same pairing
		// `normalize_non_temp` performs, without the solver passes around
		// it.
		tref fm = get_nso_rr("ex x (x:bv[2] | y:bv[2] = { 0 }:bv[2]).")
			.value().main->get();
		tref r = anti_prenex<node_t>(fm,
			analyse_formula<node_t>(fm, analysis_context<node_t>{}));
		CHECK( tau::get(r).find_top(is_quantifier<node_t>) == nullptr );
	}

	TEST_CASE("atomic-BA counterexample: a bv[1] negated pair is not distributed") {
		// `ex x (x != 0 && x != 1)` is UNSAT at bv[1] (a two-element BA);
		// the atomless-only distribution of step 2a -- and the `!=`
		// witness constructions of the squeeze -- would wrongly answer T.
		// Spelled without a complement so the pair is plainly all-negated
		// and plainly UNSAT. Accept any result that is not T: F, or a
		// kept quantifier.
		tref fm = get_nso_rr("ex x (x:bv[1] != { 0 }:bv[1] "
			"&& x:bv[1] != { 1 }:bv[1]).").value().main->get();
		tref r = normalize_non_temp<node_t>(fm);
		CHECK_FALSE( tau::get(r).equals_T() );
	}
}

TEST_SUITE("FrozenBlockNormalization") {

	TEST_CASE("a block over only ref-entangled variables survives verbatim") {
		// q(y) freezes y; the ex y binder and its scope must come back
		// untouched (no decomposition, no blasting attempt).
		const char* s = "ex y (q(y) && y != 0).";
		tref fm = get_nso_rr(s).value().main->get();
		tref r = anti_prenex<node_t>(fm);
		CHECK(tau::get(r).find_top(is_child_quantifier<node_t>) != nullptr);
		CHECK(tau::get(r).find_top(is<node_t, tau::wff_ref>) != nullptr);
	}

	TEST_CASE("an eliminable variable over a frozen scope still eliminates") {
		// z = 0 is decomposable; q(y) freezes only y's component. The outer
		// ex z must go, the inner ex y must stay.
		const char* s = "ex z ex y (q(y) && z = 0).";
		tref fm = get_nso_rr(s).value().main->get();
		tref r = anti_prenex<node_t>(fm);
		trefs quants = tau::get(r).select_top(is_child_quantifier<node_t>);
		CHECK(quants.size() == 1);   // only y's binder survives
	}

	// The two cases above go through the 1-arg anti_prenex(fm) entry point,
	// which hands down eliminability<node_t>::arith_only() -- an instance with
	// NO explicit verdicts at all, so el.verdict_of is never `frozen` there
	// and neither this task's process_quantifier_block early-out nor its
	// three blast_block gates (both keyed off an explicit frozen verdict)
	// can ever fire for them: they pin the pre-existing
	// eliminate_block_over_clause reserved-fixpoint path only. The two
	// cases below are the SAME formulas through the 2-arg entry point with
	// a real analyse_formula analysis (the PureBaBvEliminability "pure-BA
	// bv scope" pattern) -- the only path that actually threads an explicit
	// `frozen` verdict down to process_quantifier_block/anti_prenex_block,
	// so this task's new code is reachable at all.
	TEST_CASE("a block over only ref-entangled variables survives verbatim "
		"(real analysis)") {
		const char* s = "ex y (q(y) && y != 0).";
		tref fm = get_nso_rr(s).value().main->get();
		auto el = analyse_formula<node_t>(fm, analysis_context<node_t>{});
		// Non-vacuity: y really is analysed frozen, so the assertions below
		// exercise the early-out rather than passing on an unanalysed
		// default. is_ref_fm matches the wff WRAPPING wff_ref -- that is
		// the node get_free_vars/analyse_formula key on, not the bare
		// wff_ref itself (mirrors test_eliminability.cpp's idiom).
		auto is_ref_fm = [](tref n) {
			const auto& t = tau::get(n);
			return t.is(tau::wff) && t.child_is(tau::wff_ref);
		};
		tref ref_fm = tau::get(fm).find_top(is_ref_fm);
		REQUIRE(ref_fm != nullptr);
		tref y = get_free_vars<node_t>(ref_fm)[0];
		REQUIRE(el.verdict_of(y) == elim_verdict::frozen);
		tref r = anti_prenex<node_t>(fm, el);
		CHECK(tau::get(r).find_top(is_child_quantifier<node_t>) != nullptr);
		CHECK(tau::get(r).find_top(is<node_t, tau::wff_ref>) != nullptr);
	}

	TEST_CASE("an eliminable variable over a frozen scope still eliminates "
		"(real analysis)") {
		const char* s = "ex z ex y (q(y) && z = 0).";
		tref fm = get_nso_rr(s).value().main->get();
		// Non-vacuity: the input really is a 2-binder block before
		// elimination. select_top does NOT descend into a match (it stops
		// at the first quantifier along each path), so on the nested
		// `ex z (ex y ...)` shape it reports only the outer binder -- 1,
		// not 2. select_all does descend into matches and is the correct
		// tool for counting a (possibly nested) quantifier chain; same
		// reasoning applies to the post-elimination count below, where the
		// FAILURE shape this test exists to catch (z's binder wrongly
		// surviving alongside y's) is itself a 2-deep nested chain that
		// select_top would silently misreport as "1".
		REQUIRE(tau::get(fm).select_all(
			is_child_quantifier<node_t>).size() == 2);
		auto el = analyse_formula<node_t>(fm, analysis_context<node_t>{});
		tref r = anti_prenex<node_t>(fm, el);
		trefs quants = tau::get(r).select_all(is_child_quantifier<node_t>);
		CHECK(quants.size() == 1);   // only y's binder survives
		CHECK(tau::get(r).find_top(is<node_t, tau::wff_ref>) != nullptr);
	}

	// Disjunctive variant of the "eliminates" case above (real analysis).
	// Traced in the task-6 report: the wff_or case dispatches per-disjunct,
	// and the frozen disjunct (q(y), no z) returns verbatim while the two
	// z-disjuncts still squeeze via step 2b -- so this resolves the same
	// way as the AND-only case, through the same non-gate machinery
	// (dep/indep separation + eliminate_block_over_clause's reserved
	// fixpoint / the wff_or per-disjunct dispatch), never through any
	// blast_block gate. See "Gate reachability" in the task-6 report for
	// why: a frozen verdict's ref union-find merges every atom with ALL its
	// free variables, so anything sharing an atom with frozen content is
	// itself frozen -- active and frozen variables are always
	// variable-disjoint, and dep/indep separation detaches frozen content
	// before any gate could see it.
	TEST_CASE("an eliminable variable over a frozen scope still eliminates, "
		"disjunctive form (real analysis)") {
		const char* s = "ex z ex y ((z = 0 || z = 1) && q(y)).";
		tref fm = get_nso_rr(s).value().main->get();
		REQUIRE(tau::get(fm).select_all(
			is_child_quantifier<node_t>).size() == 2);
		auto el = analyse_formula<node_t>(fm, analysis_context<node_t>{});
		tref r = anti_prenex<node_t>(fm, el);
		trefs quants = tau::get(r).select_all(is_child_quantifier<node_t>);
		CHECK(quants.size() == 1);   // only y's binder survives
		CHECK(tau::get(r).find_top(is<node_t, tau::wff_ref>) != nullptr);
	}
}

TEST_SUITE("DisplacedBinderOrdering") {

	TEST_CASE("displaced binders re-wrap ordered frozen, arithmetic, blasteable") {
		// y frozen (entangled with the reference q(y)), a arithmetic (a
		// non-constant bv multiplication -- blasting cannot express it),
		// b blasteable (a bv addition -- blasting CAN express it; plain
		// bv equality alone is `eliminable` per the pure-BA-bv directive,
		// so this needs an actual supported arithmetic operator to land
		// b as blasteable rather than eliminable), z eliminable (joins
		// blk.vars, gets fully resolved, and is never displaced at all).
		// All four variables are kept disjoint so no fixpoint's
		// union-find absorbs one into a neighbor's category. b's atom
		// is closed over constants only (no free `c`) to keep variable
		// identification below unambiguous.
		const char* s = "ex b ex z ex y ex a (q(y) && z = 0 && "
			"(a:bv[2] * a = { 0 }:bv[2]) && "
			"(b:bv[2] + { 1 }:bv[2] = { 0 }:bv[2])).";
		tref fm = get_nso_rr(s).value().main->get();
		auto el = analyse_formula<node_t>(fm, analysis_context<node_t>{});

		// Locate each variable's tref the way the FrozenBlockNormalization
		// tests do: from the specific subformula that carries it, via
		// get_free_vars on a WFF-level atom -- not by re-deriving names
		// from the binder prefix via to_str(), and not from a raw
		// operator node directly (get_free_vars requires a `bf`- or
		// `wff`-typed node, which a bare bf_mul/bf_add operator node is
		// not guaranteed to be). All lookups stay within this one parsed
		// `fm`, so identity is exact by hash-consing, no cross-parse
		// assumption needed.
		auto is_ref_fm = [](tref n) {
			const auto& t = tau::get(n);
			return t.is(tau::wff) && t.child_is(tau::wff_ref);
		};
		tref ref_fm = tau::get(fm).find_top(is_ref_fm);
		REQUIRE(ref_fm != nullptr);
		trefs y_vars = get_free_vars<node_t>(ref_fm);
		REQUIRE(y_vars.size() == 1);
		tref y = y_vars.front();

		auto is_mul_atom = [](tref n) {
			return is_atomic_fm<node_t>(n) && tau::get(n).find_top(
				is<node_t, tau::bf_mul>) != nullptr;
		};
		tref atom_a = tau::get(fm).find_top(is_mul_atom);
		REQUIRE(atom_a != nullptr);
		trefs a_vars = get_free_vars<node_t>(atom_a);
		REQUIRE(a_vars.size() == 1);
		tref a = a_vars.front();

		auto is_add_atom = [](tref n) {
			return is_atomic_fm<node_t>(n) && tau::get(n).find_top(
				is<node_t, tau::bf_add>) != nullptr;
		};
		tref atom_b = tau::get(fm).find_top(is_add_atom);
		REQUIRE(atom_b != nullptr);
		trefs b_vars = get_free_vars<node_t>(atom_b);
		REQUIRE(b_vars.size() == 1);
		tref b = b_vars.front();

		// z: the only plain atom, carrying no arithmetic operator (and
		// is_atomic_fm excludes wff_ref, so q(y) cannot match here).
		auto is_plain_atom = [](tref n) {
			return is_atomic_fm<node_t>(n)
				&& tau::get(n).find_top(is<node_t, tau::bf_mul>) == nullptr
				&& tau::get(n).find_top(is<node_t, tau::bf_add>) == nullptr;
		};
		tref atom_z = tau::get(fm).find_top(is_plain_atom);
		REQUIRE(atom_z != nullptr);
		trefs z_vars = get_free_vars<node_t>(atom_z);
		REQUIRE(z_vars.size() == 1);
		tref z = z_vars.front();

		// Non-vacuity, per the lesson this refactor keeps re-learning:
		// confirm each variable's verdict through the SAME analyse_formula
		// the 2-arg anti_prenex call below actually uses, BEFORE running
		// it.
		REQUIRE(el.verdict_of(y) == elim_verdict::frozen);
		REQUIRE(el.verdict_of(a) == elim_verdict::arithmetic);
		REQUIRE(el.verdict_of(b) == elim_verdict::blasteable);
		REQUIRE(el.verdict_of(z) == elim_verdict::eliminable);

		tref r = anti_prenex<node_t>(fm, el);

		// `anti_prenex` runs `canonize_quantifier_ids` on entry AND on
		// exit (Step 0 / Step 5), which replaces every bound variable
		// (and its free occurrences within scope) with a FRESH variable
		// named after its quantifier depth. So `r`'s binders are NOT
		// the same trefs as `y`/`a`/`b`/`z` above -- comparing them
		// directly would silently compare against nodes that can never
		// appear in `r`, making every `pos(...)` collapse to
		// `order.end()` and every `<` a false negative that looks like
		// failure without explaining why. Re-locate the (renamed)
		// variables INSIDE `r` the same structural way they were found
		// in `fm`: `bf_mul`/`bf_add`/`wff_ref` shape survives renaming
		// even though variable identity does not.
		tref ref_fm2 = tau::get(r).find_top(is_ref_fm);
		REQUIRE(ref_fm2 != nullptr);
		trefs y2_vars = get_free_vars<node_t>(ref_fm2);
		REQUIRE(y2_vars.size() == 1);
		tref y2 = y2_vars.front();

		tref atom_a2 = tau::get(r).find_top(is_mul_atom);
		REQUIRE(atom_a2 != nullptr);
		trefs a2_vars = get_free_vars<node_t>(atom_a2);
		REQUIRE(a2_vars.size() == 1);
		tref a2 = a2_vars.front();

		tref atom_b2 = tau::get(r).find_top(is_add_atom);
		REQUIRE(atom_b2 != nullptr);
		trefs b2_vars = get_free_vars<node_t>(atom_b2);
		REQUIRE(b2_vars.size() == 1);
		tref b2 = b2_vars.front();

		// Walk the outermost quantifier chain and record the bound
		// variable trefs. select_top does NOT descend into matches, so
		// a chain-walk (mirroring FrozenBlockNormalization's
		// binder-counting idiom) is the only correct way to read prefix
		// order here.
		trefs order;
		tref n = tau::get(r).find_top(is_child_quantifier<node_t>);
		while (n && is_child_quantifier<node_t>(n)) {
			order.push_back(tau::trim2(n));
			n = tau::get(n)[0].second();
		}
		// Exactly y, a, b survive as binders; z's binder (and, per
		// is_plain_atom below, its atom) are both gone.
		CHECK(order.size() == 3);
		// Structural equality, not raw tref equality: in this LCRS
		// tree the same variable at binder position and at atom
		// position are different trefs (different right-sibling
		// context) even though they denote the same variable --
		// `tau::get(x) == tau::get(y)` is the idiom
		// `has_block_var_other_than` already uses for exactly this.
		auto pos = [&](tref v) {
			return std::find_if(order.begin(), order.end(),
				[&](tref x) { return tau::get(x) == tau::get(v); });
		};
		// frozen before arithmetic before blasteable.
		CHECK(pos(y2) < pos(a2));
		CHECK(pos(a2) < pos(b2));
		// z is fully eliminated, not merely displaced: no surviving
		// atom has its shape (plain, no arithmetic operator, no ref).
		CHECK(tau::get(r).find_top(is_plain_atom) == nullptr);
	}

	TEST_CASE("displaced binders never reorder across a kind boundary") {
		// Regression pin for the final-review Critical:
		// `collect_quantifier_block` absorbs skip-matched binders of
		// BOTH kinds into `displaced` while the block's own kind is
		// still unfixed -- the `kind_fixed &&` guard only applies once
		// an active quantifier has fixed the kind. `ex a all y ex z
		// (...)` puts `a` (blasteable, ∃) and `y` (frozen, ∀) both
		// ahead of the first active quantifier `z`, so both get
		// absorbed before kind_fixed. A category-only re-wrap (ignoring
		// kind) would then emit frozen outermost regardless of its
		// original kind, turning `ex a all y ...` into `all y ex a
		// ...` -- opposite-kind quantifiers do not commute, so that
		// is strictly weaker than the input, a soundness hole. `z`
		// stays eliminable and disappears entirely (unrelated to the
		// kind-boundary bug; it just needs to be there to fix the
		// block's own active kind at all, matching the reviewer's
		// failure scenario exactly).
		const char* s = "ex a all y ex z "
			"(((a:bv[2] + { 1 }:bv[2] = { 0 }:bv[2]) <-> q(y)) "
			"&& z = 0).";
		tref fm = get_nso_rr(s).value().main->get();

		// Non-vacuity: the input's own prefix really is ex-then-all,
		// and the verdicts driving this scenario -- a blasteable,
		// y frozen -- really hold, through the SAME analyse_formula the
		// 2-arg anti_prenex call below uses.
		std::vector<bool> input_kinds;
		{
			tref curr = fm;
			while (is_child_quantifier<node_t>(curr)) {
				input_kinds.push_back(
					is_child<node_t>(curr, tau::wff_ex));
				curr = tau::get(curr)[0].second();
			}
		}
		REQUIRE(input_kinds == std::vector<bool>({ true, false, true }));

		auto el = analyse_formula<node_t>(fm, analysis_context<node_t>{});

		auto is_ref_fm = [](tref n) {
			const auto& t = tau::get(n);
			return t.is(tau::wff) && t.child_is(tau::wff_ref);
		};
		tref ref_fm = tau::get(fm).find_top(is_ref_fm);
		REQUIRE(ref_fm != nullptr);
		trefs y_vars = get_free_vars<node_t>(ref_fm);
		REQUIRE(y_vars.size() == 1);
		REQUIRE(el.verdict_of(y_vars.front()) == elim_verdict::frozen);

		auto is_add_atom = [](tref n) {
			return is_atomic_fm<node_t>(n) && tau::get(n).find_top(
				is<node_t, tau::bf_add>) != nullptr;
		};
		tref atom_a = tau::get(fm).find_top(is_add_atom);
		REQUIRE(atom_a != nullptr);
		trefs a_vars = get_free_vars<node_t>(atom_a);
		REQUIRE(a_vars.size() == 1);
		REQUIRE(el.verdict_of(a_vars.front()) == elim_verdict::blasteable);

		tref r = anti_prenex<node_t>(fm, el);

		// Whatever category-driven order the surviving binders take
		// WITHIN a kind segment, the KIND SEQUENCE of the output prefix
		// must still be ex-then-all, exactly as in the input --
		// opposite-kind quantifiers never commute, so this is not
		// negotiable regardless of category. (z is eliminable and
		// disappears entirely, leaving exactly two binders.)
		std::vector<bool> output_kinds;
		tref n = tau::get(r).find_top(is_child_quantifier<node_t>);
		while (n && is_child_quantifier<node_t>(n)) {
			output_kinds.push_back(is_child<node_t>(n, tau::wff_ex));
			n = tau::get(n)[0].second();
		}
		CHECK(output_kinds == std::vector<bool>({ true, false }));
	}
}

// Task 11: leftover measured-unhit regions in antiprenexing.tmpl.h not
// owned by Task 5 -- the gamma4 lift, the mixed-verdict category-ordered
// re-wrap and its all-frozen early return, the bv-floor solver-ownership
// demotion, and resolve_quantifiers2's per-leaf solver decision.
TEST_SUITE("coverage: remaining anti-prenex arms") {

	// RESOLVED (2026-08-19): the "order-dependent BA-constant parse
	// instability" this suite documented on 2026-08-18 was root-caused to
	// tests/test_Bool_helpers.h's ba_constants<node<bv, Bool>>::get
	// specialization, which understood only "0"/"1"/"true"/"false" as
	// Bool constants and IGNORED the requested type — so `{ 2 }:bv[8]`
	// could never parse in this pack, and the apparent order effects were
	// inference-fallback noise on top of that. The helper now dispatches
	// bv-typed sources to parse_bv like the production dispatcher; the
	// former warm_bv_constants() mitigation and the doctest::skip markers
	// are gone.

	// gamma4 (antiprenexing.tmpl.h, the `an.kind == boole_atom_case::
	// independent` arm inside anti_prenex_block's Boole-decomposition
	// pivot selection): an atom whose value does not depend on the chosen
	// pivot variable is lifted out of the block's scope instead of being
	// carried into both branches of a split.
	//
	// The existing Gamma4Guard suite already covers the case where the
	// lift must be DECLINED (the atom, after substituting the pivot,
	// still mentions another block variable). This targets the successful
	// lift instead: the atom `(y|y')w = 0` mentions the pivot `y`
	// syntactically -- confirmed not to fold away at construction time by
	// Gamma4Guard's own `(y|y')x = 0` shape surviving to be analysed --
	// but both of its Boole cofactors on `y` reduce to the identical
	// `w = 0` (y|y' = 1 regardless of y's value), so it is independent of
	// y. Its residual `w = 0` mentions no OTHER block variable (`w` is
	// free, not in the block), so the guard that declines Gamma4Guard's
	// cases does not apply here and the lift actually executes. The
	// second conjunct `x y != 0` forces the whole block through Boole
	// decomposition rather than a trivial fast path (mirrors Gamma4Guard's
	// own forcing conjunct).
	TEST_CASE("gamma4 lifts a variable-independent atom") {
		const char* sample = "ex x ex y ((y|y')w = 0 && x y != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		REQUIRE( res != nullptr );
		CHECK( are_nso_equivalent<node_t>(res, fm) );
	}

	// All-frozen early return: every block variable frozen means the
	// whole block is re-wrapped verbatim (the cheap-and-exact short
	// circuit -- no destination can take any of them, so nothing below
	// it would change the outcome).
	TEST_CASE("an all-frozen block is re-wrapped verbatim") {
		const char* sample = "ex x, y (f(x) && g(y)).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		REQUIRE( res != nullptr );
		// Both binders survive around their references.
		CHECK( tau::get(res).select_top(
			is<node_t, tau::wff_ex>).size() == 1 );
		CHECK( tau::get(res).select_top(
			is<node_t, tau::wff_ref>).size() == 2 );
	}

	// The solver-ownership demotion: `process_quantifier_block`'s local
	// `ctx_arith_is_solver_owned` (NOT `analysis_context::arith_is_solver_owned`
	// -- `atom_arith_verdict` no longer branches on that at all, per the
	// pure-BA-bv directive's comment) is recomputed once, at `anti_prenex`
	// entry, as `!has_foreign_ba_constant(formula)`. A `:tau` BA constant
	// elsewhere in the SAME formula is what actually flips it: cvc5 cannot
	// translate a foreign-BA constant, so no bv scope anywhere in the
	// formula may be handed to the solver, and any variable the bv-floor
	// would otherwise seed `blasteable` must demote to `eliminable` so
	// Boole decomposition -- the only route left -- can still resolve it
	// (the issue #70 shape the demotion's own comment names: marking it
	// `blasteable` here would strand the quantifier for good). The `:tau`
	// constant is anchored to a free `:tau` variable so it survives
	// constant folding into the tree (a closed tau atom folds to T/F at
	// build time and the constant node disappears -- see the
	// reference-tau-constants-in-tests note).
	//
	TEST_CASE("bv verdicts demote when the solver does not own bv") {
		// The foreign constant must belong to a BA this pack actually
		// carries: node<bv, Bool> has no tau_ba (test_Bool_helpers.h),
		// so a `:tau` constant can never parse here — a `:bool` one is
		// this pack's solver-foreign constant.
		const char* sample =
			"(ex x : bv[8] x + x = { 2 }:bv[8]) "
			"&& y:bool = { true }:bool.";
		tref fm = get_nso_rr(sample).value().main->get();
		// Non-vacuity: the formula really does carry a foreign BA
		// constant, so ctx_arith_is_solver_owned really is false here.
		REQUIRE( has_foreign_ba_constant<node_t>(fm) );
		tref res = anti_prenex<node_t>(fm);
		REQUIRE( res != nullptr );
		// With the solver disowned (foreign :bool constant present) the
		// demotion at antiprenexing.tmpl.h:1192-1194 routes x away from
		// the (stranding) blasteable verdict. Boole decomposition has no
		// useful split for the ARITHMETIC atom x + x, so the sound
		// outcome is a KEPT binder — pin that, not full elimination.
		// (are_nso_equivalent is undecidable on this mixed content and
		// falls back to a conservative "no"; not asserted.)
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_ex>)
			!= nullptr );
	}

	// resolve_quantifiers2's per-leaf solver site: a closed bv subformula
	// is decided via the solver/BDD path inline rather than being left as
	// a residual quantifier.
	TEST_CASE("resolve pass decides a closed bv leaf via the solver") {
		const char* sample = "ex x : bv[8] x = { 1 }:bv[8].";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		REQUIRE( res != nullptr );
		CHECK( tau::get(res).equals_T() );
	}

	// Mixed-verdict segment emission: a block whose variables carry
	// different verdicts must be re-wrapped by category and stay
	// equivalent. Seed via the real analysis of a formula holding a
	// reference (frozen), a bv arithmetic atom (blasteable) and a plain
	// variable (eliminable) in one clause. `y + y = { 2 }:bv[8]` survives
	// constant folding (unlike `y + {0}`, which would fold away) and
	// carries a real bf_add operator, so atom_arith_verdict seeds it
	// blasteable rather than the pure-BA-equality `eliminable`.
	TEST_CASE("mixed-verdict block re-wraps by category") {
		// y's type must reach the parser from SOMEWHERE, or the untyped
		// `{ 2 }:bv[8]` constant has nothing to infer its width from.
		// A *binder* type (`ex y : bv[8] ...`) sharing a quantifier
		// prefix with the ref conjunct `f(x)` trips a Debug-only assert
		// inside get_nso_rr's parse call itself -- below the REQUIRE
		// guard's reach, since the abort happens mid-parse, not on a
		// returned empty optional (confirmed: 0 assertions run, straight
		// SIGABRT). Typing the variable at its OCCURRENCE instead
		// (`y:bv[8] + y = ...`) sidesteps that interaction entirely,
		// verified to parse and normalize via the tau binary; the plain
		// comma binder list (`ex x, y, z ...`) is restored since no
		// binder needs an explicit type anymore. REQUIRE on has_value()
		// is kept as a guard regardless, so any future parse regression
		// on this shape fails cleanly instead of aborting the binary.
		const char* sample =
			"ex x, y, z (f(x) && y:bv[8] + y = { 2 }:bv[8] "
			"&& z w = 0).";
		auto parsed = get_nso_rr(sample);
		REQUIRE( parsed.has_value() );
		tref fm = parsed.value().main->get();
		analysis_context<node_t> ctx;
		ctx.arith_is_solver_owned = true;
		auto el = analyse_formula<node_t>(fm, ctx);
		tref res = anti_prenex<node_t>(fm, el);
		REQUIRE( res != nullptr );
		// The formula holds a reference, so semantic equivalence is not
		// checkable; assert the category outcome structurally instead.
		// Expected: the sat bv conjunct and the eliminable conjunct
		// dissolve, and the frozen component survives with its binder.
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_ref>)
			!= nullptr );
		CHECK( tau::get(res).select_top(
			is<node_t, tau::wff_ex>).size() == 1 );
	}
}

// AP-20 / AP-21. anti_prenex's memo is Release-only (TAU_CACHE is OFF in the
// Debug preset), which is why AP-20 -- a cache keyed on the formula alone while
// the result also depends on the runtime-mutable `preprocessing` global -- had
// no regression test at all. Guarding the case on TAU_CACHE rather than on the
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

	TEST_CASE("the memo is keyed on preprocessing") {
		// An *open* bv scope over blastable arithmetic. Both properties are
		// needed: `y` free makes the leaf clause's
		// closed-and-solvable solver test fail, so the solver does not decide it
		// first, and `+ { 1 }` is arithmetic bv_predicate_blasting can actually
		// rewrite -- with `&`, or with multiplication by a non-constant,
		// blasting is a no-op and both settings give the same answer, which
		// would make this case vacuous. Observed here: blasting off keeps
		// `ex b1 b1+1 = y`, blasting on returns the bit-level expansion.
		// The old sample `ex x (x + 1 = y)` stopped discriminating: the
		// open-scope two-way solver closing (all-closed valid) now
		// decides it T under BOTH settings once bare-atom scopes are
		// routed through the leaf elimination. The extra `x != z`
		// conjunct blocks both closing rules (neither valid nor unsat),
		// so blasting-off keeps the binder and blasting-on returns the
		// bit-level expansion -- distinct answers again.
		tref fm = get_nso_rr("ex x (x:bv[4] + { 1 }:bv[4] = y:bv[4]"
			" && x != z:bv[4]).").value().main->get();
		REQUIRE( fm != nullptr );

		// Through the normalizer entry: the bare anti_prenex call no
		// longer discriminates on this shape (its blast_block declines
		// and both settings keep the binder identically) -- the
		// blasting-dependent divergence happens at the
		// eliminate_arithmetic_and_quantifiers level, which still runs through
		// (and therefore exercises) anti_prenex's per-blasting memo.
		const bool saved = preprocessing;
		preprocessing = false;
		tref off1 = normalizer<node_t>(fm);
		preprocessing = true;
		tref on = normalizer<node_t>(fm);
		preprocessing = false;
		tref off2 = normalizer<node_t>(fm);
		preprocessing = saved;

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

// AN-2: `bool` is the two-element Boolean algebra, which is *not* atomless, so
// the atomless-only squeeze constructions do not apply to it. With no positive
// atoms they take the f_0 == f_1 branch per disequation and yield T, so
// `ex x:bool (x != 0 && x' != 0)` came back as T -- while in a two-element
// algebra it is F (no x satisfies both). eliminate_block_over_clause now
// expands a bool-typed binder as the finite disjunction phi[x/0] | phi[x/1]
// instead (leaf_clause.tmpl.h); block_atom_profile's finite_ba_content guards
// paper step 2a for the same class.
TEST_SUITE("AN-2 finite BA quantifier elimination") {

	TEST_CASE("ex x:bool (x != 0 && x' != 0) is F, not T") {
		const char* sample = "ex x:bool (x != 0 && x' != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK( tau::get(res).equals_F() );
	}

	TEST_CASE("ex x:bool (x != 0) is still T (AN-2 control)") {
		const char* sample = "ex x:bool (x != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK( tau::get(res).equals_T() );
	}

	// The same formula end-to-end through the normalizer, pinning the
	// whole pipeline rather than the anti-prenex entry alone.
	TEST_CASE("normalizer: ex x:bool (x != 0 && x' != 0) is F end-to-end") {
		const char* sample = "ex x:bool (x != 0 && x' != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = normalizer<node_t>(fm);
		CHECK( tau::get(res).equals_F() );
	}

	TEST_CASE("normalizer: ex x:bool (x != 0) is T end-to-end (control)") {
		const char* sample = "ex x:bool (x != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = normalizer<node_t>(fm);
		CHECK( tau::get(res).equals_T() );
	}
}

// AN-7: the heterogeneous-BA-type runtime guard (now in
// eliminate_block_over_clause, scanning the DEPENDENT conjuncts only) was
// promoted from DBG-only to a Release guard and had zero coverage.
TEST_SUITE("AN-7 heterogeneous block guard") {

	TEST_CASE("mixed-type dependent conjuncts keep the quantifier block") {
		// A block variable typed sbf over a clause whose x-dependent
		// content mixes in a tau-typed atom: the guard must decline
		// elimination and re-wrap the block rather than build a
		// wrongly-typed term. (A mixed conjunct NOT touching the block
		// variable is lifted out first and no longer declines -- that
		// narrowing is what lets mixed tau/bv specs run, issue #70.)
		// The block variable must be the interned occurrence node --
		// the lift tests `contains(conjunct, var)` by subtree identity,
		// and a separately built binder variable would not match,
		// sending both conjuncts to the independent side.
		// (The homogeneity scan prunes below the first typed node on
		// each path, so the tau operand goes FIRST -- behind an
		// sbf-typed product head it would be invisible and the clause
		// would scan homogeneous.)
		tref x_bf = build_bf_variable<node_t>("x", sbf_type_id<node_t>());
		tref x = tau::trim(x_bf);
		tref clause = tau::build_wff_and(
			tau::build_bf_eq_0(x_bf),
			tau::build_bf_eq_0(tau::build_bf_and(
				build_bf_variable<node_t>("y",
					tau_type_id<node_t>()),
				x_bf)));
		term_handle<node_t>::order order;
		tref res = eliminate_block_over_clause<node_t>(clause,
			trefs{ x }, block_eliminability<node_t>{}, order);
		REQUIRE( res != nullptr );
		// The quantifier survives (elimination declined).
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_ex>)
			!= nullptr );
	}
}
