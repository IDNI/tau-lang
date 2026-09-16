// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Layer 1 unit tests for src/anti_prenex/normalisers/simplify.h.
// Spec: anti_prenex.md §3 SIMPLIFY and "Equality propagation", §4 "What may
// touch a unit", §10, and TRY_WITNESS's pin match (§3).
//
// Everything here runs in the PLAIN regime — the empty order, which is what
// phases 1, 2 and 5 pass — so no term is BDD-backed and the `X` guard is
// vacuous unless a case builds a block of its own.
//
// Two facts the cases are built around. An equation is SYMMETRIC: `y = a`
// pins `y` with witness `a` AND `a` with witness `y`, both strictly and both
// with a witness of the same size, so which one the match returns is the
// content order's business. A case that needs a direction uses an
// asymmetric equation (`y = a·b` pins only `y`). And the pass matches the
// equation members of a conjunction IN CONTENT ORDER, so a pin reaches the
// equations after it and no others (§3: "a conjunct already emitted is not
// revisited"); a case that needs the rewrite to happen puts the target in a
// NON-equation member, which the traversal rewrites under the whole
// environment.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/normalisers/simplify.h"

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

tref bvar(const char* name) { return tau::build_bf_variable(name, 0); }
tref fvar(const char* name) { return tau::build_variable(name, 0); }

tref eq(tref l, tref r)   { return tau::build_bf_eq(l, r); }
tref eq0(tref t)          { return tau::build_bf_eq_0(t); }
tref neg(tref n)          { return tau::build_wff_neg(n); }
tref conj(tref l, tref r) { return tau::build_wff_and(l, r); }
tref disj(tref l, tref r) { return tau::build_wff_or(l, r); }

tref land(tref l, tref r) { return tau::build_bf_and(l, r); }
tref lor(tref l, tref r)  { return tau::build_bf_or(l, r); }
tref lneg(tref t)         { return tau::build_bf_neg(t); }
tref lxor(tref l, tref r) { return tau::build_bf_xor(l, r); }

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

trefs members(tref n) { return ap::members<node_t>(n); }

tref simp(tref n) { return ap::simplify<node_t>(n); }

/// Does `y` still occur free anywhere in `n`?
bool holds_var(tref n, const char* name) {
	return ap::fv_meets<node_t>(n, ap::block{ fvar(name) });
}

/// Invariant 4 over a whole formula: no fused negated atom anywhere.
bool no_fused_atoms(tref n) {
	bool clean = true;
	auto check = [&clean](tref m) {
		const auto& t = tau::get(m);
		if (t.is(tau::bf_neq) || t.is(tau::bf_nlt) || t.is(tau::bf_nlteq)
			|| t.is(tau::bf_gt) || t.is(tau::bf_gteq)
			|| t.is(tau::bf_ngt) || t.is(tau::bf_ngteq))
				return clean = false;
		return true;
	};
	auto all_of_it = [](tref) { return true; };
	auto up = [](tref) {};
	pre_order<node_t>(n).search_unique(check, all_of_it, up);
	return clean;
}

/// The member of `n` that is not content-equal to `skip`, or null.
tref other_member(tref n, tref skip) {
	for (tref m : members(n)) if (!same(m, skip)) return m;
	return nullptr;
}

/// `propagate_growth` for the scope of one case, restored on the way out.
struct growth_guard {
	const size_t saved = ap::propagate_growth;
	explicit growth_guard(size_t v) { ap::propagate_growth = v; }
	~growth_guard() { ap::propagate_growth = saved; }
};

} // namespace

