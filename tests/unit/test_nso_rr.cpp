// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "nso_rr.h"

TEST_SUITE("nso_rr unit tests") {

	TEST_CASE("nso_rr_apply: simple fixed-point loop unfolds") {
		const char* sample =
			"g[n](x) := !g[n-1](x)."
			"g[0](x) := T."
			"g(x).";

		auto maybe = get_nso_rr(sample);
		CHECK( maybe.has_value() );
		auto rr = maybe.value();

		// apply recurrence relations
		auto applied = nso_rr_apply<node_t>(rr);

		CHECK( applied != nullptr );
		// the result should be a wff (boolean formula) at its root
		CHECK( tau::get(applied).is(tau::wff) );
	}

	TEST_CASE("nso_rr_apply: transform ref args to captures and substitute") {
		// A recurrence that uses a variable as a ref-arg should be transformed
		// so that variables in ref args become captures. This exercises the
		// transform_ref_args_to_captures / apply substitution path.
		const char* sample =
			"f(Y) := Y = 0."  // body uses variable Y
			"f(x).";

		auto maybe = get_nso_rr(sample);
		CHECK( maybe.has_value() );
		auto rr = maybe.value();

		auto applied = nso_rr_apply<node_t>(rr);
		CHECK( applied != nullptr );

		// The applied main should contain a bf (boolean formula) reference or body
		CHECK( ((tt(applied) | tau::bf).has_value()
			|| (tt(applied) | tau::bf_eq).has_value()) );
	}

}
