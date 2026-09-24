// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include <cstdlib>

using tau_api = api<node_t>;

namespace {

// Every case here moves a global cap, so each restores what it found. The
// suite runs in one process with the rest of the api tests; a leaked budget
// would make every later api call refuse.
struct budget_guard {
	long budget = tref_budget_param;
	long soft = tref_budget_soft_param;
	~budget_guard() {
		tref_budget_param = budget;
		tref_budget_soft_param = soft;
	}
};

} // namespace

TEST_SUITE("Tau API - memory budget") {

	// The shipped default has to be "no budget": a cap that shipped
	// non-zero would refuse calls in every embedder that never asked for
	// one.
	TEST_CASE("unlimited by default") {
		budget_guard g;
		tref_budget_param = -1;
		unsetenv("TAU_TREF_BUDGET");
		CHECK( tref_budget() == 0 );
		CHECK( tref_soft_mark() == 0 );
		CHECK( over_tref_budget<node_t>() == false );
		CHECK( over_tref_soft_mark<node_t>() == false );
	}

	TEST_CASE("the setter wins over the environment") {
		budget_guard g;
		setenv("TAU_TREF_BUDGET", "4096", 1);
		CHECK( tref_budget() == 4096 );
		tau_api::set_tref_budget(99);
		CHECK( tref_budget() == 99 );
		unsetenv("TAU_TREF_BUDGET");
	}

	TEST_CASE("garbage in the environment keeps the default") {
		budget_guard g;
		tref_budget_param = -1;
		setenv("TAU_TREF_BUDGET", "not-a-number", 1);
		CHECK( tref_budget() == 0 );
		unsetenv("TAU_TREF_BUDGET");
	}

	// cap / 100 * pct truncates to 0 below 100, which would leave a small
	// budget never approaching itself and so never forcing a sweep.
	TEST_CASE("the soft mark is a percentage, not an integer division") {
		budget_guard g;
		tau_api::set_tref_budget(1000);
		tau_api::set_tref_budget_soft_percent(75);
		CHECK( tref_soft_mark() == 750 );
		tau_api::set_tref_budget(4);
		CHECK( tref_soft_mark() == 3 );
		tau_api::set_tref_budget(1);
		tau_api::set_tref_budget_soft_percent(100);
		CHECK( tref_soft_mark() == 1 );
	}

	TEST_CASE("an out-of-range soft percentage falls back to 75") {
		budget_guard g;
		tau_api::set_tref_budget_soft_percent(0);
		CHECK( tref_budget_soft_percent() == 75 );
		tau_api::set_tref_budget_soft_percent(101);
		CHECK( tref_budget_soft_percent() == 75 );
		tau_api::set_tref_budget_soft_percent(50);
		CHECK( tref_budget_soft_percent() == 50 );
	}

	// The point of the whole mechanism: a call that cannot start returns an
	// error rather than a value.
	TEST_CASE("a call starting over the budget is refused") {
		budget_guard g;
		tau_api::set_tref_budget(0);
		auto ok = tau_api::get_formula("x = 0");
		REQUIRE( ok.has_value() );

		// One node is a budget no session can meet.
		tau_api::set_tref_budget(1);
		auto refused = tau_api::get_formula("y = 0");
		CHECK_FALSE( refused.has_value() );
		CHECK( refused.report().has_error() );
	}

	// The rule that makes the budget predictable: permission is decided
	// once, on entry. Work that was allowed to begin is never thrown away
	// for having grown the store past the cap on its way.
	TEST_CASE("a call allowed to start keeps its value even if it overshoots") {
		budget_guard g;
		tau_api::set_tref_budget(0);
		// A cap one node above the current store: this call may start,
		// and parsing will certainly carry the store past it.
		const size_t before = tau_api::tref_count();
		tau_api::set_tref_budget(before + 1);
		auto allowed = tau_api::get_term("(p & q) | (r & s) | (t & u)");
		CHECK( allowed.has_value() );
		CHECK( tau_api::tref_count() > before + 1 );

		// ... and the overshoot is what refuses the next call.
		auto refused = tau_api::get_term("(v & w) | x");
		CHECK_FALSE( refused.has_value() );
	}

	TEST_CASE("a refusal is not a verdict: dnf refuses rather than answering") {
		budget_guard g;
		tau_api::set_tref_budget(0);
		auto fm = tau_api::get_term("(x & y) | z");
		REQUIRE( fm.has_value() );

		tau_api::set_tref_budget(1);
		auto refused = tau_api::dnf(fm.value());
		CHECK_FALSE( refused.has_value() );
	}

	// The entry check keeps no state, so a store back under its cap admits
	// the next call with nothing to reset. A sticky flag would have had to
	// be cleared by hand, and a missed clear poisons the process.
	TEST_CASE("a refusal leaves nothing behind to reset") {
		budget_guard g;
		tau_api::set_tref_budget(1);
		CHECK_FALSE( tau_api::get_formula("x = 0").has_value() );

		// A cap the store already fits under admits the very next call.
		tau_api::set_tref_budget(0);
		CHECK( tau_api::get_formula("x = 0").has_value() );
	}

	// The scope nests, so an api method calling another one must not be
	// mistaken for a second unit of work.
	TEST_CASE("nested scopes leave the outermost bookkeeping alone") {
		budget_guard g;
		tau_api::set_tref_budget(0);
		{
			budget_scope<node_t> outer;
			const size_t before = tref_budget_last_before;
			{
				budget_scope<node_t> inner;
				CHECK( tref_budget_last_before == before );
			}
			// The inner scope closing must not have stamped the
			// after mark: the outermost one owns it.
			CHECK( tref_budget_last_before == before );
		}
	}

	TEST_CASE("residue reports the growth a unit of work left behind") {
		budget_guard g;
		tau_api::set_tref_budget(0);
		{
			budget_scope<node_t> scope;
			auto fm = tau_api::get_term(
				"(a & b & c) | (d & e & f) | (g & h)");
			REQUIRE( fm.has_value() );
		}
		// Parsing a formula the store has not seen interns nodes for it.
		CHECK( budget_scope<node_t>::residue() > 0 );
	}

	// A full bdd node table makes the value of the call unknown: the api
	// must answer with an error and no value, never a T or F, and leave
	// the process ready for the next call.
	TEST_CASE("a call that fills a bdd node table has no value") {
		budget_guard g;
		tau_api::set_tref_budget(0);
		auto r = with_budget<node_t>([] {
			bdd_node_table_exhausted = true;
			return result<bool>{}.with_value(false);
		});
		CHECK_FALSE( r.has_value() );
		CHECK( r.has_error() );
		CHECK_FALSE( bdd_node_table_exhausted );
		auto next = with_budget<node_t>([] {
			return result<bool>{}.with_value(true);
		});
		REQUIRE( next.has_value() );
		CHECK( next.value() == true );
	}

	// The outer call cannot turn a nested failure into an answer: the flag
	// stays up until the outermost call has seen it.
	TEST_CASE("a nested bdd table overflow fails the outer call") {
		budget_guard g;
		tau_api::set_tref_budget(0);
		auto outer = with_budget<node_t>([] {
			auto inner = with_budget<node_t>([] {
				bdd_node_table_exhausted = true;
				return result<bool>{}.with_value(false);
			});
			CHECK_FALSE( inner.has_value() );
			CHECK( bdd_node_table_exhausted );
			return result<bool>{}.with_value(true);
		});
		CHECK_FALSE( outer.has_value() );
		CHECK( outer.has_error() );
		CHECK_FALSE( bdd_node_table_exhausted );
	}

	// Work outside any boundary that filled a table may have left unknown
	// values in the caches: the next call is refused and the caches reset.
	TEST_CASE("a call starting on a full-table flag is refused") {
		budget_guard g;
		tau_api::set_tref_budget(0);
		bdd_node_table_exhausted = true;
		bool called = false;
		auto r = with_budget<node_t>([&] {
			called = true;
			return result<bool>{}.with_value(true);
		});
		CHECK_FALSE( called );
		CHECK_FALSE( r.has_value() );
		CHECK_FALSE( bdd_node_table_exhausted );
	}
}