TEST_SUITE("anti_prenex/normalisers/simplify") {

// --- find_pin ---------------------------------------------------------------------

TEST_CASE("S1: the spelled pin is strict, with the other side as witness") {
	tref y = bvar("y"), a = bvar("a"), b = bvar("b");
	// `y = a·b` pins ONLY `y`: cofactoring by `a` leaves `y ∪ b`, whose
	// union with `y·b′ ∪ y′·b` is not 1.
	auto p = ap::find_pin<node_t>(eq(y, land(a, b)), ap::block{});
	REQUIRE(p.has_value());
	CHECK(same(p->var, fvar("y")));
	CHECK(p->strict);
	CHECK(same(p->witness, land(a, b)));
	// The zero-normalised spelling of the same equation pins alike: the
	// match reads `TERM_OF`, which is `l + r` either way.
	auto q = ap::find_pin<node_t>(eq0(lxor(y, land(a, b))), ap::block{});
	REQUIRE(q.has_value());
	CHECK(same(q->var, fvar("y")));
	CHECK(q->strict);
	CHECK(same(q->witness, land(a, b)));
	// `y = a` pins BOTH variables, each strictly and each with a witness
	// of size one, so the match takes whichever comes first in content
	// order — the spec's tie-break does not separate them.
	auto sym = ap::find_pin<node_t>(eq(y, a), ap::block{});
	REQUIRE(sym.has_value());
	CHECK(sym->strict);
	CHECK((same(sym->var, fvar("y")) || same(sym->var, fvar("a"))));
	CHECK(same(sym->witness, same(sym->var, fvar("y")) ? a : y));
}

TEST_CASE("S2: a weak pin keeps its residual") {
	tref y = bvar("y"), t = bvar("t"), c = bvar("c"), d = bvar("d");
	const tref G = land(c, d);
	// `(y + t) ∪ G = 0`, the shape the squeeze produces (§3): the
	// cofactors of `y` are `t ∪ G` and `t′ ∪ G`, complementary only where
	// `G = 0`, so the pin is WEAK with witness `t·G′` and residual `G`.
	// `G` is a product here so that no variable of it pins on its own.
	auto p = ap::find_pin<node_t>(eq0(lor(lxor(y, t), G)), ap::block{});
	REQUIRE(p.has_value());
	CHECK(!p->strict);
	// `y` and `t` pin each other symmetrically, so the content order
	// picks; either way the witness is "the other side times `G′`".
	REQUIRE((same(p->var, fvar("y")) || same(p->var, fvar("t"))));
	const tref other = same(p->var, fvar("y")) ? t : y;
	CHECK(same(p->witness,
		ap::simplify_term<node_t>(land(other, lneg(G)))));
	// A variable of the residue pins too when it is alone in it: `G = g`
	// gives `g` the witness `0`, the smallest of the atom, which is what
	// the tie-break then takes.
	tref g = bvar("g");
	auto q = ap::find_pin<node_t>(eq0(lor(lxor(y, t), g)), ap::block{});
	REQUIRE(q.has_value());
	CHECK(!q->strict);
	CHECK(same(q->var, fvar("g")));
	CHECK(tau::get(q->witness).equals_0());
}

TEST_CASE("S3: what does not pin") {
	tref y = bvar("y"), a = bvar("a");
	// `y·a = 0` leaves the cofactors `0` and `a`, whose union is not 1.
	CHECK(!ap::find_pin<node_t>(eq0(land(y, a)), ap::block{}).has_value());
	// An equation under `¬` pins nothing.
	CHECK(!ap::find_pin<node_t>(neg(eq(y, a)), ap::block{}).has_value());
	// Neither does anything that is not a positive equation.
	CHECK(!ap::find_pin<node_t>(conj(eq(y, a), eq0(a)),
		ap::block{}).has_value());
	// An atom meeting `X` is `TRY_WITNESS`'s business, not propagation's:
	// the whole atom is refused, whichever side the block variable is on.
	CHECK(!ap::find_pin<node_t>(eq(y, a),
		ap::block{ fvar("y") }).has_value());
	CHECK(!ap::find_pin<node_t>(eq(y, a),
		ap::block{ fvar("a") }).has_value());
}

TEST_CASE("S4: a strict pin wins over a weak one") {
	tref y = bvar("y"), a = bvar("a"), b = bvar("b"), g = bvar("g");
	// `(y + a·b) ∪ G = 0` pins `y` weakly (residual `G`), and nothing
	// strictly, so the match returns a weak pin.
	auto weak = ap::find_pin<node_t>(
		eq0(lor(lxor(y, land(a, b)), g)), ap::block{});
	REQUIRE(weak.has_value());
	CHECK(!weak->strict);
	CHECK(ap::mem_size<node_t>(weak->witness) > 0);
	// With the residual gone the same shape is strict.
	auto strict = ap::find_pin<node_t>(eq0(lxor(y, land(a, b))),
		ap::block{});
	REQUIRE(strict.has_value());
	CHECK(strict->strict);
	CHECK(same(strict->var, fvar("y")));
}

// --- propagation --------------------------------------------------------------------

TEST_CASE("S5: the spec's example — the pin folds `x·y ∪ x′·t` to `t`") {
	tref x = bvar("x"), y = bvar("y"), a = bvar("a"), b = bvar("b");
	const tref t = land(a, b);            // the witness, `a·b`
	tref pinning = eq(y, t);
	tref target = eq0(lor(land(x, y), land(lneg(x), t)));
	// As a NON-equation member — here the disjunct of a two-member
	// disjunction — the target is rewritten under the whole environment.
	tref phi = conj(pinning, disj(target, eq0(bvar("c"))));
	tref got = simp(phi);
	INFO("simplified: ", tau::get(got).to_str());
	CHECK(are_nso_equivalent<node_t>(got, phi));
	CHECK(no_fused_atoms(got));
	// `y` is gone from everything but the pinning conjunct, and the
	// rewritten atom's term folded to the witness.
	tref rest = other_member(got, pinning);
	REQUIRE(rest != nullptr);
	CHECK(!holds_var(rest, "y"));
	bool folded = false;
	for (tref m : members(rest))
		if (ap::is_literal<node_t>(m)
			&& is_child<node_t>(m, tau::bf_eq)
			&& same(ap::term_of<node_t>(m, {}), t)) folded = true;
	CHECK(folded);
}

TEST_CASE("S5b: between two EQUATION members the content order decides") {
	tref x = bvar("x"), y = bvar("y"), a = bvar("a"), b = bvar("b");
	const tref t = land(a, b);
	tref pinning = eq(y, t);
	tref target = eq0(lor(land(x, y), land(lneg(x), t)));
	tref got = simp(conj(pinning, target));
	INFO("two equations: ", tau::get(got).to_str());
	CHECK(are_nso_equivalent<node_t>(got, conj(pinning, target)));
	// The pass matches the equations in content order and rewrites each
	// one under the pins admitted BEFORE it, so the target is rewritten
	// exactly when the pinning equation sorts first (§3).
	tref rest = other_member(got, pinning);
	REQUIRE(rest != nullptr);
	if (tau::subtree_less(pinning, target))
		CHECK(!holds_var(rest, "y"));
	else CHECK(same(rest, target));
}

TEST_CASE("S6: propagation reaches a disjunctive sibling and a binder body") {
	tref y = bvar("y"), a = bvar("a"), b = bvar("b"), c = bvar("c");
	const tref t = land(a, b);
	tref pinning = eq(y, t);
	// Into every member of a disjunction.
	tref phi = conj(pinning, disj(eq0(y), eq0(lor(y, c))));
	tref got = simp(phi);
	CHECK(are_nso_equivalent<node_t>(got, phi));
	tref rest = other_member(got, pinning);
	REQUIRE(rest != nullptr);
	CHECK(!holds_var(rest, "y"));
	// Into a binder body (§4: SIMPLIFY descends into a unit).
	tref under = conj(pinning,
		tau::build_wff_ex(fvar("z"), eq0(lor(y, bvar("z"))), false));
	tref got2 = simp(under);
	tref body = other_member(got2, pinning);
	REQUIRE(body != nullptr);
	CHECK(is_child<node_t>(body, tau::wff_ex));
	CHECK(!holds_var(body, "y"));
	CHECK(are_nso_equivalent<node_t>(got2, under));
}

TEST_CASE("S7: a pin is suspended under a binder that captures it") {
	tref y = bvar("y"), a = bvar("a"), b = bvar("b");
	const tref t = land(a, b);
	tref pinning = eq(y, t);
	// A binder over the pin's own variable: the body's `y` is that
	// binder's, not the pin's, so nothing is rewritten there.
	tref inner = tau::build_wff_ex(fvar("y"), eq0(lor(y, bvar("c"))), false);
	tref got = simp(conj(pinning, inner));
	tref body = other_member(got, pinning);
	REQUIRE(body != nullptr);
	CHECK(same(body, inner));
	CHECK(are_nso_equivalent<node_t>(got, conj(pinning, inner)));
	// A binder over a variable of the WITNESS: substituting would capture
	// it, so the pin is suspended there too.
	tref inner2 = tau::build_wff_ex(fvar("a"), eq0(lor(y, bvar("c"))), false);
	tref got2 = simp(conj(pinning, inner2));
	tref body2 = other_member(got2, pinning);
	REQUIRE(body2 != nullptr);
	CHECK(same(body2, inner2));
	// and it is back in force after the binder closes: the sibling beside
	// the binder — a non-equation member, so the whole environment reaches
	// it — is rewritten.
	tref sibling = disj(eq0(lor(y, bvar("d"))), eq0(bvar("e")));
	tref mixed = conj(pinning, conj(inner2, sibling));
	tref got3 = simp(mixed);
	INFO("mixed: ", tau::get(got3).to_str());
	bool outside_rewritten = true;
	for (tref m : members(got3))
		if (!same(m, pinning) && !same(m, inner2) && holds_var(m, "y"))
			outside_rewritten = false;
	CHECK(outside_rewritten);
	CHECK(are_nso_equivalent<node_t>(got3, mixed));
}

TEST_CASE("S8: pins chain within one pass") {
	tref y = bvar("y"), z = bvar("z"), a = bvar("a"), b = bvar("b"),
		c = bvar("c");
	const tref t = land(a, b);
	// `y = a·b ∧ z = y·c ∧ ψ(z)`: the second equation becomes `z = a·b·c`
	// under the first, and ψ — a non-equation member — sees neither `y`
	// nor `z`. The two equations are ordered by content, so this case
	// asserts the chain only when the first pin sorts first.
	tref p1 = eq(y, t), p2 = eq(z, land(y, c));
	tref psi = disj(eq0(lor(z, bvar("d"))), eq0(bvar("e")));
	tref phi = conj(p1, conj(p2, psi));
	tref got = simp(phi);
	INFO("chained: ", tau::get(got).to_str());
	CHECK(are_nso_equivalent<node_t>(got, phi));
	CHECK(no_fused_atoms(got));
	if (tau::subtree_less(p1, p2)) {
		// `z`'s witness was rewritten by `y`'s pin before it joined,
		// so ψ mentions neither variable.
		for (tref m : members(got))
			if (!same(m, p1) && !is_child<node_t>(m, tau::bf_eq)) {
				CHECK(!holds_var(m, "z"));
				CHECK(!holds_var(m, "y"));
			}
	}
}

TEST_CASE("S9: the cap refuses a pin") {
	tref y = bvar("y"), a = bvar("a"), b = bvar("b"), c = bvar("c");
	tref pinning = eq(y, lor(lor(a, b), c));
	tref phi = conj(pinning, disj(eq0(lor(y, bvar("d"))), eq0(bvar("e"))));
	{
		// With no room at all no pin is admitted and the sibling is
		// left as written — precision, never soundness.
		growth_guard cap(0);
		tref got = ap::simplify<node_t>(phi);
		tref rest = other_member(got, pinning);
		REQUIRE(rest != nullptr);
		CHECK(holds_var(rest, "y"));
		CHECK(are_nso_equivalent<node_t>(got, phi));
	}
	// Restored, the same formula propagates.
	CHECK(ap::propagate_growth == 4);
	tref got = simp(phi);
	tref rest = other_member(got, pinning);
	REQUIRE(rest != nullptr);
	CHECK(!holds_var(rest, "y"));
}

// --- the sweep half -------------------------------------------------------------------

TEST_CASE("S10: the sweep folds, and keeps every negation spelled") {
	tref x = bvar("x"), y = bvar("y");
	// A contradiction and a tautology, both through the sweep.
	CHECK(tau::get(simp(conj(eq0(x), neg(eq0(x))))).equals_F());
	CHECK(tau::get(simp(disj(eq0(x), neg(eq0(x))))).equals_T());
	// `¬(x = 0)` stays spelled: no `!=` is introduced anywhere.
	tref spelled = neg(eq0(x));
	CHECK(same(simp(spelled), spelled));
	CHECK(no_fused_atoms(simp(conj(neg(eq0(x)), eq0(y)))));
	CHECK(no_fused_atoms(simp(disj(neg(eq0(x)), neg(eq0(y))))));
	// A unit assumption reaches into a binder body (units are not opaque
	// to this sweep).
	tref phi = conj(eq0(x), tau::build_wff_ex(fvar("z"),
		conj(neg(eq0(x)), eq0(bvar("z"))), false));
	CHECK(tau::get(simp(phi)).equals_F());
}

TEST_CASE("S11: a temporal operator is left alone") {
	tref y = bvar("y"), a = bvar("a"), b = bvar("b");
	tref inner = tau::build_wff_always(eq0(lor(y, bvar("c"))));
	tref phi = conj(eq(y, land(a, b)), inner);
	tref got = simp(phi);
	tref rest = other_member(got, eq(y, land(a, b)));
	REQUIRE(rest != nullptr);
	// Opaque to both passes: the body is not entered, so `y` survives.
	CHECK(same(rest, inner));
}

TEST_CASE("S12: reference arguments only under `ref_args`") {
	tref ref = tau::get("f(x & (x | y))", parse_wff());
	REQUIRE(ref != nullptr);
	REQUIRE(is_child<node_t>(ref, tau::wff_ref));
	// Opaque by default.
	CHECK(ap::simplify<node_t>(ref) == ref);
	// In phase 1's entry state the argument goes through SIMPLIFY_TERM.
	tref got = ap::simplify<node_t>(ref, {}, true);
	INFO("ref_args: ", tau::get(got).to_str());
	CHECK(is_child<node_t>(got, tau::wff_ref));
	CHECK(got != ref);
	CHECK(are_nso_equivalent<node_t>(got, ref));
}

// --- shape and idempotence --------------------------------------------------------------

TEST_CASE("S13: idempotent, and equivalence-preserving") {
	tref x = bvar("x"), y = bvar("y"), a = bvar("a"), b = bvar("b");
	const tref t = land(a, b);
	const trefs inputs{
		conj(eq(y, t), disj(eq0(lor(land(x, y), land(lneg(x), t))),
			eq0(bvar("c")))),
		conj(eq(y, t), disj(eq0(y), eq0(b))),
		conj(eq0(x), neg(eq0(y))),
		disj(conj(eq(y, t), eq0(y)), eq0(b)),
		tau::build_wff_ex(fvar("z"),
			conj(eq(y, t), eq0(lor(y, bvar("z")))), false),
		conj(neg(eq0(x)), disj(eq0(y), neg(eq0(b)))),
	};
	for (tref in : inputs) {
		tref once = simp(in);
		CHECK(are_nso_equivalent<node_t>(once, in));
		CHECK(no_fused_atoms(once));
		CHECK(simp(once) == once);
	}
}

TEST_CASE("S14: a formula with nothing to do comes back as the same node") {
	tref x = bvar("x"), y = bvar("y");
	// No pin, no fold: every node is returned as it stands.
	tref atom = eq0(x);
	CHECK(simp(atom) == atom);
	CHECK(simp(neg(atom)) == neg(atom));
	CHECK(simp(tau::_T()) == tau::_T());
	tref opaque = tau::build_wff_always(conj(eq0(x), eq0(y)));
	CHECK(simp(opaque) == opaque);
	tref binder = tau::build_wff_ex(fvar("z"), eq0(bvar("z")), false);
	CHECK(simp(binder) == binder);
}

}
