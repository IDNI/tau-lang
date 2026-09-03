// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include <algorithm>
#include <string>
#include <vector>

#include "boolean_algebras/bv_ba.h"
#include "bv_widening.h"


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
