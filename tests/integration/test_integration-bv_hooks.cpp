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

TEST_SUITE("bv operator hooks:intermediate cases") {
	// --- Intermediate value tests for all operators ---

	// Addition
	TEST_CASE("addition intermediate values") {
		CHECK(parse_bf("{10}:bv[8] + {20}:bv[8]") == parse_bf("{30}:bv[8]"));
		CHECK(parse_bf("{100}:bv[8] + {155}:bv[8]") == parse_bf("{255}:bv[8]"));
		CHECK(parse_bf("{200}:bv[8] + {100}:bv[8]") == parse_bf("{44}:bv[8]")); // 300 mod 256 = 44
	}

	// Subtraction
	TEST_CASE("subtraction intermediate values") {
		CHECK(parse_bf("{50}:bv[8] - {20}:bv[8]") == parse_bf("{30}:bv[8]"));
		CHECK(parse_bf("{20}:bv[8] - {50}:bv[8]") == parse_bf("{226}:bv[8]")); // wrap
		CHECK(parse_bf("{100}:bv[8] - {44}:bv[8]") == parse_bf("{56}:bv[8]"));
	}

	// Multiplication
	TEST_CASE("multiplication intermediate values") {
		CHECK(parse_bf("{10}:bv[8] * {20}:bv[8]") == parse_bf("{200}:bv[8]"));
		CHECK(parse_bf("{15}:bv[8] * {17}:bv[8]") == parse_bf("{255}:bv[8]"));
		CHECK(parse_bf("{100}:bv[8] * {3}:bv[8]") == parse_bf("{44}:bv[8]")); // 300 mod 256 = 44
	}

	// Division
	TEST_CASE("division intermediate values") {
		CHECK(parse_bf("{100}:bv[8] / {10}:bv[8]") == parse_bf("{10}:bv[8]"));
		CHECK(parse_bf("{200}:bv[8] / {15}:bv[8]") == parse_bf("{13}:bv[8]"));
		CHECK(parse_bf("{44}:bv[8] / {2}:bv[8]") == parse_bf("{22}:bv[8]"));
	}

	// Modulo
	TEST_CASE("modulo intermediate values") {
		CHECK(parse_bf("{100}:bv[8] % {15}:bv[8]") == parse_bf("{10}:bv[8]"));
		CHECK(parse_bf("{200}:bv[8] % {17}:bv[8]") == parse_bf("{13}:bv[8]"));
		CHECK(parse_bf("{44}:bv[8] % {7}:bv[8]") == parse_bf("{2}:bv[8]"));
	}

	// Shift right
	TEST_CASE("shift right intermediate values") {
		CHECK(parse_bf("{128}:bv[8] >> {1}:bv[8]") == parse_bf("{64}:bv[8]"));
		CHECK(parse_bf("{255}:bv[8] >> {4}:bv[8]") == parse_bf("{15}:bv[8]"));
		CHECK(parse_bf("{44}:bv[8] >> {2}:bv[8]") == parse_bf("{11}:bv[8]"));
	}

	// Shift left
	TEST_CASE("shift left intermediate values") {
		CHECK(parse_bf("{2}:bv[8] << {3}:bv[8]") == parse_bf("{16}:bv[8]"));
		CHECK(parse_bf("{15}:bv[8] << {2}:bv[8]") == parse_bf("{60}:bv[8]"));
		CHECK(parse_bf("{44}:bv[8] << {1}:bv[8]") == parse_bf("{88}:bv[8]"));
	}
}

