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

TEST_SUITE("ex_subs_based_elimination") {

	TEST_CASE("ex_subs_based_elimination") {
		const char* sample =
			"x = a && y = b";
		auto var = build_variable<node_t>("x", tau_type_id<node_t>()); // tau typed variable 'x'
		tref ex_clause = parse(sample);
		tref result = ex_subs_based_elimination<node_t>(ex_clause, var);
		CHECK( result != ex_clause );
	}
}