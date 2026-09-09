// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Layer 0 unit tests for src/anti_prenex/foundations/dag.h.
// Spec: anti_prenex.md; plan: the layer-0 plan (kept outside the repository) (package tests list).
//
// The three cached facets (formula_size, neg_of/set_neg, is_negative_tree)
// reach their tables through ctx.h's find/store, which package E defines, so
// this suite links only once package E has landed.
//
// Two conventions the cases lean on. Every predicate takes the `wff` WRAPPER
// (dag.h). And a member of a chain that is not the last child of its operator
// node carries a right sibling, so it is a different `tref` from the same
// formula standing alone: every comparison against a separately built node is
// by CONTENT (`tau::subtree_equals`), never by `==`.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/foundations/dag.h"

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

/// The atom `<name> = 0`.
tref atom(const char* name) {
	return tau::build_bf_eq_0(tau::build_bf_variable(name, 0));
}

/// A formula-level variable, for a binder.
tref fvar(const char* name) { return tau::build_variable(name, 0); }

tref neg(tref n)          { return tau::build_wff_neg(n); }
tref conj(tref l, tref r) { return tau::build_wff_and(l, r); }
tref disj(tref l, tref r) { return tau::build_wff_or(l, r); }

/// A real order atom `x <= y`. On a non-bitvector type the construction
/// hooks rewrite `<=` into the equation `x·y' = 0` (src/hooks.tmpl.h:1323)
/// and `<` into a conjunction (:1194), so neither survives construction
/// here. That never reaches the algorithm, which does not CONSTRUCT order
/// atoms (D4 amendment): `is_order_atom` classifies what the input holds,
/// and the parser builds with the hooks disabled. This helper reproduces
/// that shape the same way.
tref order_atom(const char* l, const char* r) {
	use_hooks_guard<node_t> g(false);
	return tau::build_bf_lteq(tau::build_bf_variable(l, 0),
		tau::build_bf_variable(r, 0));
}

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

/// Content-comparison of two member views, order significant.
bool same_members(const trefs& got, const trefs& want) {
	if (got.size() != want.size()) return false;
	for (size_t i = 0; i < got.size(); ++i)
		if (!same(got[i], want[i])) return false;
	return true;
}

} // namespace

