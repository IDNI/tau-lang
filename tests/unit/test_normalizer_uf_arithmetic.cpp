// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "normalizer_uf_arithmetic.h"
#include "heuristics/bv_arithmetic_scan.h"

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
	auto skip = make_bv_arithmetic_skip_uf<node_t>(formula);
	for (tref v : get_free_vars<node_t>(formula))
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

	TEST_CASE("quantifier-free formula: new implementation is a superset of the flat heuristics/bv_arithmetic_scan implementation") {
		// The two implementations are NOT expected to agree exactly: the
		// existing flat implementation's pass 1 (tagging BV-typed free
		// vars of an arithmetic operator) calls get_free_vars on the bare
		// operator node (e.g. bf_add), but get_free_vars only returns
		// non-empty for nodes of type bf/wff -- so that pass never
		// actually tags any variable, and its real output is only pass
		// 2 (tagging the enclosing atomic formula). This new
		// implementation correctly tags the atom's free variables too
		// (via a merge keyed on the atomic-formula node, which IS of
		// type wff), so it must always be a superset of the old result.
		const std::string sample = "x + { 1 }:bv[4] = { 2 }:bv[4]";
		tref fm = parse_bv_formula(sample);
		auto old_tainted = collect_bv_arithmetic_taint<node_t>(fm);
		auto new_tainted = collect_bv_arithmetic_taint_uf<node_t>(fm);
		CHECK(new_tainted.size() >= old_tainted.size());
		for (tref n : old_tainted) CHECK(new_tainted.contains(n));
		CHECK(is_tainted(fm, "x"));
	}
}
