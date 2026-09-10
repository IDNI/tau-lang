// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Layer-0 INTEGRATION test of src/anti_prenex/foundations/ — packages A (dag),
// B (terms), C (subst), D (prims) and E (ctx) working together. Plan:
// the layer-0 plan (kept outside the repository) §4 "Integration"; spec: anti_prenex.md §1 (|·|,
// FV, the term representation), §3 (primitives, phase 0), §5 (PUSH_EX_BLOCK's
// ctx fill), §10 (per-node caching). Every case crosses at least two packages;
// the five package tests are complemented, never duplicated.
//
// The milestone's four claims and how each is observed — through table state
// (ctx.h's `find` on the unconditional tables), node identity and a counting
// callback, never through timing:
//  1. sizes and FV lazily filled, never recomputed: nothing is published at
//     construction, one query measures every wrapper, a facet is stable
//     across queries and agrees with the value that fed the enclosing
//     formula; `fv` is right on module-built nodes and its reference is
//     stable. Both build types: the structural tables are unconditional.
//  2. cofactors are children: the BDD child at the top, the compose oracle
//     below it, commutation as nodes, a P-free cofactor plain and out of the
//     P-touching set. Both build types.
//  3. substitution capture-safe and untouched-by-one-FV-test: on the
//     parser's canonical ids, with the reach `atoms` and `fv` promise; the
//     F5 shape (a witness carrying a functional quantifier) pinned as
//     EXPECTED, repaired by phase 5's canonicalisation (ruling 5, Sep 10
//     2026). Both build types.
//  4. every pass caches per node: table state after one query, and the
//     `simplify_formula_fn` count on a shared subtree — one memo per
//     rewrite, per call, unconditional (plan §1 rule 2). Both build types.
//     No layer-0 function writes a gated table, so the milestone's
//     `#ifdef TAU_CACHE` clause applies from layer 3's `cof_memo` on; the one
//     Release-only case here pins that table's key identity on a real
//     (BDD_ID term, x) pair (Lucca, Sep 10 2026).
//
// Conventions (the brief): a chain member carries a right sibling, so every
// comparison against a separately built node is by CONTENT; `fv` stores
// trimmed nodes; a "no row yet" check runs on FRESH content only (hash
// consing would otherwise find the row an earlier case filled); prepared
// chains are compared as member SETS, since the content order among
// BDD-backed atoms follows the interning order of their BDDs (deterministic
// within a process, not a function of content across processes); members
// are located by a predicate, never by position; never assert on to_str()
// of a BDD-backed term.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "anti_prenex/foundations/dag.h"
#include "anti_prenex/foundations/terms.h"
#include "anti_prenex/foundations/prims.h"
#include "anti_prenex/foundations/ctx.h"
#include "anti_prenex/foundations/subst.h"

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

using tb      = tau_term_bdd<node_t>;
using th      = term_handle<node_t>;
using order_t = ap::var_order<node_t>;

// --- mirrored from tests/unit/test_anti_prenex_terms.cpp ----------------------

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

/// §5's order for a block, built the way PUSH_EX_BLOCK builds it (package E):
/// the integration test's replacement for the terms test's local `order_of`.
order_t order_for(const ap::block& P) {
	return ap::ctx<node_t>::for_component(P, 0, false).order;
}

/// The BDD ref of a term under `o` (a BDD-backed term's own ref).
tb::ref ref_of(tref t, const order_t& o) {
	if (ap::is_bdd_backed<node_t>(t))
		return th::U.find(tau::trim_right_sibling(t))->second.get();
	return tb::build_bdd(t, o);
}

/// Semantic equality of two terms over the variables `vs`: equal ROBDDs
/// under one order over all of them (leaves opaque). Both are finished
/// first, since a BDD-backed term's own ref belongs to its own order.
bool same_function(tref a, tref b, const ap::block& vs) {
	order_t o = order_for(vs);
	return tb::build_bdd(ap::finish_terms<node_t>(a), o)
		== tb::build_bdd(ap::finish_terms<node_t>(b), o);
}

/// The two sides of an `l = r` atom, trimmed.
std::pair<tref, tref> sides(tref atom) {
	const tau& t = tau::get(atom);
	REQUIRE(t.is(tau::wff));
	REQUIRE(t[0].has_child());
	return { tau::trim_right_sibling(t[0].first()),
		tau::trim_right_sibling(t[0].second()) };
}

bool has_bdd_id(tref n) {
	return tau::get(n).find_top([](tref m) {
		return tree<node_t>::get(m).is(tau::BDD_ID); }) != nullptr;
}

// --- mirrored from tests/unit/test_anti_prenex_dag.cpp ------------------------

/// A formula-level variable, for a binder (type 0, like the builder atoms).
tref fvar(const char* name) { return tau::build_variable(name, 0); }
tref neg(tref n) { return tau::build_wff_neg(n); }
bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

// --- mirrored from tests/unit/test_anti_prenex_ctx.cpp ------------------------

/// A distinct formula per name: `x_<name> = 0`, the shape §1 calls an atom.
tref mk_atom(const std::string& name) {
	return tau::build_bf_eq_0(tau::build_bf_variable("x_" + name, 0));
}

// --- mirrored from tests/unit/test_anti_prenex_subst.cpp ----------------------

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

// --- this file's own -----------------------------------------------------------

/// The atom `<v> & <w> = 0` over builder variables (type 0), so that the
/// block variable `fvar(v)` is free in it and the content is fresh per name.
tref atom2(const char* v, const char* w) {
	return tau::build_bf_eq_0(tau::build_bf_and(
		tau::build_bf_variable(v, 0), tau::build_bf_variable(w, 0)));
}

/// Membership in a set `fv` hands out (sorted by `subtree_less`).
bool has(const trefs& sorted, tref v) {
	return std::binary_search(sorted.begin(), sorted.end(), v,
		tau::subtree_less);
}

/// The member of `chain` touching every variable of `yes` and none of `no`.
tref member_touching(tref chain, const ap::block& yes, const ap::block& no) {
	return member_where(chain, [&](tref m) {
		for (tref v : yes) if (!ap::fv_meets<node_t>(m, { v })) return false;
		return !ap::fv_meets<node_t>(m, no);
	});
}

