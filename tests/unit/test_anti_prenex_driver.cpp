// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Unit tests for src/anti_prenex/driver/: §5's one block — the atom
// connectivity that partitions a block, the ∃ push over the components it
// gives, and the ∀ run's dualisation onto that push — and §4's pass over a
// WHOLE FORMULA, which collects every run and eliminates it over a matrix the
// pass has already finished. Spec: anti_prenex.md §5 (CONNECTED_COMPONENTS,
// PUSH_EX_BLOCK with its size acceptance and its close, PROCESS_BLOCK), §4
// (COLLECT_RUN, PROCESS_NODE, PROCESS_ALL_BLOCKS), §3 (the primitives those
// call), §1 (the ctx table's `keep_functional` row), the paper's assumption
// 1 (shared subtrees), invariants 2, 3 and 4.
//
// A §5 case is ONE formula through one harness: parse a quantifier run, take
// its kind, its variables and its matrix as phase 4 meets it — in NNF, with
// `f ≠ 0` spelled `¬(f = 0)` — and hand them over. A §4 case hands the whole
// normalised formula over instead. What is claimed of every result:
// EQUIVALENT to the formula it came from, no free variable ESCAPED, INVARIANT
// 4, and PLAIN — a component's close is total, so no `BDD_ID` survives it.
//
// TYPES: §7's table has no row for an untyped block and `method`
// Debug-asserts on one, so every fixture is parsed as a whole spec and typed
// by inference; what a case builds by hand says its type itself.
//
// Parsing note: a parsed quantifier's body runs to the RIGHT END and
// juxtaposition is conjunction, so every input keeps its parentheses.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "normalizer.h"
#include "anti_prenex/driver/block.h"
#include "anti_prenex/driver/driver.h"

#include <algorithm>
#include <utility>
#include <vector>

using namespace idni::tau_lang;
namespace ap = idni::tau_lang::anti_prenexing;

