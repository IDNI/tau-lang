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
		tref result = ex_subs_based_elimination<node_t>(var, ex_clause);
		CHECK( result != ex_clause );
	}

	TEST_CASE("simple case subs applied (y2)") {
		const char* sample =
			"(x = a || x = c) && x = b";
		auto var = build_variable<node_t>("x", tau_type_id<node_t>()); // tau typed variable 'x'
		tref ex_clause = parse(sample);
		tref result = ex_subs_based_elimination<node_t>(var, ex_clause);
		CHECK( result != ex_clause );
	}

	TEST_CASE("simple case no subs applied (y1)") {
		const char* sample =
			"x & a = 0 && y = b";
		auto var = build_variable<node_t>("x", tau_type_id<node_t>()); // tau typed variable 'x'
		tref ex_clause = parse(sample);
		tref result = ex_subs_based_elimination<node_t>(var, ex_clause);
		CHECK( result == ex_clause );
	}

	TEST_CASE("simple case no subs applied (y2)") {
		const char* sample =
			"(x = a || x = c) && x & b = 1";
		auto var = build_variable<node_t>("x", tau_type_id<node_t>()); // tau typed variable 'x'
		tref ex_clause = parse(sample);
		tref result = ex_subs_based_elimination<node_t>(var, ex_clause);
		CHECK( result == ex_clause );
	}

	TEST_CASE("simple case neg no subs applied (y2)") {
		const char* sample =
			"x' & b = 1";
		auto var = build_variable<node_t>("x", tau_type_id<node_t>()); // tau typed variable 'x'
		tref ex_clause = parse(sample);
		tref result = ex_subs_based_elimination<node_t>(var, ex_clause);
		CHECK( result == ex_clause );
	}

	TEST_CASE("occurs check: no subs when candidate contains var") {
		// x = x|y is not a valid substitution for x (x occurs in x|y);
		// substituting would leak the bound variable free
		const char* sample =
			"x = x | y && y = b";
		auto var = build_variable<node_t>("x", tau_type_id<node_t>()); // tau typed variable 'x'
		tref ex_clause = parse(sample);
		tref result = ex_subs_based_elimination<node_t>(var, ex_clause);
		CHECK( result == ex_clause );
	}

	TEST_CASE("occurs check: self-referential eq skipped, valid eq used") {
		// ex x (x = x|y && x = 0) ≡ y = 0: the candidate x|y must be
		// rejected but x = 0 is a valid substitution, so the result
		// must not contain x anymore
		const char* sample =
			"x = x | y && x = 0";
		auto var = build_variable<node_t>("x", tau_type_id<node_t>()); // tau typed variable 'x'
		tref ex_clause = parse(sample);
		tref result = ex_subs_based_elimination<node_t>(var, ex_clause);
		CHECK( result != ex_clause );
		CHECK( !contains<node_t>(result, var) );
	}

	TEST_CASE("occurs check: trivial self equation ignored") {
		const char* sample =
			"x = x && y = b";
		auto var = build_variable<node_t>("x", tau_type_id<node_t>()); // tau typed variable 'x'
		tref ex_clause = parse(sample);
		tref result = ex_subs_based_elimination<node_t>(var, ex_clause);
		CHECK( result == ex_clause );
	}

	TEST_CASE("capture check: no subs when term var is re-bound by inner quantifier") {
		// x = y && ex y (x & y = 0): substituting x := y inside the inner
		// 'ex y' scope would capture y, so the substitution must be
		// rejected entirely. The clause is built programmatically with
		// calculate_quant_id = false because both the parser and the
		// default builders alpha-rename bound variables; the eliminators,
		// however, can produce such repeated-name shapes internally
		auto var = build_variable<node_t>("x", tau_type_id<node_t>()); // tau typed variable 'x'
		auto y = build_variable<node_t>("y", tau_type_id<node_t>());
		auto bx = build_bf_variable<node_t>("x", tau_type_id<node_t>());
		auto by = build_bf_variable<node_t>("y", tau_type_id<node_t>());
		tref eq = build_bf_eq<node_t>(bx, by);
		tref inner = build_bf_eq_0<node_t>(build_bf_and<node_t>(bx, by));
		tref ex_clause = build_wff_and<node_t>(eq,
			build_wff_ex<node_t>(y, inner, false));
		tref result = ex_subs_based_elimination<node_t>(var, ex_clause);
		CHECK( result == ex_clause );
	}

	TEST_CASE("capture check: subs applied when inner binder is distinct from term vars") {
		// same shape but the inner quantifier binds z, not y, so x := y
		// is capture-free and must be applied
		auto var = build_variable<node_t>("x", tau_type_id<node_t>()); // tau typed variable 'x'
		auto z = build_variable<node_t>("z", tau_type_id<node_t>());
		auto bx = build_bf_variable<node_t>("x", tau_type_id<node_t>());
		auto by = build_bf_variable<node_t>("y", tau_type_id<node_t>());
		auto bz = build_bf_variable<node_t>("z", tau_type_id<node_t>());
		tref eq = build_bf_eq<node_t>(bx, by);
		tref inner = build_bf_eq_0<node_t>(build_bf_and<node_t>(bx, bz));
		tref ex_clause = build_wff_and<node_t>(eq,
			build_wff_ex<node_t>(z, inner, false));
		tref result = ex_subs_based_elimination<node_t>(var, ex_clause);
		CHECK( result != ex_clause );
		CHECK( !contains<node_t>(result, var) );
	}
}