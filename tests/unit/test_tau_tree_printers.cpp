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
}