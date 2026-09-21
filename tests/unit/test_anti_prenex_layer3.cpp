// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// THE LAYER-3 MILESTONE: clause-level equivalence against ground truth on the
// atomless method. Spec: anti_prenex.md §7 (ELIMINATE_BLOCK, its pre-steps
// and the type table, ELIMINATE_ATOMLESS_CLAUSE, the shared helpers,
// DISCHARGE), §3 (TRY_WITNESS and its weak pin), §1 (the term
// representation, the leaf hazard, the negative tree).
//
// Every case is ONE CLAUSE through one harness: parse `∃P.ψ`, normalise `ψ`
// the way phases 1 and 3 leave it, put it into §1's term representation and
// eliminate the block. What is claimed of every result: EQUIVALENT to `∃P.ψ`
// (`are_nso_equivalent` on the finished formula, or structurally where a
// reference puts that checker out of reach), no free variable ESCAPED, and
// INVARIANT 4. Per case: the block is RESOLVED (no binder is left) or what
// came back is exactly the RE-WRAP.
//
// The corpus is seeded from the single-clause inputs of the old module's
// suite (tests/unit/test_antiprenexing.cpp), which is the source of the
// INPUT STRINGS and of nothing else — every expectation here is the spec's.
// The rest of the corpus is the shapes no test covers today: a negative over
// several components, several positives each with its own negative, O1
// reached through the squeeze, an order atom, a binder unit and a block
// variable hidden in a leaf frozen, keep mode through DISCHARGE, and §3's
// hidden pin read back by the weak test.
//
// Parsing note: a parsed quantifier's body runs to the RIGHT END and
// juxtaposition is conjunction, so every input keeps its parentheses.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "normalizer.h"
#include "anti_prenex/anti_prenex.h"

#include <algorithm>

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

using tb = tau_term_bdd<node_t>;
using th = term_handle<node_t>;
using order_t = ap::var_order<node_t>;

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

/// The `bf` around a `variable` node, which is what a term holds. A BOUND
/// variable carries the parser's own id (it prints as `b1`, `b2`), so a
/// fixture's block member is never the node a NAME would build: anything
/// compared against one is assembled from the fixture's own nodes.
tref as_bf(tref var) { return tau::get(tau::bf, var); }

/// The named FREE variable of `fm`, as the parser built it.
tref var_of(tref fm, const char* name) {
	for (tref v : get_free_vars<node_t>(fm))
		if (tau::get(v).to_str() == name) return v;
	return nullptr;
}

tref eq0(tref t)          { return build_bf_eq_0<node_t>(t); }
tref land(tref l, tref r) { return build_bf_and<node_t>(l, r); }
tref lneg(tref t)         { return build_bf_neg<node_t>(t); }
tref conj(tref l, tref r) { return tau::build_wff_and(l, r); }

/// A real order atom `l ≤ r`. On a non-bitvector type the construction hooks
/// rewrite `≤` into the equation `l·r′ = 0`, so one does not survive
/// construction; the parser builds with the hooks disabled, and this helper
/// reproduces that shape the same way.
tref order_atom(tref l, tref r) {
	use_hooks_guard<node_t> g(false);
	return tau::build_bf_lteq(l, r);
}

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

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

/// Semantic equality of two TERMS over the variables `vs`: equal ROBDDs
/// under one order over all of them (leaves opaque), both finished first.
bool same_function(tref a, tref b, const ap::block& vs) {
	order_t o;
	for (size_t i = 0; i < vs.size(); ++i)
		o.emplace(vs[i], int_t(vs.size() - i));
	return tb::build_bdd(th::convert_to_tau_terms(a), o)
		== tb::build_bdd(th::convert_to_tau_terms(b), o);
}

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
/// the block, as invariant 2 allows — and the body as phase 4 meets it, put
/// into §1's term representation.
struct fixture {
	tref quantified = nullptr;   ///< `∃P.ψ` as parsed, the oracle's reference
	ap::block P;
	ap::ctx<node_t> c;
	tref clause = nullptr;       ///< `ψ`, normalised and prepared over `P`
};

/// `ψ` as PHASE 4 meets it: in NNF, with `f ≠ 0` spelled `¬(f = 0)` (phases 1
/// and 3, §3). The old suite's inputs are written with `!=`, which invariant
/// 4 forbids past phase 3, so the harness runs the two passes the pipeline
/// would have run before the block is reached.
tref normalised(tref body) {
	return ap::normalize_operators<node_t>(
		ap::to_canonically_factored_nnf<node_t>(body));
}

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
	f.clause = ap::prepare_terms<node_t>(normalised(n), f.P, f.c.order);
	return f;
}

