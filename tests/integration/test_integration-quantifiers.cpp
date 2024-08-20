// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

// tests parsing, printing, normalization and printed result of
// quantifiers: all, ex, b_all and b_ex

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "nso_rr.h"
#include "bool_ba.h"
#include "bdd_handle.h"
#include "normalizer.h"

#include "test_integration_helpers-bdd.h"
#include "../unit/test_helpers.h"

#include "debug_helpers.h"

using namespace std;
using namespace idni::rewriter;
using namespace idni::tau;

namespace testing = doctest;

bdd_test_factory bf;

using test_case = array<string, 3>;
using test_cases = vector<test_case>;

test_cases ex_cases = {
	{ "ex x x=0.",                  "ex x x = 0.",                    "T" },
	{ "ex x,y xy=0.",               "ex x, y x & y = 0.",             "T" },
	{ "ex x ex y xy=0.",            "ex x, y x & y = 0.",             "T" },
};

test_cases all_cases = {
	{ "all x x!=0.",                "all x x != 0.",                  "F" },
	{ "all x,y xy!=0.",             "all x, y x & y != 0.",           "F" },
	{ "all x all y xy!=0.",         "all x, y x & y != 0.",           "F" },
};

test_cases ex_all_cases = {
	{ "ex x all y x=y.",            "ex x all y x = y.",              "F" },
	{ "ex x,y all w,z x=w&&y=z.",   "ex x, y all w, z x = w && y = z.","F"},
};

test_cases all_ex_cases = {
	{ "all x ex y x=y.",            "all x ex y x = y.",              "T" },
	{ "all x,y ex w,z x=w && y=z.", "all x, y ex w, z x = w && y = z.","T"},
};

test_cases bool_ex_cases = {
	{ "b_ex ?P ?P.",                "b_ex ?P ?P.",                    "T" },
	{ "b_ex ?P,?Q ?P&&?Q.",         "b_ex ?P, ?Q ?P && ?Q.",          "T" },
	{ "b_ex ?P b_ex ?Q ?P&&?Q.",    "b_ex ?P, ?Q ?P && ?Q.",          "T" },
};

test_cases bool_all_cases = {
	{ "b_all ?P ?P.",               "b_all ?P ?P.",                   "F" },
	{ "b_all ?P,?Q ?P&&?Q.",        "b_all ?P, ?Q ?P && ?Q.",         "F" },
	{ "b_all ?P b_all ?Q ?P&&?Q.",  "b_all ?P, ?Q ?P && ?Q.",         "F" },
};

test_cases bool_ex_all_cases = {
	{ "b_ex ?P b_all ?Q ?P||?Q.",   "b_ex ?P b_all ?Q ?P || ?Q.",     "T" },
	{ "b_ex ?P,?Q b_all ?R,?S (?P||?R)&&(?Q||?S).",
		"b_ex ?P, ?Q b_all ?R, ?S (?P || ?R) && (?Q || ?S).",     "T" },
	{ "b_ex ?P b_ex ?Q b_all ?R b_all ?S (?P||?R)&&(?Q||?S).",
		"b_ex ?P, ?Q b_all ?R, ?S (?P || ?R) && (?Q || ?S).",     "T" },
};

test_cases bool_all_ex_cases = {
	{ "b_all ?P b_ex ?Q ?P||?Q.",   "b_all ?P b_ex ?Q ?P || ?Q.",     "T" },
	{ "b_all ?P,?Q b_ex ?R,?S (?P||?R)&&(?Q||?S).",
		"b_all ?P, ?Q b_ex ?R, ?S (?P || ?R) && (?Q || ?S).",     "T" },
	{ "b_all ?P b_all ?Q b_ex ?R b_ex ?S (?P||?R)&&(?Q||?S).",
		"b_all ?P, ?Q b_ex ?R, ?S (?P || ?R) && (?Q || ?S).",     "T" },
};

ostream& operator<<(ostream& os, const test_case& tc) {
	return os << "input source: \"" << tc[0]
		<< "\"\n\texpected: \"" << tc[1]
		<< "\" result: \""      << tc[2] << "\"";
}

string to_str(const auto& n) {
	stringstream ss; return (ss << n, ss.str());
}

bool test(const test_case& tc) {
	const auto& [ sample, exp, nexp ] = tc;
	bool fail = false;
	auto src = make_tau_source(sample.c_str());
	auto formula = make_nso_rr_using_factory<bdd_test_factory_t, bdd_test>(
								src, bf);
	auto got = to_str(formula);
	if (got != exp) fail = true;
	auto norm = normalizer<bdd_test>(formula);
	auto ngot = to_str(norm);
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

TEST_SUITE("bool quantifiers") {
	CASES("bool ex",     bool_ex_cases);
	CASES("bool all",    bool_all_cases);
	CASES("bool ex all", bool_ex_all_cases);
	CASES("bool all ex", bool_all_ex_cases);
}