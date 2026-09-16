// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Layer-1 INTEGRATION test of src/anti_prenex/normalisers/ — packages J
// (joins), N (nnf) and S (simplify) working together, the layer-1 milestone.
// Spec: anti_prenex.md §3 (TO_NNF, NEG, NORMALIZE_OPERATORS, SIMPLIFY, the
// result joins), §1 (`neg(φ)`, CONTENT ORDER), invariant 4, invariant 6.
// Every case crosses at least two of the three packages; the three package
// tests are complemented, never duplicated.
//
// The milestone's claims and how each is observed — through node identity and
// `are_nso_equivalent`, never through timing:
//  1. every rewrite is equivalence-preserving on pure inputs, through the
//     whole phase-1/phase-3 pipeline `TO_NNF → SIMPLIFY(ref_args) →
//     NORMALIZE_OPERATORS`.
//  2. the joins are AC-canonical end to end: two spellings of one formula —
//     different nesting, different member order, a negation pushed one level
//     differently — reach ONE hash-consed node through `TO_NNF`, and
//     `SIMPLIFY` of that node is idempotent, which is what makes a memo key
//     a function of content (§6).
//  3. the normal form is strong enough for the syntactic tests: invariant 4
//     holds over the whole pipeline's output — no `bf_neq`, no negated or
//     mirrored order operator anywhere, binder bodies included — even when
//     the input holds both.
//  4. `neg(φ)` is filled on demand and the dualisation round trip restores
//     the binder and temporal kinds (§3 NEG, §5 PROCESS_BLOCK).
//  5. the spec's own example of what propagation is FOR — `f = xy ∪ x′a`
//     reaching `a` — through the whole pipeline, in both regimes.
//
// Conventions (the layer-0 brief): a chain member carries a right sibling, so
// a comparison against a separately built node is by CONTENT unless the claim
// IS the one hash-consed node; members are located by a predicate, never by
// position.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/anti_prenex.h"

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

tref ex(const char* v, tref body) {
	return tau::build_wff_ex(fvar(v), body, false);
}
tref all_(const char* v, tref body) {
	return tau::build_wff_all(fvar(v), body, false);
}

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

tref nnf(tref n) { return ap::to_canonically_factored_nnf<node_t>(n); }

/// Phases 1 and 3 of §3 in one call: `TO_NNF`, then `SIMPLIFY` in phase 1's
/// entry state (`ref_args = true`, which establishes invariant 6), then
/// `NORMALIZE_OPERATORS`. The empty order is the plain regime phases 1 and 3
/// run in.
tref pipeline(tref phi) {
	return ap::normalize_operators<node_t>(
		ap::simplify<node_t>(nnf(phi), {}, true));
}

/// Invariant 4 over a whole formula: no `bf_neq` and no negated or mirrored
/// order operator ANYWHERE — under a binder, under a temporal operator and
/// inside a reference argument alike, since the walk enters everything.
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