TEST_SUITE("bv operator hooks:corner cases") {
	// --- Corner tests for all operators ---

	// --- Division corner cases ---
	TEST_CASE("division 0 / 0") {
		tref src = parse_bf("{0}:bv[8] / {0}:bv[8]");
		tref expected = parse_bf("1:bv[8]"); // <- top element 1111....
		CHECK(src == expected);
	}

	TEST_CASE("division 1 / 0") {
		tref src = parse_bf("{1}:bv[8] / {0}:bv[8]");
		tref expected = parse_bf("1:bv[8]"); // <- top element 1111....
		CHECK(src == expected);
	}

	TEST_CASE("division 0 / 1") {
		tref src = parse_bf("{0}:bv[8] / {1}:bv[8]");
		tref expected = parse_bf("0:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("division X / 1") {
		tref src = parse_bf("{42}:bv[8] / {1}:bv[8]");
		tref expected = parse_bf("{42}:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("division X / X") {
		tref src = parse_bf("{77}:bv[8] / {77}:bv[8]");
		tref expected = parse_bf("{1}:bv[8]"); // <- 1 (not top element)
		CHECK(src == expected);
	}

	// --- Modulo corner cases ---
	TEST_CASE("modulo 0 % 0") {
		tref src = parse_bf("{0}:bv[8] % {0}:bv[8]");
		tref expected = parse_bf("0:bv[8]"); // <- same element (not bottom element)
		CHECK(src == expected);
	}

	TEST_CASE("modulo 1 % 0") {
		tref src = parse_bf("{1}:bv[8] % {0}:bv[8]");
		tref expected = parse_bf("{1}:bv[8]"); // <- same element (not bottom element)
		CHECK(src == expected);
	}

	TEST_CASE("modulo 0 % 1") {
		tref src = parse_bf("{0}:bv[8] % {1}:bv[8]");
		tref expected = parse_bf("0:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("modulo X % 1") {
		tref src = parse_bf("{99}:bv[8] % {1}:bv[8]");
		tref expected = parse_bf("0:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("modulo X % X") {
		tref src = parse_bf("{77}:bv[8] % {77}:bv[8]");
		tref expected = parse_bf("0:bv[8]");
		CHECK(src == expected);
	}

	// --- Addition corner cases ---
	TEST_CASE("addition 0 + 0") {
		tref src = parse_bf("{0}:bv[8] + {0}:bv[8]");
		tref expected = parse_bf("0:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("addition 0 + X") {
		tref src = parse_bf("{0}:bv[8] + {55}:bv[8]");
		tref expected = parse_bf("{55}:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("addition X + 0") {
		tref src = parse_bf("{99}:bv[8] + {0}:bv[8]");
		tref expected = parse_bf("{99}:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("addition 1 + 1") {
		tref src = parse_bf("{1}:bv[8] + {1}:bv[8]");
		tref expected = parse_bf("{2}:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("addition X + 1") {
		tref src = parse_bf("{42}:bv[8] + {1}:bv[8]");
		tref expected = parse_bf("{43}:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("addition 1 + X") {
		tref src = parse_bf("{1}:bv[8] + {42}:bv[8]");
		tref expected = parse_bf("{43}:bv[8]");
		CHECK(src == expected);
	}

	// --- Subtraction corner cases ---
	TEST_CASE("subtraction 0 - 0") {
		tref src = parse_bf("{0}:bv[8] - {0}:bv[8]");
		tref expected = parse_bf("0:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("subtraction 0 - X") {
		tref src = parse_bf("{0}:bv[8] - {5}:bv[8]");
		tref expected = parse_bf("{251}:bv[8]"); // 256-5=251 (wrap)
		CHECK(src == expected);
	}

	TEST_CASE("subtraction X - 0") {
		tref src = parse_bf("{77}:bv[8] - {0}:bv[8]");
		tref expected = parse_bf("{77}:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("subtraction 1 - 1") {
		tref src = parse_bf("{1}:bv[8] - {1}:bv[8]");
		tref expected = parse_bf("0:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("subtraction X - 1") {
		tref src = parse_bf("{42}:bv[8] - {1}:bv[8]");
		tref expected = parse_bf("{41}:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("subtraction 1 - X") {
		tref src = parse_bf("{1}:bv[8] - {42}:bv[8]");
		tref expected = parse_bf("{215}:bv[8]"); // 1-42 mod 256 = 215
		CHECK(src == expected);
	}

	TEST_CASE("subtraction X - X") {
		tref src = parse_bf("{77}:bv[8] - {77}:bv[8]");
		tref expected = parse_bf("0:bv[8]");
		CHECK(src == expected);
	}

	// --- Multiplication corner cases ---
	TEST_CASE("multiplication 0 * 0") {
		tref src = parse_bf("{0}:bv[8] * {0}:bv[8]");
		tref expected = parse_bf("0:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("multiplication 0 * X") {
		tref src = parse_bf("{0}:bv[8] * {99}:bv[8]");
		tref expected = parse_bf("0:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("multiplication X * 0") {
		tref src = parse_bf("{99}:bv[8] * {0}:bv[8]");
		tref expected = parse_bf("0:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("multiplication 1 * 1") {
		tref src = parse_bf("{1}:bv[8] * {1}:bv[8]");
		tref expected = parse_bf("{1}:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("multiplication X * 1") {
		tref src = parse_bf("{42}:bv[8] * {1}:bv[8]");
		tref expected = parse_bf("{42}:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("multiplication 1 * X") {
		tref src = parse_bf("{1}:bv[8] * {42}:bv[8]");
		tref expected = parse_bf("{42}:bv[8]");
		CHECK(src == expected);
	}

	// --- Shift right corner cases ---
	TEST_CASE("shift right by 0") {
		tref src = parse_bf("{128}:bv[8] >> {0}:bv[8]");
		tref expected = parse_bf("{128}:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("shift right by width") {
		tref src = parse_bf("{255}:bv[8] >> {8}:bv[8]");
		tref expected = parse_bf("0:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("shift right by more than width") {
		tref src = parse_bf("{255}:bv[8] >> {12}:bv[8]");
		tref expected = parse_bf("0:bv[8]");
		CHECK(src == expected);
	}

	// --- Shift left corner cases ---
	TEST_CASE("shift left by 0") {
		tref src = parse_bf("{1}:bv[8] << {0}:bv[8]");
		tref expected = parse_bf("{1}:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("shift left by width") {
		tref src = parse_bf("{1}:bv[8] << {8}:bv[8]");
		tref expected = parse_bf("0:bv[8]");
		CHECK(src == expected);
	}

	TEST_CASE("shift left by more than width") {
		tref src = parse_bf("{1}:bv[8] << {12}:bv[8]");
		tref expected = parse_bf("0:bv[8]");
		CHECK(src == expected);
	}
}