// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for src/anti_prenex/push/push_block.h: §6's memo wrapper — the
// key canonicalisation and the strip — and the dispatcher that sends a
// formula to the class of step its shape belongs to. Spec: anti_prenex.md §6
// (PUSH_BLOCK, PUSH_BLOCK_UNCACHED), §1 (the ctx table, cache scope), §7
// (what the leaves do with a clause), invariants 3 and 8.
//
// Every case is ONE formula through one harness: parse `∃P.ψ`, normalise `ψ`
// the way phases 1 and 3 leave it, put it into §1's term representation and
// push the block into it. What is claimed of every result: EQUIVALENT to
// `∃P.ψ` (`are_nso_equivalent` on the finished formula, or structurally where
// a reference puts that checker out of reach), no free variable ESCAPED, and
// INVARIANT 4. Per case: the block is RESOLVED, or what came back is exactly
// the RE-WRAP.
//
// TYPES: only the atomless types are rows of §7's table, so every fixture
// carries typed variables — a parsed one through inference, a built one by
// spelling the type.
//
// Parsing note: a parsed quantifier's body runs to the RIGHT END and
// juxtaposition is conjunction, so every input keeps its parentheses.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "normalizer.h"
#include "anti_prenex/push/push_block.h"
#include "anti_prenex/normalisers/nnf.h"

#include <algorithm>
#include <utility>

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

using th = term_handle<node_t>;

tref parse(const char* sample) {
	auto rr = get_nso_rr(sample);
	REQUIRE(rr.has_value());
	return rr.value().main->get();
}

/// A `tau`-typed `variable` node — a block member and an order key — and the
/// `bf` around it, which is what a term holds. Only a typed block has a row
/// in §7's table, so a fixture that is built rather than parsed spells the
/// type itself.
tref tvar(const char* name) {
	return tau::build_variable(name, tau_type_id<node_t>());
}
tref tbf(const char* name) {
	return tau::build_bf_variable(name, tau_type_id<node_t>());
}

/// A real order atom `l ≤ r`. On a non-bitvector type the construction hooks
/// rewrite `≤` into the equation `l·r′ = 0`, so one does not survive
/// construction; the parser builds with the hooks disabled, and this helper
/// reproduces that shape the same way.
tref order_atom(tref l, tref r) {
	use_hooks_guard<node_t> g(false);
	return tau::build_bf_lteq(l, r);
}

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

/// A parsed `∃P.ψ` split the way `PUSH_BLOCK`'s callers hand it over: the
/// block OUTERMOST FIRST, the component's ctx over it — `type` read off the
/// block, as invariant 2 allows — and the body as phase 4 meets it, put into
/// §1's term representation.
struct fixture {
	tref quantified = nullptr;   ///< `∃P.ψ` as parsed, the oracle's reference
	ap::block P;
	ap::ctx<node_t> c;
	tref clause = nullptr;       ///< `ψ`, normalised and prepared over `P`
};

/// `ψ` as PHASE 4 meets it: in NNF, with `f ≠ 0` spelled `¬(f = 0)` (phases 1
/// and 3, §3). Inputs written with `!=` would break invariant 4 past phase 3,
/// so the harness runs the two passes the pipeline would have run before the
/// block is reached.
tref normalised(tref body) {
	return ap::normalize_operators<node_t>(
		ap::to_canonically_factored_nnf<node_t>(body));
}

/// @p vars caps how many binders of the chain become the BLOCK, the rest
/// staying part of the body: a nested binder is a UNIT to the push, and the
/// only way to write one is to leave it in the body.
fixture make(const char* src, bool keep = false, size_t vars = 0) {
	fixture f;
	f.quantified = parse(src);
	tref n = f.quantified;
	while (is_child_quantifier<node_t>(n)
		&& (vars == 0 || f.P.size() < vars))
	{
		f.P.push_back(tau::trim_right_sibling(
			ap::binder_var<node_t>(n)));
		n = tau::trim_right_sibling(ap::binder_body<node_t>(n));
	}
	REQUIRE(!f.P.empty());
	f.c = ap::ctx<node_t>::for_component(f.P,
		find_ba_type<node_t>(f.P[0]), keep);
	f.clause = ap::prepare_terms<node_t>(normalised(n), f.P, f.c.order);
	return f;
}

