// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for src/anti_prenex/witness/witness.h.
// Spec: anti_prenex.md §3 TRY_WITNESS, TRY_WITNESS_DEEP, "A CASE PIN for x",
// TRY_CASE_WITNESS and ELIMINATE_BY_SUBSTITUTION; §4 "What may touch a unit";
// §1's `case_max`.
//
// The suite `anti_prenex/witness` runs in the PLAIN regime — phase 2 has no
// block, no live order and nothing BDD-backed — which is exactly what the
// spelled mode assumes. The suite `anti_prenex/witness-cof` runs the COF mode
// instead: a block, the component's ctx and its order, and `prepare_terms`
// over the conjunction, the production spelling of phase 4. A BDD-backed term
// is never compared by `to_str()`: semantic equality is `same_function`, and
// a formula is finished with `convert_to_tau_terms` before the oracle reads
// it.
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

// --- the COF mode's fixtures ------------------------------------------------------

using th = term_handle<node_t>;
using tb = tau_term_bdd<node_t>;

/// §5's setup for one component together with the body prepared over it
/// (§1's term representation): the state phase 4 hands the COF mode.
/// Untyped variables throughout — nothing on this path consults a type table.
struct component {
	ap::block P;
	ap::ctx<node_t> c;
	tref psi;
	component(const ap::block& block, tref body)
		: P(block), c(ap::ctx<node_t>::for_component(block, 0, false)),
		psi(ap::prepare_terms<node_t>(body, block, c.order)) {}
};

/// The `l = r` sides of an atom, through no negation.
std::pair<tref, tref> sides(tref atom) {
	const tau& t = tau::get(atom);
	REQUIRE(t.is(tau::wff));
	REQUIRE(t[0].has_child());
	return { tau::trim_right_sibling(t[0].first()),
		tau::trim_right_sibling(t[0].second()) };
}

/// Semantic equality of two terms over the variables `vs`: equal ROBDDs under
/// one order over all of them (leaves opaque), both finished first.
bool same_function(tref a, tref b, const ap::block& vs) {
	ap::var_order<node_t> o;
	for (size_t i = 0; i < vs.size(); ++i)
		o.emplace(vs[i], int_t(vs.size() - i));
	return tb::build_bdd(th::convert_to_tau_terms(a), o)
		== tb::build_bdd(th::convert_to_tau_terms(b), o);
}

/// The formula with every stored BDD spelled out, which is what the oracle
/// reads.
tref finished(tref n) { return th::convert_to_tau_terms(n); }

