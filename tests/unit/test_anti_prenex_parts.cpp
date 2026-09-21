// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Layer 3 unit tests for src/anti_prenex/shared/parts.h: §6 PARTS and
// INCIDENCE. Spec: anti_prenex.md §6 (PARTS, INCIDENCE), §7 (the callers'
// contract — every conjunct touches the block).
//
// Nothing here consults a type table, so the fixtures use untyped variables
// and stay at the level the two procedures work at: free-variable sets and
// one kind test per conjunct.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/shared/parts.h"

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

/// A variable as a TERM (inside a conjunct) and as a BLOCK MEMBER (the bare
/// `variable` node free-variable sets and orders are keyed by).
tref bvar(const char* name) { return tau::build_bf_variable(name, 0); }
tref fvar(const char* name) { return tau::build_variable(name, 0); }

tref eq0(tref t)          { return tau::build_bf_eq_0(t); }
tref neg(tref n)          { return tau::build_wff_neg(n); }
tref disj(tref l, tref r) { return tau::build_wff_or(l, r); }
tref land(tref l, tref r) { return tau::build_bf_and(l, r); }

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

/// The conjuncts of a part, compared with a list of the nodes expected.
bool holds(const trefs& got, const trefs& want) {
	if (got.size() != want.size()) return false;
	for (size_t i = 0; i < got.size(); ++i)
		if (!same(got[i], want[i])) return false;
	return true;
}

} // namespace

