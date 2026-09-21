// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Layer 3 unit tests for src/anti_prenex/eliminate/conditions.h: §7's shared
// helpers — FREEZE_OPAQUE_COMPONENTS, SQUEEZE, POSITIVE_CONDITION,
// NEGATIVE_CONDITION and TREE_CONDITION. Spec: anti_prenex.md §7 (the method
// contract and the shared helpers), §6 (PARTS), §1 (the term representation,
// the negative tree, the zero form, the discharge identities).
//
// Everything runs on PREPARED terms: a block, the component's ctx and its
// order, `prepare_terms` over the clause and its member view — the state the
// method hands its helpers. Untyped variables throughout: nothing on this
// path consults a type table. A BDD-backed term is never compared by
// to_str(): term equality is `same_function` (equal ROBDDs under one order)
// and formula equivalence is `are_nso_equivalent` on the FINISHED formulas.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/eliminate/conditions.h"

#include <algorithm>

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

using tb = tau_term_bdd<node_t>;
using th = term_handle<node_t>;
using order_t = ap::var_order<node_t>;

tref bf(const char* s) {
	tref t = tau::get(s, parse_bf());
	REQUIRE(t != nullptr);
	return t;
}
tref wff(const char* s) {
	tref t = tau::get(s, parse_wff());
	REQUIRE(t != nullptr);
	return t;
}
/// The trimmed `variable` node of a one-variable term — the shape order keys,
/// blocks and free-variable sets hold.
tref vr(const char* s) { return tau::trim(bf(s)); }

tref eq0(tref t)          { return build_bf_eq_0<node_t>(t); }
tref eq(tref l, tref r)   { return tau::build_bf_eq(l, r); }
tref neg(tref n)          { return tau::build_wff_neg(n); }
tref conj(tref l, tref r) { return tau::build_wff_and(l, r); }
tref disj(tref l, tref r) { return tau::build_wff_or(l, r); }
tref land(tref l, tref r) { return build_bf_and<node_t>(l, r); }
tref lor(tref l, tref r)  { return build_bf_or<node_t>(l, r); }
tref lneg(tref t)         { return build_bf_neg<node_t>(t); }
tref lxor(tref l, tref r) { return build_bf_xor<node_t>(l, r); }

tref ex(const char* v, tref body) {
	return tau::build_wff_ex(vr(v), body, false);
}

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

/// A real order atom `l <= r`. On a non-bitvector type the construction hooks
/// rewrite `<=` into the equation `l·r′ = 0`, so one does not survive
/// construction here; the parser builds with the hooks disabled, and this
/// helper reproduces that shape the same way.
tref order_atom(const char* l, const char* r) {
	use_hooks_guard<node_t> g(false);
	return tau::build_bf_lteq(bf(l), bf(r));
}

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
	order_t o;
	for (size_t i = 0; i < vs.size(); ++i)
		o.emplace(vs[i], int_t(vs.size() - i));
	return tb::build_bdd(th::convert_to_tau_terms(a), o)
		== tb::build_bdd(th::convert_to_tau_terms(b), o);
}

/// The formula with every stored BDD spelled out, which is what the oracle
/// reads.
tref finished(tref n) { return th::convert_to_tau_terms(n); }

/// Is there a node of this kind anywhere in `n`?
bool holds(tref n, size_t nt) {
	return tau::get(n).find_top([nt](tref m) {
		return tree<node_t>::get(m).is(nt); }) != nullptr;
}

/// A member of `n` content-equal to `m`?
bool has_member(tref n, tref m) {
	for (tref c : ap::members<node_t>(n)) if (same(c, m)) return true;
	return false;
}

/// §5's setup for one component, §1's term representation over it, and the
/// member view of the clause — taken ONCE, the way `ELIMINATE_BLOCK` takes it
/// and the way every helper of §7 wants its clause.
struct prepared_clause {
	ap::block P;
	ap::ctx<node_t> c;
	tref psi;
	trefs conjuncts;
	prepared_clause(const ap::block& block, tref body)
		: P(block), c(ap::ctx<node_t>::for_component(block, 0, false)),
		psi(ap::prepare_terms<node_t>(body, block, c.order)),
		conjuncts(ap::members<node_t>(psi)) {}
};