TEST_SUITE("anti_prenex/foundations/dag") {

// --- |·| ---------------------------------------------------------------------

TEST_CASE("T1: sizes by hand count") {
	tref a = atom("a"), b = atom("b"), c = atom("c");
	// An atom is one node; the `wff` wrapper and its operator node count 1
	// together, so nothing below doubles.
	CHECK(ap::formula_size<node_t>(a) == 1);
	// |¬ψ| = 1 + |ψ|.
	CHECK(ap::formula_size<node_t>(neg(a)) == 2);
	// One connective node plus its two members.
	CHECK(ap::formula_size<node_t>(conj(a, b)) == 3);
	CHECK(ap::formula_size<node_t>(disj(a, b)) == 3);
	// A binder is 1 + |body|; the bound variable is not a formula child.
	CHECK(ap::formula_size<node_t>(
		tau::build_wff_ex(fvar("x"), a, false)) == 2);
	// A constant is one node.
	CHECK(ap::formula_size<node_t>(tau::_T()) == 1);
	CHECK(ap::formula_size<node_t>(tau::_F()) == 1);
	// Nesting: ¬(a ∧ b) is the negation node over a three-node conjunction.
	CHECK(ap::formula_size<node_t>(neg(conj(a, b))) == 4);
	// A shared subtree counts once per place it appears (a tree count over
	// the DAG): `a` sits in both operands and is counted twice.
	CHECK(ap::formula_size<node_t>(disj(conj(a, b), conj(a, c))) == 7);
}

TEST_CASE("T2: an atom counts 1 whatever its terms, an opaque operator too") {
	tref x = tau::build_bf_variable("x", 0);
	tref y = tau::build_bf_variable("y", 0);
	// Terms are not formula children: the atom counts 1 however big they are.
	CHECK(ap::formula_size<node_t>(tau::build_bf_eq(x, y)) == 1);
	CHECK(ap::formula_size<node_t>(tau::build_bf_eq_0(x)) == 1);
	CHECK(ap::formula_size<node_t>(order_atom("x", "y")) == 1);
	// A temporal operator is opaque like an atom (dag.h): its body is not
	// descended into, so a big body does not change the count.
	tref big = conj(atom("a"), conj(atom("b"), atom("c")));
	CHECK(ap::formula_size<node_t>(tau::build_wff_always(big)) == 1);
	CHECK(ap::formula_size<node_t>(tau::build_wff_sometimes(big)) == 1);
}

TEST_CASE("T3: a k-member chain costs k-1 spine nodes") {
	tref a = atom("a"), b = atom("b"), c = atom("c"), d = atom("d");
	// D1 counts every node of the binary spine, so a chain of k members is
	// (k-1) + Σ|members| — 3 spine nodes and 4 atoms here.
	tref chain = ap::canonical_and<node_t>(trefs{ a, b, c, d });
	CHECK(ap::formula_size<node_t>(chain) == 7);
	// Independent of the nesting direction: a right-nested chain over the
	// same members has the same count.
	CHECK(ap::formula_size<node_t>(conj(a, conj(b, conj(c, d)))) == 7);
	CHECK(ap::formula_size<node_t>(conj(conj(conj(a, b), c), d)) == 7);
}

// --- FV ----------------------------------------------------------------------

TEST_CASE("T4: FV excludes every binder's own variable, functional ones too") {
	tref x = tau::build_bf_variable("x", 0);
	tref y = tau::build_bf_variable("y", 0);
	tref fm = conj(tau::build_bf_eq_0(x), tau::build_bf_eq_0(y));
	CHECK(ap::fv<node_t>(fm).size() == 2);
	// A formula binder removes its own subscript.
	tref bound = tau::build_wff_ex(fvar("x"), tau::build_bf_eq_0(x), false);
	for (tref v : ap::fv<node_t>(bound)) CHECK(!same(v, fvar("x")));
	// A functional quantifier is a binder too (§1: transparent to the push,
	// still a binder for FV).
	tref fex = tau::build_bf_eq_0(tau::build_bf_fex(fvar("x"), x));
	for (tref v : ap::fv<node_t>(fex)) CHECK(!same(v, fvar("x")));
	// fv_meets is the same question against a block.
	CHECK(ap::fv_meets<node_t>(fm, ap::block{ fvar("x") }));
	CHECK(!ap::fv_meets<node_t>(fm, ap::block{ fvar("z") }));
	CHECK(!ap::fv_meets<node_t>(fm, ap::block{}));
	CHECK(!ap::fv_meets<node_t>(tau::_T(), ap::block{ fvar("x") }));
}

TEST_CASE("T5: fv_intersect keeps X's order, not the FV order") {
	tref x = tau::build_bf_variable("x", 0);
	tref y = tau::build_bf_variable("y", 0);
	tref fm = conj(tau::build_bf_eq_0(x), tau::build_bf_eq_0(y));
	// The block is ordered outermost first and the narrowing must not
	// re-order it (§6 key narrowing), whichever way `fv` sorts.
	ap::block yx{ fvar("y"), fvar("x") };
	ap::block got = ap::fv_intersect<node_t>(fm, yx);
	REQUIRE(got.size() == 2);
	CHECK(same(got[0], fvar("y")));
	CHECK(same(got[1], fvar("x")));
	// A variable that is not free drops out; one that is stays in place.
	ap::block with_z{ fvar("z"), fvar("x") };
	ap::block narrowed = ap::fv_intersect<node_t>(fm, with_z);
	REQUIRE(narrowed.size() == 1);
	CHECK(same(narrowed[0], fvar("x")));
	CHECK(ap::fv_intersect<node_t>(tau::_T(), yx).empty());
}

// --- content hash and content order -------------------------------------------

TEST_CASE("T6: the content order is a strict total order on content") {
	tref a = atom("a"), b = atom("b"), c = atom("c");
	trefs ns{ a, b, c, neg(a), conj(a, b), tau::_T(), tau::_F() };
	for (tref x : ns) {
		CHECK(!ap::content_less<node_t>(x, x));          // irreflexive
		// A node compares equal to itself under any spelling: content
		// hash and order both ignore the right sibling.
		CHECK(ap::content_hash<node_t>(x) == ap::content_hash<node_t>(x));
	}
	for (tref x : ns) for (tref y : ns) {
		if (same(x, y)) {
			CHECK(!ap::content_less<node_t>(x, y));
			CHECK(!ap::content_less<node_t>(y, x));
		} else {   // exactly one way round
			CHECK(ap::content_less<node_t>(x, y)
				!= ap::content_less<node_t>(y, x));
		}
	}
	for (tref x : ns) for (tref y : ns) for (tref z : ns)
		if (ap::content_less<node_t>(x, y)
			&& ap::content_less<node_t>(y, z))
				CHECK(ap::content_less<node_t>(x, z));
	// Equal content, different spelling: a member carrying a right sibling
	// and the same formula standing alone are one element of the order.
	trefs ms = ap::members<node_t>(conj(a, b));
	REQUIRE(ms.size() == 2);
	CHECK(!ap::content_less<node_t>(ms[0], a));
	CHECK(!ap::content_less<node_t>(a, ms[0]));
	CHECK(ap::content_hash<node_t>(ms[0]) == ap::content_hash<node_t>(a));
}

// --- the member view and the raw constructors ----------------------------------

TEST_CASE("T7: canonical_and is order-independent and deduplicates") {
	tref a = atom("a"), b = atom("b"), c = atom("c");
	tref abc = ap::canonical_and<node_t>(trefs{ a, b, c });
	// Three spellings of the same member set give ONE hash-consed node.
	CHECK(abc == ap::canonical_and<node_t>(trefs{ c, a, b }));
	CHECK(abc == ap::canonical_and<node_t>(trefs{ b, c, a }));
	// Duplicates are dropped by the constructor (the joins' AC-canonicity).
	CHECK(abc == ap::canonical_and<node_t>(trefs{ b, a, c, a, b }));
	CHECK(ap::members<node_t>(abc).size() == 3);
	// One member returns that member, without a stale right sibling.
	CHECK(ap::canonical_and<node_t>(trefs{ a }) == a);
	CHECK(ap::canonical_or<node_t>(trefs{ a }) == a);
	// The same for ∨, and the two are different nodes.
	tref abc_or = ap::canonical_or<node_t>(trefs{ c, b, a });
	CHECK(abc_or == ap::canonical_or<node_t>(trefs{ a, b, c }));
	CHECK(abc_or != abc);
	// Members come back in content order, deduplicated.
	trefs ms = ap::members<node_t>(abc);
	REQUIRE(ms.size() == 3);
	for (size_t i = 1; i < ms.size(); ++i)
		CHECK(ap::content_less<node_t>(ms[i - 1], ms[i]));
}

TEST_CASE("T8: members flattens both nesting sides and keeps multiplicity") {
	use_hooks_guard<node_t> g(false);   // no collapsing while we look
	tref a = atom("a"), b = atom("b"), c = atom("c"), d = atom("d");
	// Left-deep, right-deep and mixed spines all flatten to the same list.
	CHECK(same_members(ap::members<node_t>(conj(conj(conj(a, b), c), d)),
		trefs{ a, b, c, d }));
	CHECK(same_members(ap::members<node_t>(conj(a, conj(b, conj(c, d)))),
		trefs{ a, b, c, d }));
	CHECK(same_members(ap::members<node_t>(conj(conj(a, b), conj(c, d))),
		trefs{ a, b, c, d }));
	// The flattening stops at the other connective.
	CHECK(same_members(ap::members<node_t>(conj(a, disj(b, c))),
		trefs{ a, disj(b, c) }));
	// Multiplicity and occurrence order are the view's, not a set's:
	// deduplication is canonical_and's job and the joins'.
	CHECK(ap::members<node_t>(conj(a, a)).size() == 2);
	CHECK(same_members(ap::members<node_t>(conj(b, a)), trefs{ b, a }));
	// A node that is not a connective is its own single member.
	CHECK(same_members(ap::members<node_t>(a), trefs{ a }));
	CHECK(same_members(ap::members<node_t>(neg(a)), trefs{ neg(a) }));
	// is_member scans by content, so a member carrying a right sibling
	// matches the same formula standing alone.
	tref chain = conj(a, b);
	CHECK(ap::is_member<node_t>(chain, a));
	CHECK(ap::is_member<node_t>(chain, b));
	CHECK(!ap::is_member<node_t>(chain, c));
	CHECK(same(ap::members<node_t>(chain)[0], a));
}

TEST_CASE("T9: the hooks collapse but never reorder (D4)") {
	tref a = atom("a"), b = atom("b");
	// What the hooks do: absorb a constant, an idempotent operand, a
	// complementary pair. canonical_* accepts the collapse and publishes
	// nothing of its own.
	CHECK(conj(a, tau::_T()) == a);
	CHECK(disj(a, tau::_F()) == a);
	CHECK(ap::is_false<node_t>(conj(a, neg(a))));
	CHECK(ap::is_true<node_t>(disj(a, neg(a))));
	CHECK(ap::canonical_and<node_t>(trefs{ a, tau::_T() }) == a);
	// What they do NOT do: re-order or re-associate. Two spellings of the
	// same conjunction stay two nodes — which is exactly why canonical_and
	// has to sort.
	CHECK(conj(a, b) != conj(b, a));
	CHECK(ap::canonical_and<node_t>(trefs{ a, b })
		== ap::canonical_and<node_t>(trefs{ b, a }));
	// The same over three members, where association could differ as well
	// as order: every spelling reaches the one canonical node, and none of
	// the raw spellings is reordered into another.
	tref c = atom("c");
	CHECK(conj(conj(a, b), c) != conj(conj(b, a), c));
	CHECK(conj(conj(a, b), c) != conj(a, conj(b, c)));
	tref canonical = ap::canonical_and<node_t>(trefs{ a, b, c });
	CHECK(canonical == ap::canonical_and<node_t>(trefs{ c, b, a }));
	CHECK(canonical == ap::canonical_and<node_t>(trefs{ b, a, c }));
}

TEST_CASE("T10: complement_of strips a negation rather than doubling it") {
	tref a = atom("a");
	CHECK(same(ap::complement_of<node_t>(a), neg(a)));
	CHECK(same(ap::complement_of<node_t>(neg(a)), a));
	// Two calls are the identity, so the joins' unit-elimination test is
	// symmetric.
	CHECK(same(ap::complement_of<node_t>(ap::complement_of<node_t>(a)), a));
	CHECK(ap::is_false<node_t>(ap::complement_of<node_t>(tau::_T())));
	CHECK(ap::is_true<node_t>(ap::complement_of<node_t>(tau::_F())));
}

// --- classification -----------------------------------------------------------

TEST_CASE("T11: the §1 node classification") {
	tref x = tau::build_bf_variable("x", 0);
	tref y = tau::build_bf_variable("y", 0);
	tref a = atom("a"), b = atom("b");
	tref eq = tau::build_bf_eq(x, y);
	tref order = order_atom("x", "y");

	CHECK(ap::is_atom<node_t>(eq));
	CHECK(ap::is_equation<node_t>(eq));
	CHECK(!ap::is_order_atom<node_t>(eq));
	CHECK(ap::is_atom<node_t>(order));
	CHECK(ap::is_order_atom<node_t>(order));
	CHECK(!ap::is_equation<node_t>(order));

	CHECK(ap::is_literal<node_t>(a));
	CHECK(ap::is_literal<node_t>(neg(a)));
	CHECK(ap::is_negated_atom<node_t>(neg(a)));
	CHECK(!ap::is_negated_atom<node_t>(a));
	CHECK(ap::is_negated_equation<node_t>(neg(a)));
	CHECK(!ap::is_negated_equation<node_t>(neg(order)));
	CHECK(!ap::is_literal<node_t>(conj(a, b)));
	CHECK(same(ap::atom_of<node_t>(neg(a)), a));
	CHECK(same(ap::atom_of<node_t>(a), a));

	CHECK(ap::is_true<node_t>(tau::_T()));
	CHECK(ap::is_false<node_t>(tau::_F()));
	CHECK(!ap::is_true<node_t>(a));

	CHECK(ap::is_conjunction<node_t>(conj(a, b)));
	CHECK(!ap::is_conjunction<node_t>(disj(a, b)));
	CHECK(ap::is_disjunction<node_t>(disj(a, b)));
	CHECK(!ap::is_disjunction<node_t>(a));

	CHECK(ap::is_binder<node_t>(tau::build_wff_ex(fvar("x"), a, false)));
	CHECK(ap::is_binder<node_t>(tau::build_wff_all(fvar("x"), a, false)));
	CHECK(!ap::is_binder<node_t>(a));
	CHECK(ap::is_temporal<node_t>(tau::build_wff_always(a)));
	CHECK(ap::is_temporal<node_t>(tau::build_wff_sometimes(a)));
	CHECK(!ap::is_temporal<node_t>(a));

	// PLAIN is "not an ∨-node"; DISJUNCTIVE is the complement of that.
	CHECK(ap::is_plain_conjunct<node_t>(a));
	CHECK(ap::is_plain_conjunct<node_t>(neg(a)));
	CHECK(ap::is_plain_conjunct<node_t>(conj(a, b)));
	CHECK(!ap::is_plain_conjunct<node_t>(disj(a, b)));
	CHECK(ap::is_disjunctive_conjunct<node_t>(disj(a, b)));
	CHECK(!ap::is_disjunctive_conjunct<node_t>(a));
}

TEST_CASE("T11b: a reference is opaque and classifies as one") {
	// The one shape the builders do not construct directly; parse it.
	tref ref = tau::get("f(x)", parse_wff());
	REQUIRE(ref != nullptr);
	CHECK(ap::is_reference<node_t>(ref));
	CHECK(!ap::is_atom<node_t>(ref));
	CHECK(!ap::is_literal<node_t>(ref));
	CHECK(ap::is_plain_conjunct<node_t>(ref));
	// Opaque like an atom, whatever its arguments (dag.h).
	CHECK(ap::formula_size<node_t>(ref) == 1);
}

TEST_CASE("T12: negative trees, flat and nested") {
	tref na = neg(atom("a")), nb = neg(atom("b")), nc = neg(atom("c"));
	// An ∨-node all of whose leaves are negated equations.
	tref flat = disj(na, nb);
	CHECK(ap::is_negative_tree<node_t>(flat));
	CHECK(ap::is_flat_tree<node_t>(flat));
	// ∧-nodes are allowed inside; then the tree is NESTED, not flat.
	tref nested = disj(conj(na, nb), nc);
	CHECK(ap::is_negative_tree<node_t>(nested));
	CHECK(!ap::is_flat_tree<node_t>(nested));
	// One positive leaf disqualifies the whole tree.
	CHECK(!ap::is_negative_tree<node_t>(disj(na, atom("b"))));
	CHECK(!ap::is_negative_tree<node_t>(disj(conj(na, atom("b")), nc)));
	// So does a leaf that is not an equation at all.
	CHECK(!ap::is_negative_tree<node_t>(disj(na, neg(order_atom("x", "y")))));
	// The top node must be the ∨: a conjunction of negated equations is a
	// plain conjunct, not a negative tree.
	CHECK(!ap::is_negative_tree<node_t>(conj(na, nb)));
	CHECK(!ap::is_negative_tree<node_t>(na));
	// A binder under the ∨ is a leaf, and not a negated equation.
	CHECK(!ap::is_negative_tree<node_t>(
		disj(na, tau::build_wff_ex(fvar("x"), na, false))));
}

// --- binder accessors ----------------------------------------------------------

TEST_CASE("T13: binder accessors, no renaming") {
	tref body = atom("a");
	tref ex = tau::build_wff_ex(fvar("x"), body, false);
	tref all = tau::build_wff_all(fvar("y"), body, false);
	CHECK(ap::binder_kind<node_t>(ex) == ap::binder::ex);
	CHECK(ap::binder_kind<node_t>(all) == ap::binder::all);
	// The variable is the binder's first child, so it carries the body as
	// its right sibling: compare by content.
	CHECK(same(ap::binder_var<node_t>(ex), fvar("x")));
	CHECK(same(ap::binder_var<node_t>(all), fvar("y")));
	CHECK(same(ap::binder_body<node_t>(ex), body));
	CHECK(same(ap::binder_body<node_t>(all), body));
	// Round trip: rebuilding from the accessors gives the same node back
	// (calculate_quant_id = false renames nothing).
	CHECK(tau::build_wff_ex(ap::binder_var<node_t>(ex),
		ap::binder_body<node_t>(ex), false) == ex);
}

// --- the tables ----------------------------------------------------------------

TEST_CASE("T14: a facet is stable across queries") {
	tref a = atom("a"), b = atom("b"), c = atom("c");
	tref chain = ap::canonical_and<node_t>(trefs{ a, b, c });
	// The structural tables are unconditional (fwd.h cache gating), so the
	// second query is answered from the table in EVERY build type and must
	// agree with the first.
	const size_t first = ap::formula_size<node_t>(chain);
	CHECK(ap::formula_size<node_t>(chain) == first);
	CHECK(first == 5);
	// The fill measures every wrapper it passes, so a subformula queried
	// afterwards agrees with the count that contributed to the whole.
	CHECK(ap::formula_size<node_t>(a) == 1);
	tref tree = disj(chain, neg(a));
	CHECK(ap::formula_size<node_t>(tree) == 1 + first + 2);
	// The same for the negative-tree flag.
	tref nt = disj(neg(a), neg(b));
	const bool flag = ap::is_negative_tree<node_t>(nt);
	CHECK(flag);
	CHECK(ap::is_negative_tree<node_t>(nt) == flag);
}

TEST_CASE("T15: the neg slot round-trips and survives a GC sweep") {
	tref a = atom("a");
	tref n = conj(a, atom("b"));
	tref negated = disj(neg(a), neg(atom("b")));
	// Empty until layer 1 fills it.
	CHECK(ap::neg_of<node_t>(atom("unset")) == nullptr);
	ap::set_neg<node_t>(n, negated);
	CHECK(ap::neg_of<node_t>(n) == negated);
	// The table is GC-registered, so an entry whose key and value both
	// survive the sweep survives with them.
	htref keep_key = tau::geth(n);
	htref keep_value = tau::geth(negated);
	std::unordered_set<tref> keep{ n, negated };
	tau::gc(keep);
	CHECK(ap::neg_of<node_t>(n) == negated);
	CHECK(keep_key.get() != nullptr);
	CHECK(keep_value.get() != nullptr);
}

}
