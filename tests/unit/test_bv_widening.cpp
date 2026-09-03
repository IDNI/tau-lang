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

	TEST_CASE("idempotent: widen_atom(widen_atom(a)) == widen_atom(a)") {
		// Uses the truncating-assignment shape: needed_width on the
		// rebuilt RHS sees only the outer (bv[8]) cast (a boundary, per
		// the cast rule) and never descends into the wide internals, so
		// the second call recomputes W == base_w == 8 and is a no-op --
		// see the doc comment on widen_atom for why this specifically
		// relies on the truncating cast resetting the atom's own type.
		tref src = parse_wff("o:bv[8] = min(x * y, {200})");
		tref atom = find_atom(src, tau::bf_eq);
		tref w1 = widen_atom<node_t>(atom);
		REQUIRE(w1 != nullptr);
		CHECK(w1 != atom); // sanity: the first call did real work
		tref w2 = widen_atom<node_t>(w1);
		REQUIRE(w2 != nullptr);
		CHECK(w2 == w1);
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
}
