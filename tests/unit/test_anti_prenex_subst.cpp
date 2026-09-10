// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Layer 0 unit tests for src/anti_prenex/foundations/subst.h (package C).
// Spec: anti_prenex.md §1 (substitution consequences, atoms_memo), §3 (the two
// primitives, TRY_WITNESS_DEEP's capture conditions), §4 (what may touch a
// unit), §10 (occurrence guards, one memo per rewrite); the rulings of
// Sep 10 2026 are cited in subst.h.
//
// Conventions. Every substitution takes the `wff` WRAPPER. A member of a chain
// that is not the last child of its operator node carries a right sibling, so
// it is a different `tref` from the same formula standing alone: pointer
// identity of an UNTOUCHED member is checked on the trimmed member, content
// on anything rebuilt (`tau::subtree_equals`). The parser canonicalises binder
// ids (`ex x ex y … all z` → 3, 2, 1). Never assert on `to_str()` of a
// BDD-backed term.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/foundations/subst.h"

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

using tb      = tau_term_bdd<node_t>;
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
/// The trimmed `variable` node of a one-variable term — the shape `x`, order
/// keys and free-variable sets hold.
tref vr(const char* s) { return tau::trim(bf(s)); }

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

/// `P[i] ↦ |P| - i` (§5): inner → LOWER rank.
order_t order_of(const ap::block& P) {
	order_t o;
	for (size_t i = 0; i < P.size(); ++i) o.emplace(P[i], int_t(P.size() - i));
	return o;
}

/// Semantic equality of two terms over `vs`: equal ROBDDs under one order,
/// both finished first (a BDD-backed term's own ref belongs to its order).
bool same_function(tref a, tref b, const ap::block& vs) {
	order_t o = order_of(vs);
	return tb::build_bdd(ap::finish_terms<node_t>(a), o)
		== tb::build_bdd(ap::finish_terms<node_t>(b), o);
}

/// Both sides of an atom (through no negation), trimmed.
std::pair<tref, tref> sides(tref atom) {
	const tau& t = tau::get(atom);
	REQUIRE(t.is(tau::wff));
	REQUIRE(t[0].has_child());
	return { tau::trim_right_sibling(t[0].first()),
		tau::trim_right_sibling(t[0].second()) };
}

/// The first member of `chain` satisfying `pred`, TRIMMED; nullptr if none.
template <typename P>
tref member_where(tref chain, P pred) {
	for (tref m : ap::members<node_t>(chain))
		if (pred(m)) return tau::trim_right_sibling(m);
	return nullptr;
}

/// The first node of kind `nt` below `n` (the root included).
tref find_kind(tref n, size_t nt) {
	return tau::get(n).find_top([nt](tref m) {
		return tree<node_t>::get(m).is(nt); });
}

bool has_bdd_id(tref n) { return find_kind(n, tau::BDD_ID) != nullptr; }

/// A formula-level reference `name(arg)` with a BUILT argument — the only
/// way to get a FORMULA argument: the grammar makes every parsed argument a
/// term (parser/tau.tgf, `ref_args`).
tref reference(const char* name, tref arg) {
	return tau::get(tau::wff, tau::get(tau::wff_ref,
		tau::build_ref(name, trefs{ arg })));
}

bool in_fv(tref n, tref v) {
	const trefs& vars = get_free_vars<node_t>(n);
	return std::binary_search(vars.begin(), vars.end(), v, tau::subtree_less);
}

} // namespace

