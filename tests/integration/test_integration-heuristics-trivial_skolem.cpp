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

	TEST_CASE("elimination works when the defining equation is nested inside a disjunction") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("b = 0 || a1 = c");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		CHECK( result == tau::_T() );
	}

	TEST_CASE("elimination inside a disjunction nested within a conjunction") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("e = 0 && (b = 0 || a1 = c)");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		tref expected = parse("e = 0");
		CHECK( result == expected );
	}

	TEST_CASE("atom under implication is not substituted, variable stays kept") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("(a1 = c) -> (b = 0)");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		tref expected = build_wff_ex_many<node_t>({ a1 }, phi);
		CHECK( result == expected );
	}

	TEST_CASE("atom under wff xor is not substituted, variable stays kept") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("(a1 = c) ^^ (b = 0)");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		tref expected = build_wff_ex_many<node_t>({ a1 }, phi);
		CHECK( result == expected );
	}

	TEST_CASE("atom under temporal boundary is not substituted, variable stays kept") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		// The entire formula must be under temporal scope
		tref phi = parse("always (a1 = c && d = e)");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		tref expected = build_wff_ex_many<node_t>({ a1 }, phi);
		CHECK( result == expected );
	}

	TEST_CASE("unique occurrence inside inequality is not eliminated") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("a1 != c");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		tref expected = build_wff_ex_many<node_t>({ a1 }, phi);
		CHECK( result == expected );
	}

	TEST_CASE("two target variables sharing one atom are both kept") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		auto a2 = build_variable<node_t>("a2", tau_type_id<node_t>());
		tref phi = parse("a1 = a2");
		tref result = trivial_skolem_ex<node_t>({ a1, a2 }, phi);
		tref expected = build_wff_ex_many<node_t>({ a1, a2 }, phi);
		CHECK( result == expected );
	}

	TEST_CASE("atom under negation is not substituted, variable stays kept") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("!(a1 = c)");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		tref expected = build_wff_ex_many<node_t>({ a1 }, phi);
		CHECK( result == expected );
	}

	TEST_CASE("atom under negation keeps the variable even when a sibling clause is unaffected") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("!(a1 = c) && d = e");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		tref expected = build_wff_ex_many<node_t>({ a1 }, phi);
		CHECK( result == expected );
	}

	TEST_CASE("a variable with one eligible occurrence and one hidden occurrence is kept") {
		// a1 occurs once in the eligible atom (a1 = c) and again inside the
		// negated atom (a1 = d); eliminating a1 via the first occurrence
		// would leave the second one dangling free once the quantifier is
		// dropped, so a1 must be kept regardless of the eligible atom.
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("a1 = c && !(a1 = d)");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		tref expected = build_wff_ex_many<node_t>({ a1 }, phi);
		CHECK( result == expected );
	}

	TEST_CASE("a variable shared by a desugared equivalence's negated and positive operand is kept") {
		// (a1 = c) <-> (b = 0) desugars to and/or/neg with the (a1 = c)
		// subtree referenced both negated and positively, so a1's global
		// occurrence count is 2 even though it looks like "one clause" at
		// the source level.
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("(a1 = c) <-> (b = 0)");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		tref expected = build_wff_ex_many<node_t>({ a1 }, phi);
		CHECK( result == expected );
	}

	TEST_CASE("atom inside a nested quantifier over a different variable is not substituted") {
		auto a1 = build_variable<node_t>("a1", tau_type_id<node_t>());
		tref phi = parse("(ex y a1 = y) && c = 0");
		tref result = trivial_skolem_ex<node_t>({ a1 }, phi);
		tref expected = build_wff_ex_many<node_t>({ a1 }, phi);
		CHECK( result == expected );
	}
}
