// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

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

TEST_SUITE("bv operator hooks") {

	// Division by zero (should not crash, should return nullptr or error node)
	TEST_CASE("division by zero") {
		const char* sample = "{5}:bv[8] / {0}:bv[8]";
		tref src = parse_bf(sample);
		tref expected = parse_bf("1:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("addition of constants") {
		const char* sample = "{1}:bv[8] + {3}:bv[8]";
		tref src = parse_bf(sample);
		tref expected = parse_bf("{4}:bv[8]");
		CHECK( src == expected);
	}

	TEST_CASE("substraction of constants") {
		const char* sample = "{1}:bv[8] - {3}:bv[8]";
		tref src = parse_bf(sample);
		tref expected = parse_bf("{254}:bv[8]");
		CHECK( src == expected);
	}

	TEST_CASE("multiplication of constants") {
		const char* sample = "{3}:bv[8] * {4}:bv[8]";
		tref src = parse_bf(sample);
		tref expected = parse_bf("{12}:bv[8]");
		CHECK( src == expected);
	}

	TEST_CASE("division of constants") {
		const char* sample = "{10}:bv[8] / {3}:bv[8]";
		tref src = parse_bf(sample);
		tref expected = parse_bf("{3}:bv[8]");
		CHECK( src == expected);
	}

	TEST_CASE("mod of constants") {
		const char* sample = "{10}:bv[8] % {3}:bv[8]";
		tref src = parse_bf(sample);
		tref expected = parse_bf("{1}:bv[8]");
		CHECK( src == expected);
	}

	TEST_CASE("shift right of constants") {
		const char* sample = "{16}:bv[8] >> {3}:bv[8]";
		tref src = parse_bf(sample);
		tref expected = parse_bf("{2}:bv[8]");
		CHECK( src == expected);
	}

	TEST_CASE("shift left of constants") {
		const char* sample = "{1}:bv[8] << {3}:bv[8]";
		tref src = parse_bf(sample);
		tref expected = parse_bf("{8}:bv[8]");
		CHECK( src == expected);
	}
}