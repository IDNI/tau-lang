// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for src/anti_prenex/normalisers/joins.h.
// Spec: anti_prenex.md §3 "The result joins", invariant 7, §1 CONTENT ORDER
// and the `absorb_occ_max` knob.
//
// Two conventions the cases lean on. A member of a chain that is not the last
// child of its operator node carries a right sibling, so a comparison against
// a separately built node is by CONTENT — except where the claim IS that the
// join returns the one hash-consed node, as in every AC-canonicity case, and
// those compare trefs. And the construction hooks are ON: they fold T/F,
// `X ∧ X` and `X ∧ ¬X` one construction after the insert rules do, so a case
// that means to observe a rule observes it on the BUILDER.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/normalisers/joins.h"

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

/// The atom `<name> = 0`.
tref atom(const char* name) {
	return tau::build_bf_eq_0(tau::build_bf_variable(name, 0));
}

tref neg(tref n)          { return tau::build_wff_neg(n); }
tref conj(tref l, tref r) { return tau::build_wff_and(l, r); }
tref disj(tref l, tref r) { return tau::build_wff_or(l, r); }

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }
bool is_T(tref n) { return tau::get(n).equals_T(); }
bool is_F(tref n) { return tau::get(n).equals_F(); }

size_t member_count(tref n) { return ap::members<node_t>(n).size(); }

/// `absorb_occ_max` for the scope of one case, restored on the way out — a
/// failing CHECK must not leak a low limit into the next case.
struct occ_limit_guard {
	const size_t saved = ap::absorb_occ_max;
	explicit occ_limit_guard(size_t limit) { ap::absorb_occ_max = limit; }
	~occ_limit_guard() { ap::absorb_occ_max = saved; }
};

} // namespace