namespace {

using th = term_handle<node_t>;
using tb = tau_term_bdd<node_t>;

tref parse(const char* sample) {
	auto rr = get_nso_rr(sample);
	REQUIRE(rr.has_value());
	return rr.value().main->get();
}

bool same(tref a, tref b) { return tau::subtree_equals(a, b); }

/// Two blocks hold the same variables in the same order.
bool same_block(const ap::block& got, const ap::block& want) {
	if (got.size() != want.size()) return false;
	for (size_t i = 0; i < got.size(); ++i)
		if (!same(got[i], want[i])) return false;
	return true;
}

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

/// Every TERM-level functional quantifier of `n`: one per subscript, so a
/// chain of two counts twice.
size_t functional_count(tref n) {
	return tau::get(n).select_all([](tref m) {
		const auto& t = tree<node_t>::get(m);
		return t.is(tau::bf_fall) || t.is(tau::bf_fex);
	}).size();
}

/// The formula under the outermost @p k binders of `n`.
tref matrix_of(tref n, size_t k) {
	while (k-- > 0) {
		REQUIRE(is_child_quantifier<node_t>(n));
		n = tau::trim_right_sibling(ap::binder_body<node_t>(n));
	}
	return n;
}

/// The formula with every stored BDD spelled out, which is what the oracle
/// reads. A result is plain already — a component's close sees to that, and
/// `check_claims` claims it — so on one this is the identity.
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

/// `ψ` as PHASE 4 meets it: in NNF, with `f ≠ 0` spelled `¬(f = 0)` (phases 1
/// and 3, §3). Inputs written with `!=` would break invariant 4 past phase 3,
/// so the harness runs the two passes the pipeline would have run before the
/// block is reached.
tref normalised(tref body) {
	return ap::normalize_operators<node_t>(
		ap::to_canonically_factored_nnf<node_t>(body));
}

/// `ψ` with every atom in the PLAIN REGIME's normal form, its members
/// re-joined. `normalised` stops before `SIMPLIFY`, so a fixture keeps the
/// spelling it was parsed with, while a component's close spells every term
/// it prepared back in that normal form (§3 `FINISH_TERMS`). An expectation
/// compared BY NODE against a result therefore goes through here first. On an
/// atom already in normal form this is the identity.
tref plainly_spelled(tref n) {
	trefs ms;
	for (tref m : ap::members<node_t>(n))
		ms.push_back(ap::simplify_atom<node_t>(m, {}));
	return is_child<node_t>(n, tau::wff_or)
		? ap::simplified_or_join<node_t>(ms)
		: ap::simplified_and_join<node_t>(ms);
}

/// A `tau`-typed term variable, the type every parsed fixture here gets by
/// inference. Hand-built content has to say so itself.
tref bvar(const char* name) {
	return tau::build_bf_variable(name, tau_type_id<node_t>());
}

/// A parsed run, split the way §4's driver hands one over: the kind, the
/// variables OUTERMOST FIRST, and the matrix normalised. The loop is the
/// harness's own, so that a §5 case rests on nothing `COLLECT_RUN` does —
/// which the R cases claim separately.
struct fixture {
	tref quantified = nullptr;   ///< the run as parsed, the oracle's source
	ap::run<node_t> blk{};
};

fixture split(const char* src) {
	fixture f;
	f.quantified = parse(src);
	tref n = f.quantified;
	REQUIRE(is_child_quantifier<node_t>(n));
	f.blk.kind = ap::binder_kind<node_t>(n);
	while (is_child_quantifier<node_t>(n)
		&& ap::binder_kind<node_t>(n) == f.blk.kind)
	{
		f.blk.vars.push_back(tau::trim_right_sibling(
			ap::binder_var<node_t>(n)));
		n = tau::trim_right_sibling(ap::binder_body<node_t>(n));
	}
	REQUIRE(!f.blk.vars.empty());
	f.blk.matrix = normalised(n);
	return f;
}

/// What every result claims against the formula it came from: equivalence, no
/// free variable escaped, invariant 4, and no stored BDD left anywhere.
void check_claims(tref got, tref source) {
	CHECK(are_nso_equivalent<node_t>(finished(got), source));
	CHECK(no_escape(finished(got), source));
	CHECK(invariant_4(got));
	CHECK(!holds(got, tau::BDD_ID));
}

/// The same claims for a pushed run, whose source is the run as parsed.
void check_claims(tref got, const fixture& f) {
	check_claims(got, f.quantified);
}

/// The two acceptance knobs for the scope of one case, restored on the way
/// out (§1: the constants are read at each component's setup, so a knob set
/// here reaches every component of the calls inside).
struct accept_guard {
	const size_t growth = ap::accept_growth;
	const size_t floor  = ap::accept_floor;
	accept_guard(size_t g, size_t f) {
		ap::accept_growth = g;
		ap::accept_floor = f;
	}
	~accept_guard() {
		ap::accept_growth = growth;
		ap::accept_floor = floor;
	}
};

} // namespace

