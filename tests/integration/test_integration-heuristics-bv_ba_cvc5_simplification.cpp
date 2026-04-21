// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "testing"

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}
}

tref parse_bf(const std::string& sample) {
	static tree<node_t>::get_options opts{ .parse = { .start = tree<node_t>::bf }};
	auto src = tree<node_t>::get(sample, opts);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}

TEST_SUITE("ba bv cvc5 constant simplification") {

	// Division by zero (should not crash, should return nullptr or error node)
	TEST_CASE("division by zero") {
		const char* sample = "{5}:bv[8] / {0}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		// Accept nullptr or a special error node, but must not crash
		CHECK((simplified == nullptr || simplified != src));
	}

	// Two's complement negation of constant
	TEST_CASE("twos complement negation") {
		const char* sample = "{5}:bv[8]'";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		// Should be 251 (0xFB) for 8-bit two's complement
		const char* expected_str = "{251}:bv[8]";
		tref expected = parse_bf(expected_str);
		CHECK(simplified != nullptr);
		CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(expected));
	}

	// Variable name edge case (should not crash)
	TEST_CASE("variable name edge case") {
		const char* sample = "|:bv[8]";
		tref src = parse_bf(sample);
		bv_ba_cvc5_simplification<node_t>(src);
		// Should not crash, may be nullptr if not a valid variable
		CHECK(true);
	}

	// Chained constant addition (should fully flatten)
	TEST_CASE("chained addition flattening") {
		const char* sample = "{1}:bv[8] + {2}:bv[8] + {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		const char* expected_str = "{6}:bv[8]";
		tref expected = parse_bf(expected_str);
		CHECK(simplified != nullptr);
		CHECK(tree<node_t>::get(simplified) == tree<node_t>::get(expected));
	}

	TEST_CASE("addition of constants") {
		const char* sample = "{1}:bv[8] + {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified != src);
	}

	TEST_CASE("substraction of constants") {
		const char* sample = "{1}:bv[8] - {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified != src);
	}

	TEST_CASE("multiplication of constants") {
		const char* sample = "{3}:bv[8] * {4}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified != src);
	}

	TEST_CASE("division of constants") {
		const char* sample = "{10}:bv[8] / {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified != src);
	}

	TEST_CASE("mod of constants") {
		const char* sample = "{10}:bv[8] % {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified != src);
	}

	TEST_CASE("shift right of constants") {
		const char* sample = "{16}:bv[8] >> {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified != src);
	}

	TEST_CASE("shift left of constants") {
		const char* sample = "{1}:bv[8] << {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified != src);
	}

	TEST_CASE("not of constant") {
		const char* sample = "{0}:bv[8]'";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == src);
	}

	TEST_CASE("and of constants") {
		const char* sample = "{5}:bv[8] & {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified != src);
	}

	TEST_CASE("nand of constants") {
		const char* sample = "{5}:bv[8] !& {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified != src);
	}

	TEST_CASE("or of constants") {
		const char* sample = "{5}:bv[8] | {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified != src);
	}

	TEST_CASE("nor of constants") {
		const char* sample = "{5}:bv[8] !| {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified != src);
	}

	TEST_CASE("xor of constants") {
		const char* sample = "{5}:bv[8] ^ {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified != src);
	}

	TEST_CASE("xnor of constants") {
		const char* sample = "{5}:bv[8] !^ {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified != src);
	}
}