/// The equation member of `spine` holding `x` — a pin's shape.
tref pin_on(tref spine, tref x) {
	return member_where(spine, [x](tref m) {
		return ap::is_equation<node_t>(m) && ap::fv_meets<node_t>(m, { x });
	});
}

/// A reference `r(<x>, <psi>)` with a FORMULA second argument: the one shape
/// that reaches `simplify_formula_fn`, which is how a rewrite's per-node
/// caching is counted from outside (Lucca, Sep 10 2026). The grammar's
/// `ref_arg` is `bf` only, so it is built, not parsed; `build_ref_args`
/// wraps any node in a `ref_arg`.
tref ref_with_formula_arg(tref x_term, tref psi) {
	return tau::build_ref("r", trefs{ x_term, psi });
}

const size_t* size_row(tref n) {
	return ap::find<node_t, ap::table::size_memo>(n);
}

/// Fill rows for `n` fresh atoms in BOTH structural tables a facet reference
/// points into (`size_memo`, `get_free_vars`' own), so the maps rehash.
void churn(const char* prefix, int n) {
	for (int i = 0; i < n; ++i) {
		tref a = mk_atom(prefix + std::to_string(i));
		(void) ap::formula_size<node_t>(a);
		(void) ap::fv<node_t>(a);
	}
}

} // namespace

