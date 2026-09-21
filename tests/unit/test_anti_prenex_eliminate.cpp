// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Layer 3 unit tests for src/anti_prenex/eliminate/atomless.h and
// eliminate_block.h: §7's type table, ELIMINATE_BLOCK with its memo and its
// two pre-steps, and the atomless method end to end. Spec: anti_prenex.md §7
// (the table, the method contract, ELIMINATE_ATOMLESS_CLAUSE, DISCHARGE), §3
// (TRY_WITNESS), §1 (the term representation, the leaf hazard, cache scope).
//
// TYPES: only the ATOMLESS types (`tau`, `sbf`) and the bitvector family are
// rows of the table, so every fixture that reaches it carries typed
// variables, and `ctx.type` is read off the block. The clauses are PARSED
// whole, as `∃P.ψ`, which both types them — inference gives an untyped
// variable the `tau` type — and hands the oracle its reference formula; the
// block is read off the binder chain and the body is put into §1's term
// representation with `prepare_terms`, the state a caller of ELIMINATE_BLOCK
// hands over.
//
// A result is checked by `are_nso_equivalent` against `∃P.ψ` wherever the
// content allows it — never against an output string, and never on
// BDD-backed terms before `convert_to_tau_terms` has spelled them out — and
// structurally where a reference, a bitvector or a kept functional quantifier
// puts the checker out of reach.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "normalizer.h"
#include "anti_prenex/eliminate/eliminate_block.h"

#include <algorithm>
#include <utility>

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

using tb = tau_term_bdd<node_t>;
using th = term_handle<node_t>;

tref parse(const char* sample) {
	auto rr = get_nso_rr(sample);
	REQUIRE(rr.has_value());
	return rr.value().main->get();
}

/// A `tau`-typed `variable` node — a block member and an order key — and the
/// `bf` around it, which is what a term holds. The one fixture that is built
/// rather than parsed spells the type itself, since only a typed block has a
/// row in the table.
tref tvar(const char* name) {
	return tau::build_variable(name, tau_type_id<node_t>());
}
tref tbf(const char* name) {
	return tau::build_bf_variable(name, tau_type_id<node_t>());
}

tref eq0(tref t)          { return build_bf_eq_0<node_t>(t); }
tref land(tref l, tref r) { return build_bf_and<node_t>(l, r); }
tref lor(tref l, tref r)  { return build_bf_or<node_t>(l, r); }
tref conj(tref l, tref r) { return tau::build_wff_and(l, r); }

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

/// The `l = r` sides of an atom, through no negation.
std::pair<tref, tref> sides(tref atom) {
	const tau& t = tau::get(atom);
	REQUIRE(t.is(tau::wff));
	REQUIRE(t[0].has_child());
	return { tau::trim_right_sibling(t[0].first()),
		tau::trim_right_sibling(t[0].second()) };
}

/// Is there a node of this kind anywhere in `n`?
bool holds(tref n, size_t nt) {
	return tau::get(n).find_top([nt](tref m) {
		return tree<node_t>::get(m).is(nt); }) != nullptr;
}

/// Every formula binder of `n`, nested ones included.
size_t binder_count(tref n) {
	return tau::get(n).select_all(is_child_quantifier<node_t>).size();
}

/// The formula with every stored BDD spelled out, which is what the oracle
/// reads.
tref finished(tref n) { return th::convert_to_tau_terms(n); }

/// NO FREE VARIABLE ESCAPED: `FV(out) ⊆ FV(in)`.
bool no_escape(tref out, tref in) {
	const trefs& before = get_free_vars<node_t>(in);
	for (tref v : get_free_vars<node_t>(out))
		if (!std::binary_search(before.begin(), before.end(), v,
			tau::subtree_less)) return false;
	return true;
}