TEST_SUITE("anti_prenex/driver") {

// --- §5 CONNECTED_COMPONENTS ----------------------------------------------------------

TEST_CASE("C1: atoms sharing no block variable give one component each") {
	fixture f = split("ex x ex y (x a = 0 && y b = 0).");
	const std::vector<ap::block> ps =
		ap::connected_components<node_t>(f.blk.vars, f.blk.matrix);
	REQUIRE(ps.size() == 2);
	CHECK(same_block(ps[0], { f.blk.vars[0] }));
	CHECK(same_block(ps[1], { f.blk.vars[1] }));
}

TEST_CASE("C2: one atom mentioning both merges them") {
	fixture f = split("ex x ex y (x y = 0).");
	const std::vector<ap::block> ps =
		ap::connected_components<node_t>(f.blk.vars, f.blk.matrix);
	REQUIRE(ps.size() == 1);
	CHECK(same_block(ps[0], f.blk.vars));
}

TEST_CASE("C3: connectivity is transitive") {
	// `z` meets `x` in one atom and `y` in the other, so all three are one
	// component — in `X`'s order, not the order the atoms mention them.
	fixture f = split("ex x ex y ex z (x z = 0 && y z = 0).");
	const std::vector<ap::block> ps =
		ap::connected_components<node_t>(f.blk.vars, f.blk.matrix);
	REQUIRE(ps.size() == 1);
	CHECK(same_block(ps[0], f.blk.vars));
}

TEST_CASE("C4: an atom inside a nested binder connects") {
	// The walk descends through every formula node, units and all, so the
	// partition is the atom connectivity of the WHOLE body.
	fixture f = split("ex x ex y (all w (x y w = 0)).");
	REQUIRE(f.blk.vars.size() == 2);
	REQUIRE(is_child_quantifier<node_t>(f.blk.matrix));
	const std::vector<ap::block> ps =
		ap::connected_components<node_t>(f.blk.vars, f.blk.matrix);
	REQUIRE(ps.size() == 1);
	CHECK(same_block(ps[0], f.blk.vars));
}

TEST_CASE("C5: a reference mentioning both connects nothing") {
	// A reference is no atom and its arguments are terms, so it straddles
	// the two components harmlessly (invariant 2).
	fixture f = split("ex x ex y (q(x, y) && x = 0 && y = 0).");
	const std::vector<ap::block> ps =
		ap::connected_components<node_t>(f.blk.vars, f.blk.matrix);
	REQUIRE(ps.size() == 2);
	CHECK(same_block(ps[0], { f.blk.vars[0] }));
	CHECK(same_block(ps[1], { f.blk.vars[1] }));
}

TEST_CASE("C6: a variable only inside a reference argument is a singleton") {
	// `x` occurs in `FV(body)` and gets a component of its own, which is
	// where a frozen variable belongs (§7).
	fixture f = split("ex x ex y (q(x) && y = 0).");
	const std::vector<ap::block> ps =
		ap::connected_components<node_t>(f.blk.vars, f.blk.matrix);
	REQUIRE(ps.size() == 2);
	CHECK(same_block(ps[0], { f.blk.vars[0] }));
	CHECK(same_block(ps[1], { f.blk.vars[1] }));
}

TEST_CASE("C7: a variable the body does not mention is a singleton too") {
	// Narrowing the block is `PUSH_EX_BLOCK`'s business: every variable
	// handed over here gets a component.
	fixture f = split("ex x ex z (x = 0).");
	REQUIRE(f.blk.vars.size() == 2);
	const std::vector<ap::block> ps =
		ap::connected_components<node_t>(f.blk.vars, f.blk.matrix);
	REQUIRE(ps.size() == 2);
	CHECK(same_block(ps[0], { f.blk.vars[0] }));
	CHECK(same_block(ps[1], { f.blk.vars[1] }));
}

TEST_CASE("C8: components and members come in X's order") {
	// The atoms mention `z` before `y` and `x` last; the answer is `X`'s
	// order throughout — `{x}` first because `x` opens its class first.
	fixture f = split("ex x ex y ex z (z y = 0 && x = 0).");
	const std::vector<ap::block> ps =
		ap::connected_components<node_t>(f.blk.vars, f.blk.matrix);
	REQUIRE(ps.size() == 2);
	CHECK(same_block(ps[0], { f.blk.vars[0] }));
	CHECK(same_block(ps[1], { f.blk.vars[1], f.blk.vars[2] }));
}

// --- §5 PUSH_EX_BLOCK -----------------------------------------------------------------

TEST_CASE("B1: a variable the body does not mention leaves at entry") {
	// `X ← X ∩ FV(body)`: `z` binds nothing, so no component is set up for
	// it — the callback is asked once — and no binder for it can come
	// back. What is left is one atom, which the leaf discharges.
	fixture f = split("ex x ex z (x = 0).");
	size_t asked = 0;
	auto count = [&asked](tref) { ++asked; return false; };
	const tref got = ap::push_ex_block<node_t>(f.blk.matrix, f.blk.vars,
		count);
	CHECK(asked == 1);
	check_claims(got, f);
	CHECK(binder_count(got) == 0);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("B2: two components are pushed one after the other") {
	// The components share no atom: the first push strips the second's
	// conjunct outside the block and discharges its own, and the second
	// starts from what the first left.
	fixture f = split("ex x ex y (x a = 0 && y b = 0).");
	size_t asked = 0;
	auto count = [&asked](tref) { ++asked; return false; };
	const tref got = ap::push_ex_block<node_t>(f.blk.matrix, f.blk.vars,
		count);
	CHECK(asked == 2);
	check_claims(got, f);
	CHECK(binder_count(got) == 0);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("B3: a component that re-wraps leaves a unit the next transports") {
	// `∃x` discharges its atom; the disjunction no step of this module
	// pushes comes back re-wrapped over `y` (invariant 3), and nothing
	// later opens it.
	fixture f = split("ex x ex y (x a = 0 && (y b = 0 || y c = 0)).");
	const tref got = ap::push_ex_block<node_t>(f.blk.matrix, f.blk.vars,
		ap::keep_no_functional<node_t>);
	check_claims(got, f);
	REQUIRE(binder_count(got) == 1);
	CHECK(same(ap::binder_var<node_t>(got), f.blk.vars[1]));
}

TEST_CASE("B4: the callback is asked per component, on the block as it stands") {
	// §1's `keep_functional` row: the node is the ∃ wrap of the WHOLE
	// narrowed block over the body that component's setup finds — so the
	// first component sees the body as it arrived and the second sees it
	// with the first component's result in place.
	fixture f = split("ex x ex y (x a = 0 && y b = 0).");
	trefs seen;
	auto record = [&seen](tref n) { seen.push_back(n); return false; };
	ap::push_ex_block<node_t>(f.blk.matrix, f.blk.vars, record);
	REQUIRE(seen.size() == 2);
	CHECK(same(seen[0], ap::rewrap<node_t>(f.blk.matrix, f.blk.vars)));
	CHECK(!same(seen[1], seen[0]));
	// Both carry the whole block; under it, `x` is gone from the second.
	const ap::block first{ f.blk.vars[0] };
	CHECK(ap::fv_meets<node_t>(matrix_of(seen[0], 2), first));
	CHECK(!ap::fv_meets<node_t>(matrix_of(seen[1], 2), first));
	CHECK(ap::members<node_t>(matrix_of(seen[1], 2)).size() == 1);
}

TEST_CASE("B5: a ∀ run asks the callback on the dualised matrix") {
	// `PROCESS_BLOCK` negates before it pushes, so what the callback is
	// handed is `∃X` over `NEG(matrix)`, never the ∀ run itself.
	fixture f = split("all x (x y = 0 && x w != 0).");
	trefs seen;
	auto record = [&seen](tref n) { seen.push_back(n); return false; };
	ap::process_block<node_t>(f.blk, record);
	REQUIRE(seen.size() == 1);
	const tref dual = ap::normalize_operators<node_t>(
		ap::canonically_factored_neg<node_t>(f.blk.matrix));
	CHECK(same(seen[0], ap::rewrap<node_t>(dual, f.blk.vars)));
}

TEST_CASE("B6: two components may be answered differently") {
	// One flag per component, and each memo key carries its own: the first
	// component keeps its emission, the second discharges. The callback
	// tells a BLOCK node from a CHAIN node by the prefix `strip_chain`
	// reads off, and keeps every chain, so the first component's emission
	// survives its close.
	fixture f = split("ex x ex y (x a = 0 && y b = 0).");
	size_t blocks = 0;
	auto mixed = [&blocks](tref n) {
		if (ap::strip_chain<node_t>(n).first.empty())
			return blocks++ == 0;
		return true;
	};
	const tref got = ap::push_ex_block<node_t>(f.blk.matrix, f.blk.vars,
		mixed);
	CHECK(blocks == 2);
	CHECK(functional_count(got) == 1);
	CHECK(invariant_4(got));
	CHECK(!holds(got, tau::BDD_ID));
	CHECK(no_escape(got, f.quantified));
	CHECK(are_nso_equivalent<node_t>(finished(got), f.quantified));
}

TEST_CASE("B7: keep mode emits the chain, and the close leaves it plain") {
	// `DISCHARGE` under `ctx.keep_functional` spells the block instead of
	// solving it, `RESOLVE_FUNCTIONAL_BDD` keeps what the callback keeps,
	// and `FINISH_TERMS` spells the chain's body out: `∃x. x·y = 0` comes
	// back as the one atom `(∀_x x·y) = 0`.
	fixture f = split("ex x (x y = 0).");
	auto keep_all = [](tref) { return true; };
	const tref got = ap::push_ex_block<node_t>(f.blk.matrix, f.blk.vars,
		keep_all);
	REQUIRE(is_child<node_t>(got, tau::bf_eq));
	auto [prefix, body] = ap::strip_chain<node_t>(sides(got).first);
	REQUIRE(prefix.size() == 1);
	CHECK(prefix[0].second == tb::all);
	CHECK(!th::is_bdd_backed(body));
	CHECK(!holds(got, tau::BDD_ID));
	CHECK(binder_count(got) == 0);
	CHECK(no_escape(got, f.quantified));
	CHECK(are_nso_equivalent<node_t>(finished(got), f.quantified));
}

TEST_CASE("B8: a callback keeping blocks but no chain resolves at the close") {
	// The emission is made and then quantified away by one pass over the
	// stored BDD, which is the answer the non-keep run gives directly.
	fixture f = split("ex x (x y = 0).");
	auto keep_blocks = [](tref n) {
		return ap::strip_chain<node_t>(n).first.empty();
	};
	const tref want = ap::push_ex_block<node_t>(f.blk.matrix, f.blk.vars,
		ap::keep_no_functional<node_t>);
	const tref got = ap::push_ex_block<node_t>(f.blk.matrix, f.blk.vars,
		keep_blocks);
	CHECK(same(got, want));
	CHECK(functional_count(got) == 0);
	check_claims(got, f);
}

TEST_CASE("B9: a push past the acceptance bound is discarded for its input") {
	// With both knobs at zero every push is rejected, and what comes back
	// is the component's INPUT re-wrapped (invariant 3) with the
	// component-free conjuncts left outside the wrap. The same input under
	// the real knobs is accepted and resolves.
	fixture f = split("ex x (x y = 0 && w = 0).");
	// The members are compared against a RESULT, so they are taken as the
	// close spells them back.
	trefs dep, indep;
	for (tref m : ap::members<node_t>(f.blk.matrix))
		(ap::fv_meets<node_t>(m, f.blk.vars) ? dep : indep)
			.push_back(plainly_spelled(m));
	REQUIRE(dep.size() == 1);
	REQUIRE(indep.size() == 1);
	indep.push_back(ap::rewrap<node_t>(
		ap::simplified_and_join<node_t>(dep), f.blk.vars));
	const tref want = ap::simplified_and_join<node_t>(indep);
	{
		accept_guard g(0, 0);
		const tref got = ap::push_ex_block<node_t>(f.blk.matrix,
			f.blk.vars, ap::keep_no_functional<node_t>);
		CHECK(same(got, want));
		check_claims(got, f);
		CHECK(binder_count(got) == 1);
	}
	const tref got = ap::push_ex_block<node_t>(f.blk.matrix, f.blk.vars,
		ap::keep_no_functional<node_t>);
	check_claims(got, f);
	CHECK(binder_count(got) == 0);
}

// --- §5 PROCESS_BLOCK -----------------------------------------------------------------

TEST_CASE("D1: a ∀ run is pushed by dualisation") {
	// `∀x. x·y ≠ 0` is `¬∃x. x·y = 0`, and the ∃ push answers `T`, so the
	// outbound negation answers `F` — a real verdict, not a re-wrap.
	fixture f = split("all x (x y != 0).");
	const tref got = ap::process_block<node_t>(f.blk,
		ap::keep_no_functional<node_t>);
	check_claims(got, f);
	CHECK(tau::get(got).equals_F());
}

TEST_CASE("D2: what the ∃ push resolves comes back negated") {
	// `∀x. x·y = 0` dualises to `∃x. ¬(x·y = 0)`, which the leaf
	// discharges to `¬(y = 0)`; negated again, that is `y = 0`.
	fixture f = split("all x (x y = 0).");
	const tref got = ap::process_block<node_t>(f.blk,
		ap::keep_no_functional<node_t>);
	check_claims(got, f);
	CHECK(binder_count(got) == 0);
	CHECK(are_nso_equivalent<node_t>(finished(got), parse("y = 0.")));
}

TEST_CASE("D3: a ∀ run whose push re-wraps comes back as the same ∀ head") {
	// The dualised matrix is a disjunction, which no step of this module
	// pushes: it re-wraps as `∃x`, and the outbound negation turns that
	// back into the ∀ block over the matrix it started from.
	fixture f = split("all x (x y = 0 && x w != 0).");
	const tref got = ap::process_block<node_t>(f.blk,
		ap::keep_no_functional<node_t>);
	check_claims(got, f);
	CHECK(same(got, ap::rewrap<node_t>(plainly_spelled(f.blk.matrix),
		f.blk.vars, tb::all)));
}

TEST_CASE("D4: an ∃ run goes straight to the push") {
	fixture f = split("ex x (x y = 0).");
	const tref got = ap::process_block<node_t>(f.blk,
		ap::keep_no_functional<node_t>);
	check_claims(got, f);
	CHECK(same(got, ap::push_ex_block<node_t>(f.blk.matrix, f.blk.vars,
		ap::keep_no_functional<node_t>)));
}

// --- §4 COLLECT_RUN -------------------------------------------------------------------

TEST_CASE("R1: a same-kind chain is one run, its variables outermost first") {
	tref phi = parse("ex x ex y (x y = 0).");
	const ap::run<node_t> blk = ap::collect_run<node_t>(phi);
	CHECK(blk.kind == tb::ex);
	REQUIRE(blk.vars.size() == 2);
	CHECK(same(blk.vars[0], tau::trim_right_sibling(
		ap::binder_var<node_t>(phi))));
	CHECK(same(blk.vars[1], tau::trim_right_sibling(
		ap::binder_var<node_t>(matrix_of(phi, 1)))));
	// What stopped the walk is the matrix.
	CHECK(same(blk.matrix, matrix_of(phi, 2)));
	CHECK(is_atomic_fm<node_t>(blk.matrix));
}

TEST_CASE("R2: a kind change ends the run and leaves the inner head as matrix") {
	// Only same-kind quantifiers commute, so `∀y` is a head of its own,
	// collected when the matrix is processed.
	tref phi = parse("ex x (all y (x y = 0)).");
	const ap::run<node_t> blk = ap::collect_run<node_t>(phi);
	CHECK(blk.kind == tb::ex);
	CHECK(blk.vars.size() == 1);
	REQUIRE(is_child_quantifier<node_t>(blk.matrix));
	CHECK(ap::binder_kind<node_t>(blk.matrix) == tb::all);
	CHECK(same(blk.matrix, matrix_of(phi, 1)));
}

TEST_CASE("R3: a lone binder is a run of one variable") {
	tref phi = parse("ex x (x y = 0).");
	const ap::run<node_t> blk = ap::collect_run<node_t>(phi);
	CHECK(blk.kind == tb::ex);
	CHECK(blk.vars.size() == 1);
	CHECK(same(blk.matrix, matrix_of(phi, 1)));
}

TEST_CASE("R4: a ∀ chain is collected the same way") {
	tref phi = parse("all x all y (x y = 0).");
	const ap::run<node_t> blk = ap::collect_run<node_t>(phi);
	CHECK(blk.kind == tb::all);
	CHECK(blk.vars.size() == 2);
	CHECK(same(blk.matrix, matrix_of(phi, 2)));
}

// --- §4 PROCESS_ALL_BLOCKS ------------------------------------------------------------

TEST_CASE("A1: the inner run is eliminated first and the outer one folds") {
	// `∃b. a·b = 0` holds for every `a` — take `b = 0` — so the ∀ run is
	// left with `T`, and `T` needs no block at all.
	tref phi = parse("all a (ex b (a b = 0)).");
	const tref got = ap::process_all_blocks<node_t>(normalised(phi),
		ap::keep_no_functional<node_t>);
	check_claims(got, phi);
	CHECK(binder_count(got) == 0);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("A2: a conjunctive matrix re-wraps, and the run above it re-wraps too") {
	// The push over a conjunction is not part of the module: the inner
	// block re-wraps (invariant 3), and the ∀ run above it then sees one
	// UNIT, which its own push transports rather than opens — so both
	// binders stand in the answer.
	tref phi = parse("all a (ex b (a b = 0 && b c != 0)).");
	const tref got = ap::process_all_blocks<node_t>(normalised(phi),
		ap::keep_no_functional<node_t>);
	check_claims(got, phi);
	CHECK(binder_count(got) == 2);
}

TEST_CASE("A3: a run inside a conjunction is folded through the join") {
	// `∃x. x·y = 0` is `T`, and the join drops a `T` from the conjunction
	// the run stood in (invariant 6), leaving the other conjunct alone.
	tref phi = parse("(ex x (x y = 0)) && z = 0.");
	const tref got = ap::process_all_blocks<node_t>(normalised(phi),
		ap::keep_no_functional<node_t>);
	check_claims(got, phi);
	CHECK(binder_count(got) == 0);
	CHECK(same(got, normalised(parse("z = 0."))));
}

TEST_CASE("A4: a run inside a disjunction is folded the same way") {
	// `∃x. x·y ≠ 0` is `y ≠ 0`, which takes the run's place among the
	// disjunction's members.
	tref phi = parse("(ex x (x y != 0)) || z = 0.");
	const tref got = ap::process_all_blocks<node_t>(normalised(phi),
		ap::keep_no_functional<node_t>);
	check_claims(got, phi);
	CHECK(binder_count(got) == 0);
}

TEST_CASE("A5: a shared run head is processed once") {
	// `(∃x.ψ) ∧ ∀y.((∃x.ψ) ∨ χ)` with ONE node for the two `∃x.ψ`, which
	// the normalisation keeps shared. The second occurrence sits inside
	// the ∀ run's matrix, so a NESTED walk meets it; the pass memo spans
	// the walks and answers it, and the callback is asked once per run —
	// each of these has one component — rather than once per occurrence.
	const tref psi = parse("ex x (x a = 0 || x c = 0).");
	const tref borrowed = parse("all y (y b = 0).");
	const tref phi = tau::build_wff_and(psi,
		tau::build_wff_all(tau::trim_right_sibling(
				ap::binder_var<node_t>(borrowed)),
			tau::build_wff_or(psi, matrix_of(borrowed, 1)),
			false));
	size_t asked = 0;
	auto count = [&asked](tref) { ++asked; return false; };
	const tref got = ap::process_all_blocks<node_t>(normalised(phi),
		count);
	CHECK(asked == 2);
	check_claims(got, phi);
}

TEST_CASE("A6: a run under a temporal operator is processed") {
	// The walk enters a temporal operator like any other formula node, so
	// the block below one is reached and discharged.
	tref phi = parse("always ex x (x y = 0).");
	const tref got = ap::process_all_blocks<node_t>(normalised(phi),
		ap::keep_no_functional<node_t>);
	CHECK(binder_count(got) == 0);
	CHECK(invariant_4(got));
	CHECK(!holds(got, tau::BDD_ID));
	CHECK(no_escape(finished(got), phi));
	CHECK(are_nso_equivalent<node_t>(finished(got), phi));
}

TEST_CASE("A7: a formula with no formula binder comes back as the same node") {
	// Every step of the pass is then the identity or a re-join of an
	// already canonical chain.
	const tref phi = normalised(parse("x y = 0 && z = 0."));
	REQUIRE(binder_count(phi) == 0);
	CHECK(ap::process_all_blocks<node_t>(phi,
		ap::keep_no_functional<node_t>) == phi);
}

TEST_CASE("A8: a term is never entered, so a functional quantifier is untouched") {
	// The only quantifier here is TERM-level, and the pass stops at a term
	// (§4): resolving a chain belongs to the phases before this one.
	const tref chain = tb::build_functional_quantifiers(
		{{ tau::trim(bvar("x")), tb::all }},
		tau::build_bf_or(bvar("x"), bvar("z")));
	const tref phi = normalised(tau::build_bf_eq_0(
		tau::build_bf_and(bvar("a"), chain)));
	REQUIRE(functional_count(phi) == 1);
	const tref got = ap::process_all_blocks<node_t>(phi,
		ap::keep_no_functional<node_t>);
	CHECK(got == phi);
	CHECK(functional_count(got) == 1);
}

TEST_CASE("A9: every run of an ∃ over ∀ over ∃ nest is processed") {
	// Three runs, innermost first. Each matrix is a disjunction or a unit,
	// which no step of this module pushes, so each run re-wraps over what
	// the run below it left — and the callback is asked once per run.
	tref phi = parse("ex a (all b (ex c (a b = 0 || c d = 0))).");
	size_t asked = 0;
	auto count = [&asked](tref) { ++asked; return false; };
	const tref got = ap::process_all_blocks<node_t>(normalised(phi),
		count);
	CHECK(asked == 3);
	check_claims(got, phi);
	CHECK(binder_count(got) == 3);
}

} // TEST_SUITE
