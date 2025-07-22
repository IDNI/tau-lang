// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cassert>

#include "doctest.h"

#include "test_helpers.h"
#include "resolver.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

TEST_SUITE("resolver") {
    TEST_CASE("empty resolver") {
		resolver<Bool> r;
		// current vars map is empty
		CHECK(r.current.vars.empty());
		// no parent scope
		CHECK(r.current.parent == nullptr);
		// all data is empty
		CHECK(r.parent.empty());
		CHECK(r.rank.empty());
		CHECK(r.types.empty());
		// status return empty
		CHECK(r.status().first.empty());
		CHECK(r.status().second.empty());
	}
	
    TEST_CASE("resolver with one untyped variable") {
		resolver<Bool> r;
		auto x = build_variable<Bool>("x");
		r.add(x);
		// current vars map has one element...
		CHECK(r.current.vars.size() == 1);
		// ...which is x
		CHECK(r.current.vars[x] == 0);
		// x is untyped
		CHECK(r.type_of(x) == nullptr);
		// all data has one element except types...
		// ...which correspond to x
		CHECK(r.parent.size() == 1);
		CHECK(r.rank.size() == 1);
		CHECK(r.types.size() == 0);
		// status return an untyped variable
		CHECK(r.status().first.empty());
		CHECK(r.status().second.size() == 1);
	}

    TEST_CASE("resolver with one typed variable") {
		resolver<Bool> r;
		auto x = build_variable<Bool>("x");
		auto type = build_type<Bool>("bool");
		r.add(x);
		r.type(x, type);
		// current vars map has one element...
		CHECK(r.current.vars.size() == 1);
		// ...which is x
		CHECK(r.current.vars[x] == 0);
		// x is typed
		CHECK(r.type_of(x) == type);
		// all data has one element...
		// ...which correspond to x
		CHECK(r.parent.size() == 1);
		CHECK(r.rank.size() == 1);
		CHECK(r.types.size() == 1);
		// status return an untyped variable
		CHECK(r.status().first.size() == 1);
		CHECK(r.status().second.size() == 0);
	}

    TEST_CASE("resolver with two untyped variable") {
		resolver<Bool> r;
		auto x = build_variable<Bool>("x");
		auto y = build_variable<Bool>("y");
		r.add(x);
		r.add(y);
		// current vars map has two elements...
		CHECK(r.current.vars.size() == 2);
		// ...which are x and y
		CHECK(r.current.vars[x] == 0);
		CHECK(r.current.vars[y] == 1);
		// x and y are untyped
		CHECK(r.type_of(x) == nullptr);
		CHECK(r.type_of(y) == nullptr);
		// all data has two elements except types...
		// ...which correspond to x and y
		CHECK(r.parent.size() == 2);
		CHECK(r.rank.size() == 2);
		CHECK(r.types.size() == 0);
		// status return untyped variables
		CHECK(r.status().first.empty());
		CHECK(r.status().second.size() == 2);
		CHECK(r.status().second.count(x) == 1);
		CHECK(r.status().second.count(y) == 1);
	}

    TEST_CASE("resolver with two variables, one typed and the other one connected") {
		resolver<Bool> r;
		auto x = build_variable<Bool>("x");
		auto y = build_variable<Bool>("y");
		auto type = build_type<Bool>("bool");
		r.add(x);
		r.add(y);
		r.type(x, type);
		r.unite(x, y);
		// current vars map has two elements...
		CHECK(r.current.vars.size() == 2);
		// ...which are x and y
		CHECK(r.current.vars[x] == 0);
		CHECK(r.current.vars[y] == 1);
		// both are typed
		CHECK(r.type_of(x) == type);
		CHECK(r.type_of(y) == type);
		// all data has two elements except types...
		// ...which correspond to x and y
		CHECK(r.parent.size() == 2);
		CHECK(r.rank.size() == 2);
		CHECK(r.types.size() == 1);
		// status return no untyped variables
		CHECK(r.status().first.size() == 2);
		CHECK(r.status().second.size() == 0);
	}

    TEST_CASE("resolver with two nested scopes with same name variables and different types") {
		resolver<Bool> r;
		auto x = build_variable<Bool>("x");
		auto type_a = build_type<Bool>("type_a");
		auto type_b = build_type<Bool>("type_b");
		// we add the first x and typed it with type_a
		r.add(x);
		r.type(x, type_a);
		// we open a new scope and remove the current x
		r.open(x, type_b);
		// current vars map in the inner scope has one element...
		CHECK(r.current.vars.size() == 1);
		// ...which is x
		CHECK(r.current.vars[x] == 1);
		// its type is type_b
		CHECK(r.type_of(x) == type_b);
		// all data has one element...
		// ...which correspond to the inner x
		CHECK(r.parent.size() == 2);
		CHECK(r.rank.size() == 2);
		CHECK(r.types.size() == 2);
		// status return one typed variable
		CHECK(r.status().first.size() == 1);
		CHECK(r.status().second.size() == 0);
		// we remove x and close the inner scope
		r.close(x);
		// current vars map has one elements...
		CHECK(r.current.vars.size() == 1);
		// ...which is x
		CHECK(r.current.vars[x] == 0);
		// its type is type_a
		CHECK(r.type_of(x) == type_a);
		// all data has one element...
		// ...which correspond to the outer x
		CHECK(r.parent.size() == 2);
		CHECK(r.rank.size() == 2);
		CHECK(r.types.size() == 2);
		// status return one typed variable
		CHECK(r.status().first.size() == 1);
		CHECK(r.status().second.size() == 0);
	}

    TEST_CASE("resolver with two nested scopes with an inner untyped variable") {
		resolver<Bool> r;
		auto x = build_variable<Bool>("x");
		// we add the first x and typed it with type_a
		r.open(x);
		// current vars map in the inner scope has one element...
		CHECK(r.current.vars.size() == 1);
		// ...which is x
		CHECK(r.current.vars[x] == 0);
		// its type is type_b
		CHECK(r.type_of(x) == nullptr);
		// all data has one element...
		// ...which correspond to the inner x
		CHECK(r.parent.size() == 1);
		CHECK(r.rank.size() == 1);
		CHECK(r.types.size() == 0);
		// status return one typed variable
		CHECK(r.status().first.size() == 0);
		CHECK(r.status().second.size() == 1);
		// we remove x and close the inner scope
		r.close();
		// current vars map has one elements...
		CHECK(r.current.vars.size() == 1);
		// ...which is x
		CHECK(r.current.vars[x] == 0);
		// its type is type_a
		CHECK(r.type_of(x) == nullptr);
		// all data has one element...
		// ...which correspond to the outer x
		CHECK(r.parent.size() == 1);
		CHECK(r.rank.size() == 1);
		CHECK(r.types.size() == 0);
		// status return one typed variable
		CHECK(r.status().first.size() == 0);
		CHECK(r.status().second.size() == 1);
	}

    TEST_CASE("resolver with two nested scopes with an inner typed variable") {
		resolver<Bool> r;
		auto x = build_variable<Bool>("x");
		auto type = build_type<Bool>("bool");
		// we add the first x and typed it with type_a
		r.open(x, type);
		// current vars map in the inner scope has one element...
		CHECK(r.current.vars.size() == 1);
		// ...which is x
		CHECK(r.current.vars[x] == 0);
		// its type is type_b
		CHECK(r.type_of(x) == type);
		// all data has one element...
		// ...which correspond to the inner x
		CHECK(r.parent.size() == 1);
		CHECK(r.rank.size() == 1);
		CHECK(r.types.size() == 1);
		// status return one typed variable
		CHECK(r.status().first.size() == 1);
		CHECK(r.status().second.size() == 0);
		// close the inner scope
		r.close();
		// current vars map has one elements...
		CHECK(r.current.vars.size() == 1);
		// ...which is x
		CHECK(r.current.vars[x] == 0);
		// its type is type_a
		CHECK(r.type_of(x) == type);
		// all data has one element...
		// ...which correspond to the outer x
		CHECK(r.parent.size() == 1);
		CHECK(r.rank.size() == 1);
		CHECK(r.types.size() == 1);
		// status return one typed variable
		CHECK(r.status().first.size() == 1);
		CHECK(r.status().second.size() == 0);
	}
}
