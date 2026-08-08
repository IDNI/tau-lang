// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

bool check(std::string sample, std::string expected = "") {
	expected = expected.empty() ? sample : expected;
	std::stringstream ss;
	tau::get_options opts = {
		.reget_with_hooks = false
	};
	tau::get(tau::get(sample, opts)).print(ss);
	bool result = ss.str() == (expected.empty() ? sample : expected);
	if (!result) {
		std::cout << "Input:    " << sample << "\n";
		std::cout << "Expected: " << expected << "\n";
		std::cout << "Got:      " << ss.str() << "\n";
	}
	return result;
}

TEST_SUITE("pretty printer") {

	TEST_CASE("pretty print identical to input") {
		vector<string> identical = {
			"x = 1.",
			"ab = 0.",
			"x|ba != 1.",
			"x^ba != 0.",
			"(a|b)x = 0.",
		};

		for (auto& sample : identical)
			CHECK( check(sample) );
	}

	TEST_CASE("pretty print different from input") {
		vector<std::pair<string, string>> different = {
			{ "(x) = 1.", "x = 1." },
			{ "all x all y xy = 0.", "all b2, b1 b2 b1 = 0." },
		};

		for (auto& [sample, expected] : different)
			CHECK( check(sample, expected) );
	}

	// TT2-1: get_max_var_name_b_id ran an unguarded std::stoi over every
	// variable named b<digits>.  The grammar allows arbitrarily long digit
	// runs, so `b9999999999` is legal input; stoi threw std::out_of_range
	// and nothing on the print path caught it.  get_max_var_name_b_id runs
	// at the top of every print, so any echo of such a formula died.
	// find_biggest_quant_id already carried this guard; this copy did not.
	TEST_CASE("b<out-of-range digits> variable name does not throw when printed") {
		tau::get_options opts = {
			.reget_with_hooks = false
		};
		tref fm = tau::get("b9999999999 = 0.", opts);
		REQUIRE( fm != nullptr );
		std::stringstream ss;
		REQUIRE_NOTHROW( tau::get(fm).print(ss) );
	}
}