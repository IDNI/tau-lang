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

#include "../../src/doctest.h"

#include "../src/bdd_binding.h"

using namespace std;
using namespace idni::tau;

namespace testing = doctest;

sp_tau_source_node make_source_binding_node(const std::string& source) {
	std::vector<sp_node<tau_source_sym>> source_nodes;
	for (auto& c : source) source_nodes.push_back(
		make_node<tau_source_sym>(tau_parser::source0, {
			make_node<tau_source_sym>(tau_parser::alnum, {
				make_node<tau_source_sym>(tau_source_sym(c), {})
			})
		}));
	return make_node<tau_source_sym>(tau_parser::binding, {
		make_node<tau_source_sym>(tau_parser::source_binding, {
			make_node<tau_source_sym>(tau_parser::source,
				source_nodes)
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
		all_t<sp_tau_source_node>,
		sp_node<tau_source_sym>,
		sp_bdd_node>(
			transform, all<sp_tau_source_node>)(source);
}

TEST_SUITE("bdd binding") {

	TEST_CASE("bdd factory build") {
		bdd_init<Bool>();
	 	const char* sample = "z' | x y | a&b+c | 1^d | d^e&0'";
		const char* expected = "a b c d e' x y' z | a b c d e' x' z | "
			"a b c d' x y' z | a b c d' x' z | a b c' x y' z | "
			"a b c' x' z | a b' c x y' z | a b' c x' z | "
			"a b' c' d e' x y' z | a b' c' d e' x' z | "
			"a b' c' d' x y' z | a b' c' d' x' z | "
			"a' d e' x y' z | a' d e' x' z | a' d' x y' z | "
			"a' d' x' z | x y z | z'";
		auto st = make_bdd_statement(make_source_binding_node(sample));
		bdd_factory bf;
		auto bbv = bf.build("bdd", st);
		auto& v = std::get<bdd_binding>(std::get<std::variant<
			tau_ba<bdd_binding>, bdd_binding>>(bbv->value));
		std::stringstream ss;
		ss << v;
	 	CHECK(ss.str() == expected);
	}

}