TEST_SUITE("ba bv cvc5 constant/variable simplification") {

	TEST_CASE("addition of variable/constant") {
		const char* sample = "x:bv[8] + {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == src);
	}

	TEST_CASE("addition of constant/variable") {
		const char* sample = "{3}:bv[8] + x:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == src);
	}

	TEST_CASE("substraction of constant/variable") {
		const char* sample = "{1}:bv[8] - x:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		// (bvadd #b00000001 (bvneg x)) cvc5 is using two's complement
		CHECK( simplified != nullptr );
	}

	TEST_CASE("substraction of variable/constant") {
		const char* sample = "x:bv[8] - {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		// (bvadd x #b11111101) cvc5 is using two's complement
		CHECK( simplified != nullptr );
	}

	TEST_CASE("substraction of constant/variable") {
		const char* sample = "{1}:bv[8] - {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified != src);
	}

	TEST_CASE("multiplication of variable/constant (y1)") {
		const char* sample = "x:bv[8] * {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == src);
	}

	// When multiplying by a power of two cvc5 simplifies the multiplication
	// to a concat and an extraction.
	TEST_CASE("multiplication of variable/constant (y2)") {
		const char* sample = "x:bv[8] * {2}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		//  (concat ((_ extract 6 0) x) #b0)
		CHECK( simplified != nullptr );
	}

	// When multiplying by a power of two cvc5 simplifies the multiplication
	// to a concat and an extraction.
	TEST_CASE("multiplication of variable/constant (y3)") {
		const char* sample = "x:bv[8] * {4}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		// (concat ((_ extract 5 0) x) b00)
		CHECK( simplified != nullptr );
	}

	TEST_CASE("multiplication of constant/variable") {
		const char* sample = "{3}:bv[8] * x:bv[8]";
		// Cvc5 reorder the multiplication!
		const char* result = "x:bv[8] * {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		auto expected = parse_bf(result);
		CHECK( simplified != nullptr );
		CHECK( simplified == expected);
	}

	TEST_CASE("division of constant/variable") {
		const char* sample = "{10}:bv[8] / x:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == src);
	}

	TEST_CASE("division of variable/constant") {
		const char* sample = "x:bv[8] / {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == src);
	}

	TEST_CASE("mod of constant/variable") {
		const char* sample = "{10}:bv[8] % x:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == src);
	}

	TEST_CASE("mod of variable/constant") {
		const char* sample = "x:bv[8] % {3}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == src);
	}

	TEST_CASE("shift right of variable/constant") {
		const char* sample = "x:bv[8] >> {0}:bv[8]";
		const char* result = "x:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		auto expected = parse_bf(result);
		CHECK( simplified != nullptr );
		CHECK( simplified == expected);
	}

	TEST_CASE("shift right of constant/variable") {
		const char* sample = "{0}:bv[8] >> x:bv[8]";
		const char* result = "0:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		auto expected = parse_bf(result);
		CHECK( simplified != nullptr );
		CHECK( simplified == expected);
	}

	TEST_CASE("shift left of variable/constant") {
		const char* sample = "x:bv[8] << {0}:bv[8]";
		const char* result = "x:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		auto expected = parse_bf(result);
		CHECK( simplified != nullptr );
		CHECK( simplified == expected);
	}

	TEST_CASE("shift left of constant/variable") {
		const char* sample = "{0}:bv[8] << x:bv[8]";
		const char* result = "0:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		auto expected = parse_bf(result);
		CHECK( simplified != nullptr );
		CHECK( simplified == expected);
	}

	TEST_CASE("not of variable") {
		const char* sample = "x:bv[8]'";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == src);
	}

	TEST_CASE("and of variable/constant") {
		const char* sample = "x:bv[8] & {0}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == src);
	}

	TEST_CASE("and of constant/variable") {
		const char* sample = "{0}:bv[8] & x:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == src);
	}

	TEST_CASE("nand of variable/constant") {
		const char* sample = "x:bv[8] !& {0}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == src);
	}

	TEST_CASE("nand of constant/variable") {
		const char* sample = "{0}:bv[8] !& x:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == src);
	}

	TEST_CASE("or of variable/constant") {
		const char* sample = "x:bv[8] | {0}:bv[8]";
		const char* result = "x:bv[8]";
		tref src = parse_bf(sample);
		tref expected = parse_bf(result);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == expected);
	}

	TEST_CASE("or of constant/variable") {
		const char* sample = "{0}:bv[8] | x:bv[8]";
		const char* result = "x:bv[8]";
		tref src = parse_bf(sample);
		tref expected = parse_bf(result);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == expected);
	}

	TEST_CASE("nor of variable/constant") {
		const char* sample = "x:bv[8] !| {0}:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == src);
	}

	TEST_CASE("nor of constant/variable") {
		const char* sample = "{0}:bv[8] !| x:bv[8]";
		tref src = parse_bf(sample);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == src);
	}

	TEST_CASE("xor of variable/constant") {
		const char* sample = "x:bv[8] ^ {0}:bv[8]";
		const char* result = "x:bv[8]";
		tref src = parse_bf(sample);
		tref expected = parse_bf(result);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == expected);
	}

	TEST_CASE("xor of constant/variable") {
		const char* sample = "{0}:bv[8] ^ x:bv[8]";
		const char* result = "x:bv[8]";
		tref src = parse_bf(sample);
		tref expected = parse_bf(result);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == expected);
	}

	TEST_CASE("xnor of variable/constant") {
		const char* sample = "x:bv[8] !^ {0}:bv[8]";
		const char* result = "x:bv[8]'";
		tref src = parse_bf(sample);
		tref expected = parse_bf(result);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == expected);
	}

	TEST_CASE("xnor of constant/variable") {
		const char* sample = "{0}:bv[8] !^ x:bv[8]";
		const char* result = "x:bv[8]'";
		tref src = parse_bf(sample);
		tref expected = parse_bf(result);
		tref simplified = bv_ba_cvc5_simplification<node_t>(src);
		CHECK( simplified != nullptr );
		CHECK( simplified == expected);
	}
}

TEST_SUITE("ba bv cvc5 power of 2/variable simplification") {

}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
