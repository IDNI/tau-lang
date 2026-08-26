// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The two opposite atom-spelling preconditions that meet in this module:
//
//   squeeze_positives (normal_forms_transformations.tmpl.h:382-385) DBG-asserts
//   its input holds no wff_neg(bf_eq). Feeding it `!(g = 0)` makes its
//   select_top descend through the wff_neg and fold the equation into the
//   POSITIVE squeeze, while the disequation scan never re-adds the negation --
//   so `ex x (x a = 0 && !(x b = 0))` collapses to T, the disequation inverted
//   and dropped.
//
//   The block squeeze requires the OPPOSITE: a bf_neq-free clause, which
//   normalize_atomic_formula_operators establishes by rewriting `!= 0` into
//   `!(= 0)`.
//
// Neither spelling can be made global -- squeeze_positives has a second caller
// at interpreter.tmpl.h:1156. Each precondition is established locally at its
// point of use. These cases pin both paths against the collapse.

#include "test_init.h"
#include "test_tau_helpers.h"

// These were written against BOTH public entry points while the legacy
// step-based algorithm still existed; since its deletion there is one entry,
// the block pipeline, which now carries the `anti_prenex` name.
namespace {

tref parse(const char* sample) {
	return get_nso_rr(sample).value().main->get();
}

} // namespace

TEST_SUITE("leaf_clause") {

	// The hazard reached through the single-variable path.
	TEST_CASE("a disequation is not silently dropped, single variable") {
		{
			auto run = [](tref f) { return anti_prenex<node_t>(f); };
			CHECK( !tau::get(run(parse(
				"ex x (x a = 0 && !(x b = 0))."))).equals_T() );
		}
	}

	// The same hazard reached through the block path, which has the
	// opposite precondition.
	TEST_CASE("a disequation is not silently dropped, block") {
		{
			auto run = [](tref f) { return anti_prenex<node_t>(f); };
			CHECK( !tau::get(run(parse(
				"ex x, y (x y a = 0 && !(x y b = 0))."))).equals_T() );
		}
	}

	// Both spellings of the same disequation must reach the same answer.
	TEST_CASE("the bf_neq spelling gives the same answer as !(= 0)") {
		{
			auto run = [](tref f) { return anti_prenex<node_t>(f); };
			tref a = run(parse("ex x (x a = 0 && x b != 0)."));
			tref b = run(parse("ex x (x a = 0 && !(x b = 0))."));
			CHECK( tau::get(a) == tau::get(b) );
		}
	}

	// A conjunct free of every block variable does not belong under the
	// binder, and the binder itself is fully eliminable here.
	TEST_CASE("an independent conjunct is lifted out of the binder") {
		{
			auto run = [](tref f) { return anti_prenex<node_t>(f); };
			CHECK( !tau::get(run(parse("ex x (x a = 0 && w = 0).")))
				.find_top(is<node_t, tau::wff_ex>) );
		}
	}
}