TEST_SUITE("anti_prenex/shared/parts") {

// --- PARTS ------------------------------------------------------------------------

TEST_CASE("P1: only a shared BLOCK variable connects two conjuncts") {
	tref x = bvar("x"), y = bvar("y"), a = bvar("a");
	const ap::block X{ fvar("x"), fvar("y") };
	// `x·a = 0` and `y·a = 0` share the free variable `a`, which connects
	// nothing: the split exists for a quantifier over `X` alone.
	tref c1 = eq0(land(x, a)), c2 = eq0(land(y, a));
	auto ps = ap::parts<node_t>(trefs{ c1, c2 }, X);
	REQUIRE(ps.size() == 2);
	CHECK(holds(ps[0].conjuncts, trefs{ c1 }));
	CHECK(holds(ps[1].conjuncts, trefs{ c2 }));
	CHECK(same(ps[0].vars[0], fvar("x")));
	CHECK(same(ps[1].vars[0], fvar("y")));
	// A conjunct holding both merges their classes, transitively.
	tref c3 = eq0(land(x, y));
	auto one = ap::parts<node_t>(trefs{ c1, c2, c3 }, X);
	REQUIRE(one.size() == 1);
	CHECK(holds(one[0].conjuncts, trefs{ c1, c2, c3 }));
	CHECK(one[0].vars.size() == 2);
}

TEST_CASE("P2: parts in first-conjunct order, vars in X's order") {
	tref x = bvar("x"), y = bvar("y"), z = bvar("z");
	// X's order is neither the content order nor the order the conjuncts
	// mention the variables in, so the claim is observable.
	const ap::block X{ fvar("z"), fvar("y"), fvar("x") };
	tref c0 = eq0(land(z, bvar("a")));
	tref c1 = eq0(land(x, y));
	auto ps = ap::parts<node_t>(trefs{ c0, c1 }, X);
	REQUIRE(ps.size() == 2);
	// `c0` comes first in the input, so its part comes first.
	CHECK(holds(ps[0].conjuncts, trefs{ c0 }));
	CHECK(holds(ps[1].conjuncts, trefs{ c1 }));
	// and `{x, y}` is spelled `y, x`, the order X gives them.
	REQUIRE(ps[1].vars.size() == 2);
	CHECK(same(ps[1].vars[0], fvar("y")));
	CHECK(same(ps[1].vars[1], fvar("x")));
	// A part's conjuncts keep the input order, whichever variable opened
	// the part.
	auto all = ap::parts<node_t>(trefs{ c1, eq0(land(y, z)), c0 }, X);
	REQUIRE(all.size() == 1);
	CHECK(holds(all[0].conjuncts,
		trefs{ c1, eq0(land(y, z)), c0 }));
	REQUIRE(all[0].vars.size() == 3);
	CHECK(same(all[0].vars[0], fvar("z")));
	CHECK(same(all[0].vars[1], fvar("y")));
	CHECK(same(all[0].vars[2], fvar("x")));
}

TEST_CASE("P3: the degenerate shapes") {
	tref x = bvar("x"), y = bvar("y"), z = bvar("z");
	const ap::block X{ fvar("x"), fvar("y"), fvar("z") };
	// one conjunct
	tref c = eq0(land(x, y));
	auto one = ap::parts<node_t>(trefs{ c }, X);
	REQUIRE(one.size() == 1);
	CHECK(holds(one[0].conjuncts, trefs{ c }));
	CHECK(one[0].vars.size() == 2);
	// every conjunct in ONE part, linked in a chain: x–y, then y–z
	auto chain = ap::parts<node_t>(trefs{ eq0(land(x, y)), eq0(land(y, z)) }, X);
	REQUIRE(chain.size() == 1);
	CHECK(chain[0].conjuncts.size() == 2);
	CHECK(chain[0].vars.size() == 3);
	// every conjunct its own part
	auto split = ap::parts<node_t>(
		trefs{ eq0(x), eq0(y), eq0(z) }, X);
	REQUIRE(split.size() == 3);
	for (const auto& p : split) {
		CHECK(p.conjuncts.size() == 1);
		CHECK(p.vars.size() == 1);
	}
	// a block variable no conjunct holds belongs to no part
	auto few = ap::parts<node_t>(trefs{ eq0(x) }, X);
	REQUIRE(few.size() == 1);
	REQUIRE(few[0].vars.size() == 1);
	CHECK(same(few[0].vars[0], fvar("x")));
}

// --- INCIDENCE --------------------------------------------------------------------

TEST_CASE("P4: a disjunctive conjunct flags its block variables") {
	tref x = bvar("x"), y = bvar("y"), a = bvar("a"), b = bvar("b");
	const ap::block X{ fvar("x"), fvar("y") };
	// `x·a = 0 ∨ x·b = 0` is an ∨-node and no negative tree, so `x` is
	// still open; `y` is settled, occurring only in a plain conjunct.
	tref d = disj(eq0(land(x, a)), eq0(land(x, b)));
	auto r = ap::incidence<node_t>(trefs{ d, eq0(land(y, a)) }, X);
	REQUIRE(r.settled.size() == 1);
	CHECK(same(r.settled[0], fvar("y")));
	// the parts are `PARTS`' own
	CHECK(r.parts.size() == 2);
	// A variable in a plain AND in a disjunctive conjunct is flagged: one
	// occurrence under an ∨ is enough, and `y` next to it still settles.
	auto both = ap::incidence<node_t>(
		trefs{ eq0(land(x, a)), d, eq0(land(y, a)) }, X);
	REQUIRE(both.settled.size() == 1);
	CHECK(same(both.settled[0], fvar("y")));
}

TEST_CASE("P5: a negative tree settles, and settled keeps X's order") {
	tref x = bvar("x"), y = bvar("y"), z = bvar("z");
	tref a = bvar("a"), b = bvar("b");
	const ap::block X{ fvar("z"), fvar("y"), fvar("x") };
	// `¬(x·a = 0) ∨ ¬(x·b = 0)`: an ∨-node whose leaves are all negated
	// equations, which the leaf methods take whole (§1), so it opens
	// nothing.
	tref tree_ = disj(neg(eq0(land(x, a))), neg(eq0(land(x, b))));
	REQUIRE(ap::is_negative_tree<node_t>(tree_));
	auto r = ap::incidence<node_t>(
		trefs{ tree_, eq0(land(y, z)) }, X);
	REQUIRE(r.settled.size() == 3);
	CHECK(same(r.settled[0], fvar("z")));
	CHECK(same(r.settled[1], fvar("y")));
	CHECK(same(r.settled[2], fvar("x")));
	// The same shape with one POSITIVE leaf is no negative tree, and `x`
	// is open again.
	tref mixed = disj(neg(eq0(land(x, a))), eq0(land(x, b)));
	REQUIRE(!ap::is_negative_tree<node_t>(mixed));
	auto q = ap::incidence<node_t>(trefs{ mixed, eq0(land(y, z)) }, X);
	REQUIRE(q.settled.size() == 2);
	CHECK(same(q.settled[0], fvar("z")));
	CHECK(same(q.settled[1], fvar("y")));
}

} // TEST_SUITE
