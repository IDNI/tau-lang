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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "../src/bdd_binding.h"
#include "babdd.h"

using namespace std;
using namespace idni::tau;

namespace testing = doctest;

sp_tau_source_node make_source_binding_node(const string& source) {
	vector<sp_node<tau_source_sym>> source_nodes;
	for (auto& c : source) source_nodes.push_back(
		make_node<tau_source_sym>(tau_source_sym(c), {})
	);
	auto& p = tau_parser::instance();
	return make_node<tau_source_sym>(p.literal(tau_parser::binding),
		{ make_node<tau_source_sym>(
				p.literal(tau_parser::source_binding),
			{ make_node<tau_source_sym>(
				p.literal(tau_parser::source), source_nodes)
			})
		});
}

sp_bdd_node make_bdd_statement(const sp_tau_source_node& source) {
	using tauify_bdd = tauify<tau_ba<bdd_binding>, bdd_binding>;
	tauify_bdd tf;
	map_transformer<tauify_bdd, sp_tau_source_node, sp_bdd_node>
		transform(tf);
	return post_order_traverser<
		map_transformer<tauify_bdd, sp_tau_source_node, sp_bdd_node>,
		all_t,
		sp_node<tau_source_sym>,
		sp_bdd_node>(
			transform, all)(source);
}

sp_bdd_node build_binding(const char* src) {
	bdd_init<Bool>();
	auto st = make_bdd_statement(make_source_binding_node(src));
	bdd_factory bf;
	return bf.build("bdd", st);
}

bdd_binding& get_binding(const sp_bdd_node& n) {
	return get<bdd_binding>(get<variant<
			tau_ba<bdd_binding>, bdd_binding>>(n->value));
}

bdd_binding& build_and_get_binding(const char* src) {
	return get_binding(build_binding(src));
}

#define SAMPLES_SIZE (sizeof(samples) / sizeof(char *))

TEST_SUITE("bdd binding") {

	TEST_CASE("bdd true") {
		const char* sample = "1";
		const char* expected = "1";
		auto& v = build_and_get_binding(sample);
		stringstream ss;
		ss << v;
		CHECK(ss.str() == expected);
	}

	TEST_CASE("bdd false") {
		const char* sample = "0";
		const char* expected = "0";
		auto& v = build_and_get_binding(sample);
		stringstream ss;
		ss << v;
		CHECK(ss.str() == expected);
	}

	TEST_CASE("bdd negation") {
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

	TEST_CASE("bdd and") {
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

	TEST_CASE("bdd or") {
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

	TEST_CASE("bdd xor") {
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

	TEST_CASE("bdd parentheses") {
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

	TEST_CASE("bdd variable") {
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

	TEST_CASE("bdd variable negation") {
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

	TEST_CASE("bdd variable and") {
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

	TEST_CASE("bdd variable or") {
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

	TEST_CASE("bdd variable xor") {
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

	TEST_CASE("bdd variable parentheses") {
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

	TEST_CASE("bdd all syntax") {
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
