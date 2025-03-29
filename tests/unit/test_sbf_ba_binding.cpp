// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "boolean_algebras/sbf_ba.h"
#include "boolean_algebras/bdds/babdd.h"

using namespace std;
using namespace idni::tau_lang;

namespace testing = doctest;

tau_depreciating<tau_ba<sbf_ba>, sbf_ba> build_binding(const char* src) {
	bdd_init<Bool>();
	static sbf_ba_factory<tau_ba<sbf_ba>, sbf_ba> bf;
	return bf.parse(src).value();
}

const sbf_ba& get_binding(const tau_depreciating<tau_ba<sbf_ba>, sbf_ba>& n) {
	return get<sbf_ba>(get<variant<tau_ba<sbf_ba>, sbf_ba>>(n->value));
}

const sbf_ba& build_and_get_binding(const char* src) {
	return get_binding(build_binding(src));
}

#define SAMPLES_SIZE (sizeof(samples) / sizeof(char *))

TEST_SUITE("sbf binding") {

	TEST_CASE("sbf true") {
		const char* sample = "1";
		const char* expected = "1";
		auto& v = build_and_get_binding(sample);
		stringstream ss;
		ss << v;
		CHECK(ss.str() == expected);
	}

	TEST_CASE("sbf false") {
		const char* sample = "0";
		const char* expected = "0";
		auto& v = build_and_get_binding(sample);
		stringstream ss;
		ss << v;
		CHECK(ss.str() == expected);
	}

	TEST_CASE("sbf negation") {
		const char* samples[] = {
			"0'", "1'", "0''", "1''", "0'''", "1'''"
		};
		const char* expecteds[] = {
			"1", "0", "0", "1", "1", "0"
		};
		for (size_t i = 0; i != SAMPLES_SIZE; ++i) {
			stringstream ss;
			ss << build_and_get_binding(samples[i]);
			CHECK(ss.str() == expecteds[i]);
		}
	}

	TEST_CASE("sbf and") {
		const char* samples[] = {
			"0 0", "0 1", "1 0", "1 1",
			"0&0", "0&1", "1&0", "1&1"
		};
		const char* expecteds[] = {
			"0", "0", "0", "1",
			"0", "0", "0", "1"
		};
		for (size_t i = 0; i != SAMPLES_SIZE; ++i) {
			stringstream ss;
			ss << build_and_get_binding(samples[i]);
			CHECK(ss.str() == expecteds[i]);
		}
	}

	TEST_CASE("sbf or") {
		const char* samples[] = {
			"0|0", "0|1", "1|0", "1|1"
		};
		const char* expecteds[] = {
			"0", "1", "1", "1"
		};
		for (size_t i = 0; i != SAMPLES_SIZE; ++i) {
			stringstream ss;
			ss << build_and_get_binding(samples[i]);
			CHECK(ss.str() == expecteds[i]);
		}
	}

	TEST_CASE("sbf xor") {
		const char* samples[] = {
			"0^0", "0^1", "1^0", "1^1",
			"0+0", "0+1", "1+0", "1+1"
		};
		const char* expecteds[] = {
			"0", "1", "1", "0",
			"0", "1", "1", "0"
		};
		for (size_t i = 0; i != SAMPLES_SIZE; ++i) {
			stringstream ss;
			ss << build_and_get_binding(samples[i]);
			CHECK(ss.str() == expecteds[i]);
		}
	}

	TEST_CASE("sbf parentheses") {
		const char* samples[] = {
			"1|0&0", "1|(0&0)", "(1|0)&0",
			"1|1&0", "1|(1&0)", "(1|1)&0"
		};
		const char* expecteds[] = {
			"1", "1", "0",
			"1", "1", "0"
		};
		for (size_t i = 0; i != SAMPLES_SIZE; ++i) {
			stringstream ss;
			ss << build_and_get_binding(samples[i]);
			CHECK(ss.str() == expecteds[i]);
		}
	}

	TEST_CASE("sbf variable") {
		const char* samples[] = {
			"p", "X", "a1"
		};
		vector<string> expecteds = {
			"p", "X", "a1"
		};
		for (size_t i = 0; i != SAMPLES_SIZE; ++i) {
			stringstream ss;
			ss << build_and_get_binding(samples[i]);
			CHECK(ss.str() == expecteds[i]);
		}
	}

	TEST_CASE("sbf variable negation") {
		const char* samples[] = {
			"v'", "X''", "a1'''"
		};
		vector<string> expecteds = {
			"v'", "X", "a1'"
		};
		for (size_t i = 0; i != SAMPLES_SIZE; ++i) {
			stringstream ss;
			ss << build_and_get_binding(samples[i]);
			CHECK(ss.str() == expecteds[i]);
		}
	}

	TEST_CASE("sbf variable and") {
		const char* samples[] = {
			"v 0", "v 1", "0 v", "1 v", "v v", "v w",
			"v&0", "v&1", "0&v", "1&v", "v&v", "v&w"
		};
		vector<string> expecteds = {
			"0", "v", "0", "v", "v", "v w",
			"0", "v", "0", "v", "v", "v w"
		};
		for (size_t i = 0; i != SAMPLES_SIZE; ++i) {
			stringstream ss;
			ss << build_and_get_binding(samples[i]);
			CHECK(ss.str() == expecteds[i]);
		}
	}

	TEST_CASE("sbf variable or") {
		const char* samples[] = {
			"v|0", "v|1", "0|v", "1|v", "v|v", "v|w"
		};
		vector<string> expecteds = {
			"v", "1", "v", "1", "v", "v | v' w"
		};
		for (size_t i = 0; i != SAMPLES_SIZE; ++i) {
			stringstream ss;
			ss << build_and_get_binding(samples[i]);
			CHECK(ss.str() == expecteds[i]);
		}
	}

	TEST_CASE("sbf variable xor") {
		const char* samples[] = {
			"v^0", "0^v", "v^1", "1^v", "v^v", "v^w",
			"v+0", "0+v", "v+1", "1+v", "v+v", "v+w"
		};
		vector<string> expecteds = {
			"v", "v", "v'", "v'", "0", "v w' | v' w",
			"v", "v", "v'", "v'", "0", "v w' | v' w"
		};
		for (size_t i = 0; i != SAMPLES_SIZE; ++i) {
			stringstream ss;
			ss << build_and_get_binding(samples[i]);
			CHECK(ss.str() == expecteds[i]);
		}
	}

	TEST_CASE("sbf variable parentheses") {
		const char* samples[] = {
			"v|w&0", "v|(w&0)", "(v|w)&0",
			"1|w&v", "1|(w&v)", "(1|w)&v"
		};
		vector<string> expecteds = {
			"v", "v", "0",
			"1", "1", "v"
		};
		for (size_t i = 0; i != SAMPLES_SIZE; ++i) {
			stringstream ss;
			ss << build_and_get_binding(samples[i]);
			CHECK(ss.str() == expecteds[i]);
		}
	}

	TEST_CASE("sbf all syntax") {
	 	const char* sample = "z' | x b (1'^(a b) | 0+c | a) ^ d "
					"| d^e&1";
		string expected = "a b d e' x z | a b d' x z | a' b c d e'"
			" x z | a' b c d' x z | a' b c' d x z | a' b c' d' e x "
			"z | b' d x z | b' d' e x z | d x' z | d' e x' z | z'";
		stringstream ss;
		ss << build_and_get_binding(sample);
		CHECK(ss.str() == expected);
	}

}
