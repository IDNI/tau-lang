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