TEST_SUITE("anti_prenex/foundations/layer0") {

// --- claim 1: sizes and FV lazily filled, never recomputed ------------------------

TEST_CASE("|·|: nothing published at construction, everything measured by one query") {
	// FRESH content — these names occur nowhere else in this file, so no
	// earlier case can have filled a row for any node built here.
	tref a = atom2("l0_p1x", "l0_p1a"), b = atom2("l0_p1y", "l0_p1b");
	tref c = atom2("l0_p1x", "l0_p1c");
	tref x = fvar("l0_p1x"), y = fvar("l0_p1y");
	// A's raw constructor and D's rewrap publish nothing (plan §3 A, D).
	tref chain   = ap::canonical_and<node_t>(trefs{ a, b, c });
	tref wrapped = ap::rewrap<node_t>(chain, { x, y });
	tref inner   = ap::binder_body<node_t>(wrapped);
	REQUIRE(ap::fv_meets<node_t>(chain, { x, y }));
	CHECK(size_row(a) == nullptr);
	CHECK(size_row(chain) == nullptr);
	CHECK(size_row(inner) == nullptr);
	CHECK(size_row(wrapped) == nullptr);
	// One query: |∃x∃y chain| = 2 + (k−1 + Σ|members|) with k = 3.
	CHECK(ap::formula_size<node_t>(wrapped) == 7);
	// Every wrapper below has its row now, holding the value that fed the
	// whole: the inner binder, the chain, the chain's spine node (looked up
	// by content: it carries the third member as its right sibling) and an
	// atom.
	const size_t* top = size_row(wrapped);
	REQUIRE(top != nullptr);
	CHECK(*top == 7);
	REQUIRE(size_row(inner) != nullptr);
	CHECK(*size_row(inner) == 6);
	REQUIRE(size_row(chain) != nullptr);
	CHECK(*size_row(chain) == 5);
	tref spine = tau::get(chain)[0].first();
	REQUIRE(size_row(spine) != nullptr);
	CHECK(*size_row(spine) == 3);
	REQUIRE(size_row(a) != nullptr);
	CHECK(*size_row(a) == 1);
	// Stable across queries: the same row, the same value.
	CHECK(ap::formula_size<node_t>(wrapped) == 7);
	CHECK(size_row(wrapped) == top);
	CHECK(*top == 7);
	// A later constructor over measured parts still publishes nothing for
	// the node it returns, and that node's first query composes the parts.
	tref more = ap::canonical_or<node_t>(trefs{ wrapped, neg(a) });
	CHECK(size_row(more) == nullptr);
	CHECK(ap::formula_size<node_t>(more) == 1 + 7 + 2);
	// D's fold on a block that is used leaves the node alone.
	CHECK(ap::fold_degenerate_binders<node_t>(wrapped) == wrapped);
}

TEST_CASE("FV: right on module-built nodes, by reference, stable across construction") {
	tref x = vr("x"), y = vr("y"), z = vr("z");
	tref a = vr("a"), b = vr("b"), c = vr("c");
	tref phi = ap::canonical_and<node_t>(trefs{
		wff("x & a = 0"), wff("y & b = 0"), wff("c = 0") });
	const trefs& fv1 = ap::fv<node_t>(phi);
	CHECK(fv1.size() == 5);
	CHECK(std::is_sorted(fv1.begin(), fv1.end(), tau::subtree_less));
	for (tref v : { x, y, a, b, c }) CHECK(has(fv1, v));
	CHECK(&ap::fv<node_t>(phi) == &fv1);             // one row, handed out by reference
	// The chain's set is the union of its members' sets (each its own row).
	subtree_set<node_t> u;
	for (tref m : ap::members<node_t>(phi)) {
		const trefs& mv = ap::fv<node_t>(m);
		u.insert(mv.begin(), mv.end());
	}
	CHECK(u.size() == 5);
	for (tref v : fv1) CHECK(u.contains(v));
	// rewrap removes exactly X, and the rewrapped node's set is its own.
	tref r = ap::rewrap<node_t>(phi, { x, y });
	const trefs& fvr = ap::fv<node_t>(r);
	CHECK(fvr.size() == 3);
	CHECK(!ap::fv_meets<node_t>(r, { x, y }));
	CHECK(ap::fv_meets<node_t>(r, { z, c }));
	CHECK(!ap::fv_meets<node_t>(r, { z }));
	CHECK(ap::fv<node_t>(ap::binder_body<node_t>(r)).size() == 4);   // ∃y φ: x is free again
	// fv_intersect keeps X's order (the key narrowing of §6/§7) and returns
	// trimmed nodes — the same nodes `vr` gives.
	ap::block got = ap::fv_intersect<node_t>(phi, { y, z, x });
	REQUIRE(got.size() == 2);
	CHECK(got[0] == y);
	CHECK(got[1] == x);
	CHECK(ap::fv_intersect<node_t>(r, { y, z, x }).empty());
	// LIFETIME (ctx.h): filling rows for unrelated nodes in both tables
	// (rehashes) moves no row.
	churn("fv_churn_", 300);
	CHECK(&ap::fv<node_t>(phi) == &fv1);
	CHECK(fv1.size() == 5);
	CHECK(&ap::fv<node_t>(r) == &fvr);
}

// --- §5's setup across packages: parsed binders → ctx → prepare_terms ---------------

TEST_CASE("a parsed block through ctx into prepare_terms: inner on top, |·| and FV across D2, ids kept") {
	tref outer = wff("ex x ex y (x & y & a = 0 && x & b = 0 && c = 0)");
	REQUIRE(ap::is_binder<node_t>(outer));
	// The block read off the binders, outermost first (§1), in the spelling
	// the binder holds: the variable carrying the body as its right sibling.
	tref x = ap::binder_var<node_t>(outer);
	tref inner = ap::binder_body<node_t>(outer);
	tref y = ap::binder_var<node_t>(inner);
	tref body = ap::binder_body<node_t>(inner);
	ap::block P{ x, y };
	auto c = ap::ctx<node_t>::for_component(P, find_ba_type<node_t>(bf("a")), false);
	// E's order accepts that spelling and the trimmed one alike: the map is
	// content-keyed (tau_bdd.h), so B's lookups by the term's own variable
	// nodes hit. Inner → LOWER rank (§5).
	CHECK(c.order.contains(x));
	CHECK(c.order.contains(tau::trim_right_sibling(x)));
	CHECK(c.order.at(x) == 2);
	CHECK(c.order.at(y) == 1);
	tref prepared = ap::prepare_terms<node_t>(body, P, c.order);
	CHECK(prepared != body);
	REQUIRE(ap::members<node_t>(prepared).size() == 3);
	// The atom touching both: y (inner, rank 1) on top, x below it.
	tref both = member_touching(prepared, { x, y }, {});
	REQUIRE(both != nullptr);
	tref l0 = sides(both).first;
	REQUIRE(ap::is_bdd_backed<node_t>(l0));
	tb::ref f0 = ref_of(l0, c.order);
	REQUIRE(!tb::leaf(f0));
	CHECK(same(tb::get_var(f0), y));
	CHECK(same(tb::get_var(tb::get_high(f0)), x));
	// The atom touching x alone: x on top.
	tref only_x = member_touching(prepared, { x }, { y });
	REQUIRE(only_x != nullptr);
	tref l1 = sides(only_x).first;
	REQUIRE(ap::is_bdd_backed<node_t>(l1));
	CHECK(same(tb::get_var(ref_of(l1, c.order)), x));
	// The P-free atom: untouched, by content the parsed node.
	tref free = member_touching(prepared, {}, P);
	REQUIRE(free != nullptr);
	CHECK(same(free, wff("c = 0")));
	CHECK(!has_bdd_id(free));
	// |·| and FV are invariant across D2's boundary: an atom counts 1
	// whatever its terms, and `fv` reads the BDD leaves and decision
	// variables alike.
	CHECK(ap::formula_size<node_t>(body) == 5);
	CHECK(ap::formula_size<node_t>(prepared) == 5);
	const trefs& fvp = ap::fv<node_t>(prepared);
	const trefs& fvb = ap::fv<node_t>(body);
	REQUIRE(fvp.size() == fvb.size());
	for (size_t i = 0; i < fvp.size(); ++i) CHECK(same(fvp[i], fvb[i]));
	CHECK(ap::fv_meets<node_t>(prepared, P));
	// Re-wrapped over P (the graceful exit of inv. 3): FV loses P, |·|
	// gains |P|, the binders keep their ids and order.
	tref r = ap::rewrap<node_t>(prepared, P);
	CHECK(!ap::fv_meets<node_t>(r, P));
	CHECK(ap::formula_size<node_t>(r) == 7);
	CHECK(same(ap::binder_var<node_t>(r), x));
	CHECK(same(ap::binder_var<node_t>(ap::binder_body<node_t>(r)), y));
	// finish and rewrap commute as nodes; the D2 round trip is the identity;
	// the parser's ids survive the whole pipeline (ground rule 4).
	tref finished = ap::finish_terms<node_t>(r);
	CHECK(!has_bdd_id(finished));
	CHECK(finished == ap::rewrap<node_t>(ap::finish_terms<node_t>(prepared), P));
	CHECK(ap::prepare_terms<node_t>(ap::finish_terms<node_t>(prepared), P, c.order)
		== prepared);
	CHECK(ap::canonicalise_binder_ids<node_t>(finished) == finished);
}

// --- claim 2: cofactors are children ------------------------------------------------

TEST_CASE("cofactors under ctx's order are children; a P-free cofactor is plain and leaves the P-touching set") {
	tref x = vr("x"), y = vr("y"), a = vr("a"), b = vr("b");
	ap::block P{ x, y };
	order_t o = order_for(P);
	tref atom = ap::prepare_terms<node_t>(wff("x & y & a | x' & b = 0"), P, o);
	tref l = sides(atom).first;
	REQUIRE(ap::is_bdd_backed<node_t>(l));
	tb::ref f = ref_of(l, o);
	REQUIRE(!tb::leaf(f));
	REQUIRE(same(tb::get_var(f), y));                   // inner on top
	// The top variable: child selection.
	tref fy1 = ap::cofactor<node_t>(l, y, true, o);
	tref fy0 = ap::cofactor<node_t>(l, y, false, o);
	CHECK(ref_of(fy1, o) == tb::get_high(f));
	CHECK(ref_of(fy0, o) == tb::get_low(f));
	// Below the top: the compose oracle.
	tref fx1 = ap::cofactor<node_t>(l, x, true, o);
	tref fx0 = ap::cofactor<node_t>(l, x, false, o);
	CHECK(ref_of(fx1, o) == tb::bdd_compose(f, x, tb::T, o));
	CHECK(ref_of(fx0, o) == tb::bdd_compose(f, x, tb::F, o));
	// Child selection commutes: the same node either way round.
	CHECK(ap::cofactor<node_t>(fy1, x, true, o) == ap::cofactor<node_t>(fx1, y, true, o));
	CHECK(ap::cofactor<node_t>(fy0, x, false, o) == ap::cofactor<node_t>(fx0, y, false, o));
	// Fully cofactored: plain, and the atom over it is out of the P-touching
	// set (§5's `dep` selection) while still counting 1.
	tref g = ap::cofactor<node_t>(fy1, x, true, o);      // x = y = 1: a
	CHECK(!ap::is_bdd_backed<node_t>(g));
	CHECK(same(g, bf("a")));
	tref ga = build_bf_eq_0<node_t>(g);
	CHECK(ap::formula_size<node_t>(ga) == 1);
	CHECK(!ap::fv_meets<node_t>(ga, P));
	CHECK(ap::fv<node_t>(ga).size() == 1);
	// Partially cofactored: still backed on x, in the set, y gone.
	CHECK(ap::is_bdd_backed<node_t>(fy1));
	tref hb = build_bf_eq_0<node_t>(fy1);
	CHECK(ap::formula_size<node_t>(hb) == 1);
	CHECK(ap::fv_meets<node_t>(hb, P));
	CHECK(ap::fv_meets<node_t>(hb, { x }));
	CHECK(!ap::fv_meets<node_t>(hb, { y }));
	CHECK(same_function(fy1, bf("x & a | x' & b"), { x, a, b }));
	// The §1 classification is invariant across D2's boundary.
	CHECK(ap::is_atom<node_t>(atom));
	CHECK(ap::is_equation<node_t>(atom));
	CHECK(ap::is_literal<node_t>(atom));
	CHECK(!ap::is_order_atom<node_t>(atom));
	CHECK(ap::is_plain_conjunct<node_t>(atom));
	CHECK(ap::is_negated_equation<node_t>(neg(atom)));
	CHECK(ap::is_literal<node_t>(neg(atom)));
	tref other = ap::prepare_terms<node_t>(wff("x & b = 0"), P, o);
	tref tree = ap::canonical_or<node_t>(trefs{ neg(atom), neg(other) });
	CHECK(ap::is_negative_tree<node_t>(tree));
	CHECK(ap::is_flat_tree<node_t>(tree));
	CHECK(ap::is_disjunctive_conjunct<node_t>(tree));
	CHECK(!ap::is_negative_tree<node_t>(ap::canonical_or<node_t>(trefs{ neg(atom), other })));
}

#ifdef TAU_CACHE
TEST_CASE("cof_memo: one row for the trimmed and the untrimmed spelling of a BDD-backed key (Release)") {
	// The gated table exists here alone (plan §1 rule 2). Its key is
	// (settled term, x) with the term a `BDD_ID` node minted by B: both
	// halves are hashed by content (ctx.h `tref_pair_hash`), so the spelling
	// a caller reads off an atom or a binder — carrying a right sibling —
	// lands on the row the trimmed spelling wrote.
	tref x = vr("x"), y = vr("y"), a = vr("a");
	ap::block P{ x, y };
	order_t o = order_for(P);
	tref atom = ap::prepare_terms<node_t>(wff("x & y & a = 0"), P, o);
	tref l = sides(atom).first;                          // trimmed
	tref l_in_place = tau::get(atom)[0].first();         // carries the right side
	REQUIRE(ap::is_bdd_backed<node_t>(l));
	REQUIRE(l_in_place != l);
	REQUIRE(same(l_in_place, l));
	tref x_bound = ap::binder_var<node_t>(build_wff_ex<node_t>(x, atom, false));
	REQUIRE(x_bound != x);
	REQUIRE(same(x_bound, x));
	size_t calls = 0;
	auto compute = [&] {
		++calls;
		return ap::cof_entry{ ap::cofactor<node_t>(l, x, false, o),
			ap::cofactor<node_t>(l, x, true, o), nullptr, true, false };
	};
	ap::cof_entry e1 = ap::memoised<node_t, ap::table::cof_memo>({ l, x }, compute);
	CHECK(calls == 1);
	ap::cof_entry e2 = ap::memoised<node_t, ap::table::cof_memo>({ l_in_place, x_bound }, compute);
	CHECK(calls == 1);
	CHECK(e2.f0 == e1.f0);
	CHECK(e2.f1 == e1.f1);
	CHECK(ap::find<node_t, ap::table::cof_memo>({ l_in_place, x_bound }) != nullptr);
	// A different variable is a different row.
	ap::memoised<node_t, ap::table::cof_memo>({ l, y }, compute);
	CHECK(calls == 2);
	// And the row holds the children (claim 2 through the table).
	CHECK(same_function(e1.f1, bf("y & a"), { y, a }));
	CHECK(!ap::is_bdd_backed<node_t>(e1.f0));
}
#endif // TAU_CACHE

// --- D on A: the D3 key and the graceful exit -----------------------------------------

TEST_CASE("rewrap over fv_intersect is the D3 key; fold cleans a re-wrapped lost variable") {
	tref x = vr("x"), y = vr("y"), z = vr("z");
	tref phi = wff("x & a = 0 && y & b = 0");
	tref xy = ap::rewrap<node_t>(phi, { x, y });
	tref yx = ap::rewrap<node_t>(phi, { y, x });
	CHECK(xy != yx);
	CHECK(ap::rewrap<node_t>(phi, ap::block{ x, y }) == xy);
	CHECK(ap::formula_size<node_t>(xy) == 5);
	CHECK(ap::formula_size<node_t>(yx) == 5);
	CHECK(ap::fv<node_t>(xy).size() == 2);
	CHECK(!ap::fv_meets<node_t>(xy, { x, y }));
	CHECK(!ap::fv_meets<node_t>(yx, { x, y }));
	// Inv. 3 may re-wrap a variable the body has since lost; the callers
	// narrow through fv_intersect first (§6/§7), and phase 5's fold drops
	// what a re-wrap without narrowing left — through A's `fv`, keeping the
	// order of the rest.
	ap::block X{ x, z, y };
	tref lost = ap::rewrap<node_t>(phi, X);
	CHECK(ap::formula_size<node_t>(lost) == 6);
	CHECK(ap::rewrap<node_t>(phi, ap::fv_intersect<node_t>(phi, X)) == xy);
	CHECK(ap::fold_degenerate_binders<node_t>(lost) == xy);
	CHECK(ap::fold_degenerate_binders<node_t>(xy) == xy);
}

// --- A over B: canonical chains of prepared atoms -----------------------------------------

TEST_CASE("canonical chains over prepared atoms and a unit: one node, members as a set, facets composed") {
	tref x = vr("x");
	ap::block P{ x };
	order_t o = order_for(P);
	tref A = ap::prepare_terms<node_t>(wff("x & a = 0"), P, o);
	tref B = ap::prepare_terms<node_t>(wff("x & b = 0"), P, o);
	tref U = wff("ex y (x & y = 0)");                     // a unit: opaque to prepare_terms
	tref C = wff("c = 0");
	REQUIRE(ap::is_bdd_backed<node_t>(sides(A).first));
	REQUIRE(ap::is_bdd_backed<node_t>(sides(B).first));
	CHECK(ap::prepare_terms<node_t>(U, P, o) == U);
	tref chain = ap::canonical_and<node_t>(trefs{ A, U, C, B });
	CHECK(chain == ap::canonical_and<node_t>(trefs{ B, C, U, A }));
	CHECK(chain == ap::canonical_and<node_t>(trefs{ A, A, U, C, B, U }));
	// Membership as a set: the content order among BDD-backed atoms is the
	// interning order of their BDDs, so no position is pinned.
	CHECK(ap::members<node_t>(chain).size() == 4);
	for (tref m : { A, U, C, B }) CHECK(ap::is_member<node_t>(chain, m));
	CHECK(ap::formula_size<node_t>(chain) == 3 + (1 + 2 + 1 + 1));   // the unit is 2
	CHECK(ap::fv<node_t>(chain).size() == 4);              // a, b, c, x — the unit's y is bound
	CHECK(ap::fv_meets<node_t>(chain, P));
	CHECK(!ap::fv_meets<node_t>(chain, { ap::binder_var<node_t>(U) }));
	// Negative trees over prepared literals, flat and nested.
	CHECK(ap::is_negative_tree<node_t>(ap::canonical_or<node_t>(trefs{ neg(A), neg(B) })));
	CHECK(ap::is_flat_tree<node_t>(ap::canonical_or<node_t>(trefs{ neg(A), neg(B) })));
	CHECK(!ap::is_negative_tree<node_t>(ap::canonical_or<node_t>(trefs{ neg(A), U })));
	tref nested = ap::canonical_or<node_t>(trefs{
		ap::canonical_and<node_t>(trefs{ neg(A), neg(B) }),
		neg(ap::prepare_terms<node_t>(wff("x & c = 0"), P, o)) });
	CHECK(ap::is_negative_tree<node_t>(nested));
	CHECK(!ap::is_flat_tree<node_t>(nested));
	// The component closes and re-opens on the same chain node.
	tref fin = ap::finish_terms<node_t>(chain);
	CHECK(!has_bdd_id(fin));
	CHECK(ap::prepare_terms<node_t>(fin, P, o) == chain);
}

// --- claim 4, the term half: one memo per rewrite -------------------------------------------

TEST_CASE("subst_term rewrites a shared subterm once: the formula-argument count is 1") {
	// (S · p) ∪ (S · q) with S = x · r(x, ψ(x)): the two occurrences of S
	// are two spellings (each carries its own right sibling), one content,
	// and their children are one tref. The per-call memo is content-keyed,
	// so ψ is re-simplified exactly once however many places S appears in
	// (§10, the second bullet).
	tref x = vr("x"), z = vr("z");
	tref psi = wff("x & v1 = 0");
	tref rterm = tau::get(tau::bf, tau::get(tau::bf_ref, ref_with_formula_arg(bf("x"), psi)));
	tref S = build_bf_and<node_t>(bf("x"), rterm);
	tref f = build_bf_or<node_t>(build_bf_and<node_t>(S, bf("v2")),
		build_bf_and<node_t>(S, bf("v3")));
	REQUIRE(ap::fv_meets<node_t>(f, { x }));
	size_t calls = 0;
	auto count = [&calls](tref fm) { ++calls; return fm; };
	tref s = ap::subst_term<node_t>(f, x, bf("z"), {}, count);
	CHECK(calls == 1);
	CHECK(s != f);
	CHECK(!ap::fv_meets<node_t>(s, { x }));
	CHECK(ap::fv_meets<node_t>(s, { z }));
	// Both copies were rewritten, the argument formula with them: the result
	// is the term built over the rewritten S.
	tref psi_z = wff("z & v1 = 0");
	tref S_z = build_bf_and<node_t>(bf("z"), tau::get(tau::bf,
		tau::get(tau::bf_ref, ref_with_formula_arg(bf("z"), psi_z))));
	CHECK(s == build_bf_or<node_t>(build_bf_and<node_t>(S_z, bf("v2")),
		build_bf_and<node_t>(S_z, bf("v3"))));
	CHECK(tau::get(s).find_top([psi](tref m) {
		return tau::subtree_equals(m, psi); }) == nullptr);
	// An argument the variable does not reach is not re-simplified.
	tref g = build_bf_and<node_t>(bf("x"), tau::get(tau::bf,
		tau::get(tau::bf_ref, ref_with_formula_arg(bf("v1"), wff("v1 & v4 = 0")))));
	calls = 0;
	(void) ap::subst_term<node_t>(g, x, bf("z"), {}, count);
	CHECK(calls == 0);
}

// --- claim 3 and the formula half of claim 4: package C ------------------------------------

TEST_CASE("subst_var: capture-safe on the parser's canonical ids, and the ids stay canonical") {
	// z = 1, y = 2, x = 3: an outer binder's id is above every id inside it
	// (prims.h), so a witness that is free at the spine meets no binder on
	// the path into it — the property the descent into units rides on.
	tref phi = wff("ex x ex y (x = y && a = 0 && ex z (x & z = 0))");   // a binder's body runs to the right end
	REQUIRE(ap::is_binder<node_t>(phi));
	tref x = ap::binder_var<node_t>(phi);
	tref ex_y = ap::binder_body<node_t>(phi);
	tref y = ap::binder_var<node_t>(ex_y);
	tref S = ap::binder_body<node_t>(ex_y);              // the spine
	CHECK(get_var_name<node_t>(x) == "3");
	CHECK(get_var_name<node_t>(y) == "2");
	REQUIRE(ap::members<node_t>(S).size() == 3);
	tref pin = pin_on(S, x);
	tref unit = member_where(S, ap::is_binder<node_t>);
	REQUIRE(pin != nullptr);
	REQUIRE(unit != nullptr);
	tref z = ap::binder_var<node_t>(unit);
	CHECK(get_var_name<node_t>(z) == "1");
	tref t = sides(pin).second;                           // the witness y, as a term
	CHECK(!ap::fv_meets<node_t>(t, { z }));
	CHECK(ap::fv_meets<node_t>(t, { y }));
	tref res = ap::subst_var<node_t>(S, x, t, {});
	CHECK(res != S);
	const trefs& fvr = ap::fv<node_t>(res);
	CHECK(!has(fvr, x));
	CHECK(has(fvr, y));
	CHECK(has(fvr, vr("a")));
	// The pinning conjunct became y = y and folded through the hooks; the
	// unit was DESCENDED, its binder kept, its body now holding y.
	CHECK(ap::members<node_t>(res).size() == 2);
	tref unit2 = member_where(res, ap::is_binder<node_t>);
	REQUIRE(unit2 != nullptr);
	CHECK(same(ap::binder_var<node_t>(unit2), z));
	CHECK(ap::fv_meets<node_t>(ap::binder_body<node_t>(unit2), { y }));
	CHECK(!ap::fv_meets<node_t>(unit2, { x }));
	// Ids stay canonical for a witness without functional quantifiers
	// (ruling 5): the rewrite, binder deleted, is a memo-key-stable formula.
	CHECK(ap::canonicalise_binder_ids<node_t>(res) == res);
	tref rewritten = ap::rewrap<node_t>(res, { y });
	CHECK(ap::canonicalise_binder_ids<node_t>(rewritten) == rewritten);
	// §3's TRY_WITNESS_DEEP counterexamples: the pin's witness IS the inner
	// bound variable and meets a binder on the path — condition (c) fails
	// on A's `fv_meets` and the primitive is never called (its refusal is a
	// Debug assert).
	{
		tref bad = wff("ex x all y (x = y && a = 0)");
		tref bx = ap::binder_var<node_t>(bad);
		tref all_y = ap::binder_body<node_t>(bad);
		tref by = ap::binder_var<node_t>(all_y);
		tref p = pin_on(ap::binder_body<node_t>(all_y), bx);
		REQUIRE(p != nullptr);
		CHECK(ap::fv_meets<node_t>(sides(p).second, { by }));
	}
	{
		tref bad = wff("ex x all y ex z (x = z && z = y)");
		tref bx = ap::binder_var<node_t>(bad);
		tref ex_z = ap::binder_body<node_t>(ap::binder_body<node_t>(bad));
		tref bz = ap::binder_var<node_t>(ex_z);
		tref p = pin_on(ap::binder_body<node_t>(ex_z), bx);
		REQUIRE(p != nullptr);
		CHECK(ap::fv_meets<node_t>(sides(p).second, { bz }));
	}
}

TEST_CASE("subst_var with a witness carrying a functional quantifier: the F5 shadowing pair, repaired by phase 5") {
	// Ruling 5 (Sep 10 2026) and plan §4: a `t` carrying a functional
	// quantifier may leave a SHADOWING PAIR of ids on one path — the unit's
	// binder and the quantifier's subscript are both "1" — until phase 5's
	// CANONICALISE_BINDER_IDS. Recorded as EXPECTED, not as an error:
	// semantically safe (the subscript is bound inside its own body, and
	// A's FV reads it so), and the depth numbering repairs it in one pass.
	tref x = vr("x");
	tref unit = wff("ex z (x & z = 0)");                  // z is "1"
	tref t = bf("fall w (w | q5)");                       // w is "1" as well
	REQUIRE(get_var_name<node_t>(ap::binder_var<node_t>(unit)) == "1");
	tref tq = find_kind(t, tau::bf_fall);
	REQUIRE(tq != nullptr);
	REQUIRE(get_var_name<node_t>(tau::get(tq).first()) == "1");
	tref res = ap::subst_var<node_t>(unit, x, t, {});
	REQUIRE(ap::is_binder<node_t>(res));
	CHECK(get_var_name<node_t>(ap::binder_var<node_t>(res)) == "1");
	tref rq = find_kind(res, tau::bf_fall);
	REQUIRE(rq != nullptr);
	CHECK(get_var_name<node_t>(tau::get(rq).first()) == "1");
	CHECK(ap::fv<node_t>(res).size() == 1);
	CHECK(ap::fv_meets<node_t>(res, { vr("q5") }));
	CHECK(!ap::fv_meets<node_t>(res, { x }));
	tref canon = ap::canonicalise_binder_ids<node_t>(res);
	CHECK(canon != res);
	CHECK(get_var_name<node_t>(ap::binder_var<node_t>(canon)) == "2");
	CHECK(get_var_name<node_t>(tau::get(find_kind(canon, tau::bf_fall)).first()) == "1");
	CHECK(ap::canonicalise_binder_ids<node_t>(canon) == canon);
	CHECK(ap::fv<node_t>(canon).size() == 1);
}

TEST_CASE("subst_var: a node without the variable is returned untouched, spelling and all") {
	tref x = vr("x");
	tref A = wff("s1 = 0"), B = wff("s2 = 0");
	tref ab = build_wff_and<node_t>(A, B), ba = build_wff_and<node_t>(B, A);
	tref canon = ap::canonical_and<node_t>(trefs{ A, B });
	tref phi = canon == ab ? ba : ab;                      // the spelling NOT in content order
	REQUIRE(phi != canon);
	// One cached FV test, no rebuild: a rebuild through the raw constructor
	// would have put the members in content order.
	CHECK(ap::subst_var<node_t>(phi, x, bf("z"), {}) == phi);
	// A unit that rebinds the variable does not hold it free.
	tref unit = wff("ex x (x & s1 = 0)");
	CHECK(ap::subst_var<node_t>(unit, ap::binder_var<node_t>(unit), bf("z"), {}) == unit);
	// At a site that IS rewritten, the untouched conjuncts ride along and
	// only the touching one changes.
	tref site = build_wff_and<node_t>(phi, wff("x & s3 = 0"));
	tref res = ap::subst_var<node_t>(site, x, bf("z"), {});
	CHECK(res != site);
	CHECK(ap::members<node_t>(res).size() == 3);
	CHECK(ap::is_member<node_t>(res, A));
	CHECK(ap::is_member<node_t>(res, B));
	CHECK(ap::is_member<node_t>(res, wff("z & s3 = 0")));
	CHECK(!ap::fv_meets<node_t>(res, { x }));
}

TEST_CASE("atoms vs fv on a unit: [atm ↦ T/F] is unit-opaque, [x ← t] descends; rows on members, none on the spine") {
	tref x = vr("x");
	tref A = wff("k1 = 0"), B = wff("k2 = 0"), U = wff("ex y (x & y = 0)");
	tref inner = ap::binder_body<node_t>(U);
	tref phi = ap::canonical_and<node_t>(trefs{ A, U, B });
	REQUIRE(ap::members<node_t>(phi).size() == 3);
	// A's reach: through the unit, minus its own variable.
	CHECK(ap::fv<node_t>(phi).size() == 3);
	CHECK(ap::fv_meets<node_t>(phi, { x }));
	CHECK(!ap::fv_meets<node_t>(phi, { ap::binder_var<node_t>(U) }));
	// C's reach: the unit is opaque (§4).
	const trefs& at = ap::atoms<node_t>(phi);
	CHECK(at.size() == 2);
	CHECK(ap::has_atom<node_t>(phi, A));
	CHECK(ap::has_atom<node_t>(phi, B));
	CHECK(!ap::has_atom<node_t>(phi, inner));
	CHECK(!ap::has_atom<node_t>(phi, U));
	CHECK(ap::atoms<node_t>(U).empty());
	CHECK(ap::has_atom<node_t>(inner, inner));
	// Rows: the queried root and every member wrapper; none on the chain's
	// spine node, like get_free_vars (ruling, Sep 10 2026).
	CHECK(ap::find<node_t, ap::table::atoms_memo>(phi) != nullptr);
	CHECK(ap::find<node_t, ap::table::atoms_memo>(A) != nullptr);
	CHECK(ap::find<node_t, ap::table::atoms_memo>(U) != nullptr);
	CHECK(ap::find<node_t, ap::table::atoms_memo>(tau::get(phi)[0].first()) == nullptr);
	// The atom substitution erases what it reaches and nothing inside the unit.
	CHECK(ap::subst_atom<node_t>(phi, inner, true) == phi);
	tref dropped = ap::subst_atom<node_t>(phi, A, true);    // A ↦ T, folded by the hooks
	CHECK(dropped != phi);
	CHECK(!ap::is_member<node_t>(dropped, A));
	CHECK(ap::is_member<node_t>(dropped, U));
	CHECK(ap::is_member<node_t>(dropped, B));
	CHECK(ap::is_false<node_t>(ap::subst_atom<node_t>(phi, A, false)));
	// The variable substitution descends into the unit and leaves the
	// vocabulary as it was.
	tref res = ap::subst_var<node_t>(phi, x, bf("z"), {});
	CHECK(res != phi);
	CHECK(!ap::fv_meets<node_t>(res, { x }));
	tref unit2 = member_where(res, ap::is_binder<node_t>);
	REQUIRE(unit2 != nullptr);
	CHECK(ap::fv_meets<node_t>(ap::binder_body<node_t>(unit2), { vr("z") }));
	CHECK(ap::atoms<node_t>(res).size() == 2);
}

TEST_CASE("subst_var reaches a reference's arguments and re-simplifies them") {
	// §1/§4: substitution is the one rewrite that reaches inside a
	// reference; every argument it touches is re-emitted through
	// SIMPLIFY_TERM (ruling 4, Sep 10 2026).
	tref x = vr("x"), z = vr("z");
	tref phi = wff("f(x) && x & u1 = 0");
	REQUIRE(ap::members<node_t>(phi).size() == 2);
	tref res = ap::subst_var<node_t>(phi, x, bf("z"), {});
	CHECK(res != phi);
	CHECK(!ap::fv_meets<node_t>(res, { x }));
	CHECK(ap::fv_meets<node_t>(res, { z }));
	CHECK(ap::is_member<node_t>(res, wff("f(z)")));
	CHECK(ap::is_member<node_t>(res, wff("z & u1 = 0")));
	// A witness that folds inside the argument folds there: the argument
	// is SIMPLIFY_TERM's own output (per-path contradiction, u1′ under
	// u1), the function u1·u2, the symbol untouched.
	tref t2 = bf("u1 & (u1' | u2)");
	tref res2 = ap::subst_var<node_t>(phi, x, t2, {});
	tref ref2 = member_where(res2, ap::is_reference<node_t>);
	REQUIRE(ref2 != nullptr);
	tref arg2 = find_kind(ref2, tau::ref_arg);
	REQUIRE(arg2 != nullptr);
	tref a2 = tau::trim_right_sibling(tau::get(arg2).first());
	CHECK(same(a2, ap::simplify_term<node_t>(t2)));
	CHECK(same_function(a2, bf("u1 & u2"), { vr("u1"), vr("u2") }));
	CHECK(find_kind(a2, tau::bf_neg) == nullptr);
	CHECK(same(find_kind(ref2, tau::sym), find_kind(phi, tau::sym)));
	// A reference whose arguments do not hold the variable is the same node.
	CHECK(ap::subst_var<node_t>(wff("f(u1)"), x, bf("z"), {}) == wff("f(u1)"));
}

TEST_CASE("subst_var under the live order: compose on the block variable, the same function as the plain rewrite") {
	tref x = vr("x"), y = vr("y"), z = vr("z"), a = vr("a"), b = vr("b");
	ap::block P{ x, y };
	order_t o = order_for(P);
	tref phi = wff("x & y & a = 0 && x & b = 0 && ex w (x & w = 0)");
	tref t = bf("y & z");                                  // touches P and a free variable
	tref prepared = ap::prepare_terms<node_t>(phi, P, o);
	REQUIRE(has_bdd_id(prepared));
	tref res_b = ap::subst_var<node_t>(prepared, x, t, o);
	tref res_p = ap::subst_var<node_t>(phi, x, t, {});
	CHECK(!ap::fv_meets<node_t>(res_b, { x }));
	CHECK(!ap::fv_meets<node_t>(res_p, { x }));
	CHECK(ap::fv_meets<node_t>(res_b, { z }));
	trefs mb = ap::members<node_t>(res_b), mp = ap::members<node_t>(res_p);
	REQUIRE(mb.size() == 3);
	REQUIRE(mp.size() == 3);
	for (tref m : mb) {
		if (ap::is_binder<node_t>(m)) {
			// The unit is opaque to prepare_terms, so its atom is plain
			// on both sides; compare the rewritten bodies as functions.
			tref w = ap::binder_var<node_t>(m);
			bool found = false;
			for (tref n : mp) if (ap::is_binder<node_t>(n))
				found = same_function(
					ap::term_of<node_t>(ap::binder_body<node_t>(m), {}),
					ap::term_of<node_t>(ap::binder_body<node_t>(n), {}),
					{ y, z, w });
			CHECK(found);
			continue;
		}
		REQUIRE(ap::is_atom<node_t>(m));
		CHECK(ap::is_bdd_backed<node_t>(sides(m).first));      // y is still a decision variable
		bool found = false;
		for (tref n : mp) if (ap::is_atom<node_t>(n))
			found = found || same_function(ap::term_of<node_t>(m, o),
				ap::term_of<node_t>(n, {}), { y, z, a, b });
		CHECK(found);
	}
	CHECK(!has_bdd_id(ap::finish_terms<node_t>(res_b)));
}

TEST_CASE("subst_var rewrites a shared subtree once: the formula-argument count is exactly 1") {
	// (S ∧ p) ∨ (S ∧ q) with a reference r(x, ψ(x)) inside S (ruling 2,
	// Sep 10 2026): ψ is re-simplified once per call, however many places
	// S appears in — one memo per rewrite, shared across the site (§10). S
	// is a canonical chain (A), so inside each disjunct it is a SPINE node:
	// entered without a memo entry, its members the hits.
	tref x = vr("x"), z = vr("z");
	tref psi = wff("x & w1 = 0");
	tref R = tau::get(tau::wff, tau::get(tau::wff_ref, ref_with_formula_arg(bf("x"), psi)));
	REQUIRE(ap::is_reference<node_t>(R));
	tref S = ap::canonical_and<node_t>(trefs{ wff("x & w2 = 0"), R });
	tref phi = ap::canonical_or<node_t>(trefs{
		ap::canonical_and<node_t>(trefs{ S, wff("w3 = 0") }),
		ap::canonical_and<node_t>(trefs{ S, wff("w4 = 0") }) });
	REQUIRE(ap::members<node_t>(phi).size() == 2);
	size_t calls = 0;
	auto count = [&calls](tref fm) { ++calls; return fm; };
	tref res = ap::subst_var<node_t>(phi, x, bf("z"), {}, count);
	CHECK(calls == 1);
	CHECK(res != phi);
	CHECK(!ap::fv_meets<node_t>(res, { x }));
	CHECK(ap::fv_meets<node_t>(res, { z }));
	// Both copies were rewritten: each disjunct holds the rewritten reference.
	for (tref d : ap::members<node_t>(res)) {
		bool found = false;
		for (tref m : ap::members<node_t>(d))
			if (ap::is_reference<node_t>(m) && ap::fv_meets<node_t>(m, { z }))
				found = true;
		CHECK(found);
	}
}

// --- last: a sweep rebuilds every table in the process --------------------------------------

TEST_CASE("facet rows survive construction, and a sweep keeps the live ones and their values") {
	tref x = vr("x"), y = vr("y");
	ap::block P{ x, y };
	order_t o = order_for(P);
	tref body = wff("x & y & a = 0 && b = 0");
	tref prepared = ap::prepare_terms<node_t>(body, P, o);
	tref r = ap::rewrap<node_t>(prepared, P);
	const size_t sz = ap::formula_size<node_t>(r);
	CHECK(sz == 5);
	const size_t* row = size_row(r);
	REQUIRE(row != nullptr);
	const trefs& fvr = ap::fv<node_t>(r);
	const trefs fv_copy = fvr;
	CHECK(fv_copy.size() == 2);
	// Filling many rows in both tables (rehashes) moves nothing (ctx.h,
	// LIFETIME).
	churn("gc_churn_", 600);
	CHECK(size_row(r) == row);
	CHECK(*row == sz);
	CHECK(&ap::fv<node_t>(r) == &fvr);
	CHECK(fvr == fv_copy);
	// A sweep as the interpreter runs one (interpreter.tmpl.h:964-968): the
	// BDD store holds its decision variables and leaf terms as raw trefs and
	// is never swept, so they are pinned through collect_live_refs; the
	// formula through its handle. The TRIMMED bf(BDD_ID) wrapper is U's key
	// and I's value and no subtree of `r` — the atom's side carries `0` as
	// its right sibling — so it is pinned by a handle of its own (the hazard
	// is recorded in the plan's §4 bullet). The kept node then keeps its rows
	// and its interned terms; the churn dies.
	tref l_before = sides(member_touching(prepared, P, {})).first;
	REQUIRE(ap::is_bdd_backed<node_t>(l_before));
	htref keep = tau::geth(r), keep_l = tau::geth(l_before);
	std::unordered_set<tref> ks{ r };
	tb::collect_live_refs(ks);
	tau::gc(ks);
	const size_t* row2 = size_row(r);
	REQUIRE(row2 != nullptr);
	CHECK(*row2 == sz);
	CHECK(ap::formula_size<node_t>(r) == sz);
	CHECK(ap::fv<node_t>(r) == fv_copy);
	tref l = sides(member_touching(ap::binder_body<node_t>(ap::binder_body<node_t>(r)), P, {})).first;
	CHECK(l == l_before);
	CHECK(ap::is_bdd_backed<node_t>(l));
	CHECK(ap::prepare_terms<node_t>(ap::finish_terms<node_t>(prepared), P, o) == prepared);
	CHECK(keep.get() != nullptr);
	CHECK(keep_l.get() != nullptr);
}

} // TEST_SUITE
