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
#include "parser.h"

#include "experimental/execution.h"

using namespace std;
using namespace idni::tau;
using namespace idni::tau::experimental;

namespace testing = doctest;

namespace idni::tau::experimental {

sp_node<char> n(const char& value) {
	return make_node<char>(value, {});
}

sp_node<char> n(const char& value, const vector<sp_node<char>>& child) {
	return make_node<char>(value, child);
}

sp_node<char> d(const char& value) {
	vector<sp_node<char>> children;
	return std::make_shared<node<char>>(value, children);
}

sp_node<char> d(const char& value, const vector<sp_node<char>>& child) {
	return std::make_shared<node<char>>(value, child);
}

sp_node<char> f(const sp_node<char>& node) {
	return node->value == 'a' ? d('b') : d('c');
}

sp_node<char> g(const sp_node<char>& node) {
	return node->value == 'b' ? d('d') : d('e');
}

sp_node<char> h(const sp_node<char>&, const sp_node<char>& node) {
	return node->value == 'b' ? d('d') : d('e');
}
struct fo {
	sp_node<char> operator()(const sp_node<char>& node) {
		return node->value == 'a' ? d('b') : d('c');
	}
};

TEST_SUITE("operator| on nodes") {

	TEST_CASE("pointer to function") {
		auto check = d('a') | f;
		CHECK(check->value == 'b');
	}

	TEST_CASE("lambda expression") {
		auto check = d('a') | [](const sp_node<char>& node) {
			return node->value == 'a' ? d('b') : d('c');
		};
		CHECK(check->value == 'b');
	}

	// TODO (LOW) fix this tests
	/*TEST_CASE("bind expression") {
		auto check = d('a') | std::bind(g, d('d'), std::placeholders::_1);
		CHECK(check->value == 'b');
	}*/

	TEST_CASE("functional object") {
		auto check = d('a') | fo();
		CHECK(check->value == 'b');
	}
}

TEST_SUITE("operator| on pointer to functions") {

	/*TEST_CASE("pointer to function") {
		auto check = d('a') | (f | g);
		CHECK( check->value == 'd');
	}*/

	TEST_CASE("lambda expression") {
		CHECK(true);
	}

	TEST_CASE("bind expression") {
		CHECK(true);
	}

	TEST_CASE("functional object") {
		CHECK(true);
	}
}

TEST_SUITE("operator| on lambda expressions") {

	TEST_CASE("pointer to function") {
		CHECK(true);
	}

	TEST_CASE("lambda expression") {
		CHECK(true);
	}

	TEST_CASE("bind expression") {
		CHECK(true);
	}

	TEST_CASE("functional object") {
		CHECK(true);
	}
}

TEST_SUITE("operator| on bind expressions") {

	TEST_CASE("pointer to function") {
		CHECK(true);
	}

	TEST_CASE("lambda expression") {
		CHECK(true);
	}

	TEST_CASE("bind expression") {
		CHECK(true);
	}

	TEST_CASE("functional object") {
		CHECK(true);
	}
}

TEST_SUITE("operator| on funtional objects") {

	TEST_CASE("pointer to function") {
		CHECK(true);
	}

	TEST_CASE("lambda expression") {
		CHECK(true);
	}

	TEST_CASE("bind expression") {
		CHECK(true);
	}

	TEST_CASE("functional object") {
		CHECK(true);
	}
}

} // namespace idni::tau::experimental