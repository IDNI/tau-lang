// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "parser_helper.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("bv operator hooks:intermediate cases") {
	// --- Intermediate value tests for all operators ---

	// Addition
	TEST_CASE("addition intermediate values") {
		CHECK(tau::get("{10}:bv[8] + {20}:bv[8]", parse_opts_bf) == tau::get("{30}:bv[8]", parse_opts_bf));
		CHECK(tau::get("{100}:bv[8] + {155}:bv[8]", parse_opts_bf) == tau::get("{255}:bv[8]", parse_opts_bf));
		CHECK(tau::get("{200}:bv[8] + {100}:bv[8]", parse_opts_bf) == tau::get("{44}:bv[8]", parse_opts_bf)); // 300 mod 256 = 44
	}

	// Subtraction
	TEST_CASE("subtraction intermediate values") {
		CHECK(tau::get("{50}:bv[8] - {20}:bv[8]", parse_opts_bf) == tau::get("{30}:bv[8]", parse_opts_bf));
		CHECK(tau::get("{20}:bv[8] - {50}:bv[8]", parse_opts_bf) == tau::get("{226}:bv[8]", parse_opts_bf)); // wrap
		CHECK(tau::get("{100}:bv[8] - {44}:bv[8]", parse_opts_bf) == tau::get("{56}:bv[8]", parse_opts_bf));
	}

	// Multiplication
	TEST_CASE("multiplication intermediate values") {
		CHECK(tau::get("{10}:bv[8] * {20}:bv[8]", parse_opts_bf) == tau::get("{200}:bv[8]", parse_opts_bf));
		CHECK(tau::get("{15}:bv[8] * {17}:bv[8]", parse_opts_bf) == tau::get("{255}:bv[8]", parse_opts_bf));
		CHECK(tau::get("{100}:bv[8] * {3}:bv[8]", parse_opts_bf) == tau::get("{44}:bv[8]", parse_opts_bf)); // 300 mod 256 = 44
	}

	// Division
	TEST_CASE("division intermediate values") {
		CHECK(tau::get("{100}:bv[8] / {10}:bv[8]", parse_opts_bf) == tau::get("{10}:bv[8]", parse_opts_bf));
		CHECK(tau::get("{200}:bv[8] / {15}:bv[8]", parse_opts_bf) == tau::get("{13}:bv[8]", parse_opts_bf));
		CHECK(tau::get("{44}:bv[8] / {2}:bv[8]", parse_opts_bf) == tau::get("{22}:bv[8]", parse_opts_bf));
	}

	// Modulo
	TEST_CASE("modulo intermediate values") {
		CHECK(tau::get("{100}:bv[8] % {15}:bv[8]", parse_opts_bf) == tau::get("{10}:bv[8]", parse_opts_bf));
		CHECK(tau::get("{200}:bv[8] % {17}:bv[8]", parse_opts_bf) == tau::get("{13}:bv[8]", parse_opts_bf));
		CHECK(tau::get("{44}:bv[8] % {7}:bv[8]", parse_opts_bf) == tau::get("{2}:bv[8]", parse_opts_bf));
	}

	// Shift right
	TEST_CASE("shift right intermediate values") {
		CHECK(tau::get("{128}:bv[8] >> {1}:bv[8]", parse_opts_bf) == tau::get("{64}:bv[8]", parse_opts_bf));
		CHECK(tau::get("{255}:bv[8] >> {4}:bv[8]", parse_opts_bf) == tau::get("{15}:bv[8]", parse_opts_bf));
		CHECK(tau::get("{44}:bv[8] >> {2}:bv[8]", parse_opts_bf) == tau::get("{11}:bv[8]", parse_opts_bf));
	}

	// Shift left
	TEST_CASE("shift left intermediate values") {
		CHECK(tau::get("{2}:bv[8] << {3}:bv[8]", parse_opts_bf) == tau::get("{16}:bv[8]", parse_opts_bf));
		CHECK(tau::get("{15}:bv[8] << {2}:bv[8]", parse_opts_bf) == tau::get("{60}:bv[8]", parse_opts_bf));
		CHECK(tau::get("{44}:bv[8] << {1}:bv[8]", parse_opts_bf) == tau::get("{88}:bv[8]", parse_opts_bf));
	}
}