/// THE HARNESS: `∃P.ψ` eliminated exactly as a component's caller does it —
/// the block eliminated, and in KEEP MODE the component's close run over what
/// came back, which resolves every chain `DISCHARGE` emitted (§7). @p emitted
/// takes the result BEFORE that close, where the chains are still there.
tref eliminated(fixture& f, tref* emitted = nullptr) {
	const tref r = ap::eliminate_block<node_t>(f.clause, f.P, f.c);
	if (emitted) *emitted = r;
	if (!f.c.keep_functional) return r;
	return ap::resolve_functional_quantifiers_bdd<node_t>(r, f.c.order,
		ap::keep_no_functional<node_t>);
}

/// What every eliminated clause claims: equivalent to `∃P.ψ`, no free
/// variable escaped, invariant 4.
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

/// `∃P.ψ` re-wrapped whole — what a method that cannot read the clause at all
/// gives back (invariant 3).
tref rewrapped(const fixture& f) {
	return ap::rewrap<node_t>(
		ap::simplified_and_join<node_t>(ap::members<node_t>(f.clause)),
		f.P);
}

} // namespace

TEST_SUITE("anti_prenex/layer3") {

// --- the old suite's single-clause inputs ---------------------------------------------

TEST_CASE("L1: a mixed-sign clause — one positive, one negative") {
	// test_antiprenexing.cpp:189. `∀_x x·y = 0` is `T`, and the one
	// condition is `∃_x (x·y)′·x·w ≠ 0`, i.e. `y′·w ≠ 0`.
	fixture f = make("ex x (xy = 0 && xw != 0).");
	const tref got = eliminated(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(are_nso_equivalent<node_t>(finished(got), parse("!(y' w = 0).")));
}

TEST_CASE("L2: a component with no common zero is F") {
	// test_antiprenexing.cpp:276. `x = 0` and `x′ = 0` are ONE component,
	// squeezed into `x ∪ x′ = 1`, and `∀_x 1 = 0` is `F` — the clause is
	// genuinely unsatisfiable, which is the one place `F` may come out.
	fixture f = make("ex x (x = 0 && x' = 0).");
	const tref got = eliminated(f);
	check_against_source(got, f);
	CHECK(tau::get(got).equals_F());
}

TEST_CASE("L3: a pin is witnessed before any method runs") {
	// test_antiprenexing.cpp:404. `x = w` pins `x` strictly (invariant 8),
	// so pre-step 1 substitutes `w` into the sibling and the strip scopes
	// what is left out of the block: `w·y = 0`.
	fixture f = make("ex x (xy = 0 && x = w).");
	const tref got = eliminated(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(are_nso_equivalent<node_t>(finished(got), parse("w y = 0.")));
}

TEST_CASE("L4: a conjunct the block does not touch rides outside it") {
	// test_antiprenexing.cpp:429. Pre-step 2, `∃X(indep ∧ dep) = indep ∧
	// ∃X.dep`: `w·z = 0` never enters the method, and `∀_x x·y = 0` is `T`.
	fixture f = make("ex x (xy = 0 && wz = 0).");
	const tref got = eliminated(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(are_nso_equivalent<node_t>(finished(got), parse("w z = 0.")));
}

TEST_CASE("L5: two negatives and no positive — O2 for each") {
	// test_antiprenexing.cpp:184. With no positives there is no component,
	// so every negative takes O2 and is discharged on its own: the
	// clause-level instance of 2a.
	fixture f = make("ex x (xy != 0 && xw != 0).");
	const tref got = eliminated(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(are_nso_equivalent<node_t>(finished(got),
		parse("!(y = 0) && !(w = 0).")));
}

TEST_CASE("L6: a lone negative — O2 with X_g = the block") {
	// The ∃ form of test_antiprenexing.cpp:429's `all x xy != 0`, which is
	// what the leaf sees: `∃_x x·y ≠ 0` is `y ≠ 0`.
	fixture f = make("ex x (xy != 0).");
	const tref got = eliminated(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(are_nso_equivalent<node_t>(finished(got), parse("!(y = 0).")));
}

TEST_CASE("L7: a negative tree as the one conjunct") {
	// test_antiprenexing.cpp:169. The disjunction is ONE conjunct (§1) and
	// becomes its TREE_CONDITION — one condition per literal, the `∨`
	// re-assembled by the ∨-join — and what comes back is a negative tree
	// over the free variables.
	fixture f = make("ex x (xy != 0 || xw != 0).");
	const tref got = eliminated(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(ap::is_negative_tree<node_t>(got));
	CHECK(are_nso_equivalent<node_t>(finished(got),
		parse("!(y = 0) || !(w = 0).")));
}

TEST_CASE("L8: a nested negative tree beside a conjunct outside the block") {
	// The all-negative form of test_antiprenexing.cpp:346: `z = 0` is
	// stripped and the tree's `∧` node is assembled by the ∧-join, its `∨`
	// node by the ∨-join.
	fixture f = make("ex x (z = 0 && (xy != 0 || (xw != 0 && xk != 0))).");
	const tref got = eliminated(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(are_nso_equivalent<node_t>(finished(got),
		parse("z = 0 && (!(y = 0) || (!(w = 0) && !(k = 0))).")));
}

TEST_CASE("L8b: a disjunction with a positive leaf is no negative tree") {
	// test_antiprenexing.cpp:346 as it stands. A negative tree is an
	// ∨-node ALL of whose leaves are negated equations (§1), and `x·w = 0`
	// is not one, so the conjunct is neither an equation nor a tree: this
	// engine cannot read it, and its component is re-wrapped. Splitting a
	// disjunction is the push's own work (§5, §6), not a leaf method's.
	fixture f = make("ex x (z = 0 && (xy != 0 || (xw = 0 && xk != 0))).");
	const tref got = eliminated(f);
	check_against_source(got, f);
	CHECK(binder_count(got) == 1);
	// `z = 0` was stripped out of the block before the method ran, and
	// rides beside the re-wrap.
	const trefs ms = ap::members<node_t>(got);
	REQUIRE(ms.size() == 2);
	CHECK(binder_count(ms[0]) + binder_count(ms[1]) == 1);
}

TEST_CASE("L9: two positives squeezed, two negatives — the whole block goes") {
	// test_antiprenexing.cpp:899 (QuantBlockPush/1). `x·y = 0` and `y·x =
	// 0` are one component; each negative absorbs against it, and both
	// conditions hold, so the block resolves to `T`.
	fixture f = make("ex x ex y xy = 0 && yx = 0 && !(x|y = 0) "
		"&& !(x = y).");
	REQUIRE(f.P.size() == 2);
	const tref got = eliminated(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("L10: a reference and the negative sharing its variable freeze") {
	// test_antiprenexing.cpp:1292. This engine answers no queries, so
	// `q(y)` is opaque, and `∃` does not distribute over `∧` across shared
	// variables: the negative is frozen WITH it and the binder comes back
	// around both (invariant 3). The claim is structural — a clause whose
	// component is a lone reference is undecided for the checker.
	fixture f = make("ex y (q(y) && y != 0).");
	const tref got = eliminated(f);
	check_structurally(got, f);
	CHECK(same(got, rewrapped(f)));
	REQUIRE(binder_count(got) == 1);
	CHECK(same(ap::binder_var<node_t>(got), f.P[0]));
	CHECK(ap::members<node_t>(ap::binder_body<node_t>(got)).size() == 2);
}

TEST_CASE("L11: an all-frozen block is re-wrapped whole") {
	// test_antiprenexing.cpp:1666. Two components, each opaque, so nothing
	// is left for the method and what comes back is exactly the re-wrap.
	fixture f = make("ex x, y (f(x) && g(y)).");
	REQUIRE(f.P.size() == 2);
	const tref got = eliminated(f);
	check_against_source(got, f);
	CHECK(same(got, rewrapped(f)));
	CHECK(binder_count(got) == 2);
}

// --- the shapes no test covers today --------------------------------------------------

TEST_CASE("L12: one negative over two components") {
	// `K` is BOTH components and the absorption runs over `X_K ∪ X_g`:
	// `∃_{x,y} (x·a ∪ y·b)′·x·y·c ≠ 0`, i.e. `a′·b′·c ≠ 0`.
	fixture f = make("ex x ex y (x a = 0 && y b = 0 && !(x y c = 0)).");
	REQUIRE(f.P.size() == 2);
	const tref got = eliminated(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(are_nso_equivalent<node_t>(finished(got),
		parse("!(a' b' c = 0).")));
}

TEST_CASE("L13: two components, each with its own negative") {
	// Each condition absorbs against the ONE component its literal touches
	// — the rest of the clause never enters the absorption term.
	fixture f = make("ex x ex y (x a = 0 && !(x c = 0) && y b = 0 "
		"&& !(y d = 0)).");
	REQUIRE(f.P.size() == 2);
	const tref got = eliminated(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(are_nso_equivalent<node_t>(finished(got),
		parse("!(a' c = 0) && !(b' d = 0).")));
}

TEST_CASE("L14: a negative whose zero form is 1 never reaches the method") {
	// O1 (`g ≡ 1` ⇒ `T` under `pos`) is UNREACHABLE from parsed input: the
	// construction hooks decide `x = x′` themselves, so `¬(x = x′)` folds
	// to `T` and is gone from the conjunction before anything here sees
	// it — `ex x (x = 0 && !(x = x'))` is built as `∃x. x = 0`. Were one
	// to survive, SQUEEZE's own `SIMPLIFY_ATOM` on the zero form would
	// drop it there (D5), one step before `NEGATIVE_CONDITION`. What is
	// left is a lone positive, and `∃x. x = 0` is `T`.
	fixture f = make("ex x (x = 0 && !(x = x')).");
	const trefs ms = ap::members<node_t>(f.clause);
	CHECK(ms.size() == 1);
	const ap::squeeze_result<node_t> s = ap::squeeze<node_t>(ms, f.P,
		f.c.order);
	CHECK(s.negatives.empty());
	CHECK(!s.decided_false);
	const tref got = eliminated(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("L15: an order atom freezes its component") {
	// An order atom is no equation: this engine never cofactors one, so it
	// is opaque and `x·a = 0` is frozen with it. The `y` component is
	// untouched by that and is discharged (`∀_y y·b = 0` is `T`). Built by
	// hand: on a `tau` type the construction hooks rewrite `≤` into an
	// equation, and only the parser builds one with the hooks off.
	const ap::block P{ tvar("x"), tvar("y") };
	auto c = ap::ctx<node_t>::for_component(P, tau_type_id<node_t>(),
		false);
	const tref plain = conj(conj(eq0(land(tbf("x"), tbf("a"))),
		order_atom(tbf("x"), tbf("b"))),
		eq0(land(tbf("y"), tbf("b"))));
	const tref clause = ap::prepare_terms<node_t>(plain, P, c.order);
	const tref source = ap::rewrap<node_t>(plain, P);

	const tref got = ap::eliminate_block<node_t>(clause, P, c);
	CHECK(are_nso_equivalent<node_t>(finished(got), source));
	CHECK(no_escape(finished(got), source));
	CHECK(invariant_4(got));
	// ONE binder, over `x` alone, around the two conjuncts of its
	// component.
	REQUIRE(binder_count(got) == 1);
	REQUIRE(is_child<node_t>(got, tau::wff_ex));
	CHECK(same(ap::binder_var<node_t>(got), P[0]));
	CHECK(ap::members<node_t>(ap::binder_body<node_t>(got)).size() == 2);
	CHECK(!ap::fv_meets<node_t>(got, ap::block{ P[1] }));
}

TEST_CASE("L16: a surviving binder is opaque and freezes its component") {
	// A unit is swallowed whole by no BDD engine — it answers no queries —
	// so the inner `∃z` freezes, and `x·a = 0`, which shares `x` with it,
	// freezes too.
	fixture f = make("ex x (x a = 0 && (ex z (x z = 0))).");
	const tref got = eliminated(f);
	check_against_source(got, f);
	CHECK(same(got, rewrapped(f)));
	CHECK(binder_count(got) == 2);
	REQUIRE(is_child<node_t>(got, tau::wff_ex));
	CHECK(same(ap::binder_var<node_t>(got), f.P[0]));
}

TEST_CASE("L17: a block variable hidden in a leaf freezes its component") {
	// `q(x)·c` is a term whose LEAF holds `x` (§1 leaf hazard):
	// quantification here is cofactoring, and a leaf is opaque to it, so
	// `DISCHARGE` would emit that `x` free. The equation is therefore
	// unusable — `LEAF_FV(t) ∩ X ≠ ∅` — and `x·a = 0`, which shares `x`
	// with it, is frozen too.
	fixture f = make("ex x (x a = 0 && (q(x) c) = 0).");
	const trefs ms = ap::members<node_t>(f.clause);
	REQUIRE(ms.size() == 2);
	size_t opaque = 0;
	for (tref m : ms)
		if (ap::atomless_opaque<node_t>(m, f.P, f.c.order)) ++opaque;
	CHECK(opaque == 1);
	const tref got = eliminated(f);
	check_against_source(got, f);
	CHECK(same(got, rewrapped(f)));
	CHECK(binder_count(got) == 1);
}

// --- keep mode ------------------------------------------------------------------------

TEST_CASE("L18: keep mode emits the chain, and the close resolves it") {
	// `DISCHARGE` under `ctx.keep_functional` spells the block instead of
	// solving it: `pos` comes out as `∀_X F = 0` and a condition as
	// `∃_X h ≠ 0`, both as functional-quantifier chains over the stored
	// BDD. The component's close (`RESOLVE_FUNCTIONAL_BDD`) is one
	// quantification per chain, and what it leaves is the non-keep answer.
	for (const char* src : { "ex x (xy = 0 && xw != 0).",
		"ex x (xy != 0 && xw != 0).",
		"ex x (x a = 0 && !(x b = 0))." }) {
		fixture plain = make(src);
		fixture kept = make(src, true);
		const tref want = eliminated(plain);
		tref emitted = nullptr;
		const tref got = eliminated(kept, &emitted);
		// BEFORE the close: a chain of the block's own kind — `∀` for a
		// positive component, `∃` for a negative's absorption term.
		CHECK((holds(emitted, tau::bf_fall) || holds(emitted,
			tau::bf_fex)));
		CHECK(!same(emitted, want));
		// AFTER it: the same answer, with nothing kept.
		check_against_source(got, kept);
		check_resolved(got, kept);
		CHECK(!holds(got, tau::bf_fall));
		CHECK(!holds(got, tau::bf_fex));
		CHECK(are_nso_equivalent<node_t>(finished(got),
			finished(want)));
	}
}

// --- §3's hidden pin ------------------------------------------------------------------

TEST_CASE("L19: the squeeze's hidden pin is read back by the weak test") {
	// A settled sub-block emits `∀_X f = 0` over ALL its positives, so a
	// pin `y = t` among them returns as `(y + t) ∪ G = 0` — cofactors
	// `t ∪ G` and `t′ ∪ G`, complementary only where `G = 0`. The STRICT
	// test misses it; the weak one reads it back, witness `t·G′` and
	// residual `G = 0`, and the witness pass resolves the block with it.
	fixture f = make("ex x ex y (((y ^ a) | (x b)) = 0 && !(y c = 0)).");
	REQUIRE(f.P.size() == 2);
	const tref x = f.P[0], y = f.P[1];
	// The positive's term, `COF`'s key as every pin site forms it.
	const trefs ms = ap::members<node_t>(f.clause);
	tref positive = nullptr;
	for (tref m : ms) if (is_child<node_t>(m, tau::bf_eq)) positive = m;
	REQUIRE(positive != nullptr);
	const tref term = ap::term_of<node_t>(positive, f.c.order);
	const ap::cof_entry e = ap::cof<node_t>(term, y, f.c);
	CHECK(e.usable);
	CHECK(e.pin);
	REQUIRE(e.p != nullptr);
	CHECK(!tau::get(e.p).equals_0());                  // weak, not strict
	const tref a = var_of(f.quantified, "a");
	const tref b = var_of(f.quantified, "b");
	const tref cv = var_of(f.quantified, "c");
	REQUIRE(a != nullptr); REQUIRE(b != nullptr); REQUIRE(cv != nullptr);
	const ap::block vs{ x, y, a, b, cv };
	// the residual is `G = x·b` and the witness is `t·G′ = a·(x·b)′`
	const tref G = land(as_bf(x), as_bf(b));
	CHECK(same_function(e.p, G, vs));
	CHECK(same_function(ap::simplify_term<node_t>(lneg(e.f1), f.c.order),
		land(as_bf(a), lneg(G)), vs));
	// The pass fires on it, and the block resolves: `∃x∃y.((y + a) ∪ x·b =
	// 0 ∧ y·c ≠ 0)` is `a·c ≠ 0`.
	CHECK(ap::try_witness<node_t>(y, f.clause, f.c).has_value());
	const tref got = eliminated(f);
	check_against_source(got, f);
	check_resolved(got, f);
	CHECK(are_nso_equivalent<node_t>(finished(got), parse("!(a c = 0).")));
}

} // TEST_SUITE