/// INVARIANT 4 over a whole formula: no `bf_neq` and no negated or mirrored
/// order operator anywhere, and every `¬` sits directly over an ATOM.
bool invariant_4(tref n) {
	bool clean = true;
	auto check = [&clean](tref m) {
		const auto& t = tau::get(m);
		if (t.is(tau::bf_neq) || t.is(tau::bf_nlt) || t.is(tau::bf_nlteq)
			|| t.is(tau::bf_gt) || t.is(tau::bf_gteq)
			|| t.is(tau::bf_ngt) || t.is(tau::bf_ngteq))
				return clean = false;
		if (t.child_is(tau::wff_neg)
			&& !is_atomic_fm<node_t>(
				tau::trim_right_sibling(t[0].first())))
				return clean = false;
		return true;
	};
	auto all_of_it = [](tref) { return true; };
	auto up = [](tref) {};
	pre_order<node_t>(n).search_unique(check, all_of_it, up);
	return clean;
}

/// A parsed `∃P.ψ` split the way `ELIMINATE_BLOCK`'s callers hand it over:
/// the block OUTERMOST FIRST, the component's ctx over it — `type` read off
/// the block, as invariant 2 allows — and the body in §1's term
/// representation.
struct fixture {
	tref quantified = nullptr;   ///< `∃P.ψ` as parsed, the oracle's reference
	ap::block P;
	ap::ctx<node_t> c;
	tref clause = nullptr;       ///< `ψ`, prepared over `P`
};

fixture make(const char* src, bool keep = false) {
	fixture f;
	f.quantified = parse(src);
	tref n = f.quantified;
	while (is_child_quantifier<node_t>(n)) {
		f.P.push_back(tau::trim_right_sibling(
			ap::binder_var<node_t>(n)));
		n = tau::trim_right_sibling(ap::binder_body<node_t>(n));
	}
	REQUIRE(!f.P.empty());
	f.c = ap::ctx<node_t>::for_component(f.P,
		find_ba_type<node_t>(f.P[0]), keep);
	f.clause = ap::prepare_terms<node_t>(n, f.P, f.c.order);
	return f;
}

/// What every eliminated clause claims: equivalent to `∃P.ψ`, no free
/// variable escaped, invariant 4.
void check_against_source(tref got, const fixture& f) {
	CHECK(are_nso_equivalent<node_t>(finished(got), f.quantified));
	CHECK(no_escape(finished(got), f.quantified));
	CHECK(invariant_4(got));
}

} // namespace

