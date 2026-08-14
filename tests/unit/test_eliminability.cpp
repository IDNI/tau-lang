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
		CHECK(join(elim_verdict::eliminable, elim_verdict::blasteable)
			== elim_verdict::blasteable);
		CHECK(join(elim_verdict::arithmetic, elim_verdict::eliminable)
			== elim_verdict::arithmetic);
	}

	TEST_CASE("frozen is the top element") {
		// A reference makes content untouchable by *any* destination,
		// including the solver -- so nothing demotes frozen.
		CHECK(join(elim_verdict::frozen, elim_verdict::eliminable)
			== elim_verdict::frozen);
		CHECK(join(elim_verdict::frozen, elim_verdict::blasteable)
			== elim_verdict::frozen);
		CHECK(join(elim_verdict::frozen, elim_verdict::arithmetic)
			== elim_verdict::frozen);
	}

	TEST_CASE("arithmetic outranks blasteable") {
		// Arithmetic cvc5 cannot express must not be routed to it: the
		// narrower verdict wins where both apply.
		CHECK(join(elim_verdict::blasteable, elim_verdict::arithmetic)
			== elim_verdict::arithmetic);
		CHECK(join(elim_verdict::arithmetic, elim_verdict::blasteable)
			== elim_verdict::arithmetic);
	}

	TEST_CASE("join is commutative across every pair") {
		const elim_verdict all[] = { elim_verdict::eliminable,
			elim_verdict::blasteable, elim_verdict::arithmetic,
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

	TEST_CASE("a pure-BA bv atom is eliminable even where the solver owns bv") {
		// User directive 2026-08-14: "bv variables that appear only in atoms
		// that are purely BA are also eliminable." `&` is not arithmetic, so
		// this atom is decided by Boole's expansion -- valid in any Boolean
		// algebra -- and shipping it to cvc5 is work Tau can do itself. The
		// verdict is recorded EXPLICITLY, which is what lets it beat the
		// bv floor downstream.
		tref c = get_nso_rr("x:bv[4] & y:bv[4] = 0:bv[4].")
			.value().main->get();
		analysis_context<node_t> ctx; ctx.bv_is_solver_owned = true;
		auto a = analyse_block<node_t>(conj_vars(c), { c }, ctx);
		for (tref v : conj_vars(c)) {
			REQUIRE(a.verdicts.contains(v));
			CHECK(a.verdict_of(v) == elim_verdict::eliminable);
		}
	}

	TEST_CASE("bv arithmetic is blasteable when the solver owns bv") {
		// The other side of the directive: an atom carrying an arithmetic
		// operator blasting supports still belongs to the solver / blasting,
		// whatever its variables would be worth on their own.
		tref c = get_nso_rr("x:bv[4] + y:bv[4] = { 0 }:bv[4].")
			.value().main->get();
		analysis_context<node_t> ctx; ctx.bv_is_solver_owned = true;
		auto a = analyse_block<node_t>(conj_vars(c), { c }, ctx);
		for (tref v : conj_vars(c))
			CHECK(a.verdict_of(v) == elim_verdict::blasteable);
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

	TEST_CASE("arithmetic outranks blasteable on the same atom") {
		// Multiplication by a non-constant is arithmetic blasting cannot
		// express, so it must not be routed to the solver even though the
		// atom is bv-typed.
		tref c = get_nso_rr("x:bv[4] * y:bv[4] = 0:bv[4].")
			.value().main->get();
		analysis_context<node_t> ctx; ctx.bv_is_solver_owned = true;
		auto a = analyse_block<node_t>(conj_vars(c), { c }, ctx);
		for (tref v : conj_vars(c))
			CHECK(a.verdict_of(v) == elim_verdict::arithmetic);
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
		// be seeded blasteable. Gating on block_vars let this fall
		// through to eliminable and reach generic Boole decomposition --
		// the exact blow-up this task exists to prevent -- because the
		// deliberately coarse per-conjunct union still ties b's verdict
		// to the same component as the bv atom.
		//
		// The bv content is arithmetic (`+`), not `&`: since the 2026-08-14
		// directive a purely Boolean bv atom seeds `eliminable` on purpose,
		// so only an arithmetic one still exercises the gate this pins.
		tref c = get_nso_rr(
			"(b = 0) -> (x:bv[4] + y:bv[4] = { 0 }:bv[4]).").value().main->get();
		trefs fvs = conj_vars(c);
		tref b = *std::find_if(fvs.begin(), fvs.end(),
			[](tref v) { return !is_tref_bv_type_family<node_t>(v); });
		analysis_context<node_t> ctx; ctx.bv_is_solver_owned = true;
		auto a = analyse_block<node_t>({ b }, { c }, ctx);
		CHECK(a.verdict_of(b) == elim_verdict::blasteable);
	}

	TEST_CASE("members maps every category to its vars and atoms") {
		// x is entangled with a reference; the atom `x z = 0` and both its
		// variables land in members[frozen]; nothing else is keyed.
		tref c1 = get_nso_rr("f(x).").value().main->get();
		tref c2 = get_nso_rr("x z = 0.").value().main->get();
		trefs vars = get_free_vars<node_t>(c2);
		auto a = analyse_block<node_t>(vars, { c1, c2 },
			analysis_context<node_t>{});
		const auto& fz = a.members.at(elim_verdict::frozen);
		for (tref v : vars) CHECK(fz.contains(v));
		CHECK(fz.contains(c2));           // the atom itself
		CHECK(a.verdict_of(c2) == elim_verdict::frozen);
	}

	TEST_CASE("bv_floor makes bv-typed nodes blasteable without analysis") {
		auto e = eliminability<node_t>::bv_only();
		tref bv = get_nso_rr("x:bv[4] = { 1 }:bv[4].").value().main->get();
		tref v = get_free_vars<node_t>(bv)[0];
		CHECK(e.verdict_of(v) == elim_verdict::blasteable);
		CHECK(e.skip(v));
		CHECK_FALSE(eliminability<node_t>::none().skip(v));
	}

	TEST_CASE("an explicit verdict beats the bv floor in both directions") {
		eliminability<node_t> e; e.bv_floor = true;
		tref bv = get_nso_rr("x:bv[4] = { 1 }:bv[4].").value().main->get();
		tref v = get_free_vars<node_t>(bv)[0];
		e.verdicts.emplace(v, elim_verdict::arithmetic);
		CHECK(e.verdict_of(v) == elim_verdict::arithmetic);
		// Explicit eliminable wins over the floor too -- Task 11 (pure-BA bv
		// variables are eliminable) depends on this direction.
		e.verdicts.insert_or_assign(v, elim_verdict::eliminable);
		CHECK(e.verdict_of(v) == elim_verdict::eliminable);
	}

	TEST_CASE("has_skip_content stops at an atom whose bv variables are eliminable") {
		// What `find_top(skip)` cannot express, and why the probe is a
		// method: the atom is a `wff` whose own BA type id is 0, while its
		// terms and constants are bv-typed and floor to blasteable. A plain
		// find_top therefore reports skip content INSIDE an atom the
		// analysis explicitly cleared, and every pure-BA bv clause would
		// still divert to blasting.
		tref fm = get_nso_rr("x:bv[4] & y:bv[4] = 0:bv[4].")
			.value().main->get();
		auto el = analyse_formula<node_t>(fm, analysis_context<node_t>{});
		CHECK_FALSE(el.has_skip_content(fm));
		// The floor is untouched where the analysis vouched for nothing:
		// bv_only() has no explicit verdicts, so no atom is ever covered --
		// which is what keeps blasting's re-entry behaving as before.
		CHECK(eliminability<node_t>::bv_only().has_skip_content(fm));
		CHECK_FALSE(eliminability<node_t>::none().has_skip_content(fm));
	}

	TEST_CASE("has_skip_content still reports bv arithmetic") {
		tref fm = get_nso_rr("x:bv[4] + y:bv[4] = { 0 }:bv[4].")
			.value().main->get();
		auto el = analyse_formula<node_t>(fm, analysis_context<node_t>{});
		CHECK(el.has_skip_content(fm));
	}

	// The two cases below were written as parity oracles against the
	// reference-usage and arithmetic-taint collectors this analysis replaced.
	// Those modules are gone (2026-08-14); the expectations they established
	// are spelled out directly here.
	//
	// Every node checked is taken from the analysed formula itself, never from
	// a second parse of a fragment: `analyse_formula` keys on the very trefs
	// it walked, and a bound occurrence does not hash-cons to a standalone
	// parse's free one -- `verdict_of` would then silently answer with its
	// `eliminable` default, which is exactly what a check for `eliminable`
	// cannot distinguish. The `verdicts.contains` REQUIREs below make that
	// impossible: they establish the node was analysed at all.
	TEST_CASE("analyse_formula: a reference freezes what reaches it, only that") {
		tref fm = get_nso_rr("ex y ex z (q(y) && z = 0).").value().main->get();
		auto el = analyse_formula<node_t>(fm, analysis_context<node_t>{});
		// The wff wrapping the reference, as analyse_formula's own
		// `is_ref_fm` recognises it, and the equation next to it.
		auto is_ref_fm = [](tref n) {
			const auto& t = tau::get(n);
			return t.is(tau::wff) && t.child_is(tau::wff_ref);
		};
		auto is_eq_fm = [](tref n) {
			const auto& t = tau::get(n);
			return t.is(tau::wff) && t.child_is(tau::bf_eq);
		};
		tref ref_fm = tau::get(fm).find_top(is_ref_fm);
		tref eq_fm = tau::get(fm).find_top(is_eq_fm);
		REQUIRE(ref_fm != nullptr);
		REQUIRE(eq_fm != nullptr);
		// y is an argument of the unresolved reference; z shares nothing
		// with it.
		const trefs& yvars = get_free_vars<node_t>(ref_fm);
		const trefs& zvars = get_free_vars<node_t>(eq_fm);
		REQUIRE(yvars.size() == 1);
		REQUIRE(zvars.size() == 1);
		const tref y = yvars[0], z = zvars[0];
		REQUIRE(el.verdicts.contains(y));
		REQUIRE(el.verdicts.contains(z));
		CHECK(el.verdict_of(y) == elim_verdict::frozen);
		CHECK(el.verdict_of(z) == elim_verdict::eliminable);
		CHECK(el.skip(y));
		CHECK_FALSE(el.skip(z));
	}

	TEST_CASE("analyse_formula: bv arithmetic taints its component, only it") {
		tref fm = get_nso_rr(
			"ex x (x:bv[4] + y:bv[4] = { 0 }:bv[4] && w = 0).")
			.value().main->get();
		auto el = analyse_formula<node_t>(fm, analysis_context<node_t>{});
		// Both atoms of `fm`, split by whether they carry bv content. The
		// variables have to be read off the atom WFF, never off the bare
		// `bf_add` term: `get_free_vars` returns nothing for the latter,
		// and that is also how the analysis itself calls it.
		auto is_eq_fm = [](tref n) {
			const auto& t = tau::get(n);
			return t.is(tau::wff) && t.child_is(tau::bf_eq);
		};
		tref bv_atm = nullptr, ba_atm = nullptr;
		for (tref a : tau::get(fm).select_all(is_eq_fm)) {
			bool has_bv = false;
			for (tref v : get_free_vars<node_t>(a))
				if (is_tref_bv_type_family<node_t>(v)) {
					has_bv = true; break;
				}
			(has_bv ? bv_atm : ba_atm) = a;
		}
		REQUIRE(bv_atm != nullptr);
		REQUIRE(ba_atm != nullptr);
		// Both variables of the arithmetic atom are non-eliminable. `+` is
		// an operator blasting supports, so the verdict is blasteable
		// rather than arithmetic.
		const trefs& bvvars = get_free_vars<node_t>(bv_atm);
		REQUIRE(bvvars.size() == 2);
		for (tref v : bvvars) {
			REQUIRE(el.verdicts.contains(v));
			CHECK(el.skip(v));
			CHECK(el.verdict_of(v) == elim_verdict::blasteable);
		}
		// The atomless conjunct's variable stays eliminable.
		const trefs& bavars = get_free_vars<node_t>(ba_atm);
		REQUIRE(bavars.size() == 1);
		const tref w = bavars[0];
		REQUIRE(el.verdicts.contains(w));
		CHECK(el.verdict_of(w) == elim_verdict::eliminable);
		CHECK_FALSE(el.skip(w));
	}

	TEST_CASE("analyse_formula: supported arithmetic is blasteable, "
		"unsupported is arithmetic") {
		// x + 1 is blasteable; u * v (no constant factor) is not.
		tref f1 = get_nso_rr("x:bv[4] + { 1 }:bv[4] = { 0 }:bv[4].")
			.value().main->get();
		tref f2 = get_nso_rr("u:bv[4] * v:bv[4] = { 0 }:bv[4].")
			.value().main->get();
		auto e1 = analyse_formula<node_t>(f1, analysis_context<node_t>{});
		auto e2 = analyse_formula<node_t>(f2, analysis_context<node_t>{});
		CHECK(e1.verdict_of(get_free_vars<node_t>(f1)[0])
			== elim_verdict::blasteable);
		CHECK(e2.verdict_of(get_free_vars<node_t>(f2)[0])
			== elim_verdict::arithmetic);
	}

	TEST_CASE("analyse_formula: bv floor follows bv_is_solver_owned") {
		tref fm = get_nso_rr("x:bv[4] = { 1 }:bv[4].").value().main->get();
		analysis_context<node_t> owned{}, foreign{}; foreign.bv_is_solver_owned = false;
		CHECK(analyse_formula<node_t>(fm, owned).bv_floor);
		CHECK_FALSE(analyse_formula<node_t>(fm, foreign).bv_floor);
		// Without the floor a plain bv equality is eliminable (Boole
		// decomposition is its only route -- the issue #70 shape).
		tref v = get_free_vars<node_t>(fm)[0];
		CHECK(analyse_formula<node_t>(fm, foreign).verdict_of(v)
			== elim_verdict::eliminable);
	}

	TEST_CASE("analyse_formula: quantifier scoping does not cross-contaminate") {
		// Two unrelated binders of the same name: the union-find must not
		// merge across their scopes (w, entangled only with the second
		// binder's body, stays eliminable) -- but the two "y" occurrences
		// are the SAME tref (structurally-identical variables hash-cons to
		// one node regardless of which quantifier scope pushed them), so
		// the flat per-node result conservatively conflates them, exactly
		// as the old collectors' flat sets do: y is frozen because ONE of
		// its two binders reaches a reference, even though the other does
		// not.
		tref fm = get_nso_rr("(ex y q(y)) && (ex y (y w = 0)).")
			.value().main->get();
		auto el = analyse_formula<node_t>(fm, analysis_context<node_t>{});
		tref w = get_free_vars<node_t>(fm)[0]; // w is the only free var
		CHECK(el.verdict_of(w) == elim_verdict::eliminable);
		auto vars = tau::get(fm).select_all(
			(bool(*)(tref)) is_var_or_capture<node_t>);
		tref y = *std::find_if(vars.begin(), vars.end(),
			[w](tref v) { return v != w; });
		CHECK(el.verdict_of(y) == elim_verdict::frozen);
	}
}