TEST_SUITE("bv operator hooks:corner cases") {
	// --- Corner tests for all operators ---

	// --- Division corner cases ---
	TEST_CASE("division 0 / 0") {
		tref src = tau::get("{0}:bv[8] / {0}:bv[8]", parse_opts_bf);
		tref expected = tau::get("1:bv[8]", parse_opts_bf); // <- top element 1111....
		CHECK(src == expected);
	}

	TEST_CASE("division 1 / 0") {
		tref src = tau::get("{1}:bv[8] / {0}:bv[8]", parse_opts_bf);
		tref expected = tau::get("1:bv[8]", parse_opts_bf); // <- top element 1111....
		CHECK(src == expected);
	}

	TEST_CASE("division 0 / 1") {
		tref src = tau::get("{0}:bv[8] / {1}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("division X / 1") {
		tref src = tau::get("{42}:bv[8] / {1}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{42}:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	// Addition tests
	TEST_CASE("bv addition: 0 + 0 = 0") {
		auto fm = normalize_test_tau("G(o1[t]:bv[8] = {0}:bv[8] + {0}:bv[8]).");
		CHECK(fm != nullptr);
	}

	TEST_CASE("bv addition: constants") {
		auto fm = normalize_test_tau(
			"G(o1[t]:bv[8] = {1}:bv[8] + {1}:bv[8]).");
		CHECK(fm != nullptr);
	}

	// Subtraction tests
	TEST_CASE("bv subtraction: 1 - 0 = 1") {
		auto fm = normalize_test_tau(
			"G(o1[t]:bv[8] = {1}:bv[8] - {0}:bv[8]).");
		CHECK(fm != nullptr);
	}

	TEST_CASE("bv subtraction: 1 - 1 = 0") {
		auto fm = normalize_test_tau(
			"G(o1[t]:bv[8] = {1}:bv[8] - {1}:bv[8]).");
		CHECK(fm != nullptr);
	}

	// Multiplication tests
	TEST_CASE("bv multiplication: 1 * 0 = 0") {
		auto fm = normalize_test_tau(
			"G(o1[t]:bv[8] = {1}:bv[8] * {0}:bv[8]).");
		CHECK(fm != nullptr);
	}

	TEST_CASE("bv multiplication: 2 * 3") {
		auto fm = normalize_test_tau(
			"G(o1[t]:bv[8] = {2}:bv[8] * {3}:bv[8]).");
		CHECK(fm != nullptr);
	}

	// Shift right tests
	TEST_CASE("bv shift right: 8 >> 1 = 4") {
		auto fm = normalize_test_tau(
			"G(o1[t]:bv[8] = {8}:bv[8] >> {1}:bv[8]).");
		CHECK(fm != nullptr);
	}

	// Shift left tests
	TEST_CASE("bv shift left: 1 << 1 = 2") {
		auto fm = normalize_test_tau(
			"G(o1[t]:bv[8] = {1}:bv[8] << {1}:bv[8]).");
		CHECK(fm != nullptr);
	}

	TEST_CASE("division X / X") {
		tref src = tau::get("{77}:bv[8] / {77}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{1}:bv[8]", parse_opts_bf); // <- 1 (not top element)
		CHECK(src == expected);
	}

	// --- Modulo corner cases ---
	TEST_CASE("modulo 0 % 0") {
		tref src = tau::get("{0}:bv[8] % {0}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf); // <- same element (not bottom element)
		CHECK(src == expected);
	}

	TEST_CASE("modulo 1 % 0") {
		tref src = tau::get("{1}:bv[8] % {0}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{1}:bv[8]", parse_opts_bf); // <- same element (not bottom element)
		CHECK(src == expected);
	}

	TEST_CASE("modulo 0 % 1") {
		tref src = tau::get("{0}:bv[8] % {1}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("modulo X % 1") {
		tref src = tau::get("{99}:bv[8] % {1}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("modulo X % X") {
		tref src = tau::get("{77}:bv[8] % {77}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	// --- Addition corner cases ---
	TEST_CASE("addition 0 + 0") {
		tref src = tau::get("{0}:bv[8] + {0}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("addition 0 + X") {
		tref src = tau::get("{0}:bv[8] + {55}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{55}:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("addition X + 0") {
		tref src = tau::get("{99}:bv[8] + {0}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{99}:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("addition 1 + 1") {
		tref src = tau::get("{1}:bv[8] + {1}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{2}:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("addition X + 1") {
		tref src = tau::get("{42}:bv[8] + {1}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{43}:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("addition 1 + X") {
		tref src = tau::get("{1}:bv[8] + {42}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{43}:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	// --- Subtraction corner cases ---
	TEST_CASE("subtraction 0 - 0") {
		tref src = tau::get("{0}:bv[8] - {0}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("subtraction 0 - X") {
		tref src = tau::get("{0}:bv[8] - {5}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{251}:bv[8]", parse_opts_bf); // 256-5=251 (wrap)
		CHECK(src == expected);
	}

	TEST_CASE("subtraction X - 0") {
		tref src = tau::get("{77}:bv[8] - {0}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{77}:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("subtraction 1 - 1") {
		tref src = tau::get("{1}:bv[8] - {1}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("subtraction X - 1") {
		tref src = tau::get("{42}:bv[8] - {1}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{41}:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("subtraction 1 - X") {
		tref src = tau::get("{1}:bv[8] - {42}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{215}:bv[8]", parse_opts_bf); // 1-42 mod 256 = 215
		CHECK(src == expected);
	}

	TEST_CASE("subtraction X - X") {
		tref src = tau::get("{77}:bv[8] - {77}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	// --- Multiplication corner cases ---
	TEST_CASE("multiplication 0 * 0") {
		tref src = tau::get("{0}:bv[8] * {0}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("multiplication 0 * X") {
		tref src = tau::get("{0}:bv[8] * {99}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("multiplication X * 0") {
		tref src = tau::get("{99}:bv[8] * {0}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("multiplication 1 * 1") {
		tref src = tau::get("{1}:bv[8] * {1}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{1}:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("multiplication X * 1") {
		tref src = tau::get("{42}:bv[8] * {1}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{42}:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("multiplication 1 * X") {
		tref src = tau::get("{1}:bv[8] * {42}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{42}:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	// --- Shift right corner cases ---
	TEST_CASE("shift right by 0") {
		tref src = tau::get("{128}:bv[8] >> {0}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{128}:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("shift right by width") {
		tref src = tau::get("{255}:bv[8] >> {8}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("shift right by more than width") {
		tref src = tau::get("{255}:bv[8] >> {12}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	// --- Shift left corner cases ---
	TEST_CASE("shift left by 0") {
		tref src = tau::get("{1}:bv[8] << {0}:bv[8]", parse_opts_bf);
		tref expected = tau::get("{1}:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("shift left by width") {
		tref src = tau::get("{1}:bv[8] << {8}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}

	TEST_CASE("shift left by more than width") {
		tref src = tau::get("{1}:bv[8] << {12}:bv[8]", parse_opts_bf);
		tref expected = tau::get("0:bv[8]", parse_opts_bf);
		CHECK(src == expected);
	}
}