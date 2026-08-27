// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("anti_prenex") {
	TEST_CASE("1") {
		const char* sample = "ex v { o1[t]o2[t] = 0 } : tau ({ o1[t]o2[t] = 0 } : tau v)' = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK(tau::get(res).equals_T());
	}
	TEST_CASE("2") {
		const char* sample = "all o1[0], o2[0] !o1[0]o2[0] = 0 || o1[0]o2[0] = 0 && (ex o2[1], o1[1] o1[1]o2[1] = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK(tau::get(res).equals_T());
	}
	TEST_CASE("3") {
		const char* sample = "all y !({ <:a><:d><:c>|<:a>'<:d><:c>|<:b> = 0 } : tau y != 0 ) || { <:a><:b>|<:c><:d> = 0 } : tau y != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK(tau::get(res).equals_T());
	}
	TEST_CASE("4") {
		const char* sample = "{!(always <:a>&(<:b>|<:d><:c>)|<:a>'<:d><:c> = 0)}&{always <:b>|<:d><:c> = 0} != 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
	// The next three cases originally pinned the legacy algorithm's B4
	// squeeze/absorb; since its deletion they pin the block pipeline on the
	// same inputs. The expected shapes changed with the switch -- each new
	// shape was checked equivalent to its input by hand (the conservative
	// are_nso_equivalent cannot decide reference-carrying formulas) before
	// being added here, per the redesign's acceptance rule: expectations may
	// be rewritten, semantics may not. The old shapes are kept in the lists
	// deliberately -- they are equivalent too, and a future simplification
	// improvement may legitimately return to them.
	TEST_CASE("b4 squeeze_absorb below ex") {
		const char* sample = "ex x (((xyz = 0 && xw = 0 && f(x)) || w = 0 || xyz != 0) && xy = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		// Order flipped again by the 2026-08-27 parser regen (left-assoc arithmetic + cast disambiguation).
		CHECK( matches_to_str_to_any_of(res, {
			// complete_quantifier_elimination (the residual-quantifier
			// fallback added when this variable occurs only in a
			// non-negated pivot-less shape): a single disjunct, folding
			// `w = 0` into the kept scope instead of factoring it out.
			// Equivalent by hand: under `b1 y = 0`, `b1 yz != 0` is
			// unsatisfiable (b1 yz = (b1 y) z = 0), so the scope reduces
			// to `b1 w = 0 && (w = 0 || f(b1))`, i.e.
			// `(w = 0 && ex b1 (b1 y = 0 && b1 w = 0)) || ex b1 (b1 y = 0
			// && b1 w = 0 && f(b1))`; the first disjunct's existential is
			// a tautology (b1 = 0), so it collapses to
			// `w = 0 || (ex b1 b1 w = 0 && b1 y = 0 && f(b1))` -- the
			// pre-deletion shape below. Canonical (produced) shape FIRST:
			// Debug's matches_to_any_of only checks expected[0].
			"ex b1 b1 y = 0 && b1 w = 0 && (b1 yz != 0 || w = 0 || f(b1))",
			"ex b1 b1 w = 0 && b1 y = 0 && (b1 yz != 0 || f(b1) || w = 0)",
			// the same single disjunct with the ltl-side pivot tie-break
			// order (conjuncts and disjuncts permuted; equivalent by
			// commutativity of the hand-check above).
			
			// bare-atom leaf routing + the fallback: same two disjuncts
			// as the 2026-08-04 shape below, with disjunct and conjunct
			// order flipped by the pivot tie-breaks; equivalent by the
			// same hand-check.
			"(ex b1 b1 y = 0 && b1 w != 0 && (b1 yz != 0 || w = 0)) "
			"|| (ex b1 b1 y = 0 && b1 w = 0 && (b1 yz != 0 || w = 0 || f(b1)))",
			// block pipeline, 2026-08-04: carries a redundant second
			// disjunct (its two conjuncts force w = 0 and w != 0, so
			// it is F) and an unabsorbed b1 yz != 0 literal (dead
			// under b1 y = 0); verified equivalent by hand.
			"(ex b1 b1 w = 0 && b1 y = 0 && (b1 yz != 0 || w = 0 || f(b1))) "
			"|| (ex b1 b1 y = 0 && b1 w != 0 && (b1 yz != 0 || w = 0))",
			// pre-deletion shapes, equivalent; a future simplification
			// improvement may legitimately return to them.
			"w = 0 || (ex b1 b1 w = 0 && b1 y = 0 && f(b1))",
			"w = 0 || (ex b1 b1 y = 0 && b1 w = 0 && f(b1))",
			"(ex b1 b1 w = 0 && b1 y = 0 && f(b1)) || w = 0",
			"(ex b1 b1 y = 0 && b1 w = 0 && f(b1)) || w = 0",
		}) );
	}
	TEST_CASE("b4 squeeze_absorb below all") {
		const char* sample = "all x !((((xyz = 0 && xw = 0 && f(x)) || w = 0 || xyz != 0) && xy = 0)).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			// complete_quantifier_elimination's shape, dual of the ex
			// case above: process_quantifier_block dualises an all-block
			// by resolving the negated scope as an ex-block and negating
			// back (`to_nnf(neg(pushed))`), so this is exactly `!(ex-case
			// result)` renamed to NNF -- sound for the same reason the ex
			// shape is, by construction, independent of what shape the
			// wrapped ex-elimination happens to return. Canonical
			// (produced) shape FIRST: Debug's matches_to_any_of only
			// checks expected[0].
			"(all b1 b1 y != 0 || b1 w != 0 || b1 yz = 0 && w != 0 && !f(b1)) "
			"&& (w != 0 || wy' = 0)",
			"(all b1 b1 w != 0 || b1 y != 0 || b1 yz = 0 && w != 0 && !f(b1)) "
			"&& (w != 0 || wy' = 0)",
			// the same two conjuncts with the ltl-side pivot tie-break
			// order (disjuncts permuted; equivalent by commutativity).
			
			// bare-atom leaf routing + the fallback: dual of the ex
			// case, conjunct/disjunct order flipped by the pivot
			// tie-breaks; equivalent by the same hand-check.
			"(all b1 b1 y != 0 || b1 w = 0 || b1 yz = 0 && w != 0) "
			"&& (all b1 b1 y != 0 || b1 w != 0 || b1 yz = 0 && w != 0 && !f(b1))",
			// block pipeline, 2026-08-04: the dual, second conjunct is
			// identically T, and the first folds to the old shape
			// (b1 = 0 forces w != 0); verified equivalent by hand.
			"(all b1 b1 w != 0 || b1 y != 0 || b1 yz = 0 && w != 0 && !f(b1)) "
			"&& (all b1 b1 y != 0 || b1 w = 0 || b1 yz = 0 && w != 0)",
			// pre-deletion shapes, equivalent.
			"w != 0 && (all b1 b1 w != 0 || b1 y != 0 || !f(b1))",
			"w != 0 && (all b1 b1 y != 0 || b1 w != 0 || !f(b1))",
			"(all b1 b1 w != 0 || b1 y != 0 || !f(b1)) && w != 0",
			"(all b1 b1 y != 0 || b1 w != 0 || !f(b1)) && w != 0",
		}) );
	}
	TEST_CASE("b4 squeeze_absorb below all, fully eliminated") {
		// equivalence guard: same scope under a plain all resolves
		// completely, with and without the squeeze
		const char* sample = "all x (((xyz = 0 && xw = 0 && f(x)) || w = 0 || xyz != 0) && xy = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		// Order flipped again by the 2026-08-27 parser regen (left-assoc arithmetic + cast disambiguation).
		CHECK( matches_to_str_to_any_of(res, {
			// disjunct order flipped by the 8f1a74c1 parser regen
			// (Debug's matches_to_any_of only checks expected[0] --
			// see test_helpers.h); actual current shape first.
			"y = 0 && ((all b1 b1 yz != 0 || b1 w = 0 && f(b1)) || w = 0)",
			"y = 0 && (w = 0 || (all b1 b1 yz != 0 || b1 w = 0 && f(b1)))",
			// block pipeline, 2026-08-04 (canonical shape first):
			// under y = 0 the kept universal reduces to
			// w = 0 && (all b1 f(b1)), whose disjunction with w = 0
			// is w = 0 -- so this is y = 0 && w = 0 in a bulkier
			// spelling; verified equivalent by hand.
			
			// pre-deletion shapes, equivalent.
			"y = 0 && w = 0",
			"w = 0 && y = 0",
		}) );
	}

	// complete_quantifier_elimination branch coverage (added with the
	// bc99a82b port). The block pipeline's pivot selection only splits on
	// non-negated atoms, so a variable occurring solely in `!=` atoms
	// reaches the fallback; each case below pins one branch of the fallback
	// so a regression in any of them fails a test instead of passing as a
	// conservatively-undecided formula.
	TEST_CASE("cqe: neq-starved ex block is eliminated") {
		const char* sample = "ex b (by != 0 && bz != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"z != 0 && y != 0",
		}) );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}
	TEST_CASE("cqe: neq-starved all block is eliminated via dualization") {
		const char* sample = "all b (by = 0 || bz = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"z = 0 || y = 0",
		}) );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}
	TEST_CASE("cqe: disjunctive scope distributes per clause") {
		const char* sample = "ex b (by != 0 && bz != 0 || bw != 0 && bu != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"z != 0 && y != 0 || u != 0 && w != 0",
		}) );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}
	TEST_CASE("cqe: scope over the clause cap keeps its quantifier") {
		// 2 CNF factors, naive product 4 > cap 3 -> cqe must decline and
		// re-wrap verbatim. Called DIRECTLY: through anti_prenex the block
		// pipeline would settle the scope before cqe is ever reached;
		// atoms are canonicalized first, as anti_prenex's step 3 does.
		const char* sample = "ex b ((by != 0 || z != 0) && (bx != 0 || w != 0)).";
		tref fm = normalize_atomic_formula_operators<node_t>(
			get_nso_rr(sample).value().main->get());
		cqe_max_clauses = 3;
		tref r = complete_quantifier_elimination<node_t>(fm);
		cqe_max_clauses = std::numeric_limits<size_t>::max();
		CHECK( tau::get(r).find_top(is_quantifier<node_t>) != nullptr );
	}
	TEST_CASE("cqe: a scope that needs distributing is still eliminated") {
		// The reducing to_dnf rebuilds negated atoms as `!=`; cqe must
		// re-canonicalize them or the per-clause squeeze declines every
		// clause and re-wraps the quantifier (latent until 2026-08-27,
		// since the cases above hand cqe scopes that are already DNF).
		const char* sample = "ex b ((by != 0 || z != 0) && (bx != 0 || w != 0)).";
		tref fm = normalize_atomic_formula_operators<node_t>(
			get_nso_rr(sample).value().main->get());
		tref r = complete_quantifier_elimination<node_t>(fm);
		CHECK( tau::get(r).find_top(is_quantifier<node_t>) == nullptr );
		// (x != 0 || z != 0) && (y != 0 || w != 0): 4 two-atom clauses,
		// order-insensitive since cqe's own clause order is hash-driven.
		trefs clauses = get_dnf_wff_clauses<node_t>(r);
		CHECK( clauses.size() == 4 );
		for (tref c : clauses)
			CHECK( get_cnf_wff_clauses<node_t>(c).size() == 2 );
	}
	TEST_CASE("cqe: v-free conjuncts are not distributed") {
		// 4 CNF factors, only the last mentions b. Full distribution is
		// 2^4 = 16 clauses (over cap 8); miniscoped, the b-residue is a
		// single 2-clause factor and elimination proceeds.
		cqe_max_clauses = 8;
		const char* sample = "ex b ((p != 0 || q != 0) && (r != 0 || s != 0) "
			"&& (t != 0 || u != 0) && (by != 0 || bz != 0)).";
		tref fm = normalize_atomic_formula_operators<node_t>(
			get_nso_rr(sample).value().main->get());
		tref r = complete_quantifier_elimination<node_t>(fm);
		cqe_max_clauses = std::numeric_limits<size_t>::max();
		CHECK( tau::get(r).find_top(is_quantifier<node_t>) == nullptr );
		// The b-free factors survive verbatim (not multiplied out).
		CHECK( get_cnf_wff_clauses<node_t>(r).size() >= 4 );
	}
	TEST_CASE("cqe: nested starved quantifiers resolve innermost-first") {
		const char* sample = "ex a, b (ab != 0 && ay != 0 && bz != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"z != 0 && y != 0",
		}) );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
	}
	TEST_CASE("cqe: NZ-1 temporal scope keeps its quantifier") {
		// The grammar has no quantifier-over-always position, so build the
		// NZ-1 shape the way the pipeline meets it: internally.
		tref spec = get_nso_rr("always o1[t]b != 0.").value().main->get();
		const trefs& fv = get_free_vars<node_t>(spec);
		REQUIRE( !fv.empty() );
		tref fm = tau::build_wff_all_many(fv, spec);
		tref res = anti_prenex<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"all b2, b1 (always b1 b2 != 0)",
		}) );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) != nullptr );
		CHECK( tau::get(res).find_top(
			is_child<node_t, tau::wff_always>) != nullptr );
	}
	TEST_CASE("cqe: wff_ref scope is frozen verbatim") {
		const char* sample = "ex b (bw != 0 && q(b)).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"ex b1 b1 w != 0 && q(b1)",
		}) );
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) != nullptr );
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_ref>) != nullptr );
	}
	TEST_CASE("cqe: tau constant internals are not entered") {
		const char* sample = "ex b (by != 0 && bz != 0) &&"
			" { (ex v o1[t]v = 0) && o2[t] = 0 } : tau x = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex<node_t>(fm);
		// No exact-shape pin here: a formula holding a tau constant prints
		// with run-to-run conjunct order (node ordering compares hashes
		// first, and the constant's tau_ba hash is allocation-order
		// dependent -- the known pivot-order-sensitivity follow-up), so
		// assert order-insensitively: the starved outer quantifier is
		// gone from the tree, while the constant still displays with its
		// internal quantifier verbatim (constant internals are pool
		// values, not tree children, so find_top cannot see that one).
		const std::string out = tau::get(res).to_str();
		CHECK( tau::get(res).find_top(is_quantifier<node_t>) == nullptr );
		CHECK( out.find("ex b1") != std::string::npos );
		CHECK( out.find("z != 0") != std::string::npos );
		CHECK( out.find("y != 0") != std::string::npos );
	}

	// Test to see the blow up caused by quantified free function symbols
	// In particular conversion to Boole normal form causes blow up
	// TEST_CASE("5") {
	// 	const char* sample = "all x ex y (f1(x,y)=0 && g1(x,y)!=0 && h1(x,y)!=0) || (f2(x,y)=0 && g2(x,y)!=0 && h2(x,y)!=0).";
	// 	tref fm = get_nso_rr(sample).value().main->get();
	// 	tref res = anti_prenex<node_t>(fm);
	// 	std::cout << "res: " << tau::get(res) << "\n";
	// 	res = boole_normal_form<node_t>(res);
	// 	std::cout << "res: " << tau::get(res) << "\n";
	// 	CHECK(true);
	// }
}

