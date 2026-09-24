// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

using tau_api = api<node_t>;

TEST_SUITE("Tau API - reset") {

	TEST_CASE("frees the nodes nothing holds") {
		tau_api::reset();
		const size_t base = tau_api::tref_count();
		REQUIRE( tau_api::get_formula("(a = 0 && b = 0) || (c != 0 && d = 0)")
			.has_value() );
		REQUIRE( tau_api::get_term("(p & q) | (r & s) | (t' & u)")
			.has_value() );
		const size_t grown = tau_api::tref_count();
		REQUIRE( grown > base );
		CHECK( tau_api::reset() > 0 );
		CHECK( tau_api::tref_count() < grown );
	}

	TEST_CASE("a second reset has nothing left to free") {
		tau_api::reset();
		CHECK( tau_api::reset() == 0 );
	}

	TEST_CASE("a value held as htref survives") {
		auto h = tau_api::geth_term("(x & y) | z'");
		REQUIRE( h.has_value() );
		const std::string before = tau_api::to_str(h.value());
		tau_api::reset();
		CHECK( tau_api::to_str(h.value()) == before );
	}

	TEST_CASE("drops the definitions") {
		REQUIRE( tau_api::get_definition("P(x) := x > 0").has_value() );
		REQUIRE( definitions<node_t>::instance().size() > 0 );
		tau_api::reset();
		CHECK( definitions<node_t>::instance().size() == 0 );
	}

	TEST_CASE("the engine answers the same after a reset") {
		const std::string fm = "x = 0 && (y != 0 || x != 0)";
		auto first = tau_api::sat(fm);
		REQUIRE( first.has_value() );
		tau_api::reset();
		auto second = tau_api::sat(fm);
		REQUIRE( second.has_value() );
		CHECK( first.value() == second.value() );
	}

#ifdef TAU_PACK_HAS_BA_SBF
	TEST_CASE("a stream may be retyped and a spec run after a reset") {
		{
			auto first = tau_api::get_interpreter("o7[t]:sbf = i7[t]:sbf.");
			REQUIRE( first.has_value() );
		}
		tau_api::reset();
		auto i = tau_api::get_interpreter("o7[t]:tau = o7[t]:tau.");
		REQUIRE( i.has_value() );
		auto again = tau_api::get_interpreter("o8[t]:sbf = i8[t]:sbf.");
		REQUIRE( again.has_value() );
		auto maybe_inputs = tau_api::get_inputs_for_step(again.value());
		REQUIRE( maybe_inputs.has_value() );
		std::map<stream_at, std::string> assigned;
		for (const auto& at : maybe_inputs.value()) assigned[at] = "1";
		CHECK( tau_api::step(again.value(), assigned).has_value() );
	}
#endif // TAU_PACK_HAS_BA_SBF
}
