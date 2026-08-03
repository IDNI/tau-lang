// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

// The entry point is anti_prenex_block, not the legacy anti_prenex: the
// partition these cases exercise lives in push_ex_block_into_clause, which only
// the block algorithm reaches (legacy anti_prenex goes through
// treat_ex_quantified_clause and its own blocks_elimination guard, which Task 6
// covers).
TEST_SUITE("partial clause elimination") {

	TEST_CASE("a reference freezes only its own component") {
		// y is entangled with nothing that reaches f, so its binder must
		// go even though x's cannot. Before the partition the whole
		// clause was kept with both binders intact.
		const char* sample = "ex x, y (f(x) && y w = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex_block<node_t>(fm);
		// x's binder survives around f; y's does not survive anywhere.
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_ref>) );
		// Exactly one existential binder is left.
		CHECK( tau::get(res).select_top(is<node_t, tau::wff_ex>).size() == 1 );
	}

	TEST_CASE("an all-frozen clause is kept whole, as before") {
		// Equivalence guard: when every block variable reaches the
		// reference there is nothing to split, and the result must be
		// what it was.
		const char* sample = "ex x, y (f(x) && x y = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex_block<node_t>(fm);
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_ref>) );
		CHECK( tau::get(res).find_top(is<node_t, tau::wff_ex>) );
	}

	TEST_CASE("a reference-free clause is unaffected") {
		const char* sample = "ex x, y (x w = 0 && y w = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = anti_prenex_block<node_t>(fm);
		CHECK( !tau::get(res).find_top(is<node_t, tau::wff_ex>) );
	}
}