TEST_SUITE("anti_prenex/eliminate") {

// --- the type table -----------------------------------------------------------------

TEST_CASE("T1: one row per type, and none for what is outside the algorithm") {
	// The atomless row IS the module's statement of which types are
	// atomless, and `EX_DISTRIBUTES_OVER_NEGATIVES` reads the same row.
	CHECK(ap::method<node_t>(tau_type_id<node_t>())
		== ap::method_kind::atomless);
	CHECK(ap::method<node_t>(sbf_type_id<node_t>())
		== ap::method_kind::atomless);
	CHECK(ap::ex_distributes_over_negatives<node_t>(tau_type_id<node_t>()));
	CHECK(ap::ex_distributes_over_negatives<node_t>(sbf_type_id<node_t>()));
	// The bitvector family is §7's router, and `∃` does not distribute
	// there: over `bv[1]`, `∃x(x ≠ 0 ∧ x′ ≠ 0)` is `F`.
	CHECK(ap::method<node_t>(bv_type_id<node_t>(8))
		== ap::method_kind::bitvector);
	CHECK(!ap::ex_distributes_over_negatives<node_t>(bv_type_id<node_t>(8)));
#ifndef DEBUG
	// The untyped id is no row at all: a block of such a type reaching the
	// push is a caller bug, which Debug asserts on. Release answers
	// `none`, so that the dispatch still has its graceful exit.
	CHECK(ap::method<node_t>(untyped_type_id<node_t>())
		== ap::method_kind::none);
	CHECK(!ap::ex_distributes_over_negatives<node_t>(
		untyped_type_id<node_t>()));
#endif
}

TEST_CASE("T2: a parsed block is typed, and the oracle reads its content") {
	// Type inference gives an untyped variable the `tau` type, so a parsed
	// fixture lands in the atomless row without spelling a type.
	fixture f = make("ex x (x y = 0).");
	CHECK(find_ba_type<node_t>(f.P[0]) == tau_type_id<node_t>());
	CHECK(ap::method<node_t>(f.c.type) == ap::method_kind::atomless);
	// `∃x.(x = a)` and `∀x.(x = x)` are both `T`: the equivalence checker
	// decides `tau`-typed content, which every case below relies on.
	CHECK(are_nso_equivalent<node_t>(parse("ex x (x = a)."),
		parse("all x (x = x).")));
}

// --- ELIMINATE_BLOCK's entry tests ---------------------------------------------------

TEST_CASE("E1: a decided clause, and one the block does not scope") {
	fixture f = make("ex x (x y = 0).");
	// `T` and `F` are returned before anything is looked at.
	CHECK(ap::eliminate_block<node_t>(_T<node_t>(), f.P, f.c)
		== _T<node_t>());
	CHECK(ap::eliminate_block<node_t>(_F<node_t>(), f.P, f.c)
		== _F<node_t>());
	// `X ∩ FV(clause) = ∅`: nothing enters the key that `∃X` does not
	// scope, and the clause comes back as the same node.
	fixture g = make("ex x (y z = 0).");
	CHECK(ap::eliminate_block<node_t>(g.clause, g.P, g.c) == g.clause);
}

// --- the pre-steps -------------------------------------------------------------------

TEST_CASE("E2: the witness pass deletes a binder and goes on with the next") {
	// `∃x∃y.(x = a ∧ y = b ∧ x·y·c = 0)`: each pin is exact (invariant 8)
	// and cheaper than any method, so the pass takes both before the
	// dispatch is reached. Every conjunct then holds a free variable
	// alone, the strip scopes them all out, and the block is empty.
	fixture f = make("ex x ex y (x = a && y = b && x y c = 0).");
	REQUIRE(f.P.size() == 2);
	const tref got = ap::eliminate_block_uncached<node_t>(f.clause, f.P,
		f.c);
	check_against_source(got, f);
	CHECK(!holds(got, tau::wff_ex));
	CHECK(!ap::fv_meets<node_t>(got, f.P));
	// `a·b·c = 0`, the pin's image in the surviving conjunct.
	CHECK(are_nso_equivalent<node_t>(finished(got), parse("a b c = 0.")));
}

TEST_CASE("E3: the strip scopes out what no longer holds a block variable") {
	// `∃x∃y.(x = a ∧ y·b = 0)`: the witness takes `x` and leaves the pin's
	// image `T`, so `y·b = 0` is all that is left of the clause and `x`
	// is in none of it. `y` stays, and its component is discharged.
	fixture f = make("ex x ex y (x = a && y b = 0).");
	REQUIRE(f.P.size() == 2);
	const tref got = ap::eliminate_block_uncached<node_t>(f.clause, f.P,
		f.c);
	check_against_source(got, f);
	// `∀_y y·b = 0` is `0 = 0`: the whole clause is `T`.
	CHECK(tau::get(got).equals_T());
}

// --- the dispatch --------------------------------------------------------------------

TEST_CASE("E4: a bitvector block is re-wrapped exactly, with the rest outside") {
	// The bitvector router has no arm here, so the dependent conjuncts
	// come back under the block and the independent ones stay outside it
	// (invariant 3). `x·a = 0` pins nothing — its cofactors are `0` and
	// `a` — so the witness pass leaves the block alone.
	fixture f = make("ex x (x:bv[2] a:bv[2] = { 0 }:bv[2] "
		"&& z:bv[2] b:bv[2] = { 0 }:bv[2]).");
	REQUIRE(ap::method<node_t>(f.c.type) == ap::method_kind::bitvector);
	trefs indep, dep;
	for (tref m : ap::members<node_t>(f.clause))
		(ap::fv_meets<node_t>(m, f.P) ? dep : indep).push_back(m);
	REQUIRE(dep.size() == 1);
	REQUIRE(indep.size() == 1);

	const tref got = ap::eliminate_block_uncached<node_t>(f.clause, f.P,
		f.c);
	CHECK(same(got, ap::simplified_and_join<node_t>(trefs{ indep[0],
		ap::rewrap<node_t>(dep[0], f.P) })));
	CHECK(invariant_4(got));
	CHECK(no_escape(finished(got), f.quantified));
}

// --- the atomless method, end to end -------------------------------------------------

TEST_CASE("E5: one positive and one negative") {
	// `∃x.(x·a = 0 ∧ ¬(x·b = 0))`: `pos` is `∀_x x·a = 0`, which is `T`,
	// and the one condition is `∃_x (x·a)′·x·b ≠ 0`, i.e. `a′·b ≠ 0`.
	fixture f = make("ex x (x a = 0 && !(x b = 0)).");
	const tref got = ap::eliminate_block<node_t>(f.clause, f.P, f.c);
	check_against_source(got, f);
	CHECK(!holds(got, tau::wff_ex));
	CHECK(!ap::fv_meets<node_t>(got, f.P));
	CHECK(ap::is_negated_equation<node_t>(got));
	CHECK(are_nso_equivalent<node_t>(finished(got), parse("!(a' b = 0).")));
}

TEST_CASE("E6: two positives in one component are squeezed into one atom") {
	// Neither `x·a = 0` nor `x′·c = 0` pins `x`, so the method runs. They
	// share `x` and are therefore ONE component: their terms are squeezed
	// into `F = x·a ∪ x′·c` and ONE atom goes out, `∀_x F = 0`, which is
	// `a·c = 0` — not one atom each.
	fixture f = make("ex x (x a = 0 && x' c = 0).");
	const tref got = ap::eliminate_block<node_t>(f.clause, f.P, f.c);
	check_against_source(got, f);
	CHECK(!holds(got, tau::wff_ex));
	CHECK(!ap::fv_meets<node_t>(got, f.P));
	CHECK(is_child<node_t>(got, tau::bf_eq));
	CHECK(are_nso_equivalent<node_t>(finished(got), parse("a c = 0.")));
}

TEST_CASE("E7: a negative touching two components") {
	// `∃x∃y.(x·a = 0 ∧ y·b = 0 ∧ ¬(x·y = 0))`: the two positives are
	// separate components, and the negative absorbs against BOTH, over
	// `X_K ∪ X_g = {x, y}`. Under `pos` the answer is `a′·b′ ≠ 0`.
	fixture f = make("ex x ex y (x a = 0 && y b = 0 && !(x y = 0)).");
	REQUIRE(f.P.size() == 2);
	const tref got = ap::eliminate_block<node_t>(f.clause, f.P, f.c);
	check_against_source(got, f);
	CHECK(!holds(got, tau::wff_ex));
	CHECK(!ap::fv_meets<node_t>(got, f.P));
	CHECK(are_nso_equivalent<node_t>(finished(got),
		parse("!(a' b' = 0).")));
}

TEST_CASE("E8: a flat negative tree beside a positive") {
	// The tree is ONE conjunct (§1) and becomes its TREE_CONDITION: one
	// condition per literal, the `∨` re-assembled by the ∨-join, and what
	// comes back is a negative tree over the free variables.
	fixture f = make("ex x (x a = 0 && (!(x c = 0) || !(x d = 0))).");
	const tref got = ap::eliminate_block<node_t>(f.clause, f.P, f.c);
	check_against_source(got, f);
	CHECK(!holds(got, tau::wff_ex));
	CHECK(!ap::fv_meets<node_t>(got, f.P));
	CHECK(ap::is_negative_tree<node_t>(got));
	CHECK(are_nso_equivalent<node_t>(finished(got),
		parse("!(a' c = 0) || !(a' d = 0).")));
}

TEST_CASE("E9: a negative tree as the WHOLE clause is one conjunct") {
	// `members` (dag.h) flattens an ∨-node just as it flattens an ∧-node,
	// so the member view is taken for an ∧-node ALONE: a clause that IS a
	// negative tree is ONE conjunct (§1) and goes whole to its
	// TREE_CONDITION. Read as two conjuncts it would come back as the
	// CONJUNCTION of the two conditions, which is strictly stronger than
	// the source.
	fixture f = make("ex x (!(x y = 0) || !(x w = 0)).");
	REQUIRE(ap::is_negative_tree<node_t>(f.clause));
	REQUIRE(ap::members<node_t>(f.clause).size() == 2);

	const tref got = ap::eliminate_block<node_t>(f.clause, f.P, f.c);
	check_against_source(got, f);
	CHECK(!holds(got, tau::wff_ex));
	CHECK(!ap::fv_meets<node_t>(got, f.P));
	// No positive, so every literal takes O2 on its own variables and the
	// `∨` is re-assembled: `∃_x x·y ≠ 0` is `y ≠ 0`, and likewise for `w`.
	CHECK(ap::is_negative_tree<node_t>(got));
	CHECK(are_nso_equivalent<node_t>(finished(got),
		parse("!(y = 0) || !(w = 0).")));
}

// --- what the engine cannot use ------------------------------------------------------

TEST_CASE("E10: a reference freezes its component and nothing else") {
	// This engine answers no queries, so `q(x)` is opaque; `∃` does not
	// distribute over `∧` across shared variables, so `x·a = 0` is frozen
	// with it. The `y` component is untouched by that and is discharged.
	// A reference puts the equivalence checker out of reach, so the claim
	// is structural.
	fixture f = make("ex x ex y (q(x) && x a = 0 && y b = 0 "
		"&& !(y c = 0)).");
	REQUIRE(f.P.size() == 2);
	const tref got = ap::eliminate_block<node_t>(f.clause, f.P, f.c);
	CHECK(invariant_4(got));
	CHECK(no_escape(finished(got), f.quantified));
	// ONE binder, over `x` alone, around the frozen component.
	REQUIRE(binder_count(got) == 1);
	const trefs ms = ap::members<node_t>(got);
	REQUIRE(ms.size() == 2);
	const tref frozen = is_child_quantifier<node_t>(ms[0]) ? ms[0] : ms[1];
	const tref resolved = is_child_quantifier<node_t>(ms[0]) ? ms[1] : ms[0];
	REQUIRE(is_child<node_t>(frozen, tau::wff_ex));
	CHECK(same(ap::binder_var<node_t>(frozen), f.P[0]));
	CHECK(ap::members<node_t>(ap::binder_body<node_t>(frozen)).size() == 2);
	// `∃_y (y·b)′·y·c ≠ 0`, i.e. `b′·c ≠ 0`: free of the whole block.
	CHECK(ap::is_negated_equation<node_t>(resolved));
	CHECK(!ap::fv_meets<node_t>(resolved, f.P));
}

TEST_CASE("E11: a block variable hidden in a leaf freezes its component") {
	// A settled sub-block's KEEP emission (§7 DISCHARGE) is a functional
	// quantifier over a stored BDD, and the block variables that body
	// still carries are HIDDEN from then on (§1 leaf hazard): under the
	// live order the chain is ONE LEAF, so no cofactor reaches the `x`
	// inside it and `DISCHARGE` would emit it free. The atom is therefore
	// opaque, and its component is re-wrapped.
	const tref x = tvar("x"), y = tvar("y"), z = tvar("z");
	// The COMPONENT is `{x, y, z}` — the order's keys — while the block
	// still being pushed is `{x, z}`: `y` is the settled sub-block, which
	// the emission below has already quantified.
	const ap::block component{ x, y, z };
	auto c = ap::ctx<node_t>::for_component(component,
		tau_type_id<node_t>(), false);
	const ap::block X{ x, z };
	const tref body = sides(ap::prepare_terms<node_t>(
		eq0(lor(tbf("x"), tbf("y"))), component, c.order)).first;
	REQUIRE(th::is_bdd_backed(body));
	// The constructor wraps a body holding a stored BDD and never folds
	// it, so `x` stays inside, and under the live order the whole chain is
	// one leaf: its subscript `y` is a key, and a chain binding a key is
	// no decision variable of anything built from it.
	tb::quants qs;
	qs.emplace_back(y, tb::all);
	const tref emission = eq0(tb::build_functional_quantifiers(qs, body));
	REQUIRE(ap::fv_meets<node_t>(emission, ap::block{ x }));

	const tref clause = ap::prepare_terms<node_t>(
		conj(emission, conj(eq0(land(tbf("x"), tbf("a"))),
			eq0(land(tbf("z"), tbf("b"))))), component, c.order);
	const trefs conjuncts = ap::members<node_t>(clause);
	REQUIRE(conjuncts.size() == 3);
	// EXACTLY ONE conjunct is opaque, and it is the one holding the hidden
	// `x`: the two plain equations carry their block variables as decision
	// variables, where cofactoring reaches them.
	trefs opaque;
	for (tref m : conjuncts)
		if (ap::atomless_opaque<node_t>(m, X, c.order))
			opaque.push_back(m);
	REQUIRE(opaque.size() == 1);
	CHECK(ap::fv_meets<node_t>(opaque[0], ap::block{ x }));
	CHECK(!ap::fv_meets<node_t>(opaque[0], ap::block{ y }));

	const tref got = ap::eliminate_block<node_t>(clause, X, c);
	// The `x` component — the emission and `x·a = 0` — is re-wrapped, and
	// the `z` component is discharged.
	REQUIRE(binder_count(got) == 1);
	REQUIRE(is_child<node_t>(got, tau::wff_ex));
	CHECK(same(ap::binder_var<node_t>(got), x));
	CHECK(ap::members<node_t>(ap::binder_body<node_t>(got)).size() == 2);
	CHECK(!ap::fv_meets<node_t>(got, ap::block{ z }));
}

// --- the memo ------------------------------------------------------------------------

TEST_CASE("E12: the key is the wrap node and the keep flag") {
	// A clause of its own: the table is global, so a clause another case
	// eliminated would already have an entry.
	fixture f = make("ex x (x m = 0 && !(x n = 0)).");
	const std::pair<tref, bool> key{ ap::rewrap<node_t>(f.clause, f.P),
		false };
	// The table is a cache: it does not exist in Debug, so the answer is
	// the same either way and only the entry is build-type specific.
	CHECK(ap::find<node_t, ap::table::elim_memo>(key) == nullptr);
	const tref got = ap::eliminate_block<node_t>(f.clause, f.P, f.c);
	// `keep_functional` is part of the key, because the entry embeds what
	// `DISCHARGE` emitted and the flag switches that (§1).
	const std::pair<tref, bool> kept{ key.first, true };
	auto kc = ap::ctx<node_t>::for_component(f.P, f.c.type, true);
	const tref kept_got = ap::eliminate_block<node_t>(f.clause, f.P, kc);
	CHECK(!same(kept_got, got));
#ifdef TAU_CACHE
	const tref* hit = ap::find<node_t, ap::table::elim_memo>(key);
	REQUIRE(hit != nullptr);
	CHECK(*hit == got);
	const tref* kept_hit = ap::find<node_t, ap::table::elim_memo>(kept);
	REQUIRE(kept_hit != nullptr);
	CHECK(*kept_hit == kept_got);
#else
	CHECK(ap::find<node_t, ap::table::elim_memo>(key) == nullptr);
	CHECK(ap::find<node_t, ap::table::elim_memo>(kept) == nullptr);
#endif
}

} // TEST_SUITE