/// The atomless method's `opaque?` in the shape gate 4 will carry it: not a
/// (¬)equation, or a (¬)equation whose term hides a block variable in a leaf
/// (§1 leaf hazard — quantification is cofactoring, so `DISCHARGE` would emit
/// that variable free). A negative tree is read literal-wise there; no test
/// here needs that reading.
struct opaque_test {
	ap::block X;
	order_t order;
	bool operator()(tref c) const {
		if (!is_child<node_t>(c, tau::bf_eq)
			&& !ap::is_negated_equation<node_t>(c)) return true;
		const trefs leaves =
			ap::leaf_fv<node_t>(ap::term_of<node_t>(c, order));
		for (tref x : X)
			if (std::binary_search(leaves.begin(), leaves.end(), x,
				tau::subtree_less)) return true;
		return false;
	}
};

} // namespace

TEST_SUITE("anti_prenex/conditions") {

// --- FREEZE_OPAQUE_COMPONENTS -----------------------------------------------------

TEST_CASE("F1: every shape this engine cannot use freezes its component") {
	tref x = vr("x"), y = vr("y");
	// Each opaque conjunct below holds `x`; the transparent `y ∪ b = 0`
	// holds `y` alone and is a component of its own, which nothing
	// freezes.
	const tref transparent = eq0(lor(bf("y"), bf("b")));
	// A reference, an order atom, a binder unit and a temporal operator:
	// none is an equation, and this engine answers no queries, so it can
	// swallow none of them. The hidden leaf is the fifth shape: an
	// equation whose term carries `x` inside a reference argument, where
	// no cofactor reaches it.
	const trefs opaque_shapes{
		wff("f(x)"),
		order_atom("x", "a"),
		ex("z", eq0(lor(bf("z"), bf("x")))),
		tau::build_wff_always(eq0(land(bf("x"), bf("a")))),
		eq0(lor(bf("r(x)"), land(bf("x"), bf("a")))) };
	for (tref o : opaque_shapes) {
		prepared_clause k({ x, y }, conj(o, transparent));
		REQUIRE(k.conjuncts.size() == 2);
		const opaque_test opaque{ k.P, k.c.order };
		REQUIRE(opaque(k.conjuncts[0]));
		REQUIRE(!opaque(k.conjuncts[1]));

		auto r = ap::freeze_opaque_components<node_t>(k.conjuncts,
			k.P, opaque);
		// the opaque component is re-wrapped over its own variable,
		// and the other one is untouched
		REQUIRE(is_child<node_t>(r.frozen, tau::wff_ex));
		CHECK(same(ap::binder_var<node_t>(r.frozen), x));
		CHECK(same(ap::binder_body<node_t>(r.frozen), k.conjuncts[0]));
		REQUIRE(r.conjuncts.size() == 1);
		CHECK(same(r.conjuncts[0], k.conjuncts[1]));
		REQUIRE(r.X.size() == 1);
		CHECK(same(r.X[0], y));
	}
}

TEST_CASE("F2: a component is frozen whole, and a disjoint one is not") {
	tref x = vr("x"), y = vr("y");
	// `∃` does not distribute over `∧` across shared variables: the
	// transparent `x ∪ a = 0` shares `x` with the binder unit, so it
	// cannot keep an `∃x` of its own and is frozen with it. `y ∪ b = 0`
	// shares nothing and stays.
	const tref unit = ex("z", eq0(lor(bf("z"), bf("x"))));
	const tref shared = eq0(lor(bf("x"), bf("a")));
	const tref apart = eq0(lor(bf("y"), bf("b")));
	prepared_clause k({ x, y }, conj(unit, conj(shared, apart)));
	REQUIRE(k.conjuncts.size() == 3);
	const opaque_test opaque{ k.P, k.c.order };
	REQUIRE(!opaque(k.conjuncts[1]));

	auto r = ap::freeze_opaque_components<node_t>(k.conjuncts, k.P, opaque);
	REQUIRE(is_child<node_t>(r.frozen, tau::wff_ex));
	CHECK(same(ap::binder_var<node_t>(r.frozen), x));
	const tref body = ap::binder_body<node_t>(r.frozen);
	CHECK(has_member(body, k.conjuncts[0]));
	CHECK(has_member(body, k.conjuncts[1]));
	// What comes back is the rest of the clause and the rest of the block,
	// each in its own input order.
	REQUIRE(r.conjuncts.size() == 1);
	CHECK(same(r.conjuncts[0], k.conjuncts[2]));
	REQUIRE(r.X.size() == 1);
	CHECK(same(r.X[0], y));
}

TEST_CASE("F3: no opaque conjunct freezes nothing") {
	tref x = vr("x"), y = vr("y");
	prepared_clause k({ x, y }, conj(eq0(lor(bf("x"), bf("a"))),
		eq0(lor(bf("y"), bf("b")))));
	REQUIRE(k.conjuncts.size() == 2);
	auto r = ap::freeze_opaque_components<node_t>(k.conjuncts, k.P,
		opaque_test{ k.P, k.c.order });
	// `T`, so that a caller can conjoin the frozen part unconditionally.
	CHECK(tau::get(r.frozen).equals_T());
	REQUIRE(r.conjuncts.size() == 2);
	CHECK(same(r.conjuncts[0], k.conjuncts[0]));
	CHECK(same(r.conjuncts[1], k.conjuncts[1]));
	REQUIRE(r.X.size() == 2);
	CHECK(same(r.X[0], x));
	CHECK(same(r.X[1], y));
}

// --- SQUEEZE ------------------------------------------------------------------------

TEST_CASE("S1: the zero form, the component and the negatives") {
	tref x = vr("x"), a = vr("a"), b = vr("b");
	prepared_clause k({ x }, conj(eq(bf("x"), bf("a")),
		neg(eq(bf("x"), bf("b")))));
	REQUIRE(k.conjuncts.size() == 2);

	auto r = ap::squeeze<node_t>(k.conjuncts, k.P, k.c.order);
	REQUIRE(!r.decided_false);
	REQUIRE(r.conjuncts.size() == 2);
	// `l = r` became `l + r = 0`, at both polarities.
	REQUIRE(is_child<node_t>(r.conjuncts[0], tau::bf_eq));
	CHECK(tau::get(sides(r.conjuncts[0]).second).equals_0());
	CHECK(same_function(sides(r.conjuncts[0]).first,
		lxor(bf("x"), bf("a")), { x, a }));
	REQUIRE(ap::is_negated_equation<node_t>(r.conjuncts[1]));
	CHECK(tau::get(sides(ap::atom_of<node_t>(r.conjuncts[1])).second)
		.equals_0());
	// one component over the one positive, and `f` is its term
	REQUIRE(r.comps.size() == 1);
	REQUIRE(r.comps[0].vars.size() == 1);
	CHECK(same(r.comps[0].vars[0], x));
	CHECK(same_function(r.comps[0].F, lxor(bf("x"), bf("a")), { x, a }));
	CHECK(same(r.f, r.comps[0].F));
	// the negative conjuncts, in the clause's order
	REQUIRE(r.negatives.size() == 1);
	CHECK(same(r.negatives[0], r.conjuncts[1]));
	CHECK(same_function(ap::term_of<node_t>(r.negatives[0], k.c.order),
		lxor(bf("x"), bf("b")), { x, b }));
}

TEST_CASE("S2: an atom that folds while it is rewritten") {
	tref x = vr("x");
	const ap::block P{ x };
	auto c = ap::ctx<node_t>::for_component(P, 0, false);
	// The two sides denote the same function and are spelled differently —
	// one prepared, one not — so the atom does not fold when it is built.
	// The RING SUM is what sees the equality, which is why the rewritten
	// atom has to be simplified again (invariant 6).
	const tref prepared = sides(ap::prepare_terms<node_t>(
		eq0(lor(bf("x"), bf("a"))), P, c.order)).first;
	const tref folding = eq(prepared, lor(bf("a"), bf("x")));
	REQUIRE(is_child<node_t>(folding, tau::bf_eq));
	const tref rides = eq0(land(bf("x"), bf("b")));

	// `T` says nothing about the clause and is dropped.
	auto dropped = ap::squeeze<node_t>(trefs{ folding, rides }, P, c.order);
	REQUIRE(!dropped.decided_false);
	REQUIRE(dropped.conjuncts.size() == 1);
	CHECK(is_child<node_t>(dropped.conjuncts[0], tau::bf_eq));
	CHECK(dropped.comps.size() == 1);

	// `F` decides the clause, and nothing after it is computed. Here the
	// two sides are complementary, so the ring sum is `1` and the zero
	// form is `1 = 0`.
	const tref unsatisfiable = eq(sides(ap::prepare_terms<node_t>(
		eq0(bf("x")), P, c.order)).first, lneg(bf("x")));
	REQUIRE(is_child<node_t>(unsatisfiable, tau::bf_eq));
	auto decided = ap::squeeze<node_t>(trefs{ unsatisfiable, rides }, P,
		c.order);
	CHECK(decided.decided_false);
}

TEST_CASE("S3: the components are the parts of the POSITIVES alone") {
	tref x = vr("x"), y = vr("y"), a = vr("a"), b = vr("b");
	// The negative holds both block variables, so the push's own incidence
	// pass sees one component; this grouping ignores it and sees two.
	prepared_clause k({ x, y }, conj(eq0(lor(bf("x"), bf("a"))),
		conj(eq0(lor(bf("y"), bf("b"))),
			neg(eq0(land(bf("x"), bf("y")))))));
	REQUIRE(k.conjuncts.size() == 3);

	auto r = ap::squeeze<node_t>(k.conjuncts, k.P, k.c.order);
	REQUIRE(!r.decided_false);
	REQUIRE(r.comps.size() == 2);
	REQUIRE(r.comps[0].vars.size() == 1);
	CHECK(same(r.comps[0].vars[0], x));
	CHECK(same_function(r.comps[0].F, lor(bf("x"), bf("a")), { x, a }));
	REQUIRE(r.comps[1].vars.size() == 1);
	CHECK(same(r.comps[1].vars[0], y));
	CHECK(same_function(r.comps[1].F, lor(bf("y"), bf("b")), { y, b }));
	// `f` is the whole positive part, `⋃_k F_k` over disjoint `X_k`.
	CHECK(same_function(r.f, lor(lor(bf("x"), bf("a")),
		lor(bf("y"), bf("b"))), { x, y, a, b }));
	REQUIRE(r.negatives.size() == 1);
	CHECK(ap::is_negated_equation<node_t>(r.negatives[0]));
}

TEST_CASE("S4: a negative tree is rewritten at its literals, and kept") {
	tref x = vr("x"), b = vr("b"), cc = vr("c");
	prepared_clause k({ x }, conj(eq0(lor(bf("x"), bf("a"))),
		disj(neg(eq(bf("x"), bf("b"))), neg(eq(bf("x"), bf("c"))))));
	REQUIRE(k.conjuncts.size() == 2);

	auto r = ap::squeeze<node_t>(k.conjuncts, k.P, k.c.order);
	REQUIRE(!r.decided_false);
	// The tree is a negative conjunct like a literal, and its own ∨/∧
	// structure is left as it stands.
	REQUIRE(r.negatives.size() == 1);
	const tref tree = r.negatives[0];
	REQUIRE(ap::is_negative_tree<node_t>(tree));
	const trefs leaves = ap::members<node_t>(tree);
	REQUIRE(leaves.size() == 2);
	for (tref l : leaves) {
		REQUIRE(ap::is_negated_equation<node_t>(l));
		CHECK(tau::get(sides(ap::atom_of<node_t>(l)).second).equals_0());
	}
	// the literals' terms are the ring sums of the sides they had
	CHECK(same_function(ap::term_of<node_t>(leaves[0], k.c.order),
		lxor(bf("x"), bf("b")), { x, b }));
	CHECK(same_function(ap::term_of<node_t>(leaves[1], k.c.order),
		lxor(bf("x"), bf("c")), { x, cc }));
	// the positive is the only component; the tree gives none
	REQUIRE(r.comps.size() == 1);
}

// --- POSITIVE_CONDITION -------------------------------------------------------------

TEST_CASE("PC1: one atom per component, each over its own variables") {
	tref x = vr("x"), y = vr("y");
	prepared_clause k({ x, y }, conj(eq0(lor(bf("x"), bf("a"))),
		eq0(lor(bf("y"), bf("b")))));
	auto s = ap::squeeze<node_t>(k.conjuncts, k.P, k.c.order);
	REQUIRE(s.comps.size() == 2);

	// `∀_X f = ⋃_k ∀_{X_k} F_k`, so `∀_X f = 0` is one discharged atom per
	// component: `∀_x (x ∪ a) = a` and `∀_y (y ∪ b) = b`.
	const tref pos = ap::positive_condition<node_t>(s.comps, k.c);
	REQUIRE(is_child<node_t>(pos, tau::wff_and));
	CHECK(ap::members<node_t>(pos).size() == 2);
	CHECK(has_member(pos, ap::simplify_atom<node_t>(eq0(bf("a")), k.c.order)));
	CHECK(has_member(pos, ap::simplify_atom<node_t>(eq0(bf("b")), k.c.order)));
	CHECK(are_nso_equivalent<node_t>(finished(pos),
		conj(eq0(bf("a")), eq0(bf("b")))));
	CHECK(are_nso_equivalent<node_t>(finished(pos),
		tau::build_wff_ex(x, tau::build_wff_ex(y, finished(k.psi)))));

	// No component at all: `f = 0` holds vacuously.
	CHECK(tau::get(ap::positive_condition<node_t>({}, k.c)).equals_T());
}

TEST_CASE("PC2: a component with no common zero decides F") {
	tref x = vr("x");
	// `x = 0 ∧ x′ = 0` is one component whose term is `x ∪ x′ = 1`, and
	// `∀_x 1 = 0` is `F`: no value of `x` satisfies both.
	prepared_clause k({ x }, conj(eq0(bf("x")), eq0(lneg(bf("x")))));
	auto s = ap::squeeze<node_t>(k.conjuncts, k.P, k.c.order);
	REQUIRE(s.comps.size() == 1);
	CHECK(tau::get(s.comps[0].F).equals_1());
	CHECK(tau::get(ap::positive_condition<node_t>(s.comps, k.c)).equals_F());
}

// --- NEGATIVE_CONDITION -------------------------------------------------------------

TEST_CASE("NC1: O1 — a negative whose term is 1 says nothing beyond pos") {
	tref x = vr("x");
	prepared_clause k({ x }, eq0(lor(bf("x"), bf("a"))));
	auto s = ap::squeeze<node_t>(k.conjuncts, k.P, k.c.order);
	REQUIRE(s.comps.size() == 1);
	// The term of `¬(x = x′)`: the ring sum of the two sides is `1`.
	const tref g = ap::simplify_term<node_t>(
		lxor(bf("x"), lneg(bf("x"))), k.c.order);
	REQUIRE(tau::get(g).equals_1());
	CHECK(tau::get(ap::negative_condition<node_t>(g, s.comps, k.P, k.c))
		.equals_T());
}

TEST_CASE("NC2: O2 — no component touches the negative") {
	tref x = vr("x"), y = vr("y");
	// `x ∪ a = 0 ∧ ¬(y = 0)`: the only component holds `x`, so nothing
	// absorbs against `y`, and `∃_y y ≠ 0` is `T`.
	prepared_clause k({ x, y }, conj(eq0(lor(bf("x"), bf("a"))),
		neg(eq0(bf("y")))));
	auto s = ap::squeeze<node_t>(k.conjuncts, k.P, k.c.order);
	REQUIRE(s.comps.size() == 1);
	REQUIRE(s.negatives.size() == 1);
	const tref g = ap::term_of<node_t>(s.negatives[0], k.c.order);
	CHECK(tau::get(ap::negative_condition<node_t>(g, s.comps, k.P, k.c))
		.equals_T());

	// An `X`-FREE `g` lands in the same case with an empty `X_g` and emits
	// `¬(g = 0)` itself — under a formula negation, never a fused `≠`
	// (invariant 4).
	const tref free = ap::negative_condition<node_t>(bf("a"), s.comps,
		k.P, k.c);
	REQUIRE(ap::is_negated_equation<node_t>(free));
	CHECK(!holds(free, tau::bf_neq));
	const auto [l, r] = sides(ap::atom_of<node_t>(free));
	CHECK(same(l, bf("a")));
	CHECK(tau::get(r).equals_0());
	CHECK(!ap::fv_meets<node_t>(free, k.P));
}

TEST_CASE("NC3: the absorption against the components the negative touches") {
	tref x = vr("x"), a = vr("a"), b = vr("b");
	// `x = a ∧ ¬(x·b = 0)`: the condition is `∃_x (x + a)′·x·b ≠ 0`,
	// i.e. `a·b ≠ 0`, and with `pos` it is the whole answer.
	prepared_clause k({ x }, conj(eq(bf("x"), bf("a")),
		neg(eq0(land(bf("x"), bf("b"))))));
	auto s = ap::squeeze<node_t>(k.conjuncts, k.P, k.c.order);
	REQUIRE(s.comps.size() == 1);
	REQUIRE(s.negatives.size() == 1);

	const tref pos = ap::positive_condition<node_t>(s.comps, k.c);
	const tref cond = ap::negative_condition<node_t>(
		ap::term_of<node_t>(s.negatives[0], k.c.order), s.comps, k.P,
		k.c);
	REQUIRE(ap::is_negated_equation<node_t>(cond));
	CHECK(!ap::fv_meets<node_t>(cond, k.P));
	CHECK(same_function(sides(ap::atom_of<node_t>(cond)).first,
		land(bf("a"), bf("b")), { a, b }));
	CHECK(are_nso_equivalent<node_t>(
		finished(ap::simplified_and_join<node_t>(trefs{ pos, cond })),
		ex("x", finished(k.psi))));
}

// --- TREE_CONDITION -----------------------------------------------------------------

TEST_CASE("TC1: a flat tree beside a positive") {
	tref x = vr("x");
	// `x·a = 0 ∧ (¬(x·c = 0) ∨ ¬(x·d = 0))`: each literal's condition is
	// taken against the one component, and the tree's `∨` is re-assembled
	// by the ∨-join.
	prepared_clause k({ x }, conj(eq0(land(bf("x"), bf("a"))),
		disj(neg(eq0(land(bf("x"), bf("c")))),
			neg(eq0(land(bf("x"), bf("d")))))));
	auto s = ap::squeeze<node_t>(k.conjuncts, k.P, k.c.order);
	REQUIRE(s.comps.size() == 1);
	REQUIRE(s.negatives.size() == 1);

	const tref pos = ap::positive_condition<node_t>(s.comps, k.c);
	const tref r = ap::tree_condition<node_t>(s.negatives[0], s.comps,
		k.P, k.c);
	// conditions are negated equations, so what comes back is a negative
	// tree again — plain, over the free variables alone
	CHECK(ap::is_negative_tree<node_t>(r));
	CHECK(!ap::fv_meets<node_t>(r, k.P));
	CHECK(ap::members<node_t>(r).size() == 2);
	CHECK(are_nso_equivalent<node_t>(
		finished(ap::simplified_and_join<node_t>(trefs{ pos, r })),
		ex("x", finished(k.psi))));
	CHECK(are_nso_equivalent<node_t>(finished(r),
		disj(neg(eq0(land(lneg(bf("a")), bf("c")))),
			neg(eq0(land(lneg(bf("a")), bf("d")))))));
}

TEST_CASE("TC2: a nested tree keeps its own ∧") {
	tref x = vr("x");
	// `x·a = 0 ∧ (¬(x·c = 0) ∨ (¬(x·d = 0) ∧ ¬(x·e = 0)))`: the ∧ node
	// inside is re-assembled by the ∧-join, the ∨ node around it by the
	// ∨-join, and the tree is never flattened.
	prepared_clause k({ x }, conj(eq0(land(bf("x"), bf("a"))),
		disj(neg(eq0(land(bf("x"), bf("c")))),
			conj(neg(eq0(land(bf("x"), bf("d")))),
				neg(eq0(land(bf("x"), bf("e"))))))));
	auto s = ap::squeeze<node_t>(k.conjuncts, k.P, k.c.order);
	REQUIRE(s.comps.size() == 1);
	REQUIRE(s.negatives.size() == 1);
	REQUIRE(!ap::is_flat_tree<node_t>(s.negatives[0]));

	const tref pos = ap::positive_condition<node_t>(s.comps, k.c);
	const tref r = ap::tree_condition<node_t>(s.negatives[0], s.comps,
		k.P, k.c);
	CHECK(ap::is_negative_tree<node_t>(r));
	CHECK(!ap::is_flat_tree<node_t>(r));
	CHECK(are_nso_equivalent<node_t>(
		finished(ap::simplified_and_join<node_t>(trefs{ pos, r })),
		ex("x", finished(k.psi))));
}

TEST_CASE("TC3: a repeated literal is paid for once") {
	tref x = vr("x");
	// The same literal in two branches is the same node, so the walk's
	// memo answers the second occurrence with the first one's condition.
	prepared_clause k({ x }, conj(eq0(land(bf("x"), bf("a"))),
		disj(conj(neg(eq0(land(bf("x"), bf("c")))),
				neg(eq0(land(bf("x"), bf("d"))))),
			conj(neg(eq0(land(bf("x"), bf("c")))),
				neg(eq0(land(bf("x"), bf("e"))))))));
	auto s = ap::squeeze<node_t>(k.conjuncts, k.P, k.c.order);
	REQUIRE(s.comps.size() == 1);
	REQUIRE(s.negatives.size() == 1);

	const tref r = ap::tree_condition<node_t>(s.negatives[0], s.comps,
		k.P, k.c);
	const trefs branches = ap::members<node_t>(r);
	REQUIRE(branches.size() == 2);
	// the shared literal's condition, computed here the way the walk
	// computes it, is a member of both branches — one node, twice
	const tref shared = ap::negative_condition<node_t>(
		ap::simplify_term<node_t>(land(bf("x"), bf("c")), k.c.order),
		s.comps, k.P, k.c);
	CHECK(has_member(branches[0], shared));
	CHECK(has_member(branches[1], shared));
}

TEST_CASE("TC4: a subtree that folds to T folds the tree") {
	tref x = vr("x"), y = vr("y");
	// `¬(y = 0)` touches no component, and `∃_y y ≠ 0` is `T`, so the
	// ∨-join decides before the other literal is looked at.
	prepared_clause k({ x, y }, conj(eq0(land(bf("x"), bf("a"))),
		disj(neg(eq0(bf("y"))), neg(eq0(land(bf("x"), bf("c")))))));
	auto s = ap::squeeze<node_t>(k.conjuncts, k.P, k.c.order);
	REQUIRE(s.comps.size() == 1);
	REQUIRE(s.negatives.size() == 1);
	CHECK(tau::get(ap::tree_condition<node_t>(s.negatives[0], s.comps,
		k.P, k.c)).equals_T());
}

} // TEST_SUITE
