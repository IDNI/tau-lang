// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

bool check(std::string sample, std::string expected = "") {
	expected = expected.empty() ? sample : expected;
	std::stringstream ss;
	tau::get_options opts = {
		.reget_with_hooks = false
	};
	tref parsed = tau::get(sample, opts);
	if (parsed == nullptr) {
		std::cout << "Parse failed: " << sample << "\n";
		return false;
	}
	tau::get(parsed).print(ss);
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

	// Round-trips through the operator arms of tree::print that no other
	// test reaches: order comparisons, bv arithmetic/shifts, the negated
	// gate operators, casts, bf quantifiers and time constraints. The
	// negated gate operators (!& !| !^) only parse on bv-typed operands.
	TEST_CASE("operator and quantifier arms round-trip") {
		vector<string> identical = {
			"x != y.",
			"x <= y.",
			"x !<= y.",
			"x > y.",
			"x !> y.",
			"x >= y.",
			"x !>= y.",
			"x < y.",
			"x !< y.",
			"fall p px = 0.",
			"fex p px = 0.",
		};
		for (auto& sample : identical)
			CHECK( check(sample) );
		// the printer drops variable type annotations and operator
		// whitespace on bv terms, keeps the cast, and re-annotates io
		// variables with their inferred :tau type
		vector<std::pair<string, string>> different = {
			{ "x:bv[8] + y:bv[8] = 0.",  "x+y = 0." },
			{ "x:bv[8] - y:bv[8] = 0.",  "x-y = 0." },
			{ "x:bv[8] * y:bv[8] = 0.",  "x*y = 0." },
			{ "x:bv[8] / y:bv[8] = 0.",  "x/y = 0." },
			{ "x:bv[8] % y:bv[8] = 0.",  "x%y = 0." },
			{ "x:bv[8] << y:bv[8] = 0.", "x<<y = 0." },
			{ "x:bv[8] >> y:bv[8] = 0.", "x>>y = 0." },
			{ "x:bv[8] !& y:bv[8] = 0.", "x!&y = 0." },
			{ "x:bv[8] !| y:bv[8] = 0.", "x!|y = 0." },
			{ "x:bv[8] !^ y:bv[8] = 0.", "x!^y = 0." },
			{ "(bv[16]) x:bv[8] = 0.",   "(bv[16]) x = 0." },
			{ "min(x:bv[8], y:bv[8]) = 0.", "min(x, y) = 0." },
			{ "max(x:bv[8], y:bv[8]) = 0.", "max(x, y) = 0." },
			{ "always [t < 3] -> o1[t] = 0.",
				"always [t < 3] -> o1[t]:tau = 0." },
			{ "always [t != 0] -> o1[t] = 0.",
				"always [t != 0] -> o1[t]:tau = 0." },
			{ "always [t >= 1] -> o1[t] = 0.",
				"always [t >= 1] -> o1[t]:tau = 0." },
			{ "always [t > 1] -> o1[t] = 0.",
				"always [t > 1] -> o1[t]:tau = 0." },
			{ "always [t <= 1] -> o1[t] = 0.",
				"always [t <= 1] -> o1[t]:tau = 0." },
			{ "always [t = 1] -> o1[t] = 0.",
				"always [t = 1] -> o1[t]:tau = 0." },
		};
		for (auto& [sample, expected] : different)
			CHECK( check(sample, expected) );
		// nested bf quantifiers merge into a comma list, like all/ex
		CHECK( check("fall p fall q pqx = 0.", "fall p, q pqx = 0.") );
		CHECK( check("fex p fex q pqx = 0.", "fex p, q pqx = 0.") );
	}
}