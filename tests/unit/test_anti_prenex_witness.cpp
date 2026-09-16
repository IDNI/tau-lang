// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for src/anti_prenex/witness/witness.h (layer 2).
// Spec: anti_prenex.md §3 TRY_WITNESS, TRY_WITNESS_DEEP, "A CASE PIN for x",
// TRY_CASE_WITNESS and ELIMINATE_BY_SUBSTITUTION; §4 "What may touch a unit";
// §1's `case_max`.
//
// Everything here runs in the PLAIN regime — phase 2 has no block, no live
// order and nothing BDD-backed — which is exactly what the spelled mode
// assumes.
//
// What the rewrites are checked against: `are_nso_equivalent` between the
// BINDER and the body the step returns (`Qx.Φ` against its rewrite — the step
// deletes the binder, so the binder is what the claim is about), never an
// output string; shapes are located by predicate, and `x` is checked gone
// through `fv_meets`.
//
// Two spellings matter for the ∀ form: after `TO_NNF` a negated equation is
// `¬(l = r)`, while a `bf_neq` atom still reaches phase 2 — `NORMALIZE_OPERATORS`
// is phase 3 — and both must pin. The module reads a `bf_neq` and never builds
// one (invariant 4).

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/witness/witness.h"

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

tref bvar(const char* name) { return tau::build_bf_variable(name, 0); }
tref fvar(const char* name) { return tau::build_variable(name, 0); }

tref eq(tref l, tref r)   { return tau::build_bf_eq(l, r); }
tref eq0(tref t)          { return tau::build_bf_eq_0(t); }
tref neq(tref l, tref r)  { return tau::build_bf_neq(l, r); }
tref neg(tref n)          { return tau::build_wff_neg(n); }
tref conj(tref l, tref r) { return tau::build_wff_and(l, r); }
tref disj(tref l, tref r) { return tau::build_wff_or(l, r); }

tref land(tref l, tref r) { return tau::build_bf_and(l, r); }
tref lor(tref l, tref r)  { return tau::build_bf_or(l, r); }
tref lneg(tref t)         { return tau::build_bf_neg(t); }
tref lxor(tref l, tref r) { return tau::build_bf_xor(l, r); }

tref ex(const char* v, tref body) {
	return tau::build_wff_ex(fvar(v), body, false);
}
tref all_(const char* v, tref body) {
	return tau::build_wff_all(fvar(v), body, false);
}

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

/// Is the variable still free anywhere in `n`? The one question every rewrite
/// of `x` has to answer.
bool holds_var(tref n, const char* name) {
	return ap::fv_meets<node_t>(n, ap::block{ fvar(name) });
}

/// Is there a node of this kind anywhere in `n`?
bool holds(tref n, size_t nt) {
	bool found = false;
	auto check = [&found, nt](tref m) {
		if (tau::get(m).is(nt)) return found = true, false;
		return true;
	};
	auto all_of_it = [](tref) { return true; };
	auto up = [](tref) {};
	pre_order<node_t>(n).search_unique(check, all_of_it, up);
	return found;
}

/// A member of `n` content-equal to `m`?
bool has_member(tref n, tref m) {
	for (tref c : ap::members<node_t>(n)) if (same(c, m)) return true;
	return false;
}

/// The ∃ quantifier kind, and the ∀ one.
constexpr auto EX  = tau_term_bdd<node_t>::ex;
constexpr auto ALL = tau_term_bdd<node_t>::all;

/// `case_max` for the scope of one case, restored on the way out.
struct case_max_guard {
	const size_t saved = ap::case_max;
	explicit case_max_guard(size_t v) { ap::case_max = v; }
	~case_max_guard() { ap::case_max = saved; }
};

} // namespace

