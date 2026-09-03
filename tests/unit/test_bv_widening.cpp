// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include <algorithm>
#include <string>
#include <vector>

#include "boolean_algebras/bv_ba.h"
#include "bv_widening.h"
#include "heuristics/bv_predicate_blasting.h"

using tau_api = api<node_t>;

TEST_SUITE("Configuration") {

	TEST_CASE("logging") {
		logging::trace();
	}

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

// Parses `term` as a bare `bf` (no type inference: variables are left
// untyped and needed_width is given base_w explicitly), with hooks OFF so
// the raw parse shape (e.g. `bf_parenthesis` wrappers) is stable across
// parser regens.
static size_t widths(const std::string& term, size_t base_w, size_t& maxW) {
	auto opts = tau::get_options{
		.parse = { .start = tau::bf },
		.infer_ba_types = false,
		.reget_with_hooks = false
	};
	tref src = tree<node_t>::get(term, opts);
	REQUIRE(src != nullptr);
	maxW = 0;
	return needed_width<node_t>(src, base_w, maxW);
}

// Like widths(), but with type inference ON. A `{n}:bv[k]` literal's own
// numeric value/type is only attached to its `ba_constant` node by
// `infer_ba_types` (tau_tree_from_parser.tmpl.h: `bf_cast` is the only node
// special-cased to get its type "for free" straight from the parse tree,
// independent of inference -- an ordinary `ba_constant`'s `typed` suffix is
// just a raw child node until inference folds it into `.get_ba_type()`).
// `widths()` above disables inference so bare variables can stay untyped;
// that leaves any `ba_constant` in the sample without a usable type. Samples
// that read a literal's own value (the `bf_shl` constant-amount case) use
// this helper instead, and must annotate every variable explicitly so
// inference has no untyped bv-only operand to reject.
static size_t widths_typed(const std::string& term, size_t base_w, size_t& maxW) {
	auto opts = tau::get_options{
		.parse = { .start = tau::bf },
		.reget_with_hooks = false
	};
	tref src = tree<node_t>::get(term, opts);
	REQUIRE(src != nullptr);
	maxW = 0;
	return needed_width<node_t>(src, base_w, maxW);
}

TEST_SUITE("bv widening - needed_width") {

	TEST_CASE("leaf variable is base width") {
		size_t m; CHECK(widths("x", 8, m) == 8); CHECK(m == 8);
	}
	TEST_CASE("add is max+1")      { size_t m; CHECK(widths("x + y", 8, m) == 9);  CHECK(m == 9);  }
	TEST_CASE("mul is sum")        { size_t m; CHECK(widths("x * y", 8, m) == 16); CHECK(m == 16); }
	TEST_CASE("sub is max")        { size_t m; CHECK(widths("x - y", 8, m) == 8);  }
	TEST_CASE("div/mod keep left") { size_t m; CHECK(widths("(x * y) / z", 8, m) == 16); CHECK(m == 16); }
	TEST_CASE("nested add chain")  { size_t m; CHECK(widths("x + y + z", 8, m) == 10); }
	TEST_CASE("min/max is max of children") { size_t m; CHECK(widths("min(x * y, z)", 8, m) == 16); }
	TEST_CASE("bitwise is max of children (runs at W)") {
		size_t m; CHECK(widths("(x * y) & z", 8, m) == 16);
	}
	TEST_CASE("shr never grows")   { size_t m; CHECK(widths("(x + y) >> z", 8, m) == 9); }
	TEST_CASE("interior maxW survives a shrinking parent") {
		// (x*y) % z has root width 16 = interior max
		size_t m; widths("(x * y) % z", 8, m); CHECK(m == 16);
	}
	TEST_CASE("user cast is a boundary") {
		// ((bv[16]) x) contributes its own target width (16), not x's base_w
		// (8) or the sibling's width -- bv[16] must dominate max(16, 8) + 1
		// = 17. (bv[4] would be masked by the wider sibling: max(4, 8) + 1
		// happens to equal max(8, 8) + 1, so it wouldn't catch an
		// implementation that ignores the cast boundary and just reuses
		// base_w for x.)
		size_t m; CHECK(widths("((bv[16]) x) + y", 8, m) == 17);
	}
	TEST_CASE("constant shift amount adds to the width") {
		size_t m; CHECK(widths_typed("x:bv[8] << { 3 }:bv[8]", 8, m) == 11);
	}
	TEST_CASE("variable shift amount never grows") {
		size_t m; CHECK(widths("x << y", 8, m) == 8);
	}
	TEST_CASE("opaque child (capture) propagates as 0") {
		// `$X` parses to a bare `capture` node under `bf` -- opaque to
		// needed_width (default: return 0). The whole bf_add is therefore
		// opaque too and returns 0; maxW is untouched by the opaque side
		// itself (its own recursive call returns before touching maxW) --
		// it still reflects the real sibling `y`'s width (8), since that
		// side is genuinely computed before the operator discovers its
		// other child is opaque and gives up.
		size_t m; CHECK(widths("$X + y", 8, m) == 0); CHECK(m == 8);
	}
	TEST_CASE("maxW survives a discarded (non-kept) branch") {
		// bf_mod keeps only the left operand's width (8), discarding the
		// right operand (y * z, width 16) from the return value -- but
		// maxW must still record that peak.
		size_t m; CHECK(widths("x % (y * z)", 8, m) == 8); CHECK(m == 16);
	}
}

// ---------------------------------------------------------------------------
// widen_term / widen_atom
// ---------------------------------------------------------------------------

namespace {

// Parses `s` as a wff with hooks ON (the real pipeline's construction mode --
// contrast with needed_width's tests above, which deliberately use hooks OFF
// for raw-parse-shape stability) and type inference ON (default), so leaves
// end up genuinely typed and the atom node itself carries the unified type
// via the construction-time auto-propagation (tree<node>::get's `get_type`
// short-circuit-on-first-nonzero-child logic, tau_tree.tmpl.h:404-412) --
// exactly the tree shape widen_atom is meant to run on in the real pipeline.
tref parse_wff(const std::string& s) {
	auto opts = tau::get_options{
		.parse = { .start = tau::wff },
		.reget_with_hooks = true
	};
	tref src = tree<node_t>::get(s, opts);
	REQUIRE(src != nullptr);
	return src;
}

// Finds the (unique, for these samples) node of nonterminal `nt` -- e.g. the
// bf_eq/bf_lteq/... node itself, not the enclosing wff wrapper `build_bf_eq`
// (etc.) puts around it. This is exactly the tref widen_atom expects.
tref find_atom(tref src, node_t::type nt) {
	tref a = tree<node_t>::get(src).find_top(is<node_t>(nt));
	REQUIRE(a != nullptr);
	return a;
}

// Collects the target width (get_bv_width of its own ba_type) of every
// bf_cast descendant (self included) of `t`, in pre-order.
void collect_cast_widths(tref t, std::vector<size_t>& out) {
	const auto& n = tree<node_t>::get(t);
	if (n.is(tau::bf_cast)) out.push_back(get_bv_width<node_t>(n.get_ba_type()));
	for (tref ch : n.get_children()) collect_cast_widths(ch, out);
}

// Sets bv_max_width for the lifetime of the enclosing scope and restores
// whatever value it had before, even on a REQUIRE-failure stack unwind --
// mirrors bv_widening_scope in test_bv_ba_hooks.cpp (see the memory note on
// RAII guards for these globals).
struct bv_max_width_scope {
	size_t prev;
	explicit bv_max_width_scope(size_t w) : prev(bv_max_width) {
		bv_max_width = w;
	}
	~bv_max_width_scope() { bv_max_width = prev; }
};

// Sets bv_widening for the lifetime of the enclosing scope and restores
// whatever value it had before, even on a REQUIRE-failure stack unwind.
// Local copy of the RAII guard in test_bv_ba_hooks.cpp -- widen_atom's own
// tests never needed it (widen_atom doesn't consult the flag itself; only
// widen_bv_arithmetic does), but the whole-formula pass tests below do.
struct bv_widening_scope {
	bool prev;
	bv_widening_scope() : prev(bv_widening) { bv_widening = true; }
	~bv_widening_scope() { bv_widening = prev; }
};

// Structural check for the "truncating assignment" atom shape (widen_atom
// step 4, exactly-one-bare-storage-side): exactly one side of `w` must be
// the untouched bare variable, structurally identical (subtree_equals --
// NOT raw tref ==, see the first assignment-truncation test's long comment
// for why: rebuilding a parent with one sibling replaced changes the
// untouched child's own hash-consed tref in this codebase's
// left-child/right-sibling storage, even though nothing about the child
// itself changed) to `bare_ref`; found position-agnostically since neither
// rule nor test should assume which side stays bare. The other side must
// be a single outer bf_cast to `base_w`. Checks the atom's own type resets
// to `base_w` too (auto-propagated from the untouched bare side).
void check_truncating_assignment_shape(tref w, tref bare_ref, size_t base_w) {
	const auto& wn = tree<node_t>::get(w);
	INFO("elaborated atom: " << wn.to_str());
	const bool child0_is_var =
		tree<node_t>::get(wn.child(0))[0].value.nt == tau::variable;
	const bool child1_is_var =
		tree<node_t>::get(wn.child(1))[0].value.nt == tau::variable;
	REQUIRE(child0_is_var != child1_is_var); // exactly one bare side
	tref bare_side = child0_is_var ? wn.child(0) : wn.child(1);
	tref cast_side = child0_is_var ? wn.child(1) : wn.child(0);
	CHECK(tree<node_t>::subtree_equals(bare_side, bare_ref)); // untouched

	const auto& rhs_op = tree<node_t>::get(cast_side)[0];
	CHECK(rhs_op.value.nt == tau::bf_cast);
	CHECK(get_bv_width<node_t>(rhs_op.get_ba_type()) == base_w);
	CHECK(wn.get_ba_type() == bv_type_id<node_t>(base_w));
}

} // namespace

TEST_SUITE("bv widening - atom elaboration shapes") {

	TEST_CASE("comparison: both sides extended, no truncation") {
		// x*y <= z at bv[8]: needed W = 16 (mul: 8+8; z stays bare at 8).
		// Comparisons never truncate: expect casts to bv[16] on x, y and z,
		// the mul node itself typed bv[16], and the whole atom typed
		// bv[16] too (no reset back to base_w -- see the idempotency case
		// below for why that only happens for the truncating-assignment
		// shape).
		tref src = parse_wff("x:bv[8] * y <= z");
		tref atom = find_atom(src, tau::bf_lteq);
		tref w = widen_atom<node_t>(atom);
		REQUIRE(w != nullptr);
		CHECK(w != atom);

		CHECK(tree<node_t>::get(w).get_ba_type()
			== bv_type_id<node_t>(16));
		tref mulnode = tree<node_t>::get(w).find_top(is<node_t>(tau::bf_mul));
		REQUIRE(mulnode != nullptr);
		CHECK(get_bv_width<node_t>(tree<node_t>::get(mulnode).get_ba_type())
			== 16);

		std::vector<size_t> widths_found;
		collect_cast_widths(w, widths_found);
		CHECK(widths_found.size() == 3); // x, y, z each wrapped once
		for (size_t cw : widths_found) CHECK(cw == 16);

		CHECK(tree<node_t>::get(w).to_str().find("(bv[16])")
			!= std::string::npos);
	}

	TEST_CASE("assignment truncation: o = min(x*y, k)") {
		// o:bv[8] = min(x*y, {200}): needed W = 16 (mul: 8+8; min keeps
		// max(16, 8) = 16). o is bare storage -- untouched; the RHS is
		// elaborated at 16 and wrapped in one truncating (bv[8]) cast.
		// Exact elaborated shape (Task 5/6 reviewers: pin against this):
		//   o = (bv[8]) min((bv[16]) x * (bv[16]) y, {200})
		// {200} is NOT printed with a "(bv[16])" cast prefix: it is a
		// compile-time-constant operand, and the construction-time
		// term_cast hook folds "(bv[16]) {200}" directly into a plain
		// ba_constant re-typed to bv[16] (numeric value unchanged by
		// zero-extension) rather than leaving a literal bf_cast wrapper
		// node around it -- so only x and y (genuine variables, which
		// cannot be constant-folded) keep a visible upcast.
		tref src = parse_wff("o:bv[8] = min(x * y, {200})");
		tref atom = find_atom(src, tau::bf_eq);
		tref o_side = tree<node_t>::get(atom).child(0);
		tref w = widen_atom<node_t>(atom);
		REQUIRE(w != nullptr);
		CHECK(w != atom);

		const auto& wn = tree<node_t>::get(w);
		INFO("elaborated atom: " << wn.to_str());

		// Exactly one side must be the untouched bare variable o
		// (structurally identical to o_side); the other must be the
		// outer truncating (bv[8]) cast. Checked position-agnostically
		// (rather than assuming o stays at child(0)) so a genuine
		// reordering elsewhere in the pipeline can't produce a false
		// failure here.
		//
		// Structural (subtree_equals), not raw tref (==): confirmed via
		// a controller run that the elaborated shape is exactly right
		// (o = (bv[8]) min(((bv[16]) x*(bv[16]) y), { 200 }:bv[16])) but
		// o_side, captured from `atom` before widen_atom ran, is not
		// pointer-identical to the untouched `o` found inside `w` even
		// though both are semantically "the same o" -- tree<node>'s
		// hash-consing normally guarantees pointer identity for
		// structurally-equal nodes, but apparently doesn't here (open
		// question, not this task's implementation to chase -- see the
		// task-4 report). subtree_equals is the codebase's own answer to
		// exactly this: compare by structure, not by address.
		const bool child0_is_var =
			tree<node_t>::get(wn.child(0))[0].value.nt == tau::variable;
		const bool child1_is_var =
			tree<node_t>::get(wn.child(1))[0].value.nt == tau::variable;
		REQUIRE(child0_is_var != child1_is_var); // exactly one bare side
		tref bare_side = child0_is_var ? wn.child(0) : wn.child(1);
		tref cast_side = child0_is_var ? wn.child(1) : wn.child(0);
		CHECK(tree<node_t>::subtree_equals(bare_side, o_side)); // untouched

		const auto& rhs_op = tree<node_t>::get(cast_side)[0];
		CHECK(rhs_op.value.nt == tau::bf_cast);
		CHECK(get_bv_width<node_t>(rhs_op.get_ba_type()) == 8);
		// the atom itself resets to bv[8] (base_w), via auto-propagation
		// from the untouched, still-bv[8] bare side
		CHECK(wn.get_ba_type() == bv_type_id<node_t>(8));

		std::vector<size_t> widths_found;
		collect_cast_widths(w, widths_found);
		// 1 truncating cast (bv[8]) + 2 leaf upcasts (x, y at bv[16]).
		// {200} is NOT wrapped in a persisting bf_cast: term_cast's
		// construction-time hook folds a cast of a constant into a
		// directly re-typed bv[16] ba_constant (the same constant-folding
		// this module's Trap 1 documents), so it never shows up as a
		// bf_cast node here.
		CHECK(widths_found.size() == 3);
		CHECK(std::count(widths_found.begin(), widths_found.end(), 8) == 1);
		CHECK(std::count(widths_found.begin(), widths_found.end(), 16) == 2);

		std::string printed = wn.to_str();
		CHECK(printed.find("(bv[8])") != std::string::npos);
		CHECK(printed.find("(bv[16])") != std::string::npos);
	}

	TEST_CASE("both-compound equality: exact, no truncating cast") {
		// x*y = z*k at bv[8]: neither side is bare storage, so the
		// truncating-assignment rule never applies even though the atom
		// is a bf_eq -- both sides are extended exactly, like a comparison.
		tref src = parse_wff("x:bv[8] * y = z * k");
		tref atom = find_atom(src, tau::bf_eq);
		tref w = widen_atom<node_t>(atom);
		REQUIRE(w != nullptr);
		CHECK(w != atom);
		CHECK(tree<node_t>::get(w).get_ba_type() == bv_type_id<node_t>(16));

		std::vector<size_t> widths_found;
		collect_cast_widths(w, widths_found);
		CHECK(widths_found.size() == 4); // x, y, z, k each wrapped once
		for (size_t cw : widths_found) CHECK(cw == 16); // never 8: no truncation
	}

	TEST_CASE("no arithmetic atom: returned unchanged (same tref)") {
		// x = y, both bare: needed W == base_w (8) on both sides, so there
		// is nothing to elaborate -- widen_atom must be a strict no-op,
		// returning the identical tref (not just an equal-shaped rebuild).
		tref src = parse_wff("x:bv[8] = y");
		tref atom = find_atom(src, tau::bf_eq);
		CHECK(widen_atom<node_t>(atom) == atom);
	}

	TEST_CASE("cap exceeded returns nullptr") {
		// Intentionally triggers widen_atom's LOG_ERROR (D4 cap message,
		// "bv-widening: required width 16 exceeds bv-max-width 12") --
		// that log line in the test-run output is expected noise from
		// this exact case, not a defect.
		bv_max_width_scope cap(12); // x*y at bv[8] needs 16 > 12
		tref src = parse_wff("o:bv[8] = min(x * y, z)");
		tref atom = find_atom(src, tau::bf_eq);
		CHECK(widen_atom<node_t>(atom) == nullptr);
	}

	TEST_CASE("idempotent (assignment shape): widen_atom(widen_atom(a)) == widen_atom(a)") {
		// Uses the truncating-assignment shape: needed_width on the
		// rebuilt RHS sees only the outer (bv[8]) cast (a boundary, per
		// the cast rule) and never descends into the wide internals, so
		// the second call recomputes W == base_w == 8 and is a no-op --
		// see the doc comment on widen_atom for why this specifically
		// relies on the truncating cast resetting the atom's own type
		// (rather than the is_side_saturated_at guard, which never
		// matches this shape at all -- the untouched bare side always
		// fails it).
		tref src = parse_wff("o:bv[8] = min(x * y, {200})");
		tref atom = find_atom(src, tau::bf_eq);
		tref w1 = widen_atom<node_t>(atom);
		REQUIRE(w1 != nullptr);
		CHECK(w1 != atom); // sanity: the first call did real work
		tref w2 = widen_atom<node_t>(w1);
		REQUIRE(w2 != nullptr);
		CHECK(w2 == w1);
	}

	TEST_CASE("idempotent (extend-all shape): widen_atom(widen_atom(a)) == widen_atom(a)") {
		// Regression test for a Critical review finding: comparisons (and
		// bf_interval, and both-compound equality) never truncate, so
		// nothing resets the atom's own type back to base_w the way the
		// assignment shape's outer cast does -- re-running needed_width
		// naively on an already-widened "x*y <= z" would see two
		// already-(bv[16])-cast mul operands and recompute 16+16=32,
		// inflating W without bound on every re-application. Task 6 calls
		// this pass inside normalize_non_temp, which runs every step, so
		// this MUST be a true no-op, not just "eventually settles" or
		// "hits the cap". The is_side_saturated_at guard is what makes
		// this hold: on the second call it recognizes every side is
		// already uniformly bv[16] and returns w1 immediately, before
		// needed_width ever runs again.
		tref src = parse_wff("x:bv[8] * y <= z");
		tref atom = find_atom(src, tau::bf_lteq);
		tref w1 = widen_atom<node_t>(atom);
		REQUIRE(w1 != nullptr);
		CHECK(w1 != atom); // sanity: the first call did real work
		CHECK(tree<node_t>::get(w1).get_ba_type() == bv_type_id<node_t>(16));
		tref w2 = widen_atom<node_t>(w1);
		REQUIRE(w2 != nullptr);
		CHECK(w2 == w1);
		// Three, then four, applications: confirm it truly saturates
		// rather than merely surviving one extra round.
		tref w3 = widen_atom<node_t>(w2);
		REQUIRE(w3 != nullptr);
		CHECK(w3 == w1);
	}

	TEST_CASE("bf_interval: all three sides extended together") {
		// bf_interval only survives past construction as a literal 3-child
		// node when hooks are off (with hooks on, the wff hook always
		// splits it into wff_and(bf_lteq, bf_lteq) before widen_atom could
		// ever see it -- see the task-4 report for the full trace); this
		// is a defensive/structural test of the interval branch, not a
		// claim that this shape occurs in the real hooks-on pipeline.
		auto opts = tau::get_options{
			.parse = { .start = tau::wff },
			.reget_with_hooks = false
		};
		tref src = tree<node_t>::get("x <= y:bv[8] * z <= k", opts);
		REQUIRE(src != nullptr);
		tref atom = find_atom(src, tau::bf_interval);
		tref w = widen_atom<node_t>(atom);
		REQUIRE(w != nullptr);
		CHECK(w != atom);
		CHECK(tree<node_t>::get(w).is(tau::bf_interval));
		CHECK(tree<node_t>::get(w).children_size() == 3);

		std::vector<size_t> widths_found;
		collect_cast_widths(w, widths_found);
		CHECK(widths_found.size() == 4); // x, y, z, k each wrapped once
		for (size_t cw : widths_found) CHECK(cw == 16);
	}

	TEST_CASE("assignment truncation, bare side on the right: min(x*y, k) = o") {
		// Same rule as "assignment truncation" above, but the bare storage
		// side is written on the RIGHT this time (min(x*y,{200}) = o,
		// rather than o = min(x*y,{200})) -- is_bare_storage_side is
		// checked on both sides independently and the truncating cast is
		// placed on whichever side is NOT bare, so orientation must not
		// matter. check_truncating_assignment_shape finds the bare side
		// position-agnostically, so this pins that guarantee explicitly.
		tref src = parse_wff("min(x * y, {200}) = o:bv[8]");
		tref atom = find_atom(src, tau::bf_eq);
		tref o_side = tree<node_t>::get(atom).child(1); // o is written second
		tref w = widen_atom<node_t>(atom);
		REQUIRE(w != nullptr);
		CHECK(w != atom);
		check_truncating_assignment_shape(w, o_side, 8);

		std::vector<size_t> widths_found;
		collect_cast_widths(w, widths_found);
		CHECK(widths_found.size() == 3); // x, y at bv[16] + 1 truncating bv[8]
		CHECK(std::count(widths_found.begin(), widths_found.end(), 8) == 1);
		CHECK(std::count(widths_found.begin(), widths_found.end(), 16) == 2);
	}

	TEST_CASE("assignment truncation, bf_neq: o != min(x*y, k)") {
		// Same rule, bf_neq instead of bf_eq -- widen_atom's step-4 guard
		// checks `nt == bf_eq || nt == bf_neq` explicitly; this pins that
		// bf_neq is not accidentally left out, and that the rebuilt atom
		// keeps its own nt (bf_neq, not silently turned into bf_eq).
		tref src = parse_wff("o:bv[8] != min(x * y, {200})");
		tref atom = find_atom(src, tau::bf_neq);
		tref o_side = tree<node_t>::get(atom).child(0);
		tref w = widen_atom<node_t>(atom);
		REQUIRE(w != nullptr);
		CHECK(w != atom);
		CHECK(tree<node_t>::get(w).is(tau::bf_neq)); // nt preserved
		check_truncating_assignment_shape(w, o_side, 8);
	}

	TEST_CASE("shl atom: constant shift amount is upcast, value preserved") {
		// x:bv[8] << {3} <= z, a comparison (extend-all shape): needed
		// W = 11 (bf_shl: l=8, +3 for the literal amount). Both the
		// shifted operand AND the (constant) shift amount must end up at
		// bv[11] -- the amount is "upcast to W like any leaf" per the
		// design (D2/D3 amendment): its VALUE (3) is unchanged by
		// zero-extension, only its declared width grows.
		tref src = parse_wff("x:bv[8] << {3} <= z");
		tref atom = find_atom(src, tau::bf_lteq);
		tref w = widen_atom<node_t>(atom);
		REQUIRE(w != nullptr);
		CHECK(w != atom);
		CHECK(tree<node_t>::get(w).get_ba_type() == bv_type_id<node_t>(11));

		tref shl_node = tree<node_t>::get(w).find_top(is<node_t>(tau::bf_shl));
		REQUIRE(shl_node != nullptr);
		const auto& shl = tree<node_t>::get(shl_node);
		CHECK(get_bv_width<node_t>(shl.get_ba_type()) == 11);

		// The shifted operand (x) is cast to bv[11].
		const auto& x_op = tree<node_t>::get(shl.child(0))[0];
		CHECK(x_op.value.nt == tau::bf_cast);
		CHECK(get_bv_width<node_t>(x_op.get_ba_type()) == 11);

		// The shift amount: find the ba_constant wherever it ended up --
		// either still wrapped in a literal bf_cast (if not folded) or
		// directly re-typed (if term_cast's constant-folding hook folded
		// it away, as it did for {200} in the assignment-truncation
		// tests above) -- either way its VALUE must still read "3".
		tref amt_const = tree<node_t>::get(shl.child(1))
			.find_top(is<node_t>(tau::ba_constant));
		REQUIRE(amt_const != nullptr);
		const auto& amt = tree<node_t>::get(amt_const);
		CHECK(get_bv_width<node_t>(amt.get_ba_type()) == 11);
		CHECK(std::get<bv>(amt.get_ba_constant()).getBitVectorValue(10) == "3");
	}

	TEST_CASE("div atom: both operands widened, not just the dividend") {
		// x*y/z <= k: `*`/`/`/`%` are same-precedence and left-associative
		// in this grammar (parser/tau.tgf:107-109, the right operand of
		// each is explicitly excluded from being a bare bf_mul/bf_div/
		// bf_mod, forcing left grouping) so this parses as (x*y)/z with no
		// parens needed. Needed W = 16 (bf_div keeps only the dividend's
		// width for the RESULT -- max(x*y)=16 -- but widen_term has no
		// per-operator exceptions: it widens EVERY child uniformly, so
		// the divisor z must also end up cast to bv[16] even though
		// needed_width's own formula never used its width in computing W.
		// This is the "both operands at W" property from the amended
		// D2/D3 rule (ALL operators run at W).
		tref src = parse_wff("x:bv[8] * y / z <= k");
		tref atom = find_atom(src, tau::bf_lteq);
		tref w = widen_atom<node_t>(atom);
		REQUIRE(w != nullptr);
		CHECK(w != atom);
		CHECK(tree<node_t>::get(w).get_ba_type() == bv_type_id<node_t>(16));

		tref div_node = tree<node_t>::get(w).find_top(is<node_t>(tau::bf_div));
		REQUIRE(div_node != nullptr);
		const auto& div = tree<node_t>::get(div_node);
		CHECK(get_bv_width<node_t>(div.get_ba_type()) == 16);
		// dividend (x*y): a bf_mul somewhere in this side, retyped bv[16]
		// (found via find_top rather than assumed at a fixed depth, so a
		// transparent bf_parenthesis wrapper, if any survives, can't
		// break this check).
		tref mul_node = tree<node_t>::get(div.child(0))
			.find_top(is<node_t>(tau::bf_mul));
		REQUIRE(mul_node != nullptr);
		CHECK(get_bv_width<node_t>(tree<node_t>::get(mul_node).get_ba_type())
			== 16);
		// divisor z: a leaf, so widening it means wrapping it in a cast
		const auto& divisor_op = tree<node_t>::get(div.child(1))[0];
		CHECK(divisor_op.value.nt == tau::bf_cast);
		CHECK(get_bv_width<node_t>(divisor_op.get_ba_type()) == 16);
	}

	TEST_CASE("opaque side: returned unchanged (same tref)") {
		// ((bv[8]) x) + $X <= z, parsed WITHOUT type inference
		// (infer_ba_types = false): with inference ON, a Debug-only
		// consistency check in tau_tree_from_parser.tmpl.h ("Check that
		// all term nodes have been typed") DBG-aborts, because a capture
		// ($X) is never assigned a type by inference at all -- it is not
		// in the "typeable" node-kind set inference collects
		// (ba_types_inference.tmpl.h's get_typeable_type_ids_by_type only
		// ever looks at ref/variable/ba_constant/bf_t/bf_f) -- so it is
		// left untyped post-inference, which that Debug check treats as a
		// bug. Task 3's own needed_width "$X + y" test sidesteps the same
		// check the same way (.infer_ba_types = false); with inference
		// off, the check does not run at all, for anything.
		//
		// Turning inference off means bare `x`/`z` stay untyped too (only
		// a ba_constant's or a bf_cast's OWN `[ typed ]` suffix is folded
		// "for free" straight from the parse tree, independent of
		// inference -- see bv_widening.h's needed_width doc comment and
		// the "user cast is a boundary" needed_width test above), so an
		// explicit `(bv[8])` cast on x is what gives this atom a
		// bv-family type at all: construction-time auto-propagation
		// (tree<node>::get's get_type, tied to hooks, not to inference)
		// picks up bv[8] from the cast, all the way up through the
		// bf_add and the bf_lteq atom itself.
		//
		// $X itself is opaque to needed_width (bv_widening.h's doc
		// comment lists capture alongside bf_ref as the canonical opaque
		// examples) -- the whole bf_add side is therefore opaque too, and
		// widen_atom must skip the atom entirely rather than partially
		// elaborate it. (This also exercises is_side_saturated_at's own
		// opaque-node guard on the way there: without it, checking
		// whether the bf_add side is "already saturated" would call
		// get_bv_width on $X's ba_type 0 and DBG-assert too.)
		auto opts = tau::get_options{
			.parse = { .start = tau::wff },
			.infer_ba_types = false
		};
		tref src = tree<node_t>::get("((bv[8]) x) + $X <= z", opts);
		REQUIRE(src != nullptr);
		tref atom = find_atom(src, tau::bf_lteq);
		// Sanity: confirm the premise (a genuinely bv-family atom) before
		// asserting on the opaque-side behavior it's meant to exercise.
		REQUIRE(is_bv_type_family<node_t>(tree<node_t>::get(atom).get_ba_type()));
		CHECK(widen_atom<node_t>(atom) == atom);
	}

	TEST_CASE("not bv-family: returned unchanged (same tref)") {
		// Plain x = y with no bv annotation and no arithmetic: x, y
		// default to the non-bv (sbf) boolean algebra, so the atom's own
		// BA type is not bv-family at all -- widen_atom's very first
		// check must skip it, before needed_width or anything else runs.
		tref src = parse_wff("x = y");
		tref atom = find_atom(src, tau::bf_eq);
		CHECK(!is_bv_type_family<node_t>(tree<node_t>::get(atom).get_ba_type()));
		CHECK(widen_atom<node_t>(atom) == atom);
	}
}

// ---------------------------------------------------------------------------
// widen_bv_arithmetic
// ---------------------------------------------------------------------------

TEST_SUITE("bv widening - whole formula pass") {

	TEST_CASE("OFF mode: fm returned unchanged (same tref)") {
		// bv_widening is false by default and no scope guard turns it on
		// here -- widen_bv_arithmetic must be a pure pass-through no-op,
		// checked by pointer identity (hash-consing makes this a valid
		// check for an UNTOUCHED tree: no rewrite means no rebuild at all,
		// so there is no rebuilt-sibling tref-mismatch pitfall here the way
		// there is for widen_atom's own truncating-assignment tests).
		REQUIRE(!bv_widening);
		tref src = parse_wff("o:bv[8] = min(x * y, {200})"); // would genuinely widen if ON
		CHECK(widen_bv_arithmetic<node_t>(src) == src);
	}

	TEST_CASE("mixed formula: only the bv atom is rewritten, the sbf atom is left alone") {
		// (x*y <= z) && (a = b): the left conjunct is a genuine bv-family
		// comparison needing widening (W=16, as in widen_atom's own
		// "comparison" test above); the right conjunct, plain `a = b` with
		// no bv annotation, defaults to the non-bv (sbf) boolean algebra
		// (as in widen_atom's own "not bv-family" test) -- same atom nt
		// (bf_eq/bf_lteq both live in widen_bv_arithmetic's collected atom
		// set regardless of BA type), different BA type. Confirms the pass
		// collects atoms by nt across BA types and lets widen_atom's own
		// per-atom no-op check filter out the non-bv one.
		bv_widening_scope widen;
		tref src = parse_wff("(x:bv[8] * y <= z) && (a = b)");
		tref bv_atom = find_atom(src, tau::bf_lteq);
		tref sbf_atom = find_atom(src, tau::bf_eq);
		REQUIRE(is_bv_type_family<node_t>(tree<node_t>::get(bv_atom).get_ba_type()));
		REQUIRE(!is_bv_type_family<node_t>(tree<node_t>::get(sbf_atom).get_ba_type()));
		tref direct_widened_bv_atom = widen_atom<node_t>(bv_atom);
		REQUIRE(direct_widened_bv_atom != nullptr);
		CHECK(direct_widened_bv_atom != bv_atom); // sanity: real work to do

		tref w = widen_bv_arithmetic<node_t>(src);
		REQUIRE(w != nullptr);
		CHECK(w != src);

		// The bv side was rewritten to exactly what widen_atom itself
		// produces (structural comparison -- see widen_atom's own tests
		// for why raw tref equality is the wrong tool across a rebuild
		// that changes a sibling).
		tref w_bv_atom = tree<node_t>::get(w).find_top(is<node_t>(tau::bf_lteq));
		REQUIRE(w_bv_atom != nullptr);
		CHECK(tree<node_t>::subtree_equals(w_bv_atom, direct_widened_bv_atom));

		// The sbf side is untouched: still there, still exactly `a = b`,
		// still not bv-family.
		tref w_sbf_atom = tree<node_t>::get(w).find_top(is<node_t>(tau::bf_eq));
		REQUIRE(w_sbf_atom != nullptr);
		CHECK(tree<node_t>::subtree_equals(w_sbf_atom, sbf_atom));
		CHECK(!is_bv_type_family<node_t>(tree<node_t>::get(w_sbf_atom).get_ba_type()));
	}

	TEST_CASE("quantified formula: a bv atom nested under quantifiers is rewritten") {
		// all x:bv[8] all y all z (x*y <= z): today's inference unifies all
		// three variables to one width (bv[8], from x's own annotation),
		// keeping this a single-width sample -- the point of this test is
		// only that select_top finds the atom despite the wff_all
		// wrappers, not any cross-width interaction.
		bv_widening_scope widen;
		tref src = parse_wff("all x:bv[8] all y all z (x * y <= z)");
		tref atom = find_atom(src, tau::bf_lteq);
		REQUIRE(is_bv_type_family<node_t>(tree<node_t>::get(atom).get_ba_type()));
		tref direct = widen_atom<node_t>(atom);
		REQUIRE(direct != nullptr);
		CHECK(direct != atom); // sanity: real work to do

		tref w = widen_bv_arithmetic<node_t>(src);
		REQUIRE(w != nullptr);
		CHECK(w != src);

		// Semantic check, not a specific root nt: `wff` wraps its operator
		// exactly like `bf` wraps bf_add/etc. (parser/tau.tgf:44-48), so
		// the parsed/rewritten root is a `wff` node whose child(0) is
		// `wff_all` -- not `wff_all` itself. Rather than pin that one-level
		// wrapper shape (fragile if a normalization step or a different
		// parse path adds/removes wrapping), assert the two things that
		// actually matter here: all three quantifiers survive the pass,
		// and the bv atom nested underneath them was genuinely rewritten
		// to widen_atom's own result.
		//
		// select_all (not select_top) is required to count them: each
		// wff_all's own body is itself a `wff` wrapping the next wff_all,
		// so the three quantifiers are nested inside one another, not
		// siblings -- select_top stops descending into a matched node's
		// children ("we are only interested in the top nodes",
		// tree_rewriter.tmpl.h), so it would only ever find the outermost
		// one (x's) and silently miss y's and z's nested underneath it.
		trefs w_quantifiers = tree<node_t>::get(w).select_all(is<node_t>(tau::wff_all));
		CHECK(w_quantifiers.size() == 3);
		tref w_atom = tree<node_t>::get(w).find_top(is<node_t>(tau::bf_lteq));
		REQUIRE(w_atom != nullptr);
		CHECK(tree<node_t>::subtree_equals(w_atom, direct));
	}

	TEST_CASE("nullptr propagation: any atom hitting the D4 cap aborts the whole pass") {
		// Two conjuncts: the first would need W=16 (exceeds the capped
		// bv_max_width=12, exactly like widen_atom's own "cap exceeded"
		// test), the second is a perfectly fine, small bv atom (W=9, well
		// under the cap) that WOULD be rewritten successfully in isolation.
		// widen_bv_arithmetic must still return nullptr overall: one
		// atom's cap violation aborts the whole pass, not just its own
		// atom -- LOG_ERROR noise from the cap-violating atom is expected
		// here, exactly as in widen_atom's own cap test.
		bv_widening_scope widen;
		bv_max_width_scope cap(12); // x*y at bv[8] needs 16 > 12
		tref src = parse_wff("(o:bv[8] = min(x * y, z)) && (p:bv[8] = w + w2)");
		CHECK(widen_bv_arithmetic<node_t>(src) == nullptr);
	}
}

// ---------------------------------------------------------------------------
// Task 6: pipeline integration + end-to-end cvc5 semantics.
//
// Every case below sets `bv_widening = true` BEFORE parsing, not merely
// before calling widen_bv_arithmetic: term_add/term_sub/term_mul's
// fit-gated constant folding (Task 2) checks the SAME global flag at
// construction time, and deliberately leaves an overflowing constant
// operation (e.g. `{16}*{16}` at bv[8]) symbolic -- unfolded -- exactly
// when bv_widening is on, so that the tree still contains a genuine
// bf_mul/bf_add/bf_sub node for widen_bv_arithmetic's needed_width to see.
// Parsing the same source with the flag off folds it immediately via
// ordinary (wrapping) bv hardware semantics, which is what the "modular"
// half of each pair below relies on.
// ---------------------------------------------------------------------------

TEST_SUITE("bv widening - end-to-end semantics (cvc5)") {

	TEST_CASE("mul comparison no longer wraps") {
		bv_widening = true;
		// {16}*{16} = 256 -> modular bv[8] gives 0 <= 10 TRUE;
		// exact 256 <= 10 FALSE.
		auto fm = parse_wff("{ 16 }:bv[8] * { 16 }:bv[8] <= { 10 }:bv[8]");
		CHECK( !is_bv_formula_valid<node_t>(widen_bv_arithmetic<node_t>(fm)) );
		bv_widening = false;
		CHECK( is_bv_formula_valid<node_t>(fm) );
	}

	TEST_CASE("guard-free checked multiply") {
		bv_widening = true;
		// o:bv[8] = min({16}*{16}, {200}): exact min(256,200)=200 ->
		// the truncating (bv[8]) cast is lossless and o solves to 200.
		auto fm = parse_wff("o = min({ 16 }:bv[8] * { 16 }:bv[8], { 200 }:bv[8])");
		auto sol = solve_bv<node_t>(widen_bv_arithmetic<node_t>(fm));
		REQUIRE(sol.has_value());
		// `o` is the sole free variable (every other subterm is a
		// constant): the same size==1 idiom test_bv_ba-solver2.cpp uses,
		// then read the model's constant the way
		// bf_shl_shift_amount/test_bv_widening.cpp:499 already does
		// (getBitVectorValue(10) on the ba_constant's underlying bv).
		REQUIRE(sol.value().size() == 1);
		const tref val = sol.value().begin()->second;
		CHECK(std::get<bv>(tree<node_t>::get(val)[0].get_ba_constant())
			.getBitVectorValue(10) == "200");
		bv_widening = false;
	}

	TEST_CASE("saturating add via min") {
		bv_widening = true;
		// o:bv[8] = min({200}+{90}, {250}): exact sum 290 at W=9 (bf_add
		// is max(l,r)+1 = 9, well clear of 511), min(290,250)=250,
		// truncates losslessly (250 <= 255) -> o solves to 250. (250, not
		// 255: 255 is bv[8]'s all-ones top element, which the comparison
		// hooks canonicalize to a distinct `bf_t` node rather than a
		// plain ba_constant -- picking 250 keeps the model-value read
		// below a single, unambiguous code path.)
		// Same solve+model-check shape as the checked-multiply case.
		auto fm = parse_wff("o = min({ 200 }:bv[8] + { 90 }:bv[8], { 250 }:bv[8])");
		auto sol = solve_bv<node_t>(widen_bv_arithmetic<node_t>(fm));
		REQUIRE(sol.has_value());
		REQUIRE(sol.value().size() == 1);
		const tref val = sol.value().begin()->second;
		CHECK(std::get<bv>(tree<node_t>::get(val)[0].get_ba_constant())
			.getBitVectorValue(10) == "250");
		bv_widening = false;
	}

	TEST_CASE("subtraction underflow wraps at W (D1)") {
		bv_widening = true;
		// {20}*{15} - {10} <= {50}: bf_sub's own rule is `max(l, r)` (it
		// never grows on its own), but here it sits ABOVE a widened mul
		// (needed_width(bf_mul(20,15)) = 8+8 = 16), so its own width is
		// inherited as max(16, 8) = 16, not the base width 8 -- "identical
		// to today" only holds when sub's operands are both already at
		// base_w, which is NOT the case here.
		// modular (bv_widening off): 20*15 mod 256 = 44 (cvc5 8-bit
		// hardware multiply); 44 - 10 = 34 (no underflow); 34 <= 50 TRUE.
		// exact (W=16): 20*15 = 300 (fits in 16 bits, no wrap);
		// 300 - 10 = 290 (16-bit, no underflow); 290 <= 50 FALSE.
		auto fm = parse_wff(
			"({ 20 }:bv[8] * { 15 }:bv[8]) - { 10 }:bv[8] <= { 50 }:bv[8]");
		CHECK( !is_bv_formula_valid<node_t>(widen_bv_arithmetic<node_t>(fm)) );
		bv_widening = false;
		CHECK( is_bv_formula_valid<node_t>(fm) );
	}

	TEST_CASE("complement fold: X <= top element is a trivial BA identity (unaffected by widening)") {
		// This was the original "complement at W" pin. The controller's
		// run showed the widened formula prints as the bare constant `T`
		// -- re-deriving by hand from the spec rules (not just re-running)
		// shows T is actually CORRECT here, for a reason that has nothing
		// to do with exact-vs-modular semantics: {255} is bv[8]'s all-ones
		// value, so it parses directly as `bf_t` (test_bv_ba_hooks.cpp's
		// own "an all-ones literal is the top element" fact), and
		// `wff_lteq`'s own comparison-folding hook has a standing BA law,
		// unrelated to bv_widening, "$X <= 1 ::= T." (hooks.tmpl.h:1288-
		// 1291, where "1" is the BA's top element) -- X <= top is TRUE for
		// ANY X, so the WHOLE atom folds to `wff_t` at parse time, before
		// widen_bv_arithmetic ever runs (there is no bf_lteq atom left for
		// select_top to find, so the pass is correctly a no-op on an
		// already-constant formula). T is therefore correct regardless of
		// widening; this sample never actually exercised the amended D3
		// complement-at-W rule at all -- see the next case for that.
		bv_widening = true;
		auto fm = parse_wff("({ 16 }:bv[8] * { 16 }:bv[8])' <= { 255 }:bv[8]");
		tref widened = widen_bv_arithmetic<node_t>(fm);
		REQUIRE(widened != nullptr);
		CHECK(widened == fm); // already fully folded at parse time: no-op
		CHECK( is_bv_formula_valid<node_t>(widened) );
		bv_widening = false;
		CHECK( is_bv_formula_valid<node_t>(fm) );
	}

	TEST_CASE("complement at W (amended D3): exact vs modular, solver-checked") {
		// ex x ((x * {255})' <= {200}): bf_neg's rule is `w = l` -- it runs
		// AT its operand's own computed width, which here is the widened
		// 16-bit product (amended D3: complement does not drop back to the
		// atom's base width). Avoids the previous case's trap: {200} is an
		// ordinary bv[8] constant (neither 0 nor 255), so no comparison
		// identity fires, and this is a genuine ex-quantified atom that
		// must actually reach the solver.
		//
		// modular (bv_widening off, native bv[8] hardware arithmetic): as
		// x ranges over 0..255, x*255 mod 256 = (256 - x) mod 256 takes
		// EVERY value in [0,255] (255 is -1 mod 256, so this is a
		// bijection); its one's complement (255 - that value) therefore
		// also ranges over all of [0,255]. Concretely x=1: 1*255 mod 256 =
		// 255, complement = 255-255 = 0, and 0 <= 200 -- so `ex x (...)`
		// is SAT.
		// exact (W=16, no wrap since x*255 <= 255*255 = 65025 < 65536):
		// the exact product ranges over [0, 65025], so the exact 16-bit
		// complement (65535 - product) ranges over [65535-65025, 65535] =
		// [510, 65535] -- ALWAYS >= 510, so "<= 200" can never hold for
		// ANY x: UNSAT.
		auto fm = parse_wff("ex x ((x:bv[8] * { 255 }:bv[8])' <= { 200 }:bv[8])");
		CHECK( is_bv_formula_sat<node_t>(fm) ); // modular
		bv_widening = true;
		tref widened = widen_bv_arithmetic<node_t>(fm);
		REQUIRE(widened != nullptr);
		INFO("widened formula: " << tree<node_t>::get(widened).to_str());
		CHECK( !is_bv_formula_sat<node_t>(widened) ); // exact
		bv_widening = false;
	}

	// Defs-expansion probe: `normalizer<node_t>` is the SAME shared pipeline
	// entry `normalizer(rr)` reaches after `nso_rr_apply` has already
	// expanded `fn`'s definition (normalizer.tmpl.h: normalizer(const
	// rr<node>&) calls nso_rr_apply, THEN normalize_with_temp_simp
	// directly, where the widening hook now sits at the very top). If
	// widening ran BEFORE the ref were expanded instead, the
	// atom's left side would still be an opaque bf_ref, needed_width would
	// return 0 (its documented "opaque subterm" case), and widen_atom
	// would skip the atom entirely -- leaving it modular (deciding TRUE
	// below) even with the flag on. Deciding FALSE therefore proves both
	// that the pass is wired into the real pipeline AND that it runs after
	// definition expansion, not before.
	TEST_CASE("defs probe: widening reaches an expanded definition's arithmetic") {
		// The head's argument MUST carry an explicit base-type annotation
		// (`x:bv[8]`) -- an unannotated definition head/argument defaults
		// to `:tau` (a known project trap: see
		// test_integration-ba_types_inference.cpp's "consistent
		// definitions and references are accepted", `f(x:bv[8]) := x.`),
		// and calling an inferred-:tau `fn` with a bv[8] argument is a
		// genuine type conflict (Debug asserts/aborts on it; Release
		// reports "Incompatible type information").
		const char* sample =
			"fn(x:bv[8]) := x * x."
			"fn({ 16 }:bv[8]) <= { 10 }:bv[8].";
		// modular: fn(16) = 16*16 mod 256 = 0; 0 <= 10 decides TRUE.
		CHECK(normalize_and_check(sample, tau::wff_t));
		// exact: fn(16) = 256 (W=16, no wrap); 256 <= 10 decides FALSE.
		bv_widening_scope widen;
		CHECK(normalize_and_check(sample, tau::wff_f));
	}

	// Route/eliminability audit (brief Step 5): a widened atom carries a
	// bf_cast (the (bv[16]) upcast on its operands); has_bv_arithmetic
	// (solver.tmpl.h) already treats bf_cast as arithmetic, so a widened
	// atom routes to solve_bv, not lgrs. A quantified widened atom must
	// still decide correctly through the FULL stack (is_tau_formula_sat),
	// not be silently dropped by a blasting-classification mismatch
	// between has_bv_arithmetic and atom_arith_verdict's
	// blasting_unsupported lambda (eliminability.tmpl.h:138) -- see the
	// report for the full read of both call sites.
	TEST_CASE("route/eliminability audit: quantified widened atom stays satisfiable") {
		bv_widening_scope widen;
		// ex x (x*x <= {200}): x=0 makes 0<=200 true even at the exact
		// (widened, W=16) width, so this must decide SAT through the full
		// is_tau_formula_sat stack exactly as it would unwidened.
		const char* sample = "ex x (x * x <= { 200 }:bv[8]).";
		auto nso_rr = get_nso_rr(sample);
		REQUIRE(nso_rr.has_value());
		tref normalized = normalizer<node_t>(nso_rr.value());
		REQUIRE(normalized != nullptr);
		CHECK(is_tau_formula_sat<node_t>(normalized));
	}

	// A stricter variant of the audit above: `x * {3}` (a genuine constant
	// factor) is the shape get_bvmul_arguments<node>/atom_arith_verdict's
	// `blasting_unsupported` lambda considers blasting-SUPPORTED --
	// classifying the widened atom `blasteable` -- yet after widening the
	// non-constant operand is `(bv[16]) x`, a bf_cast, not a bare
	// variable. Reading confirms this is not actually a gap:
	// bv_predicate_blasting.tmpl.h's own traversal has a dedicated
	// `case tau::bf_cast` (line ~920, pre-existing, unrelated to this
	// feature -- casts were already legal user syntax) that introduces an
	// auxiliary variable plus a linking `bvcast` predicate for any cast
	// node it visits, so a widened `bf_cast`-wrapped operand is blasted
	// like any other cast, not a special or unsupported case. Kept as an
	// empirical guard rather than relying on that reading alone: x=0
	// trivially satisfies the atom under both modular and exact semantics.
	TEST_CASE("route/eliminability audit: widened constant-factor mul stays satisfiable") {
		bv_widening_scope widen;
		const char* sample = "ex x (x:bv[8] * { 3 }:bv[8] <= { 200 }:bv[8]).";
		auto nso_rr = get_nso_rr(sample);
		REQUIRE(nso_rr.has_value());
		tref normalized = normalizer<node_t>(nso_rr.value());
		REQUIRE(normalized != nullptr);
		CHECK(is_tau_formula_sat<node_t>(normalized));
	}

	// Post-review fix: a D4 cap violation (needed width W exceeds a
	// capped bv_max_width) makes widen_bv_arithmetic return nullptr,
	// which normalize_with_temp_simp/normalize_non_temp now propagate
	// (Task 6's own gated snippet: `if (!fm) return nullptr;`). Before
	// this round's fix, none of the many call sites of those two
	// functions across solver.tmpl.h/satisfiability.tmpl.h/
	// interpreter.tmpl.h/normalizer.tmpl.h checked for that nullptr
	// before dereferencing it -- in Debug this SIGABRT'd (an assertion
	// deep in tree<node>::get on a null tref); in Release it was
	// undefined behavior. This drives the cap violation through a real,
	// public entry point (is_tau_formula_sat, which calls
	// normalize_with_temp_simp directly) and asserts a clean `false`
	// (unsat, a conservative "cannot decide" answer -- not a proof, and
	// not a crash).
	TEST_CASE("D4 cap violation through a real entry point fails cleanly, not a crash") {
		bv_widening_scope widen;
		bv_max_width_scope cap(12); // x*y at bv[8] needs W=16 > 12
		auto fm = parse_wff("o:bv[8] = x * y");
		CHECK( !is_tau_formula_sat<node_t>(fm) );
	}

	// Second post-review round: normalizer<node>(tref) (normalizer.tmpl.h,
	// a thin `return normalize_with_temp_simp<node>(fm);` wrapper) has its
	// own call sites that were still unguarded -- api<node_t>::
	// get_interpreter (api.tmpl.h) is the public entry point for building
	// an executable interpreter, and it already had a pre-existing guard
	// (`if (!normalized) return {};`, predating this feature, for other
	// nullptr sources) immediately after its own normalizer<node>() call.
	// This drives the SAME D4 cap violation through that real, public
	// entry point with a genuine executable (io-var-carrying) spec, and
	// confirms it now also fails cleanly for this new nullptr source.
	//
	// This does NOT reach interpreter<node_t>::make_interpreter's OWN
	// internal `spec = normalizer<node>(spec);` guard (interpreter.tmpl.h
	// ~571): api::get_interpreter normalizes and null-checks BEFORE ever
	// calling make_interpreter, so a cap violation is always caught one
	// layer up from here. make_interpreter's own guard is reachable
	// instead via the REPL/CLI `run(tref, ...)` path
	// (interpreter.tmpl.h's `run` function calls make_interpreter
	// directly on a freshly-parsed, not-yet-normalized spec) -- exercising
	// THAT specific path would need constructing a raw io_context<node_t>
	// and driving the REPL/CLI runner directly, which none of the
	// existing unit-test infrastructure in this file sets up; noting this
	// explicitly rather than reaching for it.
	TEST_CASE("D4 cap violation through get_interpreter fails cleanly, not a crash") {
		bv_widening_scope widen;
		bv_max_width_scope cap(12); // i[t]*i[t] at bv[8] needs W=16 > 12
		tref fm = tau_api::get_formula("o[t]:bv[8] = i[t]:bv[8] * i[t]:bv[8]");
		REQUIRE(fm != nullptr);
		auto maybe_i = tau_api::get_interpreter(fm);
		CHECK(!maybe_i.has_value());
	}

	// Task 8 pwr-update repro: an interpreter update submitted through the
	// u stream widens to exactly the shape
	// "always o1[t]:bv[8] = min(i2[t]:bv[8] * {3}:bv[8], {100}:bv[8])"
	// (confirmed by a trace-level rerun: the printed u value carried the
	// fully elaborated "(bv[8]) min(((bv[16]) i2[t]*{3}), {100}) = o1[t]"
	// shape). This pins that exact clause shape as satisfiable AND
	// executable in isolation, through both is_tau_formula_sat (basic
	// satisfiability) and transform_to_execution (the actual gate
	// get_executable_spec/compute_part_continuations uses to decide
	// whether an interpreter can run a spec part) -- confirming no defect
	// in the widened temporal/QE path for this update-shaped assignment.
	// (An initial version of the corresponding integration test, in
	// tests/integration/test_integration-interpreter.cpp, transiently hit
	// a "No update performed: updated specification is unsat" warning;
	// traced to that test's fixture pinning a conflicting baseline for
	// o1, routing pointwise_revision through its separate "I1"
	// last-resort-alternative path -- unrelated to this clause shape, and
	// not reproduced here or in the corrected integration test. See the
	// Task 8 report.)
	//
	// Fresh stream names o9/i9, unused elsewhere in this file, sidestep
	// the global stream-name-type registry trap documented in
	// test_integration-bv_stress_check.cpp -- definitions<node_t>::
	// instance() persists a name's type for the whole process; clearing
	// it first is a defensive, belt-and-braces measure since o9/i9 are
	// otherwise fresh here. Names must start with the io_context
	// classifier's recognized prefixes -- 'i'/'o'/'u'/'this' -- an
	// arbitrary name like p1/q2 is rejected by update_types before
	// parsing even gets to the formula itself.
	TEST_CASE("pwr minimal repro: update-shaped bv assignment is satisfiable and executable") {
		definitions<node_t>::instance().clear();
		bv_widening_scope widen;
		const char* sample = "always o9[t]:bv[8] = "
			"min(i9[t]:bv[8] * { 3 }:bv[8], { 100 }:bv[8]).";
		auto nso_rr = get_nso_rr(sample);
		REQUIRE(nso_rr.has_value());
		tref normalized = normalizer<node_t>(nso_rr.value());
		REQUIRE(normalized != nullptr);
		INFO("normalized: " << tree<node_t>::get(normalized).to_str());
		CHECK(is_tau_formula_sat<node_t>(normalized));
		tref executable = transform_to_execution<node_t>(normalized);
		INFO("transform_to_execution result: "
			<< (executable ? tree<node_t>::get(executable).to_str()
				: std::string("<nullptr>")));
		CHECK(executable != tau::_F());
		CHECK(executable != nullptr);
	}
}

// ---------------------------------------------------------------------------
// Task 7: blasting backend support for widened bv atoms.
//
// atomic_blasting (bv_predicate_blasting.tmpl.h) hoists a fresh auxiliary
// variable for every bf_add/bf_sub/bf_shl/bf_shr/bf_mul/bf_div/bf_mod/
// bf_min/bf_max node it blasts. Before this task, every such hoisted
// variable was typed with the ATOM's own single ba_type (captured once,
// `auto type_id = tau::get(term).get_ba_type();`, term being the whole
// atomic comparison) rather than the specific subterm's own width.
//
// That coincides with the atom's type in two situations: (1) any unwidened
// atom (today's production trees always have one uniform width throughout
// an atom, casts aside -- and bf_cast already reads its OWN target type,
// not the shared type_id), and (2) a widened "extend-all" shape
// (comparisons, both-compound equality, bf_interval): the amended D2/D3
// rule widens EVERY operator uniformly to the atom's own final W, so the
// atom's type and every subterm's type are the same value again.
//
// It breaks for the widened "truncating assignment" shape (o = ...): the
// outer cast resets the ATOM's own type back to base_w, while everything
// INSIDE that cast still runs at the wider, genuinely different W -- see
// widen_atom's own "assignment truncation" tests above. These cases build
// exactly that shape with a real (non-constant-folded) bf_mul/bf_add and
// bf_min node inside it, so the hoisted intermediates' widths actually
// matter to the final bit-level constraints.
//
// Literal-constant-only analogues of Task 6's own end-to-end samples (e.g.
// "o = min({16}*{16}, {200})") do NOT exercise this: with both operands
// literal, the widened RHS folds away to a single constant before blasting
// ever sees a bf_mul/bf_min node (Task 6's own report traces this exactly).
// These cases instead bind the variable side through a separate equality
// conjunct (`x = { 16 }:bv[8] && ... x * { 16 }:bv[8] ...`), which pins the
// value just as deterministically while keeping `x` a genuine `variable`
// node the tree can never constant-fold -- the same idiom the existing
// bv_predicate_blasting integration suite already uses throughout (e.g.
// "ex x (x = { 3 }:bv[4] && x + { 5 }:bv[4] = { 8 }:bv[4])").
// ---------------------------------------------------------------------------

namespace {

// Sets bv_blasting for the lifetime of the enclosing scope and restores
// whatever value it had before, even on a REQUIRE-failure stack unwind --
// same RAII shape as bv_widening_scope/bv_max_width_scope above.
struct bv_blasting_scope {
	bool prev;
	bv_blasting_scope() : prev(bv_blasting) { bv_blasting = true; }
	~bv_blasting_scope() { bv_blasting = prev; }
};

// Direct-call idiom, mirroring tests/integration/test_integration-heuristics-
// bv_predicate_blasting.cpp's own blast_normalize helper: widen (only when
// bv_widening is on -- widen_bv_arithmetic is otherwise a pass-through
// no-op, per the "OFF mode" test above, so calling it unconditionally would
// also be fine, but skipping it when off keeps this helper honest about
// which stage a nullptr came from), blast, then normalize the result down
// to a printed constant.
std::string widen_blast_normalize(tref fm) {
	tref widened = bv_widening ? widen_bv_arithmetic<node_t>(fm) : fm;
	if (!widened) return "widen_error";
	tref blasted = bv_predicate_blasting<node_t>(widened);
	if (!blasted) return "blast_error";
	tref result = normalizer<node_t>(blasted);
	if (!result) return "null";
	return tree<node_t>::get(result).to_str();
}

} // namespace

TEST_SUITE("bv widening - blasting backend (Task 7)") {

	TEST_CASE("checked multiply under truncating assignment: per-subterm width required") {
		// o:bv[8] = min(x * {16}, {200}): needed_width(mul) = 8+8 = 16;
		// min keeps max(16,8) = 16 -- both the mul AND the min node end up
		// typed bv[16], while the atom itself resets to bv[8] (truncating
		// assignment). x is pinned to 16 via a separate equality conjunct
		// so the mul survives blasting as a genuine bf_mul node (a literal
		// "{16}*{16}" would instead fold away to a plain 256:bv[16]
		// constant before blasting ever ran -- see the block comment
		// above). Exact: 16*16 = 256 (fits in 16 bits); min(256,200) =
		// 200; truncating cast to bv[8] is lossless (200 <= 255) -> o
		// must equal 200 for the whole conjunction to hold.
		//
		// Under the pre-fix code, the hoisted "product" (mul) and "result"
		// (min) aux variables are wrongly typed bv[8] instead of bv[16] --
		// this either corrupts the bit-level constraints (silently wrong
		// verdict) or trips a cross-width construction guard (an abort in
		// Debug, an "Incompatible type information" error in Release),
		// per the project's established pattern for that class of defect.
		bv_widening_scope widen;
		bv_blasting_scope blast;
		tref fm = parse_wff(
			"ex x ex o (x = { 16 }:bv[8] && "
			"o:bv[8] = min(x * { 16 }:bv[8], { 200 }:bv[8]) && "
			"o = { 200 }:bv[8])");
		CHECK(widen_blast_normalize(fm) == "T");
	}

	TEST_CASE("saturating add under truncating assignment: per-subterm width required") {
		// o:bv[8] = min(x + {90}, {250}): needed_width(add) =
		// max(8,8)+1 = 9; min keeps max(9,8) = 9 -- the add AND the min
		// node end up typed bv[9] while the atom resets to bv[8]. x is
		// pinned to 200 the same way as above. Exact: 200+90 = 290 (fits
		// in 9 bits, max 511, no wrap); min(290,250) = 250; truncating
		// cast to bv[8] is lossless (250 <= 255) -> o must equal 250.
		bv_widening_scope widen;
		bv_blasting_scope blast;
		tref fm = parse_wff(
			"ex x ex o (x = { 200 }:bv[8] && "
			"o:bv[8] = min(x + { 90 }:bv[8], { 250 }:bv[8]) && "
			"o = { 250 }:bv[8])");
		CHECK(widen_blast_normalize(fm) == "T");
	}

	TEST_CASE("identity for unwidened trees: same shape, bv_widening off") {
		// Same shape as the checked-multiply case above, bv_widening left
		// at its default (off): with no widening, the mul's own ba_type
		// is bv[8], identical to the atom's own type -- the pre-fix
		// shared-type_id code and the per-subterm fix compute the exact
		// same value here, so this must already pass before this task's
		// fix and must keep passing after it (the brief's "identity for
		// unwidened trees" requirement, pinned as an explicit test rather
		// than only relied on via the full-suite regression run).
		// Modular: 16*16 mod 256 = 0; min(0,200) = 0; o must equal 0.
		REQUIRE(!bv_widening);
		bv_blasting_scope blast;
		tref fm = parse_wff(
			"ex x ex o (x = { 16 }:bv[8] && "
			"o:bv[8] = min(x * { 16 }:bv[8], { 200 }:bv[8]) && "
			"o = { 0 }:bv[8])");
		CHECK(widen_blast_normalize(fm) == "T");
	}

	TEST_CASE("decline cleanly: variable*variable multiply is outside blasting's constant-factor limit") {
		// o:bv[8] = min(x * y, {200}), x and y both free variables --
		// neither side of the widened mul is a compile-time constant, so
		// get_bvmul_arguments<node> (bv_predicate_blasting.tmpl.h:25-41)
		// returns (nullptr, nullptr) and atomic_blasting's bf_mul case
		// sets error=true (bv_predicate_blasting_arithmetic.tmpl.h's
		// bvmul requires a constant factor; bv_predicate_blasting.tmpl.h
		// :897-899's "if (!constant) { error = true; break; }"). This is
		// unrelated to this task's fix (the decline happens before
		// build_variable is ever reached) -- kept here as the required
		// "outside blasting's limits" companion case, confirming the
		// widened tree still makes blasting decline CLEANLY (a nullptr
		// return), not crash. Calling bv_predicate_blasting directly
		// (rather than through solver.tmpl.h's has_bv_arithmetic-gated
		// dispatch) means the cvc5 fallback this would normally trigger
		// in production is not reachable from here -- see the task-7
		// report for the fuller read of that route.
		bv_widening_scope widen;
		bv_blasting_scope blast;
		tref fm = parse_wff("o:bv[8] = min(x * y, { 200 }:bv[8])");
		tref widened = widen_bv_arithmetic<node_t>(fm);
		REQUIRE(widened != nullptr);
		CHECK(bv_predicate_blasting<node_t>(widened) == nullptr);
	}
}

// ---------------------------------------------------------------------------
// Task 9: pinned realizability/satisfiability truth-value flips through the
// full `is_tau_formula_sat`/`api<node_t>::realizable` entry points, on vs.
// off. Every earlier end-to-end flip (Task 6's suite above) goes through
// `is_bv_formula_valid`/`is_bv_formula_sat` directly on an explicitly
// `widen_bv_arithmetic`-called tree; these instead exercise the production
// pipeline entry points a caller actually uses (`is_tau_formula_sat` calls
// `normalize_with_temp_simp` -- and hence the widening hook -- internally,
// per satisfiability.tmpl.h:1880; `api::realizable` layers `simplify` +
// `normalize_formula` + `is_tau_formula_sat` on top of that, per
// api.tmpl.h:562-566).
//
// As Task 6's report establishes, `bv_widening` must be set BEFORE parsing,
// not just before normalizing: the fit-gated constant folding in
// `term_mul`/`term_add`/`term_sub` (Task 2) reads the same global flag at
// construction time and leaves an overflowing constant operation symbolic
// exactly when the flag is on -- so each side of a flip below re-parses the
// SAME source string fresh under the flag state it is checking, rather than
// reusing one parsed tree across both states.
// ---------------------------------------------------------------------------

TEST_SUITE("bv widening - realizability on/off") {

	TEST_CASE("both-compound equality: exact product breaks the modular wrap-to-zero") {
		// {16}*{16} = {0}: LHS is a compound multiply (needed_width =
		// 8+8 = 16); RHS is a plain constant leaf (needed_width = 8) --
		// NEITHER side is a bare storage term (variable/io_var), so this
		// does not qualify for the "truncating assignment" rule. It falls
		// to the general rule (design doc S2, "Equality between two
		// compound sides"): extend both sides to the common W and compare
		// EXACTLY, with nothing to truncate.
		//
		// Modular (bv_widening off): term_mul folds the constant pair at
		// native bv[8] hardware arithmetic: 16*16 mod 256 = 0, so the
		// atom is (some representation of) 0 = 0 -- decided TRUE by
		// is_tau_formula_sat regardless of exactly which construction-
		// time step resolves it (note: {0}:bv[8] is itself canonicalized
		// to the BA bottom element bf_f, symmetric to the {255}->bf_t
		// top-element canonicalization documented in
		// test_bv_ba_hooks.cpp -- needed_width's `case tau::bf_f:` arm
		// treats it as an ordinary base_w leaf, same as a ba_constant, so
		// this does not change the width analysis below) -- SAT.
		bv_widening = false;
		CHECK(is_tau_formula_sat<node_t>(
			parse_wff("{ 16 }:bv[8] * { 16 }:bv[8] = { 0 }:bv[8]")));

		// Exact (bv_widening on, set BEFORE parsing so the fit-gate
		// leaves 16*16 unfolded -- the "load-bearing" ordering Task 6's
		// end-to-end suite established): W = needed_width(mul) = 16;
		// 16*16 = 256 exactly (fits in 16 bits, no fit-gate trip once
		// widen_bv_arithmetic re-extends both operands to bv[16] during
		// normalization, at which point the mul re-folds to the plain
		// constant {256}:bv[16]); 256 != 0 -> UNSAT.
		bv_widening_scope widen;
		CHECK(!is_tau_formula_sat<node_t>(
			parse_wff("{ 16 }:bv[8] * { 16 }:bv[8] = { 0 }:bv[8]")));
	}

	TEST_CASE("both-compound comparison: exact product clears a threshold the modular wrap never reaches") {
		// Reverse-direction companion: {16}*{16} > {200}. Same
		// needed-width analysis (LHS compound mul, RHS plain constant
		// leaf) -- this time the "Comparisons" rule applies (both sides
		// zero-extended to W, compared exactly).
		//
		// Modular: 16*16 mod 256 = 0; 0 > 200 -> FALSE, decided UNSAT by
		// is_tau_formula_sat (the {255}/bf_t "top/bottom operands are not
		// folded by comparisons" trap documented in test_bv_ba_hooks.cpp
		// means the LHS's canonicalized-to-bf_f zero may not fold away at
		// parse time the way the equality case above can -- irrelevant
		// here since is_tau_formula_sat fully normalizes and decides any
		// residual ground atom regardless).
		bv_widening = false;
		CHECK(!is_tau_formula_sat<node_t>(
			parse_wff("{ 16 }:bv[8] * { 16 }:bv[8] > { 200 }:bv[8]")));

		// Exact: 256 > 200 -> TRUE -- SAT.
		bv_widening_scope widen;
		CHECK(is_tau_formula_sat<node_t>(
			parse_wff("{ 16 }:bv[8] * { 16 }:bv[8] > { 200 }:bv[8]")));
	}

	// Temporal case: a genuine realizability flip through the full
	// always/io-stream stack, checked via api<node_t>::realizable (the
	// spec-level entry point this task's brief names explicitly).
	//
	// Fresh stream name o1, cleared from definitions<node_t>::instance()
	// first as a defensive measure (the global stream-name-type registry
	// trap documented in test_integration-bv_stress_check.cpp) even though
	// grepping this file confirms o1 is not used as a real parsed sample
	// anywhere else in it. "o1" starts with the io_context classifier's
	// recognized 'o' prefix, so it needs no separate stream declaration
	// (matching the "pwr minimal repro" case above).
	TEST_CASE("temporal: always o1*o1=0 && o1>10 flips realizable on/off") {
		definitions<node_t>::instance().clear();
		const char* spec =
			"always (o1[t]:bv[8] * o1[t]:bv[8] = { 0 }:bv[8] "
			"&& o1[t]:bv[8] > { 10 }:bv[8])";
		// o1*o1 = 0 is the same "compound (mul) vs. plain constant"
		// shape as the two non-temporal cases above (needed_width(mul) =
		// 16; RHS constant leaf needed_width = 8 -- neither side is a
		// bare storage term, since the LHS is compound). o1 > 10 is its
		// own independent atom, needed_width = max(8,8) = 8 -- already
		// at base width, so this second conjunct is IDENTICAL under
		// both flag states; only the equality conjunct's meaning changes.
		//
		// Modular (bv_widening off): o1[t] = 16 at every time step
		// satisfies BOTH conjuncts simultaneously: 16*16 mod 256 = 0
		// (first conjunct), and 16 > 10 (second) -- REALIZABLE.
		bv_widening = false;
		tref fm_off = tau_api::get_formula(spec);
		REQUIRE(fm_off != nullptr);
		CHECK(tau_api::realizable(fm_off));

		// Exact (bv_widening on, set before parsing): the equality is now
		// extended to W = 16 and compared EXACTLY (not mod 256). As an
		// unsigned integer over o1's declared domain [0,255] (o1's
		// declared width is its own range constraint regardless of
		// widening -- design doc S2, "Variables inside arithmetic ...
		// keep their declared sort"), o1*o1 computed exactly (never
		// overflowing 16 bits, since 255*255 = 65025 < 65536) equals 0
		// if and only if o1 = 0. The second conjunct demands o1 > 10, so
		// no single value of o1[t] can satisfy both conjuncts at once --
		// UNREALIZABLE.
		bv_widening_scope widen;
		tref fm_on = tau_api::get_formula(spec);
		REQUIRE(fm_on != nullptr);
		CHECK(!tau_api::realizable(fm_on));
	}
}
