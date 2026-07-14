// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "normalizer_uf_arithmetic.h"

TEST_SUITE("bv_arithmetic_resolver") {

	TEST_CASE("insert declares in current scope, kind_of defaults to logical") {
		bv_arithmetic_resolver<node_t> r;
		tref x = tau::build_variable("x", untyped_type_id<node_t>());
		r.insert(x, arith_kind::logical);
		CHECK(r.kind_of(x) == arith_kind::logical);
	}

	TEST_CASE("assign joins kinds: logical then arithmetic becomes arithmetic") {
		bv_arithmetic_resolver<node_t> r;
		tref x = tau::build_variable("x", untyped_type_id<node_t>());
		r.assign(x, arith_kind::logical);
		r.assign(x, arith_kind::arithmetic);
		CHECK(r.kind_of(x) == arith_kind::arithmetic);
	}

	TEST_CASE("merge joins two nodes' kinds and unifies their sets") {
		bv_arithmetic_resolver<node_t> r;
		tref x = tau::build_variable("x", untyped_type_id<node_t>());
		tref y = tau::build_variable("y", untyped_type_id<node_t>());
		r.assign(x, arith_kind::logical);
		r.assign(y, arith_kind::arithmetic);
		r.merge(x, y);
		CHECK(r.kind_of(x) == arith_kind::arithmetic);
		CHECK(r.kind_of(y) == arith_kind::arithmetic);
	}

	TEST_CASE("open/close isolate a same-named variable across sibling scopes") {
		bv_arithmetic_resolver<node_t> r;
		tref x1 = tau::build_variable("x", untyped_type_id<node_t>());
		r.open();
		r.insert(x1, arith_kind::logical);
		r.assign(x1, arith_kind::arithmetic);
		r.close();

		tref x2 = tau::build_variable("x", untyped_type_id<node_t>());
		r.open();
		r.insert(x2, arith_kind::logical);
		CHECK(r.kind_of(x2) == arith_kind::logical);
		r.close();
	}
}

namespace {

tref parse_bv_formula(const std::string& spec) {
	tau::get_options opts{ .parse = { .start = tau::wff } };
	return tau::get(spec, opts);
}

bool is_tainted(tref formula, const std::string& var_name) {
	// select_all (not get_free_vars) so this also finds variables bound
	// by a quantifier, not just genuinely free ones.
	auto skip = make_bv_arithmetic_skip_uf<node_t>(formula);
	for (tref v : tau::get(formula).select_all(
		(bool(*)(tref)) is_var_or_capture<node_t>))
		if (get_var_name<node_t>(v) == var_name)
			return skip(v);
	FAIL("variable not found in formula: " << var_name);
	return false;
}

} // namespace

TEST_SUITE("make_bv_arithmetic_skip_uf") {

	TEST_CASE("x + y * z = w taints w, not just y and z") {
		// y*z is arithmetic; y and z are direct operands. w is only
		// entangled via the surrounding equation -- the gap the flat
		// two-pass heuristics/bv_arithmetic_scan implementation has.
		const std::string sample = "x + y * z = w:bv[4]";
		tref fm = parse_bv_formula(sample);
		CHECK(is_tainted(fm, "y"));
		CHECK(is_tainted(fm, "z"));
		CHECK(is_tainted(fm, "w"));
	}

	TEST_CASE("quantifier-free logical-only formula has no tainted variables") {
		const std::string sample = "x = { 1 }:bv[4]";
		tref fm = parse_bv_formula(sample);
		CHECK_FALSE(is_tainted(fm, "x"));
	}

	TEST_CASE("quantifier-free arithmetic formula taints its variable") {
		const std::string sample = "x + { 1 }:bv[4] = { 2 }:bv[4]";
		tref fm = parse_bv_formula(sample);
		CHECK(is_tainted(fm, "x"));
	}

	TEST_CASE("quantifier-free arithmetic formula taints the atom's free variable via merge") {
		// Taint must reach the atom's free variable through the merge
		// keyed on the atomic-formula node (which is of type wff), not
		// just the arithmetic operator's direct operands.
		const std::string sample = "x + { 1 }:bv[4] = { 2 }:bv[4]";
		tref fm = parse_bv_formula(sample);
		auto tainted = collect_bv_arithmetic_taint_uf<node_t>(fm);
		CHECK(!tainted.empty());
		CHECK(is_tainted(fm, "x"));
	}

	TEST_CASE("unrelated sibling quantifiers with the same variable name don't cross-taint") {
		// The `x` bound by the second `ex x` is a structurally identical
		// but logically unrelated variable to the first block's `x`.
		// Only the first block's `x` participates in arithmetic.
		const std::string sample =
			"(ex x x + { 1 }:bv[4] = { 2 }:bv[4]) "
			"&& (ex x x = { 3 }:bv[4])";
		tref fm = parse_bv_formula(sample);
		auto skip = make_bv_arithmetic_skip_uf<node_t>(fm);
		int arithmetic_atoms = 0, logical_atoms = 0;
		for (tref atom : tau::get(fm).select_top(is_atomic_fm<node_t>))
			skip(atom) ? ++arithmetic_atoms : ++logical_atoms;
		CHECK(arithmetic_atoms == 1);
		CHECK(logical_atoms == 1);
	}

	TEST_CASE("taint on an atom inside a nested quantifier surfaces on an outer-scoped variable") {
		// The first-bound variable (originally "w") is bound by the
		// outer `ex`; the arithmetic atom lives inside the nested
		// `ex`'s body but shares it via the equation. Looked up
		// structurally, not by source name: Tau's parser canonically
		// renames quantifier-bound variables (e.g. to "b1"/"b2"), so
		// the original "w"/"y" spelling doesn't survive into the tree.
		const std::string sample =
			"ex w ex y w + y * { 1 }:bv[4] = { 2 }:bv[4]";
		tref fm = parse_bv_formula(sample);
		auto skip = make_bv_arithmetic_skip_uf<node_t>(fm);
		auto vars = tau::get(fm).select_all(
			(bool(*)(tref)) is_var_or_capture<node_t>);
		REQUIRE(vars.size() == 2);
		for (tref v : vars) CHECK(skip(v));
	}
}