TEST_SUITE("anti_prenex/witness") {

// --- TRY_WITNESS, spelled mode ----------------------------------------------------

TEST_CASE("W1: the spelled match reads every spelling of one equation") {
	tref x = bvar("x"), t = bvar("t"), a = bvar("a");
	// `ψ = (x = t) ∧ (x ∪ a = 0)`, in the three spellings the call sites
	// straddling NORMALIZE_OPERATORS produce (§3). The witnessed body is
	// the same formula every time: the pinning conjunct's image folds to
	// `T` for a strict pin, and the sibling becomes `t ∪ a = 0`.
	const tref sibling = eq0(lor(x, a));
	const tref want = ap::simplify<node_t>(eq0(lor(t, a)));
	for (tref pinning : { eq(x, t), eq(t, x), eq0(lxor(x, t)) }) {
		const tref psi = conj(pinning, sibling);
		auto r = ap::try_witness<node_t>(fvar("x"), psi);
		REQUIRE(r.has_value());
		CHECK(same(*r, want));
		CHECK(!holds_var(*r, "x"));
		CHECK(are_nso_equivalent<node_t>(ex("x", psi), *r));
	}
	// One conjunct is a `ψ` of its own: the member view of a non-∧ node is
	// the node itself.
	auto one = ap::try_witness<node_t>(fvar("x"), eq(x, t));
	REQUIRE(one.has_value());
	CHECK(tau::get(*one).equals_T());
}

TEST_CASE("W2: a weak pin substitutes its witness and keeps its residual") {
	tref x = bvar("x"), t = bvar("t"), b = bvar("b");
	tref c = bvar("c"), d = bvar("d");
	const tref G = land(c, d);
	// `(x + t) ∪ G = 0` — the shape the squeeze produces (§3) — pins `x`
	// WEAKLY: the cofactors are `t ∪ G` and `t′ ∪ G`, complementary only
	// where `G = 0`, so the witness is `t·G′` and the residual is `G = 0`.
	// The second conjunct `x·b = 0` pins nothing.
	const tref psi = conj(eq0(lor(lxor(x, t), G)), eq0(land(x, b)));
	auto r = ap::try_witness<node_t>(fvar("x"), psi);
	REQUIRE(r.has_value());
	CHECK(!holds_var(*r, "x"));
	CHECK(are_nso_equivalent<node_t>(ex("x", psi), *r));
	// THE PINNING CONJUNCT STAYS, substituted like every sibling, and its
	// image IS the residual `p = 0` (§3) — as a FUNCTION: the plain
	// `SIMPLIFY_TERM` has no xor algebra, so `(t·G' + t) u G` is not folded
	// to `G` on the page. It is the member that does not mention `b`.
	tref image = nullptr;
	for (tref m : ap::members<node_t>(*r))
		if (!holds_var(m, "b")) image = m;
	REQUIRE(image != nullptr);
	CHECK(are_nso_equivalent<node_t>(image, eq0(G)));
}

TEST_CASE("W3: a strict pin comes first, then the smallest witness") {
	tref x = bvar("x"), a = bvar("a"), b = bvar("b");
	tref t = bvar("t"), e = bvar("e"), f = bvar("f");
	const tref G = land(e, f);
	// `x ∪ a = 0` pins weakly (witness `0`, residual `a`); `x = b` pins
	// strictly, with witness `b`. Whichever comes first in the conjunction,
	// the STRICT pin is the one taken — the two eliminations are
	// equivalent, so what tells them apart is the SHAPE: the sibling's image
	// is `b ∪ a = 0` under the strict witness and `a = 0` under the weak one.
	const tref weak = eq0(lor(x, a)), strict = eq(x, b);
	for (tref psi : { conj(weak, strict), conj(strict, weak) }) {
		auto r = ap::try_witness<node_t>(fvar("x"), psi);
		REQUIRE(r.has_value());
		CHECK(!holds_var(*r, "x"));
		CHECK(are_nso_equivalent<node_t>(ex("x", psi), *r));
		CHECK(same(*r, ap::simplify<node_t>(eq0(lor(b, a)))));
	}
	// Two WEAK pins and no strict one: the smallest `‖f₁′‖` wins — `0`
	// over `t·G′` — which is what the first conjunct's image shows.
	const tref psi = conj(eq0(lor(x, a)), eq0(lor(lxor(x, t), G)));
	auto r = ap::try_witness<node_t>(fvar("x"), psi);
	REQUIRE(r.has_value());
	CHECK(!holds_var(*r, "x"));
	CHECK(are_nso_equivalent<node_t>(ex("x", psi), *r));
	CHECK(has_member(*r, ap::simplify<node_t>(eq0(a))));
}

TEST_CASE("W4: a negated equation pins nothing in the ∃ sense") {
	tref x = bvar("x"), t = bvar("t"), b = bvar("b");
	// Both spellings of `x ≠ t`, each beside a conjunct that pins nothing.
	CHECK(!ap::try_witness<node_t>(fvar("x"),
		conj(neg(eq(x, t)), eq0(land(x, b)))).has_value());
	CHECK(!ap::try_witness<node_t>(fvar("x"),
		conj(neq(x, t), eq0(land(x, b)))).has_value());
}

// --- TRY_WITNESS_DEEP -------------------------------------------------------------

TEST_CASE("W5: the plain pin at a spine, and its ∀ dual") {
	tref x = bvar("x"), t = bvar("t"), a = bvar("a");
	// ∃ form: the spine is the conjunction, the pin is dropped and the
	// other member is substituted.
	const tref phi = conj(eq(x, t), eq0(lor(x, a)));
	auto r = ap::try_witness_deep<node_t>(EX, fvar("x"), phi);
	REQUIRE(r.has_value());
	CHECK(same(*r, eq0(lor(t, a))));
	CHECK(!holds_var(*r, "x"));
	CHECK(are_nso_equivalent<node_t>(ex("x", phi), *r));
	// NOT simplified here: phase 2's global SIMPLIFY follows (§3).
	// ∀ form, under ¬∃x¬: the spine is a disjunction and the pin is a
	// NEGATED equation, in either spelling still present at phase 2.
	for (tref pinning : { neg(eq(x, t)), neq(x, t) }) {
		const tref psi = disj(pinning, neg(eq0(lor(x, a))));
		auto q = ap::try_witness_deep<node_t>(ALL, fvar("x"), psi);
		REQUIRE(q.has_value());
		CHECK(same(*q, neg(eq0(lor(t, a)))));
		CHECK(!holds_var(*q, "x"));
		CHECK(are_nso_equivalent<node_t>(all_("x", psi), *q));
	}
	// A WEAK pin is no pin here: the member is dropped, so its residual
	// would be lost. `x ∪ a = 0` pins `x` weakly and nothing fires.
	CHECK(!ap::try_witness_deep<node_t>(EX, fvar("x"),
		conj(eq0(lor(x, a)), eq0(land(x, bvar("b"))))).has_value());
}

TEST_CASE("W6: a pin reached through a Q-kind binder before the flip") {
	tref x = bvar("x"), a = bvar("a"), y = bvar("y");
	// `∃x ∃y.(x = y ∧ x ∪ a = 0)`: the binder on the path has `Q`'s kind
	// and nothing has flipped, so `y` is NOT barred from the witness — the
	// two binders commute (§3, condition (c)).
	const tref phi = ex("y", conj(eq(x, y), eq0(lor(x, a))));
	auto r = ap::try_witness_deep<node_t>(EX, fvar("x"), phi);
	REQUIRE(r.has_value());
	CHECK(!holds_var(*r, "x"));
	// The rewrite is LOCAL to the spine: the binder it sat under stays.
	REQUIRE(is_child<node_t>(*r, tau::wff_ex));
	CHECK(same(ap::binder_body<node_t>(*r), eq0(lor(y, a))));
	CHECK(are_nso_equivalent<node_t>(ex("x", phi), *r));
}

TEST_CASE("W7: the counterexamples of condition (c) are not rewritten") {
	tref x = bvar("x"), y = bvar("y"), z = bvar("z"), a = bvar("a");
	// `∃x ∀y.(x = y ∧ …)` — F, while its rewrite need not be: `y` is bound
	// past the kind flip, so it is barred from the witness.
	const tref one = all_("y", conj(eq(x, y), eq0(lor(y, a))));
	CHECK(!ap::try_witness_deep<node_t>(EX, fvar("x"), one).has_value());
	// `∃x ∀y ∃z.(x = z ∧ z = y)` — the same, one binder deeper: `z` is
	// bound AFTER the flip, so it joins the barred set too.
	const tref two = all_("y", ex("z", conj(eq(x, z), eq(z, y))));
	CHECK(!ap::try_witness_deep<node_t>(EX, fvar("x"), two).has_value());
}

TEST_CASE("W8: confinement — `x` free in two members declines") {
	tref x = bvar("x"), a = bvar("a"), b = bvar("b");
	// Neither member pins `x` strictly and both hold it, so there is no
	// one member to descend into: every rewrite here would leave an
	// occurrence outside the node it touches.
	const tref phi = conj(eq0(land(x, b)), eq0(lor(x, a)));
	CHECK(!ap::try_witness_deep<node_t>(EX, fvar("x"), phi).has_value());
	// The other connective under the spine: `x` in two children, ⊥ again.
	const tref psi = conj(eq0(a),
		disj(eq0(land(x, b)), eq0(land(x, bvar("c")))));
	CHECK(!ap::try_witness_deep<node_t>(EX, fvar("x"), psi).has_value());
}

TEST_CASE("W9: the case pin at a spine, and its ∀ dual") {
	tref x = bvar("x"), a = bvar("a"), c = bvar("c");
	// A guarded assignment under a conjunction: every branch pins `x`
	// strictly (`x = 0` and `x′ = 0`), so the spine distributes and each
	// branch deletes the binder.
	const tref b1 = conj(eq0(c), eq0(x));
	const tref b2 = conj(neg(eq0(c)), eq0(lneg(x)));
	const tref phi = conj(disj(b1, b2), eq0(lor(x, a)));
	auto r = ap::try_witness_deep<node_t>(EX, fvar("x"), phi);
	REQUIRE(r.has_value());
	CHECK(!holds_var(*r, "x"));
	CHECK(are_nso_equivalent<node_t>(ex("x", phi), *r));
	// ∀ form, the same shape under ¬∃x¬: the spine is a disjunction, the
	// case-pin member an ∧-of-branches, and every branch holds a NEGATED
	// equation pinning `x`.
	const tref d1 = disj(neg(eq0(c)), neg(eq0(x)));
	const tref d2 = disj(eq0(c), neg(eq0(lneg(x))));
	const tref psi = disj(conj(d1, d2), neg(eq0(lor(x, a))));
	auto q = ap::try_witness_deep<node_t>(ALL, fvar("x"), psi);
	REQUIRE(q.has_value());
	CHECK(!holds_var(*q, "x"));
	CHECK(are_nso_equivalent<node_t>(all_("x", psi), *q));
}

TEST_CASE("W10: the case-pin counterexample — the WHOLE member must avoid D") {
	tref x = bvar("x"), a = bvar("a"), y = bvar("y");
	// `∃x ∀y.((y = 0 ∧ x = 0) ∨ (y ≠ 0 ∧ x = 1))` is F while its rewrite
	// `∀y.(y = 0 ∨ y ≠ 0)` is T: past a kind flip the branch CHOICE may
	// not depend on the inner variable (§3).
	const tref guard = disj(conj(eq0(y), eq0(x)),
		conj(neg(eq0(y)), eq0(lneg(x))));
	CHECK(!ap::try_witness_deep<node_t>(EX, fvar("x"),
		all_("y", guard)).has_value());
	// The same guard inside a spine, so that the case pin is really the
	// step being refused: the member holds `y`, which is barred, and `x`
	// is free in two members, so the descent stops.
	CHECK(!ap::try_witness_deep<node_t>(EX, fvar("x"),
		all_("y", conj(guard, eq0(lor(x, a))))).has_value());
	// THE TEST IS REACHED, and `D` is the only thing refusing it: the same
	// shape under a binder of `Q`'s OWN kind — nothing flips, so `D` stays
	// empty and `y` is usable — IS rewritten, to `∃y.(y = 0 ∨ y ≠ 0)`.
	auto r = ap::try_witness_deep<node_t>(EX, fvar("x"), ex("y", guard));
	REQUIRE(r.has_value());
	CHECK(!holds_var(*r, "x"));
	CHECK(are_nso_equivalent<node_t>(ex("x", ex("y", guard)), *r));
}

TEST_CASE("W10b: a node that is no junction is a ONE-MEMBER spine") {
	tref x = bvar("x"), t = bvar("t"), a = bvar("a"), c = bvar("c");
	tref t1 = bvar("t1"), t2 = bvar("t2");
	// `∃x.(x = t)`: the body is the whole spine, the pin is its only
	// member, and what is left is the EMPTY ∧-join — `T`.
	auto r = ap::try_witness_deep<node_t>(EX, fvar("x"), eq(x, t));
	REQUIRE(r.has_value());
	CHECK(tau::get(*r).equals_T());
	// The ∀ dual, in both spellings of the negated equation: the empty
	// ∨-join is `F`, and `∀x.(x ≠ t)` is indeed F.
	for (tref pinning : { neg(eq(x, t)), neq(x, t) }) {
		auto q = ap::try_witness_deep<node_t>(ALL, fvar("x"), pinning);
		REQUIRE(q.has_value());
		CHECK(tau::get(*q).equals_F());
	}
	// Under the OTHER connective: the descent reaches the atom, which is
	// its own one-member spine, and `a ∨ T` folds.
	const tref under_or = disj(eq0(a), eq(x, t));
	auto u = ap::try_witness_deep<node_t>(EX, fvar("x"), under_or);
	REQUIRE(u.has_value());
	CHECK(!holds_var(*u, "x"));
	CHECK(are_nso_equivalent<node_t>(ex("x", under_or), *u));
	CHECK(tau::get(*u).equals_T());
	// A BARE guarded assignment, with no `R` to carry: the ∨-node is the
	// one-member spine and the case pin rewrites it to
	// `⋁ᵢ (dᵢ minus its pin)[x ← tᵢ]`.
	const tref bare = disj(conj(eq0(c), eq(x, t1)),
		conj(neg(eq0(c)), eq(x, t2)));
	auto b = ap::try_witness_deep<node_t>(EX, fvar("x"), bare);
	REQUIRE(b.has_value());
	CHECK(!holds_var(*b, "x"));
	CHECK(are_nso_equivalent<node_t>(ex("x", bare), *b));
}

TEST_CASE("W11: a branch that is a unit is never a case pin") {
	tref x = bvar("x"), a = bvar("a"), c = bvar("c"), z = bvar("z");
	// The same guarded assignment as W9, with the second branch wrapped in
	// a binder: a unit is no conjunction (§4), so the match sees no pin in
	// it, the case pin fails all-or-nothing, and nothing is rewritten.
	const tref b1 = conj(eq0(c), eq0(x));
	const tref b2 = ex("z", conj(eq0(lxor(z, c)), eq0(lneg(x))));
	const tref phi = conj(disj(b1, b2), eq0(lor(x, a)));
	CHECK(!ap::try_witness_deep<node_t>(EX, fvar("x"), phi).has_value());
}

// --- TRY_CASE_WITNESS -------------------------------------------------------------

TEST_CASE("W12: the guarded-assignment match, its branches and its rest") {
	tref x = bvar("x"), a = bvar("a"), c = bvar("c");
	tref t1 = bvar("t1"), t2 = bvar("t2");
	const tref b1 = conj(eq0(c), eq(x, t1));
	const tref b2 = conj(neg(eq0(c)), eq(x, t2));
	const tref rest = eq0(lor(x, a));
	const tref psi = conj(disj(b1, b2), rest);
	auto r = ap::try_case_witness<node_t>(fvar("x"), psi);
	REQUIRE(r.has_value());
	REQUIRE(r->branches.size() == 2);
	// The branches come back AS THEY STAND — the pin stays and becomes the
	// residual at phase 4 (§3) — each with the witness its pin gives.
	for (const auto& [branch, witness] : r->branches) {
		CHECK((same(branch, b1) || same(branch, b2)));
		CHECK((same(witness, t1) || same(witness, t2)));
	}
	CHECK(!same(r->branches[0].second, r->branches[1].second));
	// `rest` is ψ without the case pin.
	CHECK(same(r->rest, rest));
	// Nothing about `x` is decided here: this function only MATCHES.
	CHECK(holds_var(r->rest, "x"));
}

TEST_CASE("W13: the smallest |D| is the one matched") {
	tref x = bvar("x"), a = bvar("a"), c = bvar("c"), e = bvar("e");
	tref t1 = bvar("t1"), t2 = bvar("t2");
	// Two case pins in one conjunction. The wider one carries an extra
	// conjunct per branch, so its `|D|` (formula_size) is the larger.
	const tref s1 = conj(eq0(c), eq(x, t1));
	const tref s2 = conj(neg(eq0(c)), eq(x, t2));
	const tref small = disj(s1, s2);
	const tref wide = disj(conj(eq0(e), conj(eq0(c), eq(x, t1))),
		conj(eq0(e), conj(neg(eq0(c)), eq(x, t2))));
	REQUIRE(ap::formula_size<node_t>(small) < ap::formula_size<node_t>(wide));
	const tref psi = conj(wide, conj(small, eq0(lor(x, a))));
	auto r = ap::try_case_witness<node_t>(fvar("x"), psi);
	REQUIRE(r.has_value());
	REQUIRE(r->branches.size() == 2);
	for (const auto& [branch, witness] : r->branches)
		CHECK((same(branch, s1) || same(branch, s2)));
	// and the wider one is part of what is left over.
	CHECK(has_member(r->rest, wide));
}

TEST_CASE("W14: `case_max` refuses a wider case pin, and a unit branch is "
	"opaque") {
	tref x = bvar("x"), a = bvar("a"), c = bvar("c"), z = bvar("z");
	tref t1 = bvar("t1"), t2 = bvar("t2");
	const tref psi = conj(disj(conj(eq0(c), eq(x, t1)),
		conj(neg(eq0(c)), eq(x, t2))), eq0(lor(x, a)));
	REQUIRE(ap::try_case_witness<node_t>(fvar("x"), psi).has_value());
	{	// §1 `K″`, read bare at phase 2: one branch is all that is
		// allowed now, and a two-branch pin declines.
		case_max_guard cap(1);
		CHECK(!ap::try_case_witness<node_t>(fvar("x"), psi).has_value());
	}
	CHECK(ap::case_max == 16);
	// A branch that is a unit: no conjunction, so no pin is read out of it
	// and the match fails all-or-nothing (§4).
	const tref opaque = conj(disj(conj(eq0(c), eq(x, t1)),
		ex("z", conj(eq0(lxor(z, c)), eq(x, t2)))), eq0(lor(x, a)));
	CHECK(!ap::try_case_witness<node_t>(fvar("x"), opaque).has_value());
}

// --- ELIMINATE_BY_SUBSTITUTION ----------------------------------------------------

TEST_CASE("W15: one pass eliminates two binders, nested or side by side") {
	tref x = bvar("x"), w = bvar("w"), t = bvar("t");
	tref a = bvar("a"), b = bvar("b"), d = bvar("d");
	// Side by side: each binder is its own attempt.
	const tref two = conj(ex("x", conj(eq(x, t), eq0(lor(x, a)))),
		ex("w", conj(eq(w, b), eq0(lor(w, d)))));
	tref got = ap::eliminate_by_substitution<node_t>(two);
	CHECK(!holds(got, tau::wff_ex));
	CHECK(are_nso_equivalent<node_t>(got, two));
	CHECK(same(got, conj(eq0(lor(t, a)), eq0(lor(b, d)))));
	// Nested: the outer rewrite substitutes INTO the inner binder's body
	// (§4, what may touch a unit), and the walk continues into what it
	// returned, so the inner binder is reached in the SAME pass.
	const tref nested = ex("x", conj(eq(x, t),
		ex("w", conj(eq(w, b), eq0(lor(lor(x, w), a))))));
	tref r = ap::eliminate_by_substitution<node_t>(nested);
	CHECK(!holds(r, tau::wff_ex));
	CHECK(are_nso_equivalent<node_t>(r, nested));
	CHECK(same(r, eq0(lor(lor(t, b), a))));
	// A binder over a lone pin: the one-member spine leaves `T`.
	CHECK(tau::get(ap::eliminate_by_substitution<node_t>(
		ex("x", eq(x, t)))).equals_T());
	// A formula no rewrite fires on comes back as the same node.
	const tref stuck = ex("x", conj(eq0(land(x, b)), eq0(lor(x, a))));
	CHECK(ap::eliminate_by_substitution<node_t>(stuck) == stuck);
}

} // TEST_SUITE