/// THE HARNESS: `∃P.ψ` pushed exactly as a component's caller does it — the
/// block pushed, and in KEEP MODE the component's close run over what came
/// back, which resolves every chain `DISCHARGE` emitted (§5). @p emitted
/// takes the result BEFORE that close, where the chains are still there.
tref pushed(fixture& f, tref* emitted = nullptr) {
	const tref r = ap::push_block<node_t>(f.clause, f.P, f.c);
	if (emitted) *emitted = r;
	if (!f.c.keep_functional) return r;
	return ap::resolve_functional_quantifiers_bdd<node_t>(r, f.c.order,
		ap::keep_no_functional<node_t>);
}

/// What every pushed block claims: equivalent to `∃P.ψ`, no free variable
/// escaped, invariant 4.
void check_against_source(tref got, const fixture& f) {
	CHECK(are_nso_equivalent<node_t>(finished(got), f.quantified));
	CHECK(no_escape(finished(got), f.quantified));
	CHECK(invariant_4(got));
}

/// The same for a result the equivalence checker cannot read: it compares a
/// LONE REFERENCE by signature alone and answers `false` where it is
/// undecided, so such a case claims what it can — no escape, invariant 4 —
/// and says the rest structurally.
void check_structurally(tref got, const fixture& f) {
	CHECK(no_escape(finished(got), f.quantified));
	CHECK(invariant_4(got));
}

/// THE BLOCK IS RESOLVED: no binder came back and no block variable is free
/// in the result.
void check_resolved(tref got, const fixture& f) {
	CHECK(binder_count(got) == 0);
	CHECK(!ap::fv_meets<node_t>(got, f.P));
}

/// `∃P.ψ` re-wrapped whole — what the push gives back for a formula no step
/// reaches (invariant 3). The formula is re-wrapped AS IT IS: the push
/// re-assembles nothing, so a member view taken and joined again would be a
/// different node wherever the prepared atoms sort differently from the plain
/// ones they were built from.
tref rewrapped(const fixture& f) {
	return ap::rewrap<node_t>(f.clause, f.P);
}

} // namespace

