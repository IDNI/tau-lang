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
// 4, and PLAIN once spelled — the spelling is total, so no `BDD_ID` survives
// it. A §5 case converts the matrix itself (`PREPARE_TERMS`), as phase 4's
// start does, and every case spells its result (`FINISH_TERMS`), as phase 5
// does.
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

/// The formula with every stored BDD spelled out (§3 `FINISH_TERMS`), which
/// is what the oracle reads.
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

/// A `tau`-typed term variable, the type every parsed fixture here gets by
/// inference. Hand-built content has to say so itself.
tref bvar(const char* name) {
	return tau::build_bf_variable(name, tau_type_id<node_t>());
}

/// The term reference `q(arg)` over a built argument.
tref qref(tref arg) {
	return tau::get(tau::bf, tau::get(tau::bf_ref,
		tau::build_ref("q", trefs{ arg })));
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

/// §5 `PUSH_EX_BLOCK` as phase 4 reaches it: over the matrix in §1's term
/// representation (`PREPARE_TERMS`), the result spelled.
tref pushed(const fixture& f, const ap::keep_functional_fn<node_t>& kf) {
	return finished(ap::push_ex_block<node_t>(
		ap::prepare_terms<node_t>(f.blk.matrix), f.blk.vars, kf));
}

/// §5 `PROCESS_BLOCK` the same way.
tref processed(const fixture& f, const ap::keep_functional_fn<node_t>& kf) {
	ap::run<node_t> blk = f.blk;
	blk.matrix = ap::prepare_terms<node_t>(blk.matrix);
	return finished(ap::process_block<node_t>(blk, kf));
}

/// §5 `DELETE_UNATE_BINDERS` over the converted matrix, the result spelled.
tref deleted(const fixture& f) {
	return finished(ap::delete_unate_binders<node_t>(
		ap::prepare_terms<node_t>(f.blk.matrix), f.blk.vars));
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
	// back. What is left is one atom, a pin on the binate `x`, which the
	// witness discharges.
	fixture f = split("ex x ex z (x ^ a = 0).");
	size_t asked = 0;
	auto count = [&asked](tref) { ++asked; return false; };
	const tref got = pushed(f, count);
	CHECK(asked == 1);
	check_claims(got, f);
	CHECK(binder_count(got) == 0);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("B2: two components are pushed one after the other") {
	// The components share no atom: the first push strips the second's
	// conjunct outside the block and discharges its own, and the second
	// starts from what the first left. Each variable is binate, a pin.
	fixture f = split("ex x ex y (x ^ a = 0 && y ^ b = 0).");
	size_t asked = 0;
	auto count = [&asked](tref) { ++asked; return false; };
	const tref got = pushed(f, count);
	CHECK(asked == 2);
	check_claims(got, f);
	CHECK(binder_count(got) == 0);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("B3: the second component is pushed into what the first left") {
	// Two components again, the second one DISJUNCTIVE, and both variables
	// binate. `∃x` strips the disjunction outside its block and discharges
	// its own atom, a pin, to `T`; `∃y` then meets that disjunction alone,
	// where 2b squeezes the two positives on terms and `∃y(y·b = 0)`
	// discharges at `y := 0`.
	fixture f = split("ex x ex y (x ^ a = 0 && (y b = 0 || y' c = 0)).");
	const tref got = pushed(f, ap::keep_no_functional<node_t>);
	check_claims(got, f);
	CHECK(binder_count(got) == 0);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("B4: the callback is asked per component, on the block as it stands") {
	// §1's `keep_functional` row: the node is the ∃ wrap of the WHOLE
	// narrowed block over the body that component's setup finds — so the
	// first component sees the body as it arrived and the second sees it
	// with the first component's result in place.
	fixture f = split("ex x ex y (x ^ a = 0 && y ^ b = 0).");
	trefs seen;
	auto record = [&seen](tref n) { seen.push_back(n); return false; };
	pushed(f, record);
	REQUIRE(seen.size() == 2);
	// The node carries the converted body; compared once spelled.
	CHECK(same(finished(seen[0]), finished(ap::rewrap<node_t>(
		ap::prepare_terms<node_t>(f.blk.matrix), f.blk.vars))));
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
	processed(f, record);
	REQUIRE(seen.size() == 1);
	// The node carries the converted body; compared once spelled.
	const tref dual = ap::normalize_operators<node_t>(
		ap::canonically_factored_neg<node_t>(
			ap::prepare_terms<node_t>(f.blk.matrix)));
	CHECK(same(finished(seen[0]),
		finished(ap::rewrap<node_t>(dual, f.blk.vars))));
}

TEST_CASE("B6: two components may be answered differently") {
	// One flag per component, and each memo key carries its own: the first
	// component keeps its emission, the second discharges. The callback
	// tells a BLOCK node from a CHAIN node by the prefix `strip_chain`
	// reads off, and keeps every chain. A piece holding a free variable in
	// a reference stays a chain under the slide, so only the first
	// component's emission survives as one. Both variables are binate, so
	// neither leaves at block entry.
	size_t blocks = 0;
	auto mixed = [&blocks](tref n) {
		if (ap::strip_chain<node_t>(n).first.empty())
			return blocks++ == 0;
		return true;
	};
	fixture f = split("ex x ex y ((q(a) x | q(c) x') = 0 "
		"&& (q(b) y | q(d) y') = 0).");
	const tref got = pushed(f, mixed);
	CHECK(blocks == 2);
	CHECK(functional_count(got) == 1);
	CHECK(invariant_4(got));
	CHECK(!holds(got, tau::BDD_ID));
	CHECK(no_escape(got, f.quantified));
	// On pure terms every piece is closed and folds, whatever the callback
	// says: both components resolve, which the checker reads.
	blocks = 0;
	fixture p = split("ex x ex y (x a | x' c = 0 && y b | y' d = 0).");
	const tref pg = pushed(p, mixed);
	CHECK(blocks == 2);
	CHECK(functional_count(pg) == 0);
	check_claims(pg, p);
}

TEST_CASE("B7: keep mode emits the chain, and the spelling leaves it plain") {
	// `DISCHARGE` under `ctx.keep_functional` asks the callback on the
	// block's chain and keeps it SLID to the block's levels. Its piece holds
	// `y` and `w` free in references, so it stays a chain, and
	// `FINISH_TERMS` leaves it plain: `∃x. x·q(y) ∪ x′·q(w) = 0` comes back
	// as the one atom `(∀_x x·q(y) ∪ x′·q(w)) = 0`. `x` is binate, so it
	// does not leave at block entry.
	auto keep_all = [](tref) { return true; };
	fixture f = split("ex x ((q(y) x | q(w) x') = 0).");
	const tref got = pushed(f, keep_all);
	REQUIRE(is_child<node_t>(got, tau::bf_eq));
	auto [prefix, body] = ap::strip_chain<node_t>(sides(got).first);
	REQUIRE(prefix.size() == 1);
	CHECK(prefix[0].second == tb::all);
	CHECK(!th::is_bdd_backed(body));
	CHECK(!holds(got, tau::BDD_ID));
	CHECK(binder_count(got) == 0);
	CHECK(no_escape(got, f.quantified));
	// On a pure term the piece is closed and folds:
	// `∃x. x·y ∪ x′·w = 0` is `y·w = 0`, which the checker reads.
	fixture p = split("ex x (x y | x' w = 0).");
	const tref pg = pushed(p, keep_all);
	CHECK(are_nso_equivalent<node_t>(pg, parse("y w = 0.")));
	CHECK(functional_count(pg) == 0);
	check_claims(pg, p);
}

TEST_CASE("B8: a callback keeping blocks but no chain quantifies at the emission") {
	// The chain is built and the callback declines it, so `DISCHARGE`
	// quantifies at once: the answer the non-keep run gives directly. `x`
	// is binate, so the block reaches the emission.
	fixture f = split("ex x (x y | x' w = 0).");
	auto keep_blocks = [](tref n) {
		return ap::strip_chain<node_t>(n).first.empty();
	};
	const tref want = pushed(f, ap::keep_no_functional<node_t>);
	const tref got = pushed(f, keep_blocks);
	CHECK(same(got, want));
	CHECK(functional_count(got) == 0);
	check_claims(got, f);
}

TEST_CASE("B9: a push past the acceptance bound is discarded for its input") {
	// With both knobs at zero every push is rejected, and what comes back
	// is the component's INPUT re-wrapped (invariant 3) with the
	// component-free conjuncts left outside the wrap. The same input under
	// the real knobs is accepted and resolves. `x` is binate, so the
	// component reaches the push.
	fixture f = split("ex x (x y | x' v = 0 && w = 0).");
	// The input is the converted matrix, and the result is compared once
	// spelled.
	const tref input = ap::prepare_terms<node_t>(f.blk.matrix);
	trefs dep, indep;
	for (tref m : ap::members<node_t>(input))
		(ap::fv_meets<node_t>(m, f.blk.vars) ? dep : indep)
			.push_back(m);
	REQUIRE(dep.size() == 1);
	REQUIRE(indep.size() == 1);
	indep.push_back(ap::rewrap<node_t>(
		ap::simplified_and_join<node_t>(dep), f.blk.vars));
	const tref want = finished(ap::simplified_and_join<node_t>(indep));
	{
		accept_guard g(0, 0);
		const tref got = pushed(f, ap::keep_no_functional<node_t>);
		CHECK(same(got, want));
		check_claims(got, f);
		CHECK(binder_count(got) == 1);
	}
	const tref got = pushed(f, ap::keep_no_functional<node_t>);
	check_claims(got, f);
	CHECK(binder_count(got) == 0);
}

// --- §5 DELETE_UNATE_BINDERS ----------------------------------------------------------

TEST_CASE("U1: unate binders leave in one substitution, a binate one stays") {
	// `x` only lowers the truth and goes to 0, `y` only raises it and goes
	// to 1, `z` moves it both ways and stays.
	fixture f = split("ex x ex y ex z ((x a = 0) && (b x' != 0) "
		"&& (y' c = 0) && (d y != 0) && (z e = 0) && (z' g = 0)).");
	const tref got = deleted(f);
	const ap::block left = ap::fv_intersect<node_t>(got, f.blk.vars);
	REQUIRE(left.size() == 1);
	CHECK(same(left[0], f.blk.vars[2]));
	check_claims(ap::rewrap<node_t>(got, left), f);
	CHECK(are_nso_equivalent<node_t>(ap::rewrap<node_t>(got, left),
		parse("(b != 0) && (d != 0) && ex z ((z e = 0) && (z' g = 0)).")));
}

TEST_CASE("U2: a binate variable comes back as it is") {
	// x ≠ 0 rises with x and x·a′ = 0 falls with it; an equation whose
	// ring sum is neither monotone nor antitone, two opposite literals and
	// a unit are binate too.
	for (const char* src : { "ex x ((x != 0) && (x a' = 0)).",
		"ex x ((x = a) && (b x != 0)).",
		"ex x ((x ^ a = 0) && (b x != 0)).",
		"ex x ((x a = 0) && (b x' = 0)).",
		"ex x ((x a = 0) && all w (x w != 0))." })
	{
		CAPTURE(src);
		fixture f = split(src);
		const tref m = ap::prepare_terms<node_t>(f.blk.matrix);
		CHECK(ap::delete_unate_binders<node_t>(m, f.blk.vars) == m);
	}
}

TEST_CASE("U3: a variable that only raises the truth goes to 1") {
	// ∃x (x ≠ 0 ∧ y·x ≠ 0) is y ≠ 0, at x := 1.
	fixture f = split("ex x ((x != 0) && (y x != 0)).");
	const tref got = deleted(f);
	CHECK(!ap::fv_meets<node_t>(got, f.blk.vars));
	check_claims(got, f);
	CHECK(are_nso_equivalent<node_t>(got, parse("y != 0.")));
}

TEST_CASE("U4: a variable that only lowers the truth goes to 0") {
	// ∃x (x·a = 0 ∧ x′·b ≠ 0) is b ≠ 0, at x := 0.
	fixture f = split("ex x ((x a = 0) && (x' b != 0)).");
	const tref got = deleted(f);
	CHECK(!ap::fv_meets<node_t>(got, f.blk.vars));
	check_claims(got, f);
	CHECK(are_nso_equivalent<node_t>(got, parse("b != 0.")));
}

TEST_CASE("U5: a block of more than 64 variables is read whole") {
	// Seventy variables that only lower the truth, but x67 also raises it.
	std::string src, body;
	for (size_t i = 0; i < 70; ++i) {
		src += "ex x" + std::to_string(i) + " ";
		body += std::string(i ? " && " : "") + "(x" + std::to_string(i)
			+ " a = 0)";
	}
	src += "(" + body + " && (x67' b = 0)).";
	fixture f = split(src.c_str());
	REQUIRE(f.blk.vars.size() == 70);
	const tref got = deleted(f);
	const ap::block left = ap::fv_intersect<node_t>(got, f.blk.vars);
	REQUIRE(left.size() == 1);
	CHECK(same(left[0], f.blk.vars[67]));
	CHECK(are_nso_equivalent<node_t>(ap::rewrap<node_t>(got, left),
		f.quantified));
}

TEST_CASE("U6: a ∀ run deletes its unate binder on the dualised matrix") {
	// ∀x (x·a ≠ 0 ∨ b = 0) dualises to ∃x (x·a = 0 ∧ ¬(b = 0)), where
	// x := 0 leaves ¬(b = 0), negated back to b = 0.
	fixture f = split("all x ((x a != 0) || (b = 0)).");
	const tref dual = ap::normalize_operators<node_t>(
		ap::canonically_factored_neg<node_t>(
			ap::prepare_terms<node_t>(f.blk.matrix)));
	CHECK(!ap::fv_meets<node_t>(ap::delete_unate_binders<node_t>(dual,
		f.blk.vars), f.blk.vars));
	const tref got = processed(f, ap::keep_no_functional<node_t>);
	check_claims(got, f);
	CHECK(binder_count(got) == 0);
	CHECK(are_nso_equivalent<node_t>(got, parse("b = 0.")));
}

// --- §5 PROCESS_BLOCK -----------------------------------------------------------------

TEST_CASE("D1: a ∀ run is pushed by dualisation") {
	// `∀x. x·y ≠ 0` is `¬∃x. x·y = 0`, and the ∃ push answers `T`, so the
	// outbound negation answers `F` — a real verdict, not a re-wrap.
	fixture f = split("all x (x y != 0).");
	const tref got = processed(f, ap::keep_no_functional<node_t>);
	check_claims(got, f);
	CHECK(tau::get(got).equals_F());
}

TEST_CASE("D2: what the ∃ push resolves comes back negated") {
	// `∀x. x·y = 0` dualises to `∃x. ¬(x·y = 0)`, where `x` only raises
	// the truth and leaves at block entry with `x := 1`, giving `¬(y = 0)`;
	// negated again, that is `y = 0`.
	fixture f = split("all x (x y = 0).");
	const tref got = processed(f, ap::keep_no_functional<node_t>);
	check_claims(got, f);
	CHECK(binder_count(got) == 0);
	CHECK(are_nso_equivalent<node_t>(finished(got), parse("y = 0.")));
}

TEST_CASE("D3: a ∀ run is answered through the push over its dualised "
	"disjunction") {
	// `∀x(x·y = 0 ∧ x·w ≠ 0)` dualises to `∃x(x·y ≠ 0 ∨ x·w = 0)`,
	// which 2d pushes per disjunct: `∃x(x·w = 0)` is `T` at `x := 0` and
	// decides the ∨-join, so the outbound negation answers `F` — and `F`
	// it is, since `x := 0` refutes `x·w ≠ 0`.
	fixture f = split("all x (x y = 0 && x w != 0).");
	const tref got = processed(f, ap::keep_no_functional<node_t>);
	check_claims(got, f);
	CHECK(binder_count(got) == 0);
	CHECK(tau::get(got).equals_F());
}

TEST_CASE("D4: an ∃ run goes straight to the push") {
	fixture f = split("ex x (x y = 0).");
	const tref got = processed(f, ap::keep_no_functional<node_t>);
	check_claims(got, f);
	CHECK(same(got, pushed(f, ap::keep_no_functional<node_t>)));
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
	const tref got = finished(ap::process_all_blocks<node_t>(normalised(phi),
		ap::keep_no_functional<node_t>));
	check_claims(got, phi);
	CHECK(binder_count(got) == 0);
	CHECK(tau::get(got).equals_T());
}

TEST_CASE("A2: an alternating nest is answered innermost first") {
	// The inner clause is a conjunction no member of which is
	// disjunctive, so the conjunction push settles `b` whole and the leaf
	// gives `∃b(a·b = 0 ∧ b·c ≠ 0)` = `a′·c ≠ 0`. The ∀ run above it
	// then dualises that one literal, `∃a(a′·c = 0)` is `T` at `a := 1`,
	// and the outbound negation answers `F`.
	tref phi = parse("all a (ex b (a b = 0 && b c != 0)).");
	const tref got = finished(ap::process_all_blocks<node_t>(normalised(phi),
		ap::keep_no_functional<node_t>));
	check_claims(got, phi);
	CHECK(binder_count(got) == 0);
	CHECK(tau::get(got).equals_F());
}

TEST_CASE("A3: a run inside a conjunction is folded through the join") {
	// `∃x. x·y = 0` is `T`, and the join drops a `T` from the conjunction
	// the run stood in (invariant 6), leaving the other conjunct alone.
	tref phi = parse("(ex x (x y = 0)) && z = 0.");
	const tref got = finished(ap::process_all_blocks<node_t>(normalised(phi),
		ap::keep_no_functional<node_t>));
	check_claims(got, phi);
	CHECK(binder_count(got) == 0);
	CHECK(same(got, normalised(parse("z = 0."))));
}

TEST_CASE("A4: a run inside a disjunction is folded the same way") {
	// `∃x. x·y ≠ 0` is `y ≠ 0`, which takes the run's place among the
	// disjunction's members.
	tref phi = parse("(ex x (x y != 0)) || z = 0.");
	const tref got = finished(ap::process_all_blocks<node_t>(normalised(phi),
		ap::keep_no_functional<node_t>));
	check_claims(got, phi);
	CHECK(binder_count(got) == 0);
}

TEST_CASE("A5: a shared run head is processed once") {
	// `(∃x.ψ) ∧ ∀y.((∃x.ψ) ∨ χ)` with ONE node for the two `∃x.ψ`, which
	// the normalisation keeps shared. The second occurrence sits inside
	// the ∀ run's matrix, so a NESTED walk meets it; the pass memo spans
	// the walks and answers it, so the callback is asked for `ψ`'s ONE
	// component once and not once per occurrence — a second processing
	// would show up here as a second ask. The ∀ run adds none: 2b resolves
	// `ψ` to `T`, which decides the disjunction it stands in, and a matrix
	// that is a constant has no component to ask about.
	const tref psi = parse("ex x (x a = 0 || x' c = 0).");
	const tref borrowed = parse("all y (y b = 0).");
	const tref phi = tau::build_wff_and(psi,
		tau::build_wff_all(tau::trim_right_sibling(
				ap::binder_var<node_t>(borrowed)),
			tau::build_wff_or(psi, matrix_of(borrowed, 1)),
			false));
	size_t asked = 0;
	auto count = [&asked](tref) { ++asked; return false; };
	const tref got = finished(ap::process_all_blocks<node_t>(normalised(phi),
		count));
	CHECK(asked == 1);
	check_claims(got, phi);
	CHECK(binder_count(got) == 0);
}

TEST_CASE("A6: a run under a temporal operator is processed") {
	// The walk enters a temporal operator like any other formula node, so
	// the block below one is reached, and its `x`, which only lowers the
	// truth, leaves at block entry with `x := 0`.
	tref phi = parse("always ex x (x y = 0).");
	const tref got = finished(ap::process_all_blocks<node_t>(normalised(phi),
		ap::keep_no_functional<node_t>));
	CHECK(binder_count(got) == 0);
	CHECK(invariant_4(got));
	CHECK(!holds(got, tau::BDD_ID));
	CHECK(no_escape(finished(got), phi));
	CHECK(are_nso_equivalent<node_t>(finished(got), phi));
}

TEST_CASE("A7: a formula with no formula binder comes back as the conversion leaves it") {
	// Every step of the pass is then the identity, but for the re-join of
	// the conjunction, whose converted members sort by their BDDs.
	const tref phi = normalised(parse("x y = 0 && z = 0."));
	REQUIRE(binder_count(phi) == 0);
	const tref got = ap::process_all_blocks<node_t>(phi,
		ap::keep_no_functional<node_t>);
	CHECK(got == ap::simplified_and_join<node_t>(
		ap::members<node_t>(ap::prepare_terms<node_t>(phi))));
}

TEST_CASE("A8: a term is never entered, so a functional quantifier meets the conversion alone") {
	// The only quantifier here is TERM-level, and the pass stops at a term
	// (§4): resolving a chain belongs to the phases before this one. The
	// pass's entry canonicalises the binder ids, the chain's included, and
	// the conversion slides the chain (§1). Its piece keeps `z` free inside
	// a reference, so the chain stands and is spelled back as it was.
	const tref x = tau::trim(bvar("x"));
	const tref chain = tb::build_functional_quantifiers({{ x, tb::all }},
		tau::build_bf_or(bvar("x"), qref(bvar("z"))));
	const tref phi = normalised(tau::build_bf_eq_0(
		tau::build_bf_and(bvar("a"), chain)));
	REQUIRE(functional_count(phi) == 1);
	const tref got = finished(ap::process_all_blocks<node_t>(phi,
		ap::keep_no_functional<node_t>));
	CHECK(got == ap::canonicalise_binder_ids<node_t>(phi));
	CHECK(functional_count(got) == 1);
	// A CLOSED piece folds in the slide: `∀x(x ∪ z)` is `z`, so the atom is
	// spelled `a·z = 0`.
	const tref closed = tb::build_functional_quantifiers({{ x, tb::all }},
		tau::build_bf_or(bvar("x"), bvar("z")));
	const tref psi = normalised(tau::build_bf_eq_0(
		tau::build_bf_and(bvar("a"), closed)));
	REQUIRE(functional_count(psi) == 1);
	const tref folded = finished(ap::process_all_blocks<node_t>(psi,
		ap::keep_no_functional<node_t>));
	CHECK(functional_count(folded) == 0);
	CHECK(same(folded, normalised(tau::build_bf_eq_0(
		tau::build_bf_and(bvar("a"), bvar("z"))))));
}

TEST_CASE("A9: an ∃ over ∀ over ∃ nest is walked innermost first") {
	// The pass reaches the innermost run through both enclosing ones and
	// answers it first: `∃c` is distributed over the disjunction (2d),
	// where the `a·b = 0` disjunct is c-free and rides along while
	// `∃c(c + d = 0)` is `T` at `c := d` and decides the ∨-join. That `T`
	// is the matrix of both runs above, so neither has a component left
	// to ask the callback about, and phase 5 drops their binders.
	tref phi = parse("ex a (all b (ex c (a b = 0 || c ^ d = 0))).");
	size_t asked = 0;
	auto count = [&asked](tref) { ++asked; return false; };
	const tref got = finished(ap::process_all_blocks<node_t>(normalised(phi),
		count));
	CHECK(asked == 1);
	check_claims(got, phi);
	CHECK(binder_count(got) == 0);
	CHECK(tau::get(got).equals_T());
}

} // TEST_SUITE