// Direct calls: the block driver diverts skip-matched blocks to blast_block
// before a clause ever reaches the blasteable loop in
// eliminate_block_over_clause, so these paths are reachable ONLY by calling
// the function directly (leaf_clause.tmpl.h:310-362 and neighbors).
TEST_SUITE("leaf_clause direct calls") {

	// Strip `ex x ex y BODY` into (block, body) like QuantBlockPush does.
	static std::pair<trefs, tref> strip_block(tref fm, size_t n) {
		trefs block;
		for (size_t i = 0; i < n; ++i) {
			block.push_back(tau::get(fm)[0].first());
			fm = tau::get(fm)[0].second();
		}
		return { block, fm };
	}

	// Build a block_eliminability the way the production driver does
	// (antiprenexing.tmpl.h:1176-1197): copy each block variable's
	// verdict from the FORMULA-level analysis (analyse_formula, run over
	// the whole quantified sample) rather than recomputing over just the
	// stripped body. This is load-bearing, not a style choice:
	// analyse_block re-derives verdicts from a fresh per-conjunct
	// union-find over `body` alone and does not reproduce the
	// formula-level scoped analysis's verdicts for bv arithmetic content
	// -- it returned `eliminable` where analyse_formula (matching
	// production) returns `blasteable` for the same variable. `elim`'s
	// `components` map is deliberately left empty, exactly as production
	// leaves it: eliminate_block_over_clause re-derives the reserved
	// conjunct set structurally from the clause in front of it.
	static block_eliminability<node_t> production_style_elim(
		const eliminability<node_t>& el, const trefs& block)
	{
		block_eliminability<node_t> elim{};
		for (tref v : block) {
			elim_verdict vd = el.verdict_of(v);
			elim.verdicts.emplace(v, vd);
			elim.members[vd].insert(v);
		}
		return elim;
	}

	// The BODY-side occurrence of a variable `strip_block` unbinds from
	// `fm`, as opposed to the binder-side node `strip_block` itself
	// returns (`tau::get(fm)[0].first()`, the bare bound-variable slot
	// inside the quantifier node). The two can differ by inferred BA
	// type -- test_eliminability.cpp's own `conj_vars()` comment warns of
	// exactly this -- and `analyse_formula`/`analyse_block` key verdicts
	// on whichever occurrence they actually walked, i.e. the body-side
	// one. Found as the one free variable `body` has that the still-fully
	// -quantified `fm` does not: robust to however many other variables
	// are already free in the body, and independent of the binder-side
	// node's own identity.
	static tref newly_freed_var(tref fm, tref body) {
		trefs before = get_free_vars<node_t>(fm);
		for (tref v : get_free_vars<node_t>(body)) {
			bool was_free = false;
			for (tref b : before)
				if (tau::get(b) == tau::get(v)) { was_free = true; break; }
			if (!was_free) return v;
		}
		return nullptr;
	}

	TEST_CASE("a T clause is returned unchanged") {
		tref x = tau::get(parse("ex x x = 0."))[0].first();
		term_handle<node_t>::order order;
		tref res = eliminate_block_over_clause<node_t>(
			tau::_T(), { x },
			block_eliminability<node_t>{}, order);
		CHECK( tau::get(res).equals_T() );
	}

	TEST_CASE("an F clause is returned unchanged") {
		tref x = tau::get(parse("ex x x = 0."))[0].first();
		term_handle<node_t>::order order;
		tref res = eliminate_block_over_clause<node_t>(
			tau::_F(), { x },
			block_eliminability<node_t>{}, order);
		CHECK( tau::get(res).equals_F() );
	}

	TEST_CASE("non-eliminable variables reserve their conjuncts") {
		// x frozen by the reference, y free of it: the frozen
		// component's conjunct must survive under x's binder, y's
		// binder must go (leaf_clause.tmpl.h:163-171 reservation).
		// Same sample/assertions as
		// test_integration-eliminability.cpp's "a reference freezes
		// only its own component", reached there through anti_prenex
		// and here through a direct call.
		auto [block, body] = strip_block(
			parse("ex x, y (f(x) && y w = 0)."), 2);
		// Build verdicts through the real analysis. get_wff_conjuncts
		// does not exist; get_cnf_wff_clauses is the conjunct
		// splitter eliminate_block_over_clause itself uses (and what
		// analyse_block's own tests build their `conjuncts` argument
		// with the equivalent of, one conjunct at a time).
		auto conjs = get_cnf_wff_clauses<node_t>(body);
		auto el = analyse_block<node_t>(block, conjs,
			analysis_context<node_t>{});
		term_handle<node_t>::order order;
		tref res = eliminate_block_over_clause<node_t>(
			body, block, el, order);
		REQUIRE( res != nullptr );
		// The reference and exactly one surviving binder remain.
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_ref>) );
		CHECK( tau::get(res).select_top(
			is<node_t, tau::wff_ex>).size() == 1 );
	}

	// Empirically confirmed 2026-08-17: leaf_clause.tmpl.h:317-362 (the
	// eager-solver + per-leaf-blast loop) is unreachable even by a direct
	// call carrying production-faithful verdicts (built exactly as
	// production_style_elim / antiprenexing.tmpl.h:1176-1197 do), because
	// the kept_set gate at leaf_clause.tmpl.h:162-165
	// (`elim.verdict_of(v) != elim_verdict::eliminable`) catches every
	// non-eliminable verdict -- `blasteable` included -- and reserves the
	// variable's conjunct before `still_live` (and so the solver loop)
	// ever sees it. `elim.verdict_of` is a pure lookup queried
	// identically at both sites, so a variable can never be simultaneously
	// "not kept" (required to reach `still_live`) and "blasteable"
	// (required to pass line 316's `continue`). This is not a product bug
	// -- keeping the binder here is sound -- it is guarded future-proofing
	// that is dead code today. The two cases below pin the ACTUAL
	// (kept-binder) behaviour instead of the solver decision the region's
	// own comment implies a direct call would reach. These pins must FLIP
	// to `equals_T()`/`equals_F()` assertions if the kept_set gate is ever
	// narrowed to frozen-only (or the legacy rewire the region's comment
	// anticipates lands) and the solver loop becomes reachable.
	TEST_CASE("blasteable closed conjunct: binder is kept, "
		"not solver-decided (dead region pin)")
	{
		// Single bv variable, closed once bound -- would be exactly the
		// shape the eager solver branch (leaf_clause.tmpl.h:326-341)
		// is meant for, if that branch were reachable.
		//
		// `elim` is built by production_style_elim, from analyse_formula
		// over the FULL sample -- see its comment for why analyse_block
		// over the stripped body alone is not equivalent here.
		//
		// Spelled `x + x = ...` rather than a bare `x = { 1 }:bv[8]` or
		// `x + { 0 }:bv[8] = ...`: a bare equation carries no arithmetic
		// operator, so detail::atom_arith_verdict (eliminability.tmpl.h)
		// classifies it `eliminable`, not `blasteable`; `x + { 0 }` DOES
		// carry one syntactically, but the bv constant-folding hooks
		// collapse `x + { 0 }` to `x` at parse time, leaving the exact
		// same fold-free equality and the same `eliminable` verdict
		// (confirmed by build: the REQUIRE below failed on that
		// spelling). `x + x` cannot fold away like that, so the
		// arithmetic operator survives to seed `blasteable`, which the
		// REQUIRE below pins. It also keeps ex_subs_based_elimination's
		// witness search from firing: it only matches an equation with
		// the bare variable alone on one side (`var = t`), not
		// `var + var = t`, so the clause survives past the substitution
		// loop to the (unreachable) solver branch's gate instead of
		// being resolved earlier.
		tref fm = parse("ex x : bv[8] x + x = { 2 }:bv[8].");
		analysis_context<node_t> ctx; ctx.arith_is_solver_owned = true;
		auto el = analyse_formula<node_t>(fm, ctx);
		auto [binder_block, body] = strip_block(fm, 1);
		tref v = newly_freed_var(fm, body);
		REQUIRE( v != nullptr );
		// Invariant: the binder-side variable node and the body-side
		// occurrence must be interned identically, or verdict_of's
		// binder-side lookup would miss.
		CHECK( tau::get(binder_block[0]) == tau::get(v) );
		auto elim = production_style_elim(el, { v });
		REQUIRE( elim.verdict_of(v) == elim_verdict::blasteable );
		term_handle<node_t>::order order;
		tref res = eliminate_block_over_clause<node_t>(
			body, { v }, elim, order);
		REQUIRE( res != nullptr );
		// The binder survives -- no solver decision was reached.
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_ex>) );
		CHECK( !tau::get(res).equals_T() );
		CHECK( !tau::get(res).equals_F() );
	}

	TEST_CASE("blasteable closed conjunct: binder is kept, "
		"not solver-decided (dead region pin), unsat shape")
	{
		// 2x is even mod 256 for every x, so it can never equal the odd
		// constant 1 -- would be unsat if the eager solver branch were
		// reachable. `x + x`, not `x + { 0 }`, for the same
		// no-constant-folding reason as the case above.
		tref fm = parse("ex x : bv[8] x + x = { 1 }:bv[8].");
		analysis_context<node_t> ctx; ctx.arith_is_solver_owned = true;
		auto el = analyse_formula<node_t>(fm, ctx);
		auto [binder_block, body] = strip_block(fm, 1);
		tref v = newly_freed_var(fm, body);
		REQUIRE( v != nullptr );
		CHECK( tau::get(binder_block[0]) == tau::get(v) );
		auto elim = production_style_elim(el, { v });
		REQUIRE( elim.verdict_of(v) == elim_verdict::blasteable );
		term_handle<node_t>::order order;
		tref res = eliminate_block_over_clause<node_t>(
			body, { v }, elim, order);
		REQUIRE( res != nullptr );
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_ex>) );
		CHECK( !tau::get(res).equals_T() );
		CHECK( !tau::get(res).equals_F() );
	}

	TEST_CASE("open bv conjunct: the binder is kept, not dropped") {
		// Two bv variables, only one in the block: not closed, and with
		// blasting off the keep-binder fall-through
		// (leaf_clause.tmpl.h:361-362 / 486-491) must retain ex.
		struct bv_blasting_guard {
			bool saved = bv_blasting;
			~bv_blasting_guard() { bv_blasting = saved; }
		} guard;
		bv_blasting = false;
		tref fm = parse("ex x : bv[8] x + x = y.");
		analysis_context<node_t> ctx; ctx.arith_is_solver_owned = true;
		auto el = analyse_formula<node_t>(fm, ctx);
		auto [binder_block, body] = strip_block(fm, 1);
		// x is the one variable body has free that fm (x still bound)
		// does not; y was already free in fm and is filtered out.
		tref v = newly_freed_var(fm, body);
		REQUIRE( v != nullptr );
		CHECK( tau::get(binder_block[0]) == tau::get(v) );
		auto elim = production_style_elim(el, { v });
		REQUIRE( elim.verdict_of(v) == elim_verdict::blasteable );
		term_handle<node_t>::order order;
		tref res = eliminate_block_over_clause<node_t>(
			body, { v }, elim, order);
		REQUIRE( res != nullptr );
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_ex>) );
	}
}