TEST_SUITE("anti_prenex/push") {

// --- the wrapper: key canonicalisation ------------------------------------------------

TEST_CASE("P1: a block variable the body does not mention leaves the block") {
	// `X ← X ∩ FV(φ)` (§6): `z` scopes nothing, so it is gone before the
	// key is built and no binder for it can come back. What is left is one
	// atom, which the leaf discharges: `∀_x x = 0` is `0 = 0`.
	fixture f = make("ex x ex z (x = 0).");
	REQUIRE(f.P.size() == 2);
	CHECK(ap::fv_intersect<node_t>(f.clause, f.P).size() == 1);
	const tref got = pushed(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("P2: a formula the block does not scope is the same node") {
	// `X ∩ FV(φ) = ∅`: nothing enters the key that `∃X` does not scope,
	// and the formula comes back untouched.
	fixture f = make("ex x (y z = 0).");
	CHECK(ap::push_block<node_t>(f.clause, f.P, f.c) == f.clause);
}

TEST_CASE("P3: a decided formula comes back as itself") {
	// `T` and `F` have no free variables, so they take the narrowing's
	// own exit and never reach the key.
	fixture f = make("ex x (x y = 0).");
	CHECK(ap::push_block<node_t>(_T<node_t>(), f.P, f.c) == _T<node_t>());
	CHECK(ap::push_block<node_t>(_F<node_t>(), f.P, f.c) == _F<node_t>());
}

TEST_CASE("P4: the strip hoists every X-free conjunct outside the block") {
	// `∃X(indep ∧ dep) = indep ∧ ∃X.dep` (§6): `w·z = 0` rides outside,
	// and what is pushed is `∃x. x·y = 0`, which the leaf discharges to
	// `T`. Nothing about the free variables entered the key.
	fixture f = make("ex x (x y = 0 && w z = 0).");
	const tref got = pushed(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(are_nso_equivalent<node_t>(finished(got), parse("w z = 0.")));
	// `w` and `z` are still free: the strip moves a conjunct, it does not
	// decide it.
	CHECK(holds(got, tau::bf_eq));
	CHECK(get_free_vars<node_t>(finished(got)).size() == 2);
}

TEST_CASE("P5: the key is the wrap node, and the entry is the result") {
	// A formula of its own: the table is global, so one another case
	// pushed would already have an entry.
	fixture f = make("ex x (x m = 0 && !(x n = 0)).");
	const std::pair<tref, bool> key{ ap::rewrap<node_t>(f.clause, f.P),
		false };
	// The table is a cache: it does not exist in Debug, so the answer is
	// the same either way and only the entry is build-type specific.
	CHECK(ap::find<node_t, ap::table::push_memo>(key) == nullptr);
	const tref got = pushed(f);
	check_against_source(got, f);
#ifdef TAU_CACHE
	const tref* hit = ap::find<node_t, ap::table::push_memo>(key);
	REQUIRE(hit != nullptr);
	CHECK(*hit == got);
#else
	CHECK(ap::find<node_t, ap::table::push_memo>(key) == nullptr);
#endif
}

TEST_CASE("P6: keep_functional is part of the key") {
	// The entry embeds what `DISCHARGE` emitted, and the flag switches
	// that (§1): the same formula under the two flags gives two results
	// and, where the table exists, two entries.
	fixture f = make("ex x (x p = 0).");
	const std::pair<tref, bool> key{ ap::rewrap<node_t>(f.clause, f.P),
		false };
	const std::pair<tref, bool> kept{ key.first, true };
	auto kc = ap::ctx<node_t>::for_component(f.P, f.c.type, true);
	const tref got = ap::push_block<node_t>(f.clause, f.P, f.c);
	const tref kept_got = ap::push_block<node_t>(f.clause, f.P, kc);
	CHECK(!same(kept_got, got));
#ifdef TAU_CACHE
	const tref* hit = ap::find<node_t, ap::table::push_memo>(key);
	const tref* kept_hit = ap::find<node_t, ap::table::push_memo>(kept);
	REQUIRE(hit != nullptr);
	REQUIRE(kept_hit != nullptr);
	CHECK(*hit == got);
	CHECK(*kept_hit == kept_got);
#else
	CHECK(ap::find<node_t, ap::table::push_memo>(key) == nullptr);
	CHECK(ap::find<node_t, ap::table::push_memo>(kept) == nullptr);
#endif
}

// --- the dispatcher: what goes to a leaf ----------------------------------------------

TEST_CASE("P7: an atom is a one-literal clause and resolves") {
	// `∀_x x·y = 0` is `0 = 0`.
	fixture f = make("ex x (x y = 0).");
	const tref got = pushed(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("P8: a negated atom is a one-literal clause and resolves") {
	// `¬(x·y = 0)` is what phase 3 leaves of `x·y ≠ 0` (invariant 4), and
	// `∃_x x·y ≠ 0` is `y ≠ 0`.
	fixture f = make("ex x (x y != 0).");
	const tref got = pushed(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(are_nso_equivalent<node_t>(finished(got), parse("!(y = 0).")));
}

TEST_CASE("P9: a negative tree is taken whole by the leaf") {
	// An ∨-node all of whose leaves are negated equations is ONE conjunct
	// (§1), classified ahead of the disjunction arm and handed over whole
	// rather than per member.
	fixture f = make("ex x (x y != 0 || x w != 0).");
	REQUIRE(ap::is_negative_tree<node_t>(f.clause));
	const tref got = pushed(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(are_nso_equivalent<node_t>(finished(got),
		parse("!(y = 0) || !(w = 0).")));
}

TEST_CASE("P10: a binder unit is a one-unit clause, and freezes") {
	// The unit goes to the leaf (§7), where this engine can swallow it
	// into no query, so what comes back is the method's freeze — exactly
	// the re-wrap the dispatcher's last line would have built.
	fixture f = make("ex x (ex z (x z = 0)).", false, 1);
	REQUIRE(f.P.size() == 1);
	REQUIRE(is_child_quantifier<node_t>(f.clause));
	const tref got = pushed(f);
	check_against_source(got, f);
	CHECK(same(got, rewrapped(f)));
	CHECK(binder_count(got) == 2);
}

// --- the dispatcher: what re-wraps ----------------------------------------------------

TEST_CASE("P11: a plain clause re-wraps") {
	// The push over a junction is not part of the module: the block comes
	// back around the conjunction, undecided (invariant 3).
	fixture f = make("ex x (x y = 0 && x w != 0).");
	const tref got = pushed(f);
	check_against_source(got, f);
	CHECK(same(got, rewrapped(f)));
	CHECK(binder_count(got) == 1);
	CHECK(same(ap::binder_var<node_t>(got), f.P[0]));
}

TEST_CASE("P12: a clause holding a disjunctive conjunct re-wraps") {
	fixture f = make("ex x (x y = 0 && (x w = 0 || x z = 0)).");
	const tref got = pushed(f);
	check_against_source(got, f);
	CHECK(same(got, rewrapped(f)));
	CHECK(binder_count(got) == 1);
}

TEST_CASE("P13: a clause whose only ∨-member is a negative tree re-wraps") {
	// The negative-tree arm reads the WHOLE formula, not a member of it:
	// a conjunction holding one is a conjunction.
	fixture f = make("ex x (x y = 0 && (x w != 0 || x z != 0)).");
	REQUIRE(!ap::is_negative_tree<node_t>(f.clause));
	const tref got = pushed(f);
	check_against_source(got, f);
	CHECK(same(got, rewrapped(f)));
	CHECK(binder_count(got) == 1);
}

TEST_CASE("P14: a clause is stripped, and what is left re-wraps") {
	// The two halves of the wrapper in one formula: `w = 0` rides outside
	// the block, and the dependent conjunction — which no step of this
	// module pushes — comes back re-wrapped beside it.
	fixture f = make("ex x (x y = 0 && x w != 0 && w = 0).");
	trefs indep, dep;
	for (tref m : ap::members<node_t>(f.clause))
		(ap::fv_meets<node_t>(m, f.P) ? dep : indep).push_back(m);
	REQUIRE(indep.size() == 1);
	REQUIRE(dep.size() == 2);
	indep.push_back(ap::rewrap<node_t>(
		ap::simplified_and_join<node_t>(dep), f.P));
	const tref got = pushed(f);
	check_against_source(got, f);
	CHECK(same(got, ap::simplified_and_join<node_t>(indep)));
	// ONE binder, over `x` alone, around the two dependent conjuncts.
	REQUIRE(binder_count(got) == 1);
	const trefs ms = ap::members<node_t>(got);
	REQUIRE(ms.size() == 2);
	CHECK(binder_count(ms[0]) + binder_count(ms[1]) == 1);
}

TEST_CASE("P15: a disjunction re-wraps") {
	// Not a negative tree — `x·w = 0` is a positive leaf — so it is the
	// disjunction arm that answers, with the same re-wrap.
	fixture f = make("ex x (x y = 0 || x w = 0).");
	REQUIRE(is_child<node_t>(f.clause, tau::wff_or));
	const tref got = pushed(f);
	check_against_source(got, f);
	CHECK(same(got, rewrapped(f)));
	CHECK(binder_count(got) == 1);
}

TEST_CASE("P16: a reference re-wraps") {
	// A reference is no atom and no junction: it is pushed as far as it
	// goes, which is nowhere (invariant 3). The claim is structural — the
	// equivalence checker compares a lone reference by signature alone.
	fixture f = make("ex x (q(x)).");
	const tref got = pushed(f);
	check_structurally(got, f);
	CHECK(same(got, rewrapped(f)));
	REQUIRE(binder_count(got) == 1);
	CHECK(same(ap::binder_var<node_t>(got), f.P[0]));
}

TEST_CASE("P17: an order atom is a literal, and comes back re-wrapped") {
	// An order atom is a literal, so it goes to the leaf; no method there
	// reads one, and the freeze gives back exactly the re-wrap the
	// dispatcher's last line would have built. Built by hand: on a `tau`
	// type the construction hooks rewrite `≤` into an equation, and only
	// the parser builds one with the hooks off.
	const ap::block P{ tvar("x") };
	auto c = ap::ctx<node_t>::for_component(P, tau_type_id<node_t>(),
		false);
	const tref plain = order_atom(tbf("x"), tbf("b"));
	const tref clause = ap::prepare_terms<node_t>(plain, P, c.order);
	const tref source = ap::rewrap<node_t>(plain, P);
	REQUIRE(ap::is_literal<node_t>(clause));

	const tref got = ap::push_block<node_t>(clause, P, c);
	CHECK(are_nso_equivalent<node_t>(finished(got), source));
	CHECK(no_escape(finished(got), source));
	CHECK(invariant_4(got));
	CHECK(same(got, ap::rewrap<node_t>(clause, P)));
}

// --- a narrowed block under the component's order -------------------------------------

TEST_CASE("P18: a sub-block is pushed under the component's full order") {
	// A step may push a block narrower than the component's, and the live
	// order stays the component's whatever the block is — the terms are
	// backed by it. Here `∃x` alone: `x·a = 0` is stripped from `y·b = 0`
	// and discharged, and `y` stays free.
	fixture f = make("ex x ex y (x a = 0 && y b = 0).");
	REQUIRE(f.P.size() == 2);
	const ap::block sub{ f.P[0] };
	const tref source = ap::rewrap<node_t>(f.clause, sub);
	const tref got = ap::push_block<node_t>(f.clause, sub, f.c);
	CHECK(are_nso_equivalent<node_t>(finished(got), finished(source)));
	CHECK(invariant_4(got));
	CHECK(binder_count(got) == 0);
	CHECK(!ap::fv_meets<node_t>(got, sub));
	CHECK(ap::fv_meets<node_t>(got, ap::block{ f.P[1] }));
}

// --- keep mode ------------------------------------------------------------------------

TEST_CASE("P19: keep mode emits the chain, and the close resolves it") {
	// `DISCHARGE` under `ctx.keep_functional` spells the block instead of
	// solving it: the one-literal clause comes back as `∀_x x·y = 0`, a
	// functional-quantifier chain over the stored BDD. The component's
	// close is one quantification of it, and what it leaves is the
	// non-keep answer.
	fixture plain = make("ex x (x y = 0).");
	fixture kept = make("ex x (x y = 0).", true);
	const tref want = pushed(plain);
	tref emitted = nullptr;
	const tref got = pushed(kept, &emitted);
	// BEFORE the close: one atom whose term is a chain.
	REQUIRE(is_child<node_t>(emitted, tau::bf_eq));
	CHECK(!ap::strip_chain<node_t>(sides(emitted).first).first.empty());
	CHECK(!same(emitted, want));
	// AFTER it: the same answer, with nothing kept.
	check_against_source(got, kept);
	check_resolved(got, kept);
	CHECK(!holds(got, tau::bf_fall));
	CHECK(!holds(got, tau::bf_fex));
	CHECK(same(got, want));
}

} // TEST_SUITE
