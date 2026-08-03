// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("eliminability") {

	TEST_CASE("join is idempotent") {
		CHECK(join(elim_verdict::eliminable, elim_verdict::eliminable)
			== elim_verdict::eliminable);
		CHECK(join(elim_verdict::frozen, elim_verdict::frozen)
			== elim_verdict::frozen);
	}

	TEST_CASE("eliminable is the identity") {
		CHECK(join(elim_verdict::eliminable, elim_verdict::solver_owned)
			== elim_verdict::solver_owned);
		CHECK(join(elim_verdict::arith_residue, elim_verdict::eliminable)
			== elim_verdict::arith_residue);
	}

	TEST_CASE("frozen is the top element") {
		// A reference makes content untouchable by *any* destination,
		// including the solver -- so nothing demotes frozen.
		CHECK(join(elim_verdict::frozen, elim_verdict::eliminable)
			== elim_verdict::frozen);
		CHECK(join(elim_verdict::frozen, elim_verdict::solver_owned)
			== elim_verdict::frozen);
		CHECK(join(elim_verdict::frozen, elim_verdict::arith_residue)
			== elim_verdict::frozen);
	}

	TEST_CASE("arith_residue outranks solver_owned") {
		// Arithmetic cvc5 cannot express must not be routed to it: the
		// narrower verdict wins where both apply.
		CHECK(join(elim_verdict::solver_owned, elim_verdict::arith_residue)
			== elim_verdict::arith_residue);
		CHECK(join(elim_verdict::arith_residue, elim_verdict::solver_owned)
			== elim_verdict::arith_residue);
	}

	TEST_CASE("join is commutative across every pair") {
		const elim_verdict all[] = { elim_verdict::eliminable,
			elim_verdict::solver_owned, elim_verdict::arith_residue,
			elim_verdict::frozen };
		for (elim_verdict a : all) for (elim_verdict b : all)
			CHECK(join(a, b) == join(b, a));
	}

	// Helper: the free variables of a parsed conjunct, so tests never have to
	// guess whether a hand-built tau::build_variable matches a parsed one
	// (they can differ by inferred BA type).
	static trefs conj_vars(tref c) { return get_free_vars<node_t>(c); }

	TEST_CASE("a variable sharing no atom with a reference is eliminable") {
		// x reaches f; y does not.
		tref c1 = get_nso_rr("f(x).").value().main->get();
		tref c2 = get_nso_rr("y w = 0.").value().main->get();
		trefs vars = conj_vars(c1);
		vars.push_back(conj_vars(c2)[0]);
		auto a = analyse_block<node_t>(vars, { c1, c2 },
			analysis_context<node_t>{});
		CHECK(a.has_reference());
		CHECK(a.verdict_of(conj_vars(c1)[0]) == elim_verdict::frozen);
		CHECK(a.verdict_of(conj_vars(c2)[0]) == elim_verdict::eliminable);
	}

	TEST_CASE("frozen propagates across a shared atom, not just into arguments") {
		// z is not an argument of f, but shares the atom `x z = 0` with x,
		// which is. This is the case the conjunct-level guards missed --
		// entanglement *across* conjuncts through a shared atom.
		tref c1 = get_nso_rr("f(x).").value().main->get();
		tref c2 = get_nso_rr("x z = 0.").value().main->get();
		trefs vars = conj_vars(c2); // {x, z} in some order
		auto a = analyse_block<node_t>(vars, { c1, c2 },
			analysis_context<node_t>{});
		for (tref v : vars) CHECK(a.verdict_of(v) == elim_verdict::frozen);
	}

	TEST_CASE("conjuncts_of returns exactly the component's conjuncts") {
		// c2 is deliberately NOT an atom (get_cnf_wff_clauses' leaves often
		// aren't -- a negated equation is wff(wff_neg(wff(bf_eq ...)))).
		// Without the conjunct itself joining its variables' component, the
		// leaf node never enters the union-find and conjuncts_of() silently
		// drops it.
		tref c1 = get_nso_rr("f(x).").value().main->get();
		tref c2 = get_nso_rr("!(y w = 0).").value().main->get();
		trefs vars = conj_vars(c1);
		tref y = conj_vars(c2)[0];
		vars.push_back(y);
		auto a = analyse_block<node_t>(vars, { c1, c2 },
			analysis_context<node_t>{});
		CHECK(a.conjuncts_of(y).size() == 1);
		CHECK(tau::get(a.conjuncts_of(y)[0]) == tau::get(c2));
	}

	TEST_CASE("a kept binder freezes its component") {
		// Equations under a surviving inner quantifier are not top-level
		// conjuncts of this clause; squeezing them would drop the binder
		// and leak its variable free. Same soundness the deleted
		// `blocks_elimination` guard provided, now at component
		// granularity instead of whole-clause.
		tref c1 = get_nso_rr("ex q (q x = 0).").value().main->get();
		tref c2 = get_nso_rr("y w = 0.").value().main->get();
		trefs vars = conj_vars(c1);
		tref y = conj_vars(c2)[0];
		vars.push_back(y);
		auto a = analyse_block<node_t>(vars, { c1, c2 },
			analysis_context<node_t>{});
		CHECK(a.verdict_of(conj_vars(c1)[0]) == elim_verdict::frozen);
		CHECK(a.verdict_of(y) == elim_verdict::eliminable);
	}

	TEST_CASE("a reference-free block costs no reference pass") {
		tref c = get_nso_rr("x y = 0.").value().main->get();
		auto a = analyse_block<node_t>(conj_vars(c), { c },
			analysis_context<node_t>{});
		CHECK(!a.has_reference());
		for (tref v : conj_vars(c))
			CHECK(a.verdict_of(v) == elim_verdict::eliminable);
	}

	TEST_CASE("verdict_of on an unanalysed variable defaults to eliminable") {
		// block_vars is non-empty (holds `other`) so this exercises the
		// per-variable map-miss default in verdict_of/conjuncts_of, not the
		// block_vars.empty() early return.
		tref c = get_nso_rr("x y = 0.").value().main->get();
		trefs vars = conj_vars(c); // {x, y} in some order
		tref other = vars[0];
		tref unanalysed = vars[1];
		auto a = analyse_block<node_t>({ other }, { c },
			analysis_context<node_t>{});
		CHECK(a.verdict_of(unanalysed) == elim_verdict::eliminable);
		CHECK(a.conjuncts_of(unanalysed).empty());
	}

	TEST_CASE("bv-typed atoms are solver_owned when the solver owns bv") {
		tref c = get_nso_rr("x:bv[4] & y:bv[4] = 0:bv[4].")
			.value().main->get();
		analysis_context<node_t> ctx; ctx.bv_is_solver_owned = true;
		auto a = analyse_block<node_t>(conj_vars(c), { c }, ctx);
		for (tref v : conj_vars(c))
			CHECK(a.verdict_of(v) == elim_verdict::solver_owned);
	}

	TEST_CASE("bv atoms are eliminable when the solver cannot own them") {
		// A formula carrying a constant of another Boolean algebra is one
		// cvc5 cannot translate at all, so its bv scopes will never be
		// decided. Boole decomposition is the only route left -- let it
		// have them, or the quantifier is stranded for good.
		tref c = get_nso_rr("x:bv[4] & y:bv[4] = 0:bv[4].")
			.value().main->get();
		analysis_context<node_t> ctx; ctx.bv_is_solver_owned = false;
		auto a = analyse_block<node_t>(conj_vars(c), { c }, ctx);
		for (tref v : conj_vars(c))
			CHECK(a.verdict_of(v) == elim_verdict::eliminable);
	}

	TEST_CASE("arith_residue outranks solver_owned on the same atom") {
		// Multiplication by a non-constant is arithmetic blasting cannot
		// express, so it must not be routed to the solver even though the
		// atom is bv-typed.
		tref c = get_nso_rr("x:bv[4] * y:bv[4] = 0:bv[4].")
			.value().main->get();
		analysis_context<node_t> ctx; ctx.bv_is_solver_owned = true;
		auto a = analyse_block<node_t>(conj_vars(c), { c }, ctx);
		for (tref v : conj_vars(c))
			CHECK(a.verdict_of(v) == elim_verdict::arith_residue);
	}

	TEST_CASE("a reference outranks every bv verdict") {
		tref c1 = get_nso_rr("f(x:bv[4]).").value().main->get();
		analysis_context<node_t> ctx; ctx.bv_is_solver_owned = true;
		auto a = analyse_block<node_t>(conj_vars(c1), { c1 }, ctx);
		for (tref v : conj_vars(c1))
			CHECK(a.verdict_of(v) == elim_verdict::frozen);
	}

	TEST_CASE("a block with no bv-typed bound variable still seeds bv content in its body") {
		// F1 regression: the arith_tainted precompute's cheap-path gate
		// must be read from the CONJUNCTS' free variables, never from
		// block_vars' types -- and must never gate the seed itself. `b`
		// is the block's only bound variable and is not bv-typed, but
		// the conjunct's body still carries bv content (x, y) that must
		// be seeded solver_owned. Gating on block_vars let this fall
		// through to eliminable and reach generic Boole decomposition --
		// the exact blow-up this task exists to prevent -- because the
		// deliberately coarse per-conjunct union still ties b's verdict
		// to the same component as the bv atom.
		tref c = get_nso_rr(
			"(b = 0) -> (x:bv[4] & y:bv[4] = 0:bv[4]).").value().main->get();
		trefs fvs = conj_vars(c);
		tref b = *std::find_if(fvs.begin(), fvs.end(),
			[](tref v) { return !is_tref_bv_type_family<node_t>(v); });
		analysis_context<node_t> ctx; ctx.bv_is_solver_owned = true;
		auto a = analyse_block<node_t>({ b }, { c }, ctx);
		CHECK(a.verdict_of(b) == elim_verdict::solver_owned);
	}
}
