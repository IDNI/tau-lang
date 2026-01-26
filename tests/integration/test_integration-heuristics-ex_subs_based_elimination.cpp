// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "heuristics/ex_subs_based_elimination.h"

tref parse(const std::string& sample) {
	static tau::get_options opts{ .parse = { .start = tau::wff } };
	auto src = tree<node_t>::get(sample, opts);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}

TEST_SUITE("configuration") {

	TEST_CASE("logging") {
		logging::trace();
	}
}

TEST_SUITE("ex_subs_based_elimination") {

	TEST_CASE("simple case subs applied (y1)") {
		const char* sample =
			"x = a && y = b";
		auto var = build_variable<node_t>("x", tau_type_id<node_t>()); // tau typed variable 'x'
		tref ex_clause = parse(sample);
		tref result = ex_subs_based_elimination<node_t>(ex_clause, var);
		CHECK( result != ex_clause );
	}

	TEST_CASE("simple case subs applied (y2)") {
		const char* sample =
			"(x = a || x = c) && x = b";
		auto var = build_variable<node_t>("x", tau_type_id<node_t>()); // tau typed variable 'x'
		tref ex_clause = parse(sample);
		tref result = ex_subs_based_elimination<node_t>(ex_clause, var);
		CHECK( result != ex_clause );
	}

	TEST_CASE("simple case no subs applied (y1)") {
		const char* sample =
			"x & a = 0 && y = b";
		auto var = build_variable<node_t>("x", tau_type_id<node_t>()); // tau typed variable 'x'
		tref ex_clause = parse(sample);
		tref result = ex_subs_based_elimination<node_t>(ex_clause, var);
		CHECK( result == ex_clause );
	}

	TEST_CASE("simple case no subs applied (y2)") {
		const char* sample =
			"(x = a || x = c) && x & b = 1";
		auto var = build_variable<node_t>("x", tau_type_id<node_t>()); // tau typed variable 'x'
		tref ex_clause = parse(sample);
		tref result = ex_subs_based_elimination<node_t>(ex_clause, var);
		CHECK( result == ex_clause );
	}

	TEST_CASE("simple case neg no subs applied (y2)") {
		const char* sample =
			"x' & b = 1";
		auto var = build_variable<node_t>("x", tau_type_id<node_t>()); // tau typed variable 'x'
		tref ex_clause = parse(sample);
		tref result = ex_subs_based_elimination<node_t>(ex_clause, var);
		CHECK( result == ex_clause );
	}
}