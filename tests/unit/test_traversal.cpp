// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_bool_only_helpers.h"

TEST_SUITE("operator|") {

	TEST_CASE("match zero nodes") {
		const char* sample = "X & Y";
		auto pbf = parse_bf();
		auto rule = tt(tau::get(sample, pbf).value_or(nullptr));
		CHECK( !(rule | tau::main) );
	}

	TEST_CASE("match one node") {
		const char* sample = "X & Y";
		auto pbf = parse_bf();
		auto rule = tt(tau::get(sample, pbf).value_or(nullptr));
		CHECK( (rule | tau::bf_and).size() == 1 );
	}
}

TEST_SUITE("operator||") {

	TEST_CASE("match zero nodes") {
		const char* sample = "X & Y";
		auto pbf = parse_bf();
		auto rule = tt(tau::get(sample, pbf).value_or(nullptr));
		CHECK( (rule || tau::wff).empty() );
	}

	TEST_CASE("match one node") {
		const char* sample = "X & Y";
		auto pbf = parse_bf();
		auto rule = tt(tau::get(sample, pbf).value_or(nullptr));
		CHECK( (rule || tau::bf_and).size() == 1 );
	}

	TEST_CASE("match several nodes") {
		const char* sample = "X & Y";
		auto pbf = parse_bf();
		auto rule = tt(tau::get(sample, pbf).value_or(nullptr));
		CHECK( (rule | tau::bf_and || tau::bf).size() == 2 );
	}
}

TEST_SUITE("traverser extractors") {

	// Runs fn with std::cout redirected and returns what it printed.
	template <typename F>
	static std::string captured_cout(F&& fn) {
		std::stringstream ss;
		auto* old = std::cout.rdbuf(ss.rdbuf());
		fn();
		std::cout.rdbuf(old);
		return ss.str();
	}

	static tref parsed_bf(const char* src) {
		return tau::get(src, parse_bf()).value_or(nullptr);
	}

	TEST_CASE("an empty traverser yields the neutral value of every extractor") {
		tt empty;
		CHECK( !empty );
		CHECK( (empty | tt::nt) == static_cast<node_t::type>(0) );
		CHECK( (empty | tt::ba_constant_id) == 0 );
		CHECK( (empty | tt::ba_type) == 0 );
		CHECK( !(empty | tt::fourth) );
		CHECK( (empty | tt::children_range).begin()
			== (empty | tt::children_range).end() );
		CHECK( (empty | tt::children_trees_range).begin()
			== (empty | tt::children_trees_range).end() );
		CHECK( captured_cout([&]{ (void)(empty | tt::dump); }).empty() );
		CHECK( captured_cout([&]{ (void)(empty | tt::print_tree); }).empty() );
	}

	TEST_CASE("dump and print_tree print the value and pass it through") {
		tref fm = parsed_bf("X & Y");
		REQUIRE( fm != nullptr );
		tt t(fm);
		tt passed;
		std::string dumped = captured_cout([&]{ passed = t | tt::dump; });
		CHECK( (passed | tt::ref) == fm );
		CHECK( dumped == tau::get(fm).dump_to_str() + "\n" );
		std::string printed = captured_cout([&]{ passed = t | tt::print_tree; });
		CHECK( (passed | tt::ref) == fm );
		CHECK( printed == tau::get(fm).tree_to_str() + "\n" );
	}

	TEST_CASE("ba_type and ba_constant_id read the typed node") {
		tref v = tau::build_variable(std::string("x"), bool_type_id<node_t>());
		CHECK( (tt(v) | tt::ba_type) == bool_type_id<node_t>() );
		tref c = tau::get_ba_constant(Bool(true), bool_type<node_t>());
		REQUIRE( c != nullptr );
		CHECK( (tt(c) | tt::ba_constant_id) == tau::get(c).get_ba_constant_id() );
		CHECK( (tt(c) | tt::ba_constant) == node_t::constant(Bool(true)) );
	}

	TEST_CASE("fourth selects the fourth child of every value") {
		tref a = tau::get(tau::var_name, std::string("a"));
		tref b = tau::get(tau::var_name, std::string("b"));
		tref four = tau::get(tau::ref_args, { a, b, a, b });
		tref three = tau::get(tau::ref_args, { a, b, a });
		auto t = tt(trefs{ four, three });
		auto r = t | tt::fourth;
		REQUIRE( r.size() == 1 );
		CHECK( (r | tt::string) == "b" );
	}

	TEST_CASE("children_range iterates the first value's children") {
		tref fm = parsed_bf("X & Y");
		REQUIRE( fm != nullptr );
		tt conj = tt(fm) | tau::bf_and;
		REQUIRE( conj );
		size_t n = 0;
		for (tref c : conj | tt::children_range) {
			CHECK( tau::get(c).is(tau::bf) );
			++n;
		}
		CHECK( n == 2 );
	}

	TEST_CASE("children_trees_range iterates the first value's children") {
		tref fm = parsed_bf("X & Y");
		REQUIRE( fm != nullptr );
		tt conj = tt(fm) | tau::bf_and;
		REQUIRE( conj );
		size_t n = 0;
		for (const auto& c : conj | tt::children_trees_range) {
			CHECK( c.is(tau::bf) );
			++n;
		}
		CHECK( n == 2 );
	}

	TEST_CASE("f maps every value and drops the null results") {
		tref fm = parsed_bf("X & Y");
		REQUIRE( fm != nullptr );
		tt operands = tt(fm) | tau::bf_and || tau::bf;
		REQUIRE( operands.size() == 2 );
		tref first = operands.values()[0];
		auto only_first = operands | tt::f([first](tref n) -> tref {
			return n == first ? tau::get(n).first() : nullptr;
		});
		REQUIRE( only_first.size() == 1 );
		CHECK( (only_first | tt::ref) == tau::get(first).first() );
	}

	TEST_CASE("an optional tref builds a traverser only when it holds one") {
		tref fm = parsed_bf("X & Y");
		REQUIRE( fm != nullptr );
		CHECK( !tt(std::optional<tref>{}) );
		tt t(std::optional<tref>{ fm });
		REQUIRE( t );
		CHECK( (t | tt::ref) == fm );
	}
}