TEST_SUITE("AntiPrenexBlockPipeline") {
	// These tests exercise anti_prenex_block through the full normalize
	// pipeline (normalize_non_temp). Single-level quantifiers:
	TEST_CASE("ex_all_single: ex x all y xy=0 → T") {
		// ∃x. ∀y. xy=0 : pick x=0, then 0·y=0 for all y
		CHECK( normalize_and_check("ex x all y xy = 0.", tau::wff_t) );
	}
	TEST_CASE("all_ex_single: all x ex y xy=0 → T") {
		// ∀x. ∃y. xy=0 : pick y=0, then x·0=0 for any x
		CHECK( normalize_and_check("all x ex y xy = 0.", tau::wff_t) );
	}
	TEST_CASE("ex_all_neg: ex x all y xy!=0 → F") {
		// ∃x. ∀y. xy≠0 : for any x, pick y=0 → x·0=0, contradiction
		CHECK( normalize_and_check("ex x all y xy != 0.", tau::wff_f) );
	}
	TEST_CASE("all_ex_neg: all x ex y xy!=0 → F") {
		// ∀x. ∃y. xy≠0 : for x=0, need y s.t. 0≠0, impossible
		CHECK( normalize_and_check("all x ex y xy != 0.", tau::wff_f) );
	}
	// Two-level quantifier alternation:
	TEST_CASE("all_ex_all: all x ex y all z xyz=0 → T") {
		// pick y=0: x·0·z=0 for all x,z
		CHECK( normalize_and_check("all x ex y all z xyz = 0.", tau::wff_t) );
	}
	TEST_CASE("ex_all_ex: ex x all y ex z xyz!=0 → F") {
		// for x=0: 0·y·z=0 for all y,z, so can never be ≠0
		CHECK( normalize_and_check("ex x all y ex z xyz != 0.", tau::wff_f) );
	}
	// Mixed with disjunction/conjunction (exercises B11/B12/B13):
	TEST_CASE("ex_all_disjunction: ex x all y (xy=0 || x'y=0) → T") {
		// ∃x=0: 0·y=0 for all y ✓ (disjunct 1 satisfied)
		CHECK( normalize_and_check("ex x all y (xy = 0 || x'y = 0).", tau::wff_t) );
	}
	TEST_CASE("all_ex_conjunction: all x ex y (xy=0 && x'y=0) → T") {
		// pick y=0: x·0=0 and x'·0=0 for any x ✓
		CHECK( normalize_and_check("all x ex y (xy = 0 && x'y = 0).", tau::wff_t) );
	}
	// subs_elim path: ex x (x=t && phi(x)) → phi(t)
	TEST_CASE("subs_elim: ex x (xy=0 && x=w) → wy=0") {
		// Step 2 of anti_prenex_block substitutes x:=w giving wy=0.
		CHECK( normalize_and_check("ex x (xy = 0 && x = w).",
			strings{"wy = 0", "yw = 0"}) );
	}
	TEST_CASE("subs_elim: ex x (x=w) → T") {
		// After substitution the body reduces to T.
		CHECK( normalize_and_check("ex x (x = w).", tau::wff_t) );
	}
	// trivial_skolem path: an eligible atom under wff_or is unreachable by
	// subs_elim (which bails on any wff_or in scope), so only the
	// trivial_skolem wiring in process_quantifier_block can remove x here.
	TEST_CASE("trivial_skolem: ex x (x=w || z=0) → T") {
		CHECK( normalize_and_check("ex x (x = w || z = 0).", tau::wff_t) );
	}
	// Regression tests: a BV-typed atom unrelated to the (non-BV) block
	// variable must not force the whole block to fall back to anti_prenex,
	// nor be dropped/mishandled when it does get pulled into the block's
	// Boole-decomposition candidate set.
	TEST_CASE("bv atom alongside block var: ex x ((xy=0||Z=3) && xw=0) -> T") {
		// x = 0 makes both xy = 0 and xw = 0 true regardless of y, w, Z:
		// tautology, independently of how the unrelated Z:bv[8] atom is
		// handled.
		CHECK( normalize_and_check(
			"ex x ((xy = 0 || Z:bv[8] = { 3 }:bv[8]) && xw = 0).",
			tau::wff_t) );
	}
	TEST_CASE("bv atom survives block elimination: contradicts outer Z != 3") {
		// The BV constraint Z = 3 pulled inside the block must still
		// propagate out intact; ANDed with Z != 3 outside, the whole
		// formula is unsatisfiable.
		CHECK( normalize_and_check(
			"(ex x (xy = 0 && xw = 0 && Z:bv[8] = { 3 }:bv[8])) "
			"&& Z:bv[8] != { 3 }:bv[8].",
			tau::wff_f) );
	}
}