TEST_SUITE("anti_prenex/layer1") {

// --- 1. the pipeline preserves meaning ------------------------------------------

TEST_CASE("L1: every rewrite of the pipeline is equivalence-preserving") {
	tref a = eq0(bvar("a")), b = eq0(bvar("b")), c = eq0(bvar("c"));
	tref y = bvar("y"), p = bvar("p"), q = bvar("q");
	const trefs inputs{
		// De Morgan, through the or-join.
		neg(conj(a, conj(b, c))),
		// The factored negation: the members share `a`.
		neg(disj(conj(a, b), conj(a, c))),
		// `¬¬`, and a junction that the joins deduplicate.
		neg(neg(conj(a, conj(b, a)))),
		// A binder whose body is negated, so the binder flips and the
		// body goes through NEG.
		neg(ex("x", conj(a, eq0(bvar("x"))))),
		// Propagation under the pipeline: a pin and a NON-equation
		// target, which the pass rewrites under the whole environment.
		conj(eq(y, land(p, q)), disj(eq0(lor(y, bvar("d"))), b)),
		// A mixed shape: a disjunction of conjunctions under a binder,
		// with one negated member.
		ex("x", disj(conj(a, neg(b)), conj(eq0(bvar("x")), c))),
	};
	for (tref in : inputs) {
		tref got = pipeline(in);
		CHECK(are_nso_equivalent<node_t>(got, in));
		CHECK(no_fused_atoms(got));
		// Invariant 6 at the pipeline's end: running it again changes
		// nothing.
		CHECK(pipeline(got) == got);
	}
}

// --- 2. AC-canonicity end to end --------------------------------------------------

TEST_CASE("L2: three spellings of one formula reach ONE node") {
	tref a = eq0(bvar("a")), b = eq0(bvar("b")), c = eq0(bvar("c"));
	// `¬(a ∧ b ∧ c)` in three spellings: left-nested, right-nested with
	// the members in another order, and with the negation already pushed
	// one level in. The joins' flattening and content-order emission make
	// all three ONE node.
	tref left  = nnf(neg(conj(conj(a, b), c)));
	tref right = nnf(neg(conj(c, conj(b, a))));
	tref pushed = nnf(disj(neg(a), neg(conj(b, c))));
	CHECK(left == right);
	CHECK(left == pushed);
	REQUIRE(is_child<node_t>(left, tau::wff_or));
	CHECK(ap::members<node_t>(left).size() == 3);
	// The same for the dual, and the two are different nodes.
	tref dual_left  = nnf(neg(disj(disj(a, b), c)));
	tref dual_right = nnf(neg(disj(c, disj(b, a))));
	CHECK(dual_left == dual_right);
	CHECK(dual_left != left);
	// `SIMPLIFY` of the canonical node is idempotent and keeps it — the
	// property every memo key downstream depends on (§6).
	tref simplified = ap::simplify<node_t>(left);
	CHECK(ap::simplify<node_t>(simplified) == simplified);
	CHECK(are_nso_equivalent<node_t>(simplified, left));
	// A duplicated member is one member, whichever spelling arrives.
	CHECK(nnf(neg(conj(conj(a, b), a))) == nnf(neg(conj(b, a))));
}

// --- 3. invariant 4 over the whole pipeline ----------------------------------------

TEST_CASE("L3: a `≠` and a bitvector `>` both leave as formula-level "
	"negations") {
	tref neq = tau::build_bf_neq(bvar("a"), bvar("b"));
	REQUIRE(is_child<node_t>(neq, tau::bf_neq));
	tref bv_gt = tau::get("x:bv[8] > y:bv[8]", parse_wff());
	REQUIRE(bv_gt != nullptr);
	REQUIRE(holds(bv_gt, tau::bf_gt));
	// Both inside a binder, one of them under a negation the NNF pass has
	// to keep where it is.
	tref phi = ex("z", conj(neq, conj(neg(bv_gt), eq0(bvar("z")))));
	tref got = pipeline(phi);
	INFO("pipeline: ", tau::get(got).to_str());
	CHECK(no_fused_atoms(got));
	CHECK(!holds(got, tau::bf_neq));
	CHECK(!holds(got, tau::bf_gt));
	// The bitvector comparison survives as an un-negated `<` (§3's
	// postcondition for arithmetic-typed content).
	CHECK(holds(got, tau::bf_lt));
	// The binder is still there, and its body is what was normalised.
	REQUIRE(is_child<node_t>(got, tau::wff_ex));
	CHECK(no_fused_atoms(ap::binder_body<node_t>(got)));
	// Negating the pipeline's output keeps invariant 4 too: NEG never
	// builds a fused atom either.
	CHECK(no_fused_atoms(ap::canonically_factored_neg<node_t>(got)));
}

// --- 4. the neg slot and the dualisation round trip ----------------------------------

TEST_CASE("L4: `neg(φ)` is filled on demand and the round trip restores the "
	"kinds") {
	tref a = eq0(bvar("l4a")), b = eq0(bvar("l4b"));
	tref phi = ex("x", conj(a, disj(b, eq0(bvar("x")))));
	// Nothing has asked yet.
	CHECK(ap::neg_of<node_t>(phi) == nullptr);
	tref negated = ap::canonically_factored_neg<node_t>(phi);
	CHECK(ap::neg_of<node_t>(phi) == negated);
	CHECK(ap::canonically_factored_neg<node_t>(phi) == negated);
	// The binder flipped and the body is assembled through the joins.
	REQUIRE(is_child<node_t>(negated, tau::wff_all));
	CHECK(same(ap::binder_var<node_t>(negated), fvar("x")));
	CHECK(is_child<node_t>(ap::binder_body<node_t>(negated), tau::wff_or));
	// NEG(NEG(φ)) is φ again — the same node, binder kind restored.
	tref back = ap::canonically_factored_neg<node_t>(negated);
	CHECK(is_child<node_t>(back, tau::wff_ex));
	CHECK(are_nso_equivalent<node_t>(back, phi));
	CHECK(back == nnf(phi));
	// A temporal operator dualises and comes back the same way (§3 NEG,
	// as PROCESS_BLOCK's dualisation uses it).
	tref temporal = tau::build_wff_always(conj(a, b));
	tref dual = ap::canonically_factored_neg<node_t>(temporal);
	REQUIRE(is_child<node_t>(dual, tau::wff_sometimes));
	CHECK(is_child<node_t>(
		tau::trim2(dual), tau::wff_or));   // De Morgan inside
	tref restored = ap::canonically_factored_neg<node_t>(dual);
	CHECK(is_child<node_t>(restored, tau::wff_always));
	CHECK(restored == nnf(temporal));
	// `SIMPLIFY` leaves the round trip alone: it is already in normal
	// form, and a temporal operator is opaque to both its passes.
	CHECK(ap::simplify<node_t>(restored) == restored);
}

// --- 5. what propagation is for ------------------------------------------------------

TEST_CASE("L5: the spec's `f = xy ∪ x′t` example reaches `t`") {
	tref x = bvar("x"), y = bvar("y"), a = bvar("a"), b = bvar("b");
	// `t = a·b` rather than a bare variable, so the equation is
	// asymmetric and the pin direction is fixed at `y ↦ t` (§3's match
	// takes either side of `y = a`).
	const tref t = land(a, b);
	tref pinning = eq(y, t);
	tref target = eq0(lor(land(x, y), land(lneg(x), t)));
	tref phi = conj(pinning, target);
	tref got = pipeline(phi);
	INFO("flagship: ", tau::get(got).to_str());
	CHECK(are_nso_equivalent<node_t>(got, phi));
	CHECK(no_fused_atoms(got));
	CHECK(pipeline(got) == got);
	// `y` survives in the pinning conjunct alone — which still constrains
	// it, `y + t` being its term — and the target's term folded to the
	// witness: `f₀ = t` and `f₁ = y` compared unequal until `y := t`.
	size_t with_y = 0, folded = 0;
	for (tref m : ap::members<node_t>(got)) {
		if (ap::fv_meets<node_t>(m, ap::block{ fvar("y") })) {
			++with_y;
			CHECK(is_child<node_t>(m, tau::bf_eq));
			CHECK(same(ap::term_of<node_t>(m, {}),
				ap::simplify_term<node_t>(lxor(y, t))));
		} else if (is_child<node_t>(m, tau::bf_eq)
			&& same(ap::term_of<node_t>(m, {}), t)) ++folded;
	}
	CHECK(with_y == 1);
	CHECK(folded == 1);
}

TEST_CASE("L5b: the same in the BDD regime, which is where the spec meets it") {
	tref x = bvar("x"), y = bvar("y"), a = bvar("a"), b = bvar("b");
	const tref t = land(a, b);
	tref phi = conj(eq(y, t), eq0(lor(land(x, y), land(lneg(x), t))));
	// §5's setup for a one-variable block: the order over `x`, then the
	// terms prepared under it — the target touches `x` and becomes
	// BDD-backed, the pinning equation does not and stays plain.
	const ap::block P{ fvar("x") };
	const ap::var_order<node_t> o =
		ap::ctx<node_t>::for_component(P, 0, false).order;
	tref prepared = ap::prepare_terms<node_t>(phi, P, o);
	// The preparation really did back a term, or this case would silently
	// be the plain one again.
	REQUIRE(holds(prepared, tau::BDD_ID));
	tref got = ap::simplify<node_t>(prepared, o);
	// The propagation runs on the plain pinning equation (`X`-free, as
	// §3's orientation rule demands) and rewrites the BDD-backed target
	// through the library's compose: both its cofactors become `t`, the
	// BDD stops branching on `x`, and the atom comes back PLAIN.
	size_t with_y = 0, folded = 0;
	for (tref m : ap::members<node_t>(got)) {
		if (ap::fv_meets<node_t>(m, ap::block{ fvar("y") })) ++with_y;
		else if (is_child<node_t>(m, tau::bf_eq)
			&& same(ap::term_of<node_t>(m, o), t)) ++folded;
	}
	CHECK(with_y == 1);
	CHECK(folded == 1);
	// and the result is free of `x`, which is what lets the block go.
	CHECK(!ap::fv_meets<node_t>(got, P));
}

}
