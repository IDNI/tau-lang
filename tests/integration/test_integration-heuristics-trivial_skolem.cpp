// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "heuristics/trivial_skolem.h"

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

TEST_SUITE("trivial_skolem") {

	TEST_CASE("direct assignment eliminates the variable") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("a1 = c");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		CHECK( result == tau::_T() );
	}

	TEST_CASE("leftover clause is preserved when a variable is eliminated") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("a1 = c && d = e");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		tref expected = parse("d = e");
		CHECK( result == expected );
	}

	TEST_CASE("variable occurring in zero clauses is kept") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("c = d");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		tref expected = build_wff_ex_many<node_t>({ a1 }, phi);
		CHECK( result == expected );
	}

	TEST_CASE("variable occurring in two clauses is kept, both clauses preserved") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("a1 = c && a1 = d");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		tref expected = build_wff_ex_many<node_t>({ a1 }, phi);
		CHECK( result == expected );
	}

	TEST_CASE("non-invertible operator keeps the variable quantified") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("a1 & c = 0");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		tref expected = build_wff_ex_many<node_t>({ a1 }, phi);
		CHECK( result == expected );
	}

	TEST_CASE("witness referencing another target variable is rejected, both kept") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		auto a2 = build_variable<node_t>("a2", tau_type_id<node_t>());
		tref phi = parse("a1 = a2 && c = 0");
		tref result = trivial_skolem_ex<node_t>({ a1, a2 }, phi);
		tref expected = build_wff_ex_many<node_t>({ a1, a2 }, phi);
		CHECK( result == expected );
	}

	TEST_CASE("negation is invertible") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("a1' = c");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		CHECK( result == tau::_T() );
	}

	TEST_CASE("xor is invertible (left operand)") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("a1 ^ c = d");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		CHECK( result == tau::_T() );
	}

	TEST_CASE("xor is invertible (right operand)") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("c ^ a1 = d");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		CHECK( result == tau::_T() );
	}

	TEST_CASE("addition is invertible (left operand)") {
		auto a1 = build_variable<node_t>("a1", bv_type_id<node_t>(4));
		tref phi = parse("a1:bv[4] + c:bv[4] = d:bv[4]");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		CHECK( result == tau::_T() );
	}

	TEST_CASE("addition is invertible (right operand)") {
		auto a1 = build_variable<node_t>("a1", bv_type_id<node_t>(4));
		tref phi = parse("c:bv[4] + a1:bv[4] = d:bv[4]");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		CHECK( result == tau::_T() );
	}

	TEST_CASE("subtraction is invertible (minuend, left operand)") {
		auto a1 = build_variable<node_t>("a1", bv_type_id<node_t>(4));
		tref phi = parse("a1:bv[4] - c:bv[4] = d:bv[4]");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		CHECK( result == tau::_T() );
	}

	TEST_CASE("subtraction is invertible (subtrahend, right operand)") {
		auto a1 = build_variable<node_t>("a1", bv_type_id<node_t>(4));
		tref phi = parse("c:bv[4] - a1:bv[4] = d:bv[4]");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		CHECK( result == tau::_T() );
	}

	TEST_CASE("chained composed operators eliminate the variable") {
		auto a1 = build_variable<node_t>("a1", bv_type_id<node_t>(4));
		tref phi = parse("(a1:bv[4] + c:bv[4])' ^ e:bv[4] = d:bv[4]");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		CHECK( result == tau::_T() );
	}

	TEST_CASE("all variables eliminated returns the literal T") {
		auto a1 = build_variable<node_t>("a1", bv_type_id<node_t>(4));
		auto a2 = build_variable<node_t>("a2", bv_type_id<node_t>(4));
		auto a3 = build_variable<node_t>("a3", bv_type_id<node_t>(4));
		tref phi = parse(
			"a1:bv[4] = c:bv[4] && a2:bv[4] ^ c:bv[4] = d:bv[4] "
			"&& a3:bv[4] - c:bv[4] = e:bv[4]");
		tref result = trivial_skolem_ex<node_t>({ a1, a2, a3 }, phi);
		CHECK( result == tau::_T() );
	}

	TEST_CASE("partial elimination keeps only non-invertible variables quantified") {
		auto a1 = build_variable<node_t>("a1", bv_type_id<node_t>(4));
		auto a2 = build_variable<node_t>("a2", bv_type_id<node_t>(4));
		auto a3 = build_variable<node_t>("a3", bv_type_id<node_t>(4));
		tref phi = parse(
			"a1:bv[4] = c:bv[4] + f:bv[4] "
			"&& a2:bv[4] & c:bv[4] = {0}:bv[4] "
			"&& a3:bv[4] ^ c:bv[4] = {0}:bv[4] "
			"&& d:bv[4] = {0}:bv[4]");
		tref result = trivial_skolem_ex<node_t>({ a1, a2, a3 }, phi);
		tref expected_matrix = build_wff_and<node_t>(
			parse("a2:bv[4] & c:bv[4] = {0}:bv[4]"),
			parse("d:bv[4] = {0}:bv[4]"));
		tref expected = build_wff_ex_many<node_t>({ a2 }, expected_matrix);
		CHECK( result == expected );
		CHECK( !contains<node_t>(result, a1) );
		CHECK( !contains<node_t>(result, a3) );
	}
}