/// The member of `n` that does not mention `name` — how the image of a
/// pinning conjunct is told from its siblings'.
tref member_without(tref n, const char* name) {
	tref found = nullptr;
	for (tref m : ap::members<node_t>(n))
		if (!holds_var(m, name)) found = m;
	return found;
}

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
	// to `G` syntactically. It is the member that does not mention `b`.
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
	{	// §1 `K″`, read bare at phase 2: with the cap at one branch,
		// a two-branch pin declines.
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

TEST_SUITE("anti_prenex/witness-cof") {

// --- TRY_WITNESS, COF mode --------------------------------------------------------

TEST_CASE("WC1: a strict pin is witnessed in every conjunct") {
	tref x = fvar("x");
	// `ψ = (x = t) ∧ (x ∪ a = 0)` over the block `{x}`: both terms go into
	// §1's representation, `x` becoming the decision variable.
	component k({ x }, conj(eq(bvar("x"), bvar("t")),
		eq0(lor(bvar("x"), bvar("a")))));
	REQUIRE(holds(k.psi, tau::BDD_ID));
	auto r = ap::try_witness<node_t>(x, k.psi, k.c);
	REQUIRE(r.has_value());
	CHECK(!holds_var(*r, "x"));
	// the pinning conjunct's image is `p = 0`, which a STRICT pin folds to
	// `T`, so the sibling `t ∪ a = 0` is all that is left
	REQUIRE(is_child<node_t>(*r, tau::bf_eq));
	CHECK(same_function(sides(*r).first, lor(bvar("t"), bvar("a")),
		{ fvar("t"), fvar("a") }));
	CHECK(tau::get(sides(*r).second).equals_0());
	CHECK(are_nso_equivalent<node_t>(finished(*r), ex("x", finished(k.psi))));
}

TEST_CASE("WC2: the WEAK pin's conjunct carries its residual") {
	tref y = fvar("y");
	const tref G = land(bvar("c"), bvar("d"));
	// `(y + t) ∪ G = 0` — the shape a settled sub-block emits (§3) — has
	// the cofactors `t ∪ G` and `t′ ∪ G`, complementary only where `G = 0`,
	// so the witness is `t·G′` and the residual is `G = 0`.
	component k({ y }, conj(eq0(lor(lxor(bvar("y"), bvar("t")), G)),
		eq0(land(bvar("y"), bvar("b")))));
	REQUIRE(holds(k.psi, tau::BDD_ID));
	auto r = ap::try_witness<node_t>(y, k.psi, k.c);
	REQUIRE(r.has_value());
	CHECK(!holds_var(*r, "y"));
	CHECK(are_nso_equivalent<node_t>(finished(*r), ex("y", finished(k.psi))));
	// THE PINNING CONJUNCT STAYS, substituted like every sibling, and its
	// image IS the residual (§3): it is the member free of `b`.
	const tref image = member_without(*r, "b");
	REQUIRE(image != nullptr);
	REQUIRE(is_child<node_t>(image, tau::bf_eq));
	CHECK(same_function(sides(image).first, G, { fvar("c"), fvar("d") }));
	CHECK(tau::get(sides(image).second).equals_0());
}

TEST_CASE("WC3: a strict pin comes first, then the smallest witness") {
	tref x = fvar("x");
	const tref weak = eq0(lor(bvar("x"), bvar("a")));
	const tref strict = eq(bvar("x"), bvar("b"));
	// `x ∪ a = 0` pins weakly (witness `0`, residual `a`); `x = b` pins
	// strictly, with witness `b`. Whichever comes first, the STRICT pin is
	// taken — the two eliminations are equivalent, so what tells them apart
	// is the SHAPE: under the strict witness the weak conjunct's image is
	// `b ∪ a = 0` and the pinning one folds to `T`, leaving ONE atom.
	for (tref raw : { conj(weak, strict), conj(strict, weak) }) {
		component k({ x }, raw);
		auto r = ap::try_witness<node_t>(x, k.psi, k.c);
		REQUIRE(r.has_value());
		CHECK(!holds_var(*r, "x"));
		REQUIRE(is_child<node_t>(*r, tau::bf_eq));
		CHECK(same_function(sides(*r).first, lor(bvar("b"), bvar("a")),
			{ fvar("a"), fvar("b") }));
		CHECK(are_nso_equivalent<node_t>(finished(*r),
			ex("x", finished(k.psi))));
	}
	// Two WEAK pins and no strict one: the smallest `‖f₁′‖` wins — `0` over
	// `t·G′` — so the first conjunct's image is its residual `a = 0` and
	// the second's mentions `t`.
	const tref G = land(bvar("c"), bvar("d"));
	component k({ x }, conj(eq0(lor(bvar("x"), bvar("a"))),
		eq0(lor(lxor(bvar("x"), bvar("t")), G))));
	auto r = ap::try_witness<node_t>(x, k.psi, k.c);
	REQUIRE(r.has_value());
	CHECK(!holds_var(*r, "x"));
	CHECK(are_nso_equivalent<node_t>(finished(*r), ex("x", finished(k.psi))));
	const tref residual = member_without(*r, "t");
	REQUIRE(residual != nullptr);
	REQUIRE(is_child<node_t>(residual, tau::bf_eq));
	CHECK(same_function(sides(residual).first, bvar("a"), { fvar("a") }));
}

TEST_CASE("WC4: a negated equation pins nothing in the ∃ sense") {
	tref x = fvar("x");
	// `¬(x = t)` is exactly what pins in the ∀ sense; here neither conjunct
	// pins, `x·b` having the cofactors `0` and `b`.
	component k({ x }, conj(neg(eq(bvar("x"), bvar("t"))),
		eq0(land(bvar("x"), bvar("b")))));
	REQUIRE(holds(k.psi, tau::BDD_ID));
	CHECK(!ap::try_witness<node_t>(x, k.psi, k.c).has_value());
}

TEST_CASE("WC5: a conjunct whose term does not mention `x` is skipped") {
	tref x = fvar("x");
	// `a ∪ b = 0` can pin nothing about `x`, so the scan passes it by
	// instead of asking `COF`, which Debug-asserts against such a query.
	component k({ x }, conj(eq0(lor(bvar("a"), bvar("b"))),
		eq(bvar("x"), bvar("t"))));
	auto r = ap::try_witness<node_t>(x, k.psi, k.c);
	REQUIRE(r.has_value());
	CHECK(!holds_var(*r, "x"));
	CHECK(are_nso_equivalent<node_t>(finished(*r), ex("x", finished(k.psi))));
	// the same skip with nothing left that pins `x`: `nullopt`, no assertion
	component none({ x }, conj(eq0(bvar("a")),
		eq0(land(bvar("x"), bvar("b")))));
	CHECK(!ap::try_witness<node_t>(x, none.psi, none.c).has_value());
}

TEST_CASE("WC6: the result stays BDD-backed over the rest of the block, and "
	"is simplified") {
	tref x = fvar("x"), y = fvar("y");
	// A block of two: after `x ← t` the surviving conjunct still branches on
	// `y`, so its term stays in §1's representation.
	component k({ x, y }, conj(eq(bvar("x"), bvar("t")),
		eq0(lor(land(bvar("x"), bvar("y")), bvar("a")))));
	auto r = ap::try_witness<node_t>(x, k.psi, k.c);
	REQUIRE(r.has_value());
	CHECK(!holds_var(*r, "x"));
	CHECK(holds_var(*r, "y"));
	REQUIRE(is_child<node_t>(*r, tau::bf_eq));
	CHECK(th::is_bdd_backed(sides(*r).first));
	// SIMPLIFIED here (invariant 6): simplifying again changes nothing.
	CHECK(ap::simplify<node_t>(*r, k.c.order) == *r);
	CHECK(are_nso_equivalent<node_t>(finished(*r), ex("x", finished(k.psi))));
}

// --- TRY_CASE_WITNESS, COF mode ---------------------------------------------------

TEST_CASE("WC7: the guarded-assignment match, its branches and its rest") {
	tref x = fvar("x");
	const tref rest = eq0(lor(bvar("x"), bvar("a")));
	component k({ x }, conj(disj(conj(eq0(bvar("c")), eq(bvar("x"), bvar("t1"))),
		conj(neg(eq0(bvar("c"))), eq(bvar("x"), bvar("t2")))), rest));
	REQUIRE(holds(k.psi, tau::BDD_ID));
	auto r = ap::try_case_witness<node_t>(x, k.psi, k.c);
	REQUIRE(r.has_value());
	REQUIRE(r->branches.size() == 2);
	// the branches are ψ's own, AS THEY STAND — the pin stays and becomes
	// the branch's residual under `[x ← tᵢ]` (§3) — each with the witness
	// its pin gives
	tref cases = nullptr, kept = nullptr;
	for (tref m : ap::members<node_t>(k.psi))
		if (is_child<node_t>(m, tau::wff_or)) cases = m; else kept = m;
	REQUIRE(cases != nullptr);
	const trefs bs = ap::members<node_t>(cases);
	REQUIRE(bs.size() == 2);
	const ap::block ts{ fvar("t1"), fvar("t2") };
	for (const auto& [branch, witness] : r->branches) {
		CHECK((same(branch, bs[0]) || same(branch, bs[1])));
		CHECK((same_function(witness, bvar("t1"), ts)
			|| same_function(witness, bvar("t2"), ts)));
	}
	CHECK(!same(r->branches[0].first, r->branches[1].first));
	CHECK(!same_function(r->branches[0].second, r->branches[1].second, ts));
	// `rest` is ψ without the case pin, and nothing about `x` is decided
	// here: this function only MATCHES.
	CHECK(same(r->rest, kept));
	CHECK(holds_var(r->rest, "x"));
}

TEST_CASE("WC8: the smallest |D| is the one matched") {
	tref x = fvar("x");
	const tref s1 = conj(eq0(bvar("c")), eq(bvar("x"), bvar("t1")));
	const tref s2 = conj(neg(eq0(bvar("c"))), eq(bvar("x"), bvar("t2")));
	// Two case pins in one conjunction; the wider one carries an extra
	// conjunct per branch, so its `|D|` (`formula_size`) is the larger.
	component k({ x }, conj(disj(conj(eq0(bvar("e")), s1),
		conj(eq0(bvar("e")), s2)),
		conj(disj(s1, s2), eq0(lor(bvar("x"), bvar("a"))))));
	trefs ors;
	for (tref m : ap::members<node_t>(k.psi))
		if (is_child<node_t>(m, tau::wff_or)) ors.push_back(m);
	REQUIRE(ors.size() == 2);
	if (ap::formula_size<node_t>(ors[1]) < ap::formula_size<node_t>(ors[0])) {
		const tref t = ors[0]; ors[0] = ors[1]; ors[1] = t;
	}
	REQUIRE(ap::formula_size<node_t>(ors[0]) < ap::formula_size<node_t>(ors[1]));
	auto r = ap::try_case_witness<node_t>(x, k.psi, k.c);
	REQUIRE(r.has_value());
	REQUIRE(r->branches.size() == 2);
	const trefs bs = ap::members<node_t>(ors[0]);
	REQUIRE(bs.size() == 2);
	for (const auto& [branch, witness] : r->branches)
		CHECK((same(branch, bs[0]) || same(branch, bs[1])));
	// and the wider one is part of what is left over
	CHECK(has_member(r->rest, ors[1]));
}

TEST_CASE("WC9: `ctx.case_max` refuses a wider case pin") {
	tref x = fvar("x");
	component k({ x }, conj(disj(conj(eq0(bvar("c")), eq(bvar("x"), bvar("t1"))),
		conj(neg(eq0(bvar("c"))), eq(bvar("x"), bvar("t2")))),
		eq0(lor(bvar("x"), bvar("a")))));
	REQUIRE(ap::try_case_witness<node_t>(x, k.psi, k.c).has_value());
	// §1 `K″` per COMPONENT: phase 4 has a ctx, so the threshold is read
	// off it and the bare constant is never touched.
	k.c.case_max = 1;
	CHECK(!ap::try_case_witness<node_t>(x, k.psi, k.c).has_value());
	CHECK(ap::case_max == 16);
}

TEST_CASE("WC10: a unit branch is opaque, and a branch without a pin "
	"disqualifies the disjunction") {
	tref x = fvar("x");
	const tref b1 = conj(eq0(bvar("c")), eq(bvar("x"), bvar("t1")));
	const tref rest = eq0(lor(bvar("x"), bvar("a")));
	// A BINDER is no conjunction (§4): its member view is the unit itself,
	// which is no equation, so the branch holds no pin and the match fails
	// all-or-nothing.
	component k({ x }, conj(disj(b1, ex("z",
		conj(eq0(lxor(bvar("z"), bvar("c"))), eq(bvar("x"), bvar("t2"))))),
		rest));
	CHECK(!ap::try_case_witness<node_t>(x, k.psi, k.c).has_value());
	// The same all-or-nothing rule on a plain branch whose conjuncts pin
	// nothing: `x·b` has the cofactors `0` and `b`.
	component m({ x }, conj(disj(b1, conj(neg(eq0(bvar("c"))),
		eq0(land(bvar("x"), bvar("b"))))), rest));
	CHECK(!ap::try_case_witness<node_t>(x, m.psi, m.c).has_value());
}

} // TEST_SUITE
