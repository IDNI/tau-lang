// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

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
		// ((bv[4]) x) contributes width 4 regardless of what's inside
		size_t m; CHECK(widths("((bv[4]) x) + y", 8, m) == 9);
	}
	TEST_CASE("constant shift amount adds to the width") {
		size_t m; CHECK(widths_typed("x:bv[8] << { 3 }:bv[8]", 8, m) == 11);
	}
	TEST_CASE("variable shift amount never grows") {
		size_t m; CHECK(widths("x << y", 8, m) == 8);
	}
}
