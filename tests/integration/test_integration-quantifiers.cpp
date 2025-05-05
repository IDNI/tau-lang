// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

// tests parsing, printing, normalization and printed result of
// quantifiers: all, ex

#include "test_init.h"
#include "test_Bool_helpers.h"

using test_case = array<string, 3>;
using test_cases = vector<test_case>;

test_cases ex_cases = {
	{ "ex x x=0.",                  "ex x x = 0",             "T" },
	{ "ex x,y xy=0.",               "ex x, y xy = 0",         "T" },
	{ "ex x ex y xy=0.",            "ex x, y xy = 0",         "T" },
};

test_cases all_cases = {
	{ "all x x!=0.",                "all x x != 0",           "F" },
	{ "all x,y xy!=0.",             "all x, y xy != 0",       "F" },
	{ "all x all y xy!=0.",         "all x, y xy != 0",       "F" },
};

test_cases ex_all_cases = {
	{ "ex x all y x=y.",            "ex x all y x'y|xy' = 0", "F" },
	{ "ex x,y all w,z x=w&&y=z.",
		"ex x, y all w, z x'w|xw' = 0 && y'z|yz' = 0",     "F"},
};

test_cases all_ex_cases = {
	{ "all x ex y x=y.",            "all x ex y x'y|xy' = 0", "T" },
	{ "all x,y ex w,z x=w && y=z.",
		"all x, y ex w, z x'w|xw' = 0 && y'z|yz' = 0",    "T"},
};

ostream& operator<<(ostream& os, const test_case& tc) {
	return os << "input source: \"" << tc[0]
		<< "\"\n\texpected: \"" << tc[1]
		<< "\" result: \""      << tc[2] << "\"";
}

bool test(const test_case& tc) {
	const auto& [ sample, exp, nexp ] = tc;
	bool fail = false;
	auto nso_rr = get_nso_rr(sample.c_str());
	if (!nso_rr.has_value()) return fail;
	auto got = to_str<bnode>(nso_rr.value());
	if (got != exp) fail = true;
	tref norm = normalizer<bnode>(nso_rr.value());
	auto ngot = tau::get(norm).to_str();
	if (fail || ngot != nexp) fail = true,
		cout << tc << "\n\tgot:      \"" << got
				<< "\" result: \"" << ngot << "\"\n";
	return !fail;
}

#define CASES(name, cases) TEST_CASE(name) { \
		for (const auto& tc : cases) { \
			bool result = test(tc); \
			CHECK( result ); \
			if (!result) break; \
		} \
	}

TEST_SUITE("quantifiers") {
	CASES("ex",     ex_cases);
	CASES("all",    all_cases);
	CASES("ex all", ex_all_cases);
	CASES("all ex", all_ex_cases);
}
