// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Layer 1 unit tests for src/anti_prenex/normalisers/nnf.h.
// Spec: anti_prenex.md §3 TO_NNF / NEG (factored) / NORMALIZE_OPERATORS,
// §1 `neg(φ)`, invariant 4.
//
// Invariant 4 is the suite's standing claim: no output of this module holds a
// `bf_neq` or a negated/mirrored order operator ANYWHERE, binder bodies
// included. `no_fused_atoms` below walks the whole tree and every case that
// builds something runs it.
//
// The construction hooks are ON, as everywhere in the module: they fold `¬T`,
// `¬F` and `¬¬` at construction, so a case that means to observe TO_NNF's own
// rules keeps its input clear of those.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/normalisers/nnf.h"

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

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

trefs members(tref n) { return ap::members<node_t>(n); }

tref nnf(tref n)  { return ap::to_canonically_factored_nnf<node_t>(n); }
tref neg_nnf(tref n) { return ap::canonically_factored_neg<node_t>(n); }

/// Invariant 4 over a whole formula: no fused negated atom anywhere — not a
/// `bf_neq`, not one of the eight negated or mirrored order operators — and
/// binder bodies, temporal bodies and reference arguments are all walked.
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

} // namespace

TEST_SUITE("anti_prenex/normalisers/nnf") {

// --- invariant 4: the negation stays at formula level ---------------------------

TEST_CASE("N1: a negated atom keeps its `¬`, never a fused operator") {
	tref a = atom("a");
	// `¬(a = 0)` is the NNF of itself: the codebase's own `to_nnf` would
	// fuse this into `a != 0`, which invariant 4 forbids.
	tref got = nnf(neg(a));
	CHECK(same(got, neg(a)));
	CHECK(no_fused_atoms(got));
	CHECK(!holds(got, tau::bf_neq));
	// Inside a chain, under a binder, and under a temporal operator too.
	tref deep = tau::build_wff_ex(fvar("x"),
		conj(neg(a), tau::build_wff_always(neg(atom("b")))), false);
	CHECK(no_fused_atoms(nnf(deep)));
	// An input that already holds a fused atom is NOT re-spelled by this
	// pass either way: TO_NNF never touches an atom (`NORMALIZE_OPERATORS`
	// is phase 3's job, tested below).
	CHECK(no_fused_atoms(nnf(conj(a, atom("b")))));
}

TEST_CASE("N2: `¬¬` folds and the two constants are their duals") {
	tref a = atom("a");
	// The hooks fold `¬¬` at construction, and TO_NNF agrees with them.
	CHECK(nnf(neg(neg(a))) == a);
	CHECK(tau::get(neg_nnf(tau::_T())).equals_F());
	CHECK(tau::get(neg_nnf(tau::_F())).equals_T());
	CHECK(neg_nnf(neg(a)) == a);
}

// --- De Morgan ------------------------------------------------------------------

TEST_CASE("N3: De Morgan on a 3-chain is flat and in content order") {
	tref a = atom("a"), b = atom("b"), c = atom("c");
	tref got = nnf(neg(conj(conj(a, b), c)));
	// One ∨-node with three negated members, however the input nested.
	REQUIRE(is_child<node_t>(got, tau::wff_or));
	trefs ms = members(got);
	REQUIRE(ms.size() == 3);
	for (size_t i = 1; i < ms.size(); ++i)
		CHECK(tau::subtree_less(ms[i - 1], ms[i]));
	CHECK(got == ap::canonical_or<node_t>(
		trefs{ neg(a), neg(b), neg(c) }));
	// The nesting of the input does not show in the result.
	CHECK(got == nnf(neg(conj(a, conj(b, c)))));
	CHECK(no_fused_atoms(got));
}

TEST_CASE("N4: `C = ∅` makes the disjunction fall back to De Morgan") {
	tref a = atom("a"), b = atom("b"), c = atom("c");
	// Members sharing no conjunct: the result is the ∧-chain of the
	// negated members.
	tref got = nnf(neg(disj(a, conj(b, c))));
	REQUIRE(is_child<node_t>(got, tau::wff_and));
	CHECK(got == ap::canonical_and<node_t>(
		trefs{ neg(a), nnf(neg(conj(b, c))) }));
	CHECK(members(got).size() == 2);
	CHECK(no_fused_atoms(got));
}

// --- factored negation -----------------------------------------------------------

TEST_CASE("N5: the shared conjuncts come out first (the spec's shape)") {
	tref c1 = atom("c"), c2 = atom("d"), r1 = atom("p"), r2 = atom("q");
	tref C = conj(c1, c2);
	// ¬((C ∧ r₁) ∨ (C ∧ r₂)) = ¬c₁ ∨ ¬c₂ ∨ ¬(r₁ ∨ r₂)
	tref got = nnf(neg(disj(conj(C, r1), conj(C, r2))));
	REQUIRE(is_child<node_t>(got, tau::wff_or));
	trefs ms = members(got);
	REQUIRE(ms.size() == 3);
	tref want = ap::canonical_or<node_t>(trefs{ neg(c1), neg(c2),
		ap::canonical_and<node_t>(trefs{ neg(r1), neg(r2) }) });
	CHECK(got == want);
	CHECK(no_fused_atoms(got));
	// Equivalent to plain De Morgan, which is the point of the rewrite.
	CHECK(are_nso_equivalent<node_t>(got,
		neg(disj(conj(C, r1), conj(C, r2)))));
}

TEST_CASE("N6: a member equal to C drops the residue") {
	tref c1 = atom("c"), r1 = atom("p");
	// ¬(c ∨ (c ∧ p)): C = {c}, the first member leaves the empty ∧ = T, so
	// the residue join is T and its ¬ is F, which drops out — leaving ¬c.
	tref got = nnf(neg(disj(c1, conj(c1, r1))));
	CHECK(same(got, neg(c1)));
	CHECK(no_fused_atoms(got));
	CHECK(are_nso_equivalent<node_t>(got, neg(disj(c1, conj(c1, r1)))));
	// The same with two shared conjuncts and one member equal to C.
	tref c2 = atom("d");
	tref C = conj(c1, c2);
	tref got2 = nnf(neg(disj(C, conj(C, r1))));
	CHECK(got2 == ap::canonical_or<node_t>(trefs{ neg(c1), neg(c2) }));
	CHECK(are_nso_equivalent<node_t>(got2, neg(disj(C, conj(C, r1)))));
}

// --- binders, temporal operators, references -------------------------------------

TEST_CASE("N7: a binder flips without renaming, a temporal operator dualises") {
	tref a = atom("a");
	tref x = fvar("x");
	tref ex = tau::build_wff_ex(x, a, false);
	tref all_ = tau::build_wff_all(x, a, false);
	// `¬∃x.ψ ↦ ∀x.NEG(ψ)` and back, with the SAME bound variable node —
	// `calculate_quant_id = false` everywhere (ground rule 4).
	tref got = nnf(neg(ex));
	REQUIRE(is_child<node_t>(got, tau::wff_all));
	CHECK(same(ap::binder_var<node_t>(got), x));
	CHECK(same(ap::binder_body<node_t>(got), neg(a)));
	CHECK(got == tau::build_wff_all(x, neg(a), false));
	tref back = nnf(neg(got));
	CHECK(back == ex);
	CHECK(nnf(neg(all_)) == tau::build_wff_ex(x, neg(a), false));
	// `¬always ψ ↦ sometimes NEG(ψ)`, and its double negation is `always`
	// again.
	tref always_ = tau::build_wff_always(a);
	tref sometimes_neg = nnf(neg(always_));
	REQUIRE(is_child<node_t>(sometimes_neg, tau::wff_sometimes));
	CHECK(sometimes_neg == tau::build_wff_sometimes(neg(a)));
	CHECK(nnf(neg(sometimes_neg)) == always_);
	CHECK(no_fused_atoms(sometimes_neg));
}

TEST_CASE("N8: a reference keeps its `¬` and is never entered") {
	tref ref = tau::get("f(x)", parse_wff());
	REQUIRE(ref != nullptr);
	tref got = nnf(neg(ref));
	CHECK(same(got, neg(ref)));
	CHECK(no_fused_atoms(got));
}

// --- the neg slot -----------------------------------------------------------------

TEST_CASE("N9: `neg(ψ)` is filled on the first demand and answers the second") {
	tref a = atom("slot_a"), b = atom("slot_b");
	tref psi = conj(a, b);
	// Nothing has asked yet.
	CHECK(ap::neg_of<node_t>(psi) == nullptr);
	tref first = neg_nnf(psi);
	CHECK(ap::neg_of<node_t>(psi) == first);
	// The second call is the slot, not a walk: the same node comes back.
	CHECK(neg_nnf(psi) == first);
	CHECK(first == ap::canonical_or<node_t>(trefs{ neg(a), neg(b) }));
	// And a `¬ψ` met inside a formula is answered from that slot.
	CHECK(nnf(neg(psi)) == first);
	CHECK(no_fused_atoms(first));
}

// --- what phase 1 canonicalises ----------------------------------------------------

TEST_CASE("N10: a raw positive chain comes out canonical, a canonical one is "
	"the same tref") {
	tref a = atom("a"), b = atom("b"), c = atom("c");
	// No negation anywhere: the walk still re-emits every junction through
	// the joins, which is what canonicalises phase 1's raw input (§3).
	tref raw = conj(conj(b, a), a);
	tref canonical = ap::canonical_and<node_t>(trefs{ a, b });
	CHECK(nnf(raw) == canonical);
	CHECK(nnf(canonical) == canonical);
	// Idempotent, and equal for two spellings of one member set.
	CHECK(nnf(nnf(raw)) == canonical);
	CHECK(nnf(disj(disj(c, a), b))
		== ap::canonical_or<node_t>(trefs{ a, b, c }));
	// A junction under a binder is canonicalised too.
	tref under = tau::build_wff_ex(fvar("x"), conj(b, a), false);
	CHECK(nnf(under) == tau::build_wff_ex(fvar("x"), canonical, false));
}

// --- equivalence --------------------------------------------------------------------

TEST_CASE("N11: every rewrite preserves meaning") {
	tref a = atom("a"), b = atom("b"), c = atom("c");
	const trefs inputs{
		neg(conj(a, b)),
		neg(disj(a, b)),
		neg(disj(conj(a, b), conj(a, c))),
		neg(conj(a, disj(b, c))),
		conj(neg(a), disj(b, neg(c))),
		neg(neg(conj(a, disj(b, c)))),
		tau::build_wff_ex(fvar("x"), neg(conj(a, b)), false),
	};
	for (tref in : inputs) {
		tref got = nnf(in);
		CHECK(are_nso_equivalent<node_t>(got, in));
		CHECK(no_fused_atoms(got));
		// Idempotence: the NNF of an NNF is itself.
		CHECK(nnf(got) == got);
	}
}

// --- NORMALIZE_OPERATORS -------------------------------------------------------------

TEST_CASE("N12: `≠` is rewritten in every type") {
	tref x = tau::build_bf_variable("x", 0);
	tref neq = tau::build_bf_neq(x, tau::build_bf_variable("y", 0));
	REQUIRE(is_child<node_t>(neq, tau::bf_neq));
	tref got = ap::normalize_operators<node_t>(neq);
	CHECK(!holds(got, tau::bf_neq));
	CHECK(is_child<node_t>(got, tau::wff_neg));
	CHECK(no_fused_atoms(got));
	CHECK(are_nso_equivalent<node_t>(got, neq));
	// Under a connective and a binder as well.
	tref deep = tau::build_wff_ex(fvar("z"),
		tau::build_wff_and(neq, atom("a")), false);
	CHECK(no_fused_atoms(ap::normalize_operators<node_t>(deep)));
}

TEST_CASE("N13: the six comparison rewrites fire only on an arithmetic type") {
	// A Boolean order atom never reaches the pass: the construction hooks
	// rewrite `<`/`<=` over a non-bv type into equations (dag.h, D4), so
	// there is nothing left to normalise.
	tref bool_le = tau::build_bf_lteq(tau::build_bf_variable("x", 0),
		tau::build_bf_variable("y", 0));
	CHECK(!holds(bool_le, tau::bf_lteq));
	CHECK(no_fused_atoms(ap::normalize_operators<node_t>(bool_le)));
	// A bitvector comparison is the case the rewrites are for.
	tref bv_gt = tau::get("x:bv[8] > y:bv[8]", parse_wff());
	REQUIRE(bv_gt != nullptr);
	REQUIRE(holds(bv_gt, tau::bf_gt));
	{
		tref got = ap::normalize_operators<node_t>(bv_gt);
		CHECK(!holds(got, tau::bf_gt));
		CHECK(holds(got, tau::bf_lt));
		CHECK(no_fused_atoms(got));
	}
	// The guard is on the TYPE, not on the operator: a mirrored operator
	// whose operands carry no bitvector type — built with the hooks off,
	// which is the only way to hold one — is left alone, because
	// `a ≥ b ↦ b ≤ a` is guarded together with the four negated rewrites
	// that are unsound for a lattice order (§3).
	tref lattice_gteq = [] {
		use_hooks_guard<node_t> g(false);
		return tau::build_bf_gteq(tau::build_bf_variable("x", 0),
			tau::build_bf_variable("y", 0));
	}();
	REQUIRE(is_child<node_t>(lattice_gteq, tau::bf_gteq));
	CHECK(ap::normalize_operators<node_t>(lattice_gteq) == lattice_gteq);
	// while the default instantiation, which every other caller uses,
	// still rewrites it.
	CHECK(!holds(normalize_atomic_formula_operators<node_t>(lattice_gteq),
		tau::bf_gteq));
}

}