TEST_SUITE("anti_prenex/foundations/subst") {

// --- the occurrence guard: untouched means the same tref ---------------------------

TEST_CASE("subst_var: an x-free formula is the same tref, and so is an x-free member") {
	tref x = vr("x");
	tref phi = wff("a = 0 && b = 0");
	CHECK(ap::subst_var<node_t>(phi, x, bf("y"), {}) == phi);

	tref psi = wff("x = 0 && (a = 0 || b = 0)");
	tref d = member_where(psi, is_child<node_t, tau::wff_or>);
	REQUIRE(d != nullptr);
	tref r = ap::subst_var<node_t>(psi, x, bf("y"), {});
	CHECK(r != psi);
	// The untouched member is reused, not rebuilt: pointer identity.
	CHECK(member_where(r, is_child<node_t, tau::wff_or>) == d);
	CHECK(same(member_where(r, is_atomic_fm<node_t>), wff("y = 0")));
	CHECK(!in_fv(r, x));
}

TEST_CASE("subst_atom: an atom that is not reachable leaves the formula untouched") {
	tref phi = wff("a = 0 && b = 0");
	CHECK(ap::subst_atom<node_t>(phi, wff("c = 0"), true) == phi);
	// Only inside a unit: not reachable (§4).
	tref only = wff("ex z (a = 0 && z = 0)");
	CHECK(ap::subst_atom<node_t>(only, wff("a = 0"), false) == only);
	// An x-free member of a rewritten chain keeps its tref too.
	tref psi = wff("a = 0 && (b = 0 || c = 0)");
	tref d = member_where(psi, is_child<node_t, tau::wff_or>);
	tref r = ap::subst_atom<node_t>(psi, wff("a = 0"), true);
	CHECK(r == d); // T ∧ d → d: the whole result IS the untouched member
}

// --- capture safety on canonical ids ------------------------------------------------

TEST_CASE("subst_var: descends into a unit on the TRY_WITNESS_DEEP shape, no renaming") {
	// ex x ex y (x = y & a && all z (x & z = 0)): ids 3, 2, 1. The spine
	// under ∃2 holds the pin 3 = 2·a and the unit ∀1. Substituting the pin's
	// witness (a variable bound ABOVE the site) is what condition (c) of
	// TRY_WITNESS_DEEP licenses: FV(t) = {2, a} meets no binder on the path.
	tref phi = wff("ex x ex y (x = y & a && all z (x & z = 0))");
	REQUIRE(is_child_quantifier<node_t>(phi));
	tref x3 = tau::trim_right_sibling(ap::binder_var<node_t>(phi));
	tref inner = ap::binder_body<node_t>(phi);
	REQUIRE(is_child_quantifier<node_t>(inner));
	tref y2 = tau::trim_right_sibling(ap::binder_var<node_t>(inner));
	tref spine = ap::binder_body<node_t>(inner);
	REQUIRE(is_child<node_t>(spine, tau::wff_and));
	tref pin  = member_where(spine, is_atomic_fm<node_t>);
	tref unit = member_where(spine, is_child_quantifier<node_t>);
	REQUIRE(pin != nullptr);
	REQUIRE(unit != nullptr);
	tref z1 = tau::trim_right_sibling(ap::binder_var<node_t>(unit));
	tref t = sides(pin).second;

	tref r = ap::subst_var<node_t>(spine, x3, t, {});
	// The pin became 2·a = 2·a, folded to T by the hooks and left the chain;
	// the unit was entered and rebuilt with the SAME bound variable.
	REQUIRE(is_child_quantifier<node_t>(r));
	CHECK(ap::binder_kind<node_t>(r) == ap::binder::all);
	CHECK(same(ap::binder_var<node_t>(r), z1));
	tref body = ap::binder_body<node_t>(r);
	REQUIRE(is_atomic_fm<node_t>(body));
	CHECK(same_function(sides(body).first,
		build_bf_and<node_t>(t, tau::get(tau::bf, z1)), { y2, vr("a"), z1 }));
	// x is gone, the inner variable is still bound, the outer one is free here.
	CHECK(!in_fv(r, x3));
	CHECK(!in_fv(r, z1));
	CHECK(in_fv(r, y2));
}

TEST_CASE("subst_var: enters a unit whose body holds x; a free t under a nested binder") {
	tref x = vr("x");
	tref unit = wff("ex z (x & z = 0)");
	tref r = ap::subst_var<node_t>(unit, x, bf("y"), {});
	CHECK(same(r, wff("ex z (y & z = 0)")));
	// Two levels, a mixed kind: the ids are 2 and 1, neither is `y`.
	tref deep = wff("all w ex z (x & z = w)");
	tref rd = ap::subst_var<node_t>(deep, x, bf("y"), {});
	CHECK(same(rd, wff("all w ex z (y & z = w)")));
	// Simultaneous (ruling 6): x ∈ FV(t) is allowed; t is not re-entered.
	tref two = wff("x = 0 && x = 1");
	tref rs = ap::subst_var<node_t>(two, x, bf("x & y"), {});
	CHECK(same(rs, ap::canonical_and<node_t>({ wff("x & y = 0"), wff("x & y = 1") })));
	// x ← x changes nothing, so nothing is re-shaped: the same tref.
	CHECK(ap::subst_var<node_t>(two, x, bf("x"), {}) == two);
}

// --- what may touch a unit (§4) ------------------------------------------------------

TEST_CASE("[atm ↦ T/F] treats a unit as an opaque leaf") {
	tref a = wff("a = 0");
	tref phi = wff("a = 0 && ex z (a = 0 && z = 0)");
	tref u = member_where(phi, is_child_quantifier<node_t>);
	REQUIRE(u != nullptr);
	tref s = ap::subst_atom<node_t>(phi, a, true);
	CHECK(s == u);                    // T ∧ unit → the unit, the SAME tref
	CHECK(!ap::has_atom<node_t>(u, a));
	CHECK(ap::atoms<node_t>(u).empty());
	// The same unit under [x ← t] IS entered (the one licensed rewrite).
	tref z1 = tau::trim_right_sibling(ap::binder_var<node_t>(u));
	tref r = ap::subst_var<node_t>(phi, vr("a"), bf("b"), {});
	tref ru = member_where(r, is_child_quantifier<node_t>);
	REQUIRE(ru != nullptr);
	CHECK(ru != u);
	CHECK(same(ap::binder_var<node_t>(ru), z1));
	CHECK(!in_fv(ru, vr("a")));
	CHECK(in_fv(ru, vr("b")));
}

// --- one memo per call: a shared subtree is rewritten once ----------------------------

TEST_CASE("subst_var: a shared reference argument is re-simplified exactly once") {
	tref x = vr("x");
	tref psi = wff("x = 0 && b = 0");
	tref R = reference("f", psi);
	REQUIRE(is_child<node_t>(R, tau::wff_ref));
	REQUIRE(in_fv(R, x));
	tref p = wff("p = 0"), q = wff("q = 0");
	tref phi = tau::build_wff_or(tau::build_wff_and(R, p), tau::build_wff_and(R, q));

	size_t calls = 0;
	ap::simplify_formula_fn counting = [&calls](tref f) { ++calls; return f; };
	tref r = ap::subst_var<node_t>(phi, x, bf("y"), {}, counting);
	CHECK(calls == 1);
	// The formula argument got the formula-level substitution, both copies
	// are one node, and the site is a canonical chain over them.
	tref arg = ap::subst_var<node_t>(psi, x, bf("y"), {});
	CHECK(same(arg, ap::canonical_and<node_t>({ wff("y = 0"), wff("b = 0") })));
	tref R2 = reference("f", arg);
	CHECK(same(r, ap::canonical_or<node_t>({
		ap::canonical_and<node_t>({ R2, p }),
		ap::canonical_and<node_t>({ R2, q }) })));

	// The callback's result IS the argument: a marker proves it is applied.
	ap::simplify_formula_fn to_T = [](tref) { return tau::_T(); };
	tref m = ap::subst_var<node_t>(R, x, bf("y"), {}, to_T);
	tref ra = find_kind(m, tau::ref_arg);
	REQUIRE(ra != nullptr);
	CHECK(tau::get(ra)[0].equals_T());

	// A unit inside a formula argument is entered like any other.
	tref Ru = reference("h", wff("ex z (x & z = 0)"));
	tref mu = ap::subst_var<node_t>(Ru, x, bf("y"), {});
	tref rau = find_kind(mu, tau::ref_arg);
	REQUIRE(rau != nullptr);
	CHECK(same(tau::get(rau).first(), wff("ex z (y & z = 0)")));

	// An argument the substitution does not touch is not re-simplified.
	calls = 0;
	tref G = reference("g", wff("c = 0"));
	CHECK(ap::subst_var<node_t>(G, x, bf("y"), {}, counting) == G);
	CHECK(calls == 0);
}

TEST_CASE("subst_var: a reference's term argument goes through subst_term and simplify_term") {
	tref x = vr("x");
	tref phi = wff("f(x & z) && x = 0");
	tref r = ap::subst_var<node_t>(phi, x, bf("z'"), {});
	tref ref = member_where(r, is_child<node_t, tau::wff_ref>);
	REQUIRE(ref != nullptr);
	tref arg = find_kind(ref, tau::ref_arg);
	REQUIRE(arg != nullptr);
	CHECK(tau::get(arg)[0].equals_0());          // z′·z → 0, re-simplified
	CHECK(same(find_kind(ref, tau::sym), find_kind(phi, tau::sym)));
	CHECK(same(member_where(r, is_atomic_fm<node_t>), wff("z' = 0")));
	// An argument without x: the reference is the same tref.
	tref g = wff("g(y) && x = 0");
	tref gm = member_where(g, is_child<node_t, tau::wff_ref>);
	CHECK(member_where(ap::subst_var<node_t>(g, x, bf("z"), {}),
		is_child<node_t, tau::wff_ref>) == gm);
	// A reference inside an ATOM's term is package B's business and folds too.
	tref s = ap::subst_var<node_t>(wff("x & r(x & z) = 0"), x, bf("z'"), {});
	tref sa = find_kind(s, tau::ref_arg);
	REQUIRE(sa != nullptr);
	CHECK(tau::get(sa)[0].equals_0());
}

// --- the canonical chain shape (D1) ---------------------------------------------------

TEST_CASE("the result of a rewrite is a canonical chain: sorted, deduplicated, left-nested") {
	tref x = vr("x");
	// A right-nested parsed chain comes back canonical.
	tref phi = wff("x = 0 && (a = 0 && b = 0)");
	tref r = ap::subst_var<node_t>(phi, x, bf("y"), {});
	CHECK(same(r, ap::canonical_and<node_t>({ wff("y = 0"), wff("a = 0"), wff("b = 0") })));
	trefs ms = ap::members<node_t>(r);
	CHECK(ms.size() == 3);
	CHECK(std::is_sorted(ms.begin(), ms.end(), tau::subtree_less));
	// Left-nested: the right operand of the top node is a member.
	tref right = tau::get(tau::get(r).first()).second();
	CHECK(!is_child<node_t>(right, tau::wff_and));
	// Two members that become equal collapse to one.
	CHECK(same(ap::subst_var<node_t>(wff("x = 0 && y = 0"), x, bf("y"), {}),
		wff("y = 0")));
	// The dual.
	CHECK(same(ap::subst_var<node_t>(wff("x = 0 || (a = 0 || b = 0)"), x, bf("y"), {}),
		ap::canonical_or<node_t>({ wff("y = 0"), wff("a = 0"), wff("b = 0") })));
	// And for [atm ↦ T/F].
	CHECK(same(ap::subst_atom<node_t>(wff("a = 0 && (b = 0 && c = 0)"), wff("a = 0"), true),
		ap::canonical_and<node_t>({ wff("b = 0"), wff("c = 0") })));
}

// --- atoms through package B; the hooks fold, SIMPLIFY does not run --------------------

TEST_CASE("subst_var: atoms are rewritten through subst_term, constants fold, nothing deeper") {
	tref x = vr("x"), y = vr("y");
	CHECK(tau::get(ap::subst_var<node_t>(wff("x = 0"), x, bf("1"), {})).equals_F());
	CHECK(tau::get(ap::subst_var<node_t>(wff("x = y"), x, bf("y"), {})).equals_T());
	// The chain folds around a decided atom.
	CHECK(tau::get(ap::subst_var<node_t>(wff("x = 0 && b = 0"), x, bf("1"), {})).equals_F());
	CHECK(same(ap::subst_var<node_t>(wff("x = 0 || b = 0"), x, bf("1"), {}), wff("b = 0")));
	// The term hooks fold what they fold inside a rewritten side: y′·y is 0
	// before the atom is rebuilt, and 0 = 0 is T.
	CHECK(tau::get(ap::subst_var<node_t>(wff("x & y = 0"), x, bf("y'"), {})).equals_T());
	// What no hook folds stays: the deep folding is the caller's SIMPLIFY.
	tref z = vr("z");
	tref r = ap::subst_var<node_t>(wff("x & y = 0"), x, bf("z'"), {});
	REQUIRE(is_atomic_fm<node_t>(r));
	CHECK(find_kind(sides(r).first, tau::bf_neg) != nullptr);
	CHECK(same_function(sides(r).first, bf("z' & y"), { y, z }));
	// A negated atom keeps its negation; a negated decided atom folds.
	CHECK(same(ap::subst_var<node_t>(wff("!(x = 0)"), x, bf("y"), {}), wff("!(y = 0)")));
	CHECK(tau::get(ap::subst_var<node_t>(wff("!(x = 0)"), x, bf("0"), {})).equals_F());
}

TEST_CASE("subst_var: phase 4 — BDD-backed sides compose under the live order") {
	tref x = vr("x"), y = vr("y"), z = vr("z"), w = vr("w");
	ap::block P{ x };
	order_t o = order_of(P);
	tref prepared = ap::prepare_terms<node_t>(wff("x & y = 0 && z = 0"), P, o);
	REQUIRE(has_bdd_id(prepared));
	auto is_z = [](tref m) { return tau::subtree_equals(m, wff("z = 0")); };
	auto not_z = [&](tref m) { return !is_z(m); };
	tref plain = member_where(prepared, is_z);
	REQUIRE(plain != nullptr);
	tref r = ap::subst_var<node_t>(prepared, x, bf("z'"), o);
	CHECK(member_where(r, is_z) == plain);        // the x-free atom: same tref
	tref e = member_where(r, not_z);
	REQUIRE(e != nullptr);
	REQUIRE(is_atomic_fm<node_t>(e));
	CHECK(!ap::is_bdd_backed<node_t>(sides(e).first)); // no decision variable left
	CHECK(same_function(sides(e).first, bf("z' & y"), { y, z }));
	// A BDD-backed witness composes on the decision variable.
	ap::block Q{ x, w };
	order_t oq = order_of(Q);
	tref f = ap::prepare_terms<node_t>(wff("x & z = 0"), Q, oq);
	tref t = sides(ap::prepare_terms<node_t>(wff("w & y = 0"), Q, oq)).first;
	REQUIRE(ap::is_bdd_backed<node_t>(t));
	tref s = ap::subst_var<node_t>(f, x, t, oq);
	REQUIRE(is_atomic_fm<node_t>(s));
	CHECK(ap::is_bdd_backed<node_t>(sides(s).first));
	CHECK(same_function(sides(s).first, bf("w & y & z"), { x, w, y, z }));
	CHECK(!has_bdd_id(ap::finish_terms<node_t>(s)));
}

TEST_CASE("an order atom is rebuilt through the hooks (ruling 2)") {
	// Bitvector: the shape is kept.
	tref xb = vr("x:bv[8]");
	tref lt = wff("x:bv[8] & z:bv[8] <= y:bv[8]");
	REQUIRE(ap::is_order_atom<node_t>(lt));
	tref r = ap::subst_var<node_t>(lt, xb, bf("w:bv[8]"), {});
	REQUIRE(ap::is_order_atom<node_t>(r));
	CHECK(same(sides(r).first, bf("w:bv[8] & z:bv[8]")));
	CHECK(same(sides(r).second, bf("y:bv[8]")));
	// Boolean, built with the hooks off as the parser does: the rebuild is
	// the hooks' equation f·g′ = 0 — the module's one order-atom construction.
	tref bx = tau::build_bf_variable("x", 0), by = tau::build_bf_variable("y", 0),
	     bz = tau::build_bf_variable("z", 0);
	tref bl;
	{
		use_hooks_guard<node_t> g(false);
		bl = tau::build_bf_lteq(bx, by);
	}
	REQUIRE(ap::is_order_atom<node_t>(bl));
	tref rb = ap::subst_var<node_t>(bl, tau::trim(bx), bz, {});
	CHECK(is_child<node_t>(rb, tau::bf_eq));
	CHECK(same(rb, tau::build_bf_eq_0(build_bf_and<node_t>(bz, build_bf_neg<node_t>(by)))));
}

TEST_CASE("a pre-NNF shape is rebuilt generically through the hooks (ruling 3)") {
	tref x = vr("x");
	// The parser desugars `<->` and `->` into ∧/∨ before anything reaches the
	// module; the chains then come back canonical.
	tref eq = wff("x = 0 <-> b = 0");
	REQUIRE(is_child<node_t>(eq, tau::wff_and));
	tref r = ap::subst_var<node_t>(eq, x, bf("y"), {});
	CHECK(same(r, ap::canonical_and<node_t>({
		ap::canonical_or<node_t>({ wff("!(y = 0)"), wff("b = 0") }),
		ap::canonical_or<node_t>({ wff("y = 0"), wff("!(b = 0)") }) })));
	CHECK(!in_fv(r, x));
	// A genuine `wff_imply` node exists only with the hooks off; rebuilt
	// generically it goes through the imply hook, i.e. the builder's result.
	tref a = wff("x = 0"), b = wff("b = 0");
	tref imp;
	{
		use_hooks_guard<node_t> g(false);
		imp = tau::get(tau::wff, tau::get(tau::wff_imply, a, b));
	}
	REQUIRE(tau::get(imp).child_is(tau::wff_imply));
	tref ri = ap::subst_var<node_t>(imp, x, bf("y"), {});
	CHECK(same(ri, build_wff_imply<node_t>(wff("y = 0"), b)));
	CHECK(!in_fv(ri, x));
	CHECK(in_fv(ri, vr("y")));
	// Without x it is untouched, hooks or not.
	CHECK(ap::subst_var<node_t>(imp, vr("q"), bf("y"), {}) == imp);
}

TEST_CASE("temporal operators are opaque to both substitutions (ruling 1)") {
	tref x = vr("x");
	tref a = wff("x = 0");
	tref alw = tau::build_wff_always(a);
	REQUIRE(is_child_temporal_quantifier<node_t>(alw));
	REQUIRE(in_fv(alw, x));
	CHECK(ap::subst_var<node_t>(alw, x, bf("y"), {}) == alw);
	tref phi = tau::build_wff_and(alw, wff("x = 0 && b = 0"));
	tref r = ap::subst_var<node_t>(phi, x, bf("y"), {});
	CHECK(member_where(r, is_child_temporal_quantifier<node_t>) == alw);
	CHECK(same(member_where(r, is_atomic_fm<node_t>), wff("y = 0")));
	CHECK(ap::atoms<node_t>(alw).empty());
	tref psi = tau::build_wff_and(alw, wff("c = 0"));
	CHECK(ap::subst_atom<node_t>(psi, a, true) == psi);
	CHECK(ap::subst_atom<node_t>(psi, wff("c = 0"), true) == alw);
}

// --- [atm ↦ T/F] ----------------------------------------------------------------------

TEST_CASE("subst_atom: every reachable occurrence, the complement under ¬, the hooks fold") {
	tref a = wff("a = 0"), b = wff("b = 0"), c = wff("c = 0");
	CHECK(same(ap::subst_atom<node_t>(wff("a = 0 && b = 0"), a, true), b));
	CHECK(tau::get(ap::subst_atom<node_t>(wff("a = 0 && b = 0"), a, false)).equals_F());
	CHECK(tau::get(ap::subst_atom<node_t>(wff("a = 0 || b = 0"), a, true)).equals_T());
	CHECK(same(ap::subst_atom<node_t>(wff("a = 0 || b = 0"), a, false), b));
	CHECK(tau::get(ap::subst_atom<node_t>(wff("!(a = 0) && b = 0"), a, true)).equals_F());
	CHECK(same(ap::subst_atom<node_t>(wff("!(a = 0) && b = 0"), a, false), b));
	CHECK(tau::get(ap::subst_atom<node_t>(wff("!(a = 0) || b = 0"), a, false)).equals_T());
	// Both polarities at once, shared across two disjuncts.
	tref phi = wff("(a = 0 && b = 0) || (!(a = 0) && c = 0)");
	CHECK(same(ap::subst_atom<node_t>(phi, a, true), b));
	CHECK(same(ap::subst_atom<node_t>(phi, a, false), c));
	// The root itself, plain and negated.
	CHECK(tau::get(ap::subst_atom<node_t>(a, a, true)).equals_T());
	CHECK(tau::get(ap::subst_atom<node_t>(wff("!(a = 0)"), a, true)).equals_F());
	// The atom as it occurs: a semantically equal spelling is another atom.
	tref spelled = wff("a' = 1 && b = 0");
	CHECK(ap::subst_atom<node_t>(spelled, a, true) == spelled);
}

// --- atoms_memo ---------------------------------------------------------------------

TEST_CASE("atoms: the vocabulary with subst_atom's reach, sorted, one row per node") {
	tref a = wff("a = 0"), b = wff("b = 0"), c = wff("c = 0");
	tref phi = wff("a = 0 && !(b = 0) || c = 0");
	const trefs& as = ap::atoms<node_t>(phi);
	CHECK(as.size() == 3);
	CHECK(std::is_sorted(as.begin(), as.end(), tau::subtree_less));
	for (tref t : { a, b, c }) CHECK(ap::has_atom<node_t>(phi, t));
	CHECK(!ap::has_atom<node_t>(phi, wff("d = 0")));
	CHECK(&ap::atoms<node_t>(phi) == &as);          // one row, never recomputed
	// The reach: ¬ is passed; a unit, a reference, a temporal operator and a
	// constant are opaque.
	REQUIRE(ap::atoms<node_t>(wff("!(a = 0)")).size() == 1);
	CHECK(same(ap::atoms<node_t>(wff("!(a = 0)"))[0], a));
	CHECK(ap::atoms<node_t>(wff("ex z (a = 0 && z = 0)")).empty());
	CHECK(ap::atoms<node_t>(wff("f(x)")).empty());
	CHECK(ap::atoms<node_t>(tau::build_wff_sometimes(a)).empty());
	CHECK(ap::atoms<node_t>(tau::_T()).empty());
	CHECK(ap::atoms<node_t>(tau::_F()).empty());
	CHECK(ap::atoms<node_t>(wff("a = 0 && ex z (b = 0)")).size() == 1);
	// Duplicates collapse; a member carrying a right sibling is found.
	tref dup = wff("a = 0 && (a = 0 || b = 0)");
	CHECK(ap::atoms<node_t>(dup).size() == 2);
	CHECK(ap::has_atom<node_t>(dup, ap::members<node_t>(dup)[0]));
	// atoms vs fv on a unit: its variables are free, its atoms unreachable.
	tref u = wff("a = 0 && ex z (x & z = 0)");
	CHECK(ap::atoms<node_t>(u).size() == 1);
	CHECK(in_fv(u, vr("x")));
}

TEST_CASE("atoms: no row on a spine node, a row on every other wrapper (ruling 5)") {
	// Fresh names: the table is process-wide and hash-consing would hand a
	// row from another case to the same node.
	tref s1 = wff("s1 = 0"), s2 = wff("s2 = 0"), s3 = wff("s3 = 0");
	tref inner = tau::build_wff_and(s2, s3);
	tref phi = tau::build_wff_and(s1, inner);     // right-nested: `inner` is a spine node
	REQUIRE(ap::find<node_t, ap::table::atoms_memo>(inner) == nullptr);
	CHECK(ap::atoms<node_t>(phi).size() == 3);
	for (tref m : ap::members<node_t>(phi))
		CHECK(ap::find<node_t, ap::table::atoms_memo>(m) != nullptr);
	CHECK(ap::find<node_t, ap::table::atoms_memo>(phi) != nullptr);
	CHECK(ap::find<node_t, ap::table::atoms_memo>(inner) == nullptr);
	// Asked about directly it is a root and gets its row.
	CHECK(ap::atoms<node_t>(inner).size() == 2);
	CHECK(ap::find<node_t, ap::table::atoms_memo>(inner) != nullptr);
	// A spine node that has a row is not entered again, and its members are
	// still what a chain top above it reads.
	tref s0 = wff("s0 = 0");
	CHECK(ap::atoms<node_t>(tau::build_wff_and(s0, inner)).size() == 3);
}

// --- GC (last: a sweep rebuilds every table in the process) ----------------------------

TEST_CASE("gc: an atoms row survives a sweep with a live key, its trimmed atoms pinned") {
	tref phi = wff("g1 = 0 && g2 = 0");
	const trefs& as = ap::atoms<node_t>(phi);
	REQUIRE(as.size() == 2);
	// The row holds TRIMMED atoms; the first member carries its sibling in
	// `phi`, so its trimmed twin is NOT a subtree of the key and lives only
	// through the row's for_each_tref.
	tref a0 = as[0], a1 = as[1];
	htref keep_phi = tau::geth(phi);
	std::unordered_set<tref> keep;
	tau::gc(keep);
	const auto* row = ap::find<node_t, ap::table::atoms_memo>(phi);
	REQUIRE(row != nullptr);
	REQUIRE(row->items.size() == 2);
	CHECK(row->items[0] == a0);
	CHECK(row->items[1] == a1);
	CHECK(ap::has_atom<node_t>(phi, wff("g1 = 0")));
	CHECK(ap::has_atom<node_t>(phi, wff("g2 = 0")));
}

} // TEST_SUITE
