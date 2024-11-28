// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "parser.h"

#include "experimental/execution.h"

using namespace std;
using namespace idni::rewriter;
using namespace idni::tau;
using namespace idni::tau::experimental;

namespace testing = doctest;

namespace idni::tau::experimental {

sp_node<char> d(const char& value) {
	vector<sp_node<char>> children;
	return make_node<char>(value, children);
}

sp_node<char> d(const char& value, const vector<sp_node<char>>& child) {
	return make_node<char>(value, child);
}

sp_node<char> func_value_is_a(const sp_node<char>& node) {
	return node->value == 'a' ? d('T') : d('F');
}

sp_node<char> func_value_equals_to(const sp_node<char>& node, const char& value) {
	return node->value == value ? d('T') : d('F');
}

bool func_is_true(const sp_node<char>& node) {
	return node->value == 'T';
}

bool func_is_false(const sp_node<char>& node) {
	return node->value == 'F';
}

struct struct_is_true {
	bool operator()(const sp_node<char>& node) const {
		return node->value == 'T';
	}
};

struct struct_is_false {
	bool operator()(const sp_node<char>& node) const {
		return node->value == 'F';
	}
};

struct struct_value_is_a {
	sp_node<char> operator()(const sp_node<char>& node) const {
		return node->value == 'a' ? d('T') : d('F');
	}
};

struct struct_value_equals_to {
	struct_value_equals_to(const char& value) : value(value) {}

	sp_node<char> operator()(const sp_node<char>& node) const {
		return node->value == value ? d('T') : d('F');
	}

	const char& value;
};

template <typename F>
auto my_bind(F&& f) {
	return std::bind(f, std::placeholders::_1);
}

TEST_SUITE("node operator| on nodes") {

	TEST_CASE("pointer to function") {
		auto check = d('a') | func_value_is_a;
		CHECK(check->value == 'T');
	}

	TEST_CASE("lambda expression") {
		auto check = d('b') | [](const sp_node<char>& node) {
			return node->value == 'a' ? d('T') : d('F');
		};
		CHECK(check->value == 'F');
	}

	TEST_CASE("bind expression: case 1") {
		auto check = d('a') | std::bind(func_value_equals_to, std::placeholders::_1, 'b');
		CHECK(check->value == 'F');
	}

	TEST_CASE("bind expression: case 1") {
		auto check = d('a') | std::bind(func_value_is_a, std::placeholders::_1);
		CHECK(check->value == 'T');
	}

	TEST_CASE("functional object: case 1") {
		auto check = d('a') | struct_value_is_a();
		CHECK(check->value == 'T');
	}

	TEST_CASE("functional object:case 2") {
		auto check = d('a') | struct_value_equals_to('a');
		CHECK(check->value == 'T');
	}

	// TODO (LOW) There is a lot of more possibilities to apply functional objects,
	// add more test cases to cover all of them. Check the following links for more
	// information:
	//
	// - https://en.cppreference.com/w/cpp/utility/functional/function
	// - https://en.cppreference.com/w/cpp/utility/functional/bind

}

TEST_SUITE("node operator| on pointer to functions") {

	TEST_CASE("pointer to function") {
		auto check = d('a') | func_value_is_a | func_is_true;
		CHECK( check );
	}

	TEST_CASE("lambda expression") {
		auto check = d('a') | func_value_is_a | [](const sp_node<char>& node) {
			return node->value == 'T';
		};
		CHECK( check );
	}

	TEST_CASE("bind expression") {
		auto check = d('a') | func_value_is_a | std::bind(func_is_true, std::placeholders::_1);
		CHECK( check );
	}

	TEST_CASE("functional object") {
		auto check = d('a') | func_value_is_a | struct_is_true();
		CHECK(check);
	}
}

TEST_SUITE("function operator| on function") {

	TEST_CASE("pointer to function") {
		auto check = std::bind(func_value_is_a, std::placeholders::_1) | func_is_true;
		CHECK( (d('a') | check));
	}

	TEST_CASE("lambda expression") {
		auto check = func_value_is_a | [](const sp_node<char>& node) {
			return node->value == 'T';
		};
		CHECK( (d('a') | check));
	}

	TEST_CASE("bind expression: right") {
		auto check = func_value_is_a | my_bind(func_is_true);
		CHECK( (d('a') | check));
	}

	TEST_CASE("bind expression: left") {
		auto check = my_bind(func_value_is_a) | func_is_true;
		CHECK( (d('a') | check));
	}

	TEST_CASE("functional object") {
		auto check = func_value_is_a | struct_is_true();
		CHECK( (d('a') | check));
	}
}

TEST_SUITE("repeat") {

	TEST_CASE("of a pointer to function") {
		auto check = d('a') | repeat(func_value_is_a);
		CHECK( (check | func_is_false));
	}

	TEST_CASE("of a lambda expression") {
		auto check = d('a') | repeat([](const sp_node<char>& node) {
			return node->value == 'a' ? d('T') : d('F');
		});
		CHECK( (check | func_is_false));
	}

	TEST_CASE("of a bind expression") {
		auto check = d('a') | repeat(my_bind(func_value_is_a));
		CHECK( (check | func_is_false));
	}

	TEST_CASE("of a functional object") {
		auto check = d('a') | repeat(struct_value_is_a());
		CHECK( (check | func_is_false));
	}

	TEST_CASE("chaining with pointer to a function") {
		auto check = d('a')
			| repeat(struct_value_is_a())
			| func_value_is_a;
		CHECK( (check | func_is_false));
	}

	TEST_CASE("chaining with a lambda expression") {
		auto check = d('a')
			| repeat(struct_value_is_a())
			| [](const sp_node<char>& node) {
				return node->value == 'a' ? d('T') : d('F');
			};
		CHECK( (check | func_is_false));
	}

	TEST_CASE("chaining with a bind expression") {
		auto check = d('a')
			| repeat(struct_value_is_a())
			| my_bind(func_value_is_a);
		CHECK( (check | func_is_false));
	}

	TEST_CASE("chaining with a functional object") {
		auto check = d('a')
			| repeat(struct_value_is_a())
			| struct_value_is_a();
		CHECK( (check | func_is_false));
	}

	TEST_CASE("chaining with itself") {
		auto check = d('a')
			| repeat(struct_value_is_a())
			| repeat(struct_value_is_a());
		CHECK( (check | func_is_false));
	}

}

} // namespace idni::tau::experimental