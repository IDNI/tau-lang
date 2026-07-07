// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "execution.h"

namespace {

rewriter::rule swap_rule(tref from, tref to) {
	return { tau::geth(from), tau::geth(to) };
}

tref bf_var(const char* name) {
	return build_bf_variable<node_t>(name, tau_type_id<node_t>());
}

} // namespace

TEST_SUITE("execution: steps") {

	TEST_CASE("empty steps returns input unchanged") {
		tref x = bf_var("x");
		steps<node_t, step<node_t>> s(std::vector<step<node_t>>{});
		CHECK( s(x) == x );
	}

	TEST_CASE("applies libraries in order") {
		tref x = bf_var("x"), y = bf_var("y"), z = bf_var("z");
		step<node_t> x_to_y({ swap_rule(x, y) });
		step<node_t> y_to_z({ swap_rule(y, z) });
		steps<node_t, step<node_t>> s({ x_to_y, y_to_z });
		CHECK( s(x) == z );
	}
}

TEST_SUITE("execution: repeat_once") {

	TEST_CASE("applies each library exactly once") {
		tref x = bf_var("x"), y = bf_var("y");
		step<node_t> x_to_y({ swap_rule(x, y) });
		repeat_once<node_t, step<node_t>> once(x_to_y);
		CHECK( once(x) == y );
		// applying to an already-stable input does not oscillate back
		CHECK( once(y) == y );
	}
}

TEST_SUITE("execution: repeat_each") {

	TEST_CASE("repeats a single library until it stops changing") {
		tref x = bf_var("x"), y = bf_var("y"), z = bf_var("z");
		step<node_t> lib({ swap_rule(x, y), swap_rule(y, z) });
		repeat_each<node_t, step<node_t>> re(lib);
		CHECK( re(x) == z );
	}
}

TEST_SUITE("execution: repeat_all") {

	// RR-2: repeat_all's visited set was declared but never used and the
	// whole sequence was applied twice per round, risking a hang on a
	// period-2+ oscillating sequence or an ever-growing rewrite. A pair of
	// mutually-inverse rules (x -> y, y -> x) does not reproduce this: a
	// full cycle composed within a single round always cancels back to
	// its start, so it never exercises the bug regardless of the fix.
	// Constructing a genuine round-level cycle needs a single
	// capture-based rule whose output keeps re-matching its own pattern
	// (e.g. swapping the operands of a commutative connective), which is
	// beyond what a simple regression test can safely and quickly set up.
	// repeat_all::operator() now guards both failure modes directly:
	// cycles via visited, and unbounded growth via a max_rounds cap with
	// a LOG_ERROR diagnostic.
	TEST_CASE("reaches a fixpoint across the whole sequence") {
		tref x = bf_var("x"), y = bf_var("y"), z = bf_var("z");
		step<node_t> x_to_y({ swap_rule(x, y) });
		step<node_t> y_to_z({ swap_rule(y, z) });
		steps<node_t, step<node_t>> s({ x_to_y, y_to_z });
		repeat_all<node_t, step<node_t>> ra(s);
		CHECK( ra(x) == z );
	}
}