TEST_SUITE("anti_prenex/normalisers/joins") {

// --- the insert rules ----------------------------------------------------------

TEST_CASE("J1: rules 1 and 2 — the neutral constant is skipped, the deciding "
	"one decides") {
	tref a = atom("a"), b = atom("b");
	// ∧: T is neutral, F decides.
	CHECK(ap::simplified_and_join<node_t>(trefs{ a, tau::_T() }) == a);
	CHECK(ap::simplified_and_join<node_t>(trefs{ tau::_T(), a }) == a);
	CHECK(is_T(ap::simplified_and_join<node_t>(
		trefs{ tau::_T(), tau::_T() })));
	CHECK(is_F(ap::simplified_and_join<node_t>(trefs{ a, tau::_F(), b })));
	// ∨: the dual.
	CHECK(ap::simplified_or_join<node_t>(trefs{ a, tau::_F() }) == a);
	CHECK(ap::simplified_or_join<node_t>(trefs{ tau::_F(), a }) == a);
	CHECK(is_F(ap::simplified_or_join<node_t>(
		trefs{ tau::_F(), tau::_F() })));
	CHECK(is_T(ap::simplified_or_join<node_t>(trefs{ a, tau::_T(), b })));
}

TEST_CASE("J2: rule 3 — a same-connective operand is flattened one level") {
	tref a = atom("a"), b = atom("b"), c = atom("c"), d = atom("d");
	// Left-deep, right-deep and mixed spines all contribute four members,
	// and the join is the one canonical chain over them.
	tref want = ap::canonical_and<node_t>(trefs{ a, b, c, d });
	CHECK(ap::simplified_and_join<node_t>(
		trefs{ conj(conj(a, b), c), d }) == want);
	CHECK(ap::simplified_and_join<node_t>(
		trefs{ a, conj(b, conj(c, d)) }) == want);
	CHECK(ap::simplified_and_join<node_t>(
		trefs{ conj(a, b), conj(c, d) }) == want);
	CHECK(member_count(want) == 4);
	// The flattening stops at the other connective: `b ∨ c` stays one
	// member of the conjunction.
	tref mixed = ap::simplified_and_join<node_t>(trefs{ conj(a, disj(b, c)) });
	CHECK(member_count(mixed) == 2);
	// ∨: the dual.
	tref want_or = ap::canonical_or<node_t>(trefs{ a, b, c, d });
	CHECK(ap::simplified_or_join<node_t>(
		trefs{ disj(disj(a, b), c), d }) == want_or);
	CHECK(ap::simplified_or_join<node_t>(
		trefs{ a, disj(b, disj(c, d)) }) == want_or);
	CHECK(member_count(want_or) == 4);
}

TEST_CASE("J3: rule 4 — an operand that is already a member is skipped") {
	tref a = atom("a"), b = atom("b");
	CHECK(ap::simplified_and_join<node_t>(trefs{ a, a }) == a);
	CHECK(ap::simplified_or_join<node_t>(trefs{ a, a }) == a);
	// A duplicate that arrives through the flattening is skipped too, and
	// the member carrying a right sibling matches the same formula
	// standing alone (the set compares by content).
	tref ab = ap::canonical_and<node_t>(trefs{ a, b });
	CHECK(ap::simplified_and_join<node_t>(trefs{ conj(a, b), b }) == ab);
	CHECK(member_count(ap::simplified_and_join<node_t>(
		trefs{ conj(a, b), b, a })) == 2);
	tref ab_or = ap::canonical_or<node_t>(trefs{ a, b });
	CHECK(ap::simplified_or_join<node_t>(trefs{ disj(a, b), b }) == ab_or);
}

TEST_CASE("J4: rule 5 — a literal against a complementary member decides") {
	tref a = atom("a"), b = atom("b");
	CHECK(is_F(ap::simplified_and_join<node_t>(trefs{ a, neg(a) })));
	CHECK(is_F(ap::simplified_and_join<node_t>(trefs{ neg(a), a })));
	CHECK(is_T(ap::simplified_or_join<node_t>(trefs{ a, neg(a) })));
	CHECK(is_T(ap::simplified_or_join<node_t>(trefs{ neg(a), a })));
	// The pair need not be adjacent, and it decides through the flattening
	// as well.
	CHECK(is_F(ap::simplified_and_join<node_t>(
		trefs{ a, b, conj(b, neg(a)) })));
	// ONLY a literal decides by this rule: `¬(a ∨ b)` is no literal, so
	// the builder does not decide on it — the hooks fold the pair one
	// construction later, when the chain is assembled.
	ap::join_builder<node_t, true> builder;
	CHECK(!builder.insert(disj(a, b)));
	CHECK(!builder.insert(neg(disj(a, b))));
	CHECK(!builder.decided());
	CHECK(is_F(builder.result()));
}

TEST_CASE("J5: invariant 7 — a deciding operand stops the later ones") {
	tref a = atom("a"), b = atom("b"), c = atom("c");
	// The counting loop is what a real caller is: it BUILDS the next
	// operand only when the builder has not decided. Feeding `¬a` after
	// `a` decides, so `c` is never built and never fed.
	const trefs operands{ a, neg(a), c };
	ap::join_builder<node_t, true> builder;
	size_t fed = 0;
	for (tref r : operands) {
		++fed;
		if (builder.insert(r)) break;
	}
	CHECK(fed == 2);
	CHECK(builder.decided());
	CHECK(is_F(builder.result()));
	// A later insert is a no-op, and the result does not move.
	CHECK(builder.insert(b));
	CHECK(is_F(builder.result()));
	// ∨: the dual, decided by T this time.
	ap::join_builder<node_t, false> dual;
	CHECK(!dual.insert(a));
	CHECK(dual.insert(tau::_T()));
	CHECK(dual.decided());
	CHECK(dual.insert(b));
	CHECK(is_T(dual.result()));
}

// --- the degenerate joins -------------------------------------------------------

TEST_CASE("J6: the empty join is the neutral constant, one member is itself") {
	tref a = atom("a"), b = atom("b");
	CHECK(is_T(ap::simplified_and_join<node_t>(trefs{})));
	CHECK(is_F(ap::simplified_or_join<node_t>(trefs{})));
	CHECK(ap::simplified_and_join<node_t>(trefs{ a }) == a);
	CHECK(ap::simplified_or_join<node_t>(trefs{ a }) == a);
	// A single member that arrives carrying a right sibling comes back
	// trimmed, i.e. as the node it is by content.
	trefs ms = ap::members<node_t>(conj(a, b));
	REQUIRE(ms.size() == 2);
	CHECK(ap::simplified_and_join<node_t>(trefs{ ms[0] }) == a);
	// The builder may be asked for its result twice.
	ap::join_builder<node_t, true> builder;
	CHECK(!builder.insert(a));
	CHECK(builder.result() == a);
	CHECK(builder.result() == a);
}

// --- AC-canonicity --------------------------------------------------------------

TEST_CASE("J7: the assembled node is a function of the member SET") {
	tref a = atom("a"), b = atom("b"), c = atom("c");
	// Commutativity: two insertion orders, one node.
	tref abc = ap::simplified_and_join<node_t>(trefs{ a, b, c });
	CHECK(abc == ap::simplified_and_join<node_t>(trefs{ c, a, b }));
	CHECK(abc == ap::simplified_and_join<node_t>(trefs{ b, c, a }));
	CHECK(abc == ap::canonical_and<node_t>(trefs{ a, b, c }));
	// Associativity: through the flattening, any spelling of the operands
	// reaches that same node.
	CHECK(abc == ap::simplified_and_join<node_t>(trefs{ conj(b, a), c }));
	CHECK(abc == ap::simplified_and_join<node_t>(trefs{ c, conj(a, b) }));
	CHECK(abc == ap::simplified_and_join<node_t>(
		trefs{ conj(c, b), conj(b, a) }));
	// The dual, and the two are different nodes.
	tref abc_or = ap::simplified_or_join<node_t>(trefs{ a, b, c });
	CHECK(abc_or == ap::simplified_or_join<node_t>(trefs{ disj(c, b), a }));
	CHECK(abc_or != abc);
}

// --- the absorption pass ---------------------------------------------------------

TEST_CASE("J8: absorption, d ∨ (d ∧ e) = d, for a literal absorber") {
	tref d = atom("d"), e = atom("e"), f = atom("f");
	CHECK(ap::simplified_or_join<node_t>(trefs{ d, conj(d, e) }) == d);
	// Independent of the insertion order.
	CHECK(ap::simplified_or_join<node_t>(trefs{ conj(d, e), d }) == d);
	// The absorbed member may have any number of further parts.
	CHECK(ap::simplified_or_join<node_t>(
		trefs{ d, conj(conj(d, e), f) }) == d);
	// Only a member whose parts are a SUPERSET is absorbed: a disjoint
	// conjunction stays, and two members with the same part count never
	// absorb each other.
	CHECK(member_count(ap::simplified_or_join<node_t>(
		trefs{ d, conj(e, f) })) == 2);
	CHECK(member_count(ap::simplified_or_join<node_t>(
		trefs{ conj(d, e), conj(d, f) })) == 2);
}

TEST_CASE("J9: absorption for a conjunction absorber, and the dual") {
	tref a = atom("a"), b = atom("b"), e = atom("e");
	// `d` is itself a conjunction: its PARTS are its members, and it
	// absorbs every member holding all of them.
	tref d = ap::canonical_and<node_t>(trefs{ a, b });
	tref bigger = ap::canonical_and<node_t>(trefs{ a, b, e });
	CHECK(ap::simplified_or_join<node_t>(trefs{ d, bigger }) == d);
	CHECK(ap::simplified_or_join<node_t>(trefs{ bigger, d }) == d);
	// The dual join: `d ∧ (d ∨ e) = d`, with `d` a literal and with `d` a
	// disjunction.
	tref lit = atom("d");
	CHECK(ap::simplified_and_join<node_t>(trefs{ lit, disj(lit, e) }) == lit);
	tref d_or = ap::canonical_or<node_t>(trefs{ a, b });
	tref bigger_or = ap::canonical_or<node_t>(trefs{ a, b, e });
	CHECK(ap::simplified_and_join<node_t>(trefs{ d_or, bigger_or }) == d_or);
	CHECK(ap::simplified_and_join<node_t>(trefs{ bigger_or, d_or }) == d_or);
}

TEST_CASE("J10: a member whose parts all exceed absorb_occ_max stays") {
	tref a = atom("a"), b = atom("b"), c = atom("c");
	const trefs operands{ a, conj(a, b), conj(a, c) };
	// With the default limit, `a` absorbs both conjunctions.
	CHECK(ap::simplified_or_join<node_t>(operands) == a);
	{
		// `a` occurs in all three members, so with a limit of 1 it is
		// no candidate key and `a` absorbs nothing; the rarest part of
		// either conjunction (`b`, `c`) occurs in that conjunction
		// alone, which has no member with more parts to absorb. So
		// every member survives — precision, never soundness.
		occ_limit_guard limit(1);
		CHECK(member_count(
			ap::simplified_or_join<node_t>(operands)) == 3);
	}
	// The knob is restored, and with it the absorption.
	CHECK(ap::absorb_occ_max == 32);
	CHECK(ap::simplified_or_join<node_t>(operands) == a);
}

// --- idempotence ------------------------------------------------------------------

TEST_CASE("J11: joining a canonical chain's members gives the chain back") {
	tref a = atom("a"), b = atom("b"), c = atom("c");
	tref chain = ap::canonical_and<node_t>(trefs{ a, b, c });
	CHECK(ap::simplified_and_join<node_t>(ap::members<node_t>(chain))
		== chain);
	tref chain_or = ap::canonical_or<node_t>(trefs{ a, b, c });
	CHECK(ap::simplified_or_join<node_t>(ap::members<node_t>(chain_or))
		== chain_or);
	// A chain with a disjunctive member: nothing absorbs, nothing
	// flattens, and the chain comes back as it stands.
	tref mixed = ap::canonical_and<node_t>(trefs{ a, disj(b, c) });
	REQUIRE(member_count(mixed) == 2);
	CHECK(ap::simplified_and_join<node_t>(ap::members<node_t>(mixed))
		== mixed);
	// And once more over the join's own output, which is the property
	// every later memo key depends on.
	tref once = ap::simplified_and_join<node_t>(trefs{ conj(b, a), c, a });
	CHECK(ap::simplified_and_join<node_t>(ap::members<node_t>(once)) == once);
	CHECK(same(once, chain));
}

}
