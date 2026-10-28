// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"

variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> parse(const char* src) {
	auto parsed = parse_sbf<tau_ba<bv, sbf_ba>, bv, sbf_ba>(src);
	assert(parsed.has_value());
	return parsed.value().first;
}

sbf_ba parse_and_unpack(const char* src) {
	return std::get<sbf_ba>(parse(src));
}

#define SAMPLES_SIZE (sizeof(samples) / sizeof(char *))


TEST_SUITE("sbf binding") {

	TEST_CASE("sbf true") {
		const char* sample = "1";
		const char* expected = "1";
		stringstream ss;
		ss << parse_and_unpack(sample);
		CHECK(ss.str() == expected);
	}

	TEST_CASE("sbf false") {
		const char* sample = "0";
		const char* expected = "0";
		stringstream ss;
		ss << parse_and_unpack(sample);
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
			ss << parse_and_unpack(samples[i]);
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
			ss << parse_and_unpack(samples[i]);
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
			ss << parse_and_unpack(samples[i]);
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
			ss << parse_and_unpack(samples[i]);
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
			ss << parse_and_unpack(samples[i]);
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
			ss << parse_and_unpack(samples[i]);
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
			ss << parse_and_unpack(samples[i]);
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
			ss << parse_and_unpack(samples[i]);
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
			ss << parse_and_unpack(samples[i]);
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
			ss << parse_and_unpack(samples[i]);
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
			ss << parse_and_unpack(samples[i]);
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
		ss << parse_and_unpack(sample);
		CHECK(ss.str() == expected);
	}

}

TEST_SUITE("sbf charvar mode") {

	using desc = ba_descriptor<sbf_ba, node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>;

	// In var mode a variable name has several characters.
	TEST_CASE("var mode reads a multi-character variable") {
		desc::set_charvar(false);
		auto ab = parse_and_unpack("ab");
		auto a_and_b = parse_and_unpack("a & b");
		auto a1_b = parse_and_unpack("a_1b");
		desc::set_charvar(true);
		CHECK(ab != a_and_b);
		CHECK_FALSE(ab->is_zero());
		CHECK_FALSE(ab->is_one());
		CHECK_FALSE(a1_b->is_zero());
	}

	// `xy` is `x & y` in charvar mode and one variable in var mode, so the
	// same source read again after the mode changes is another constant.
	TEST_CASE("the same source reads again after the mode changes") {
		desc::set_charvar(true);
		auto charvar = parse_and_unpack("xy");
		CHECK(charvar == parse_and_unpack("x & y"));
		desc::set_charvar(false);
		auto var = parse_and_unpack("xy");
		desc::set_charvar(true);
		CHECK(var != charvar);
		CHECK(parse_and_unpack("xy") == charvar);
	}
}
