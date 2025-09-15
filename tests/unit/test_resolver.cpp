// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

#include "resolver.h"

// Adding comprehensive doctest test cases for scope in resolver.h
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using namespace idni::tau_lang;

TEST_SUITE("Configuration") {
	TEST_CASE("logging") {
		logging::trace();
	}
}

TEST_SUITE("scopes") {
    TEST_CASE("empty scopes") {
		scoped_resolver<size_t, size_t> r(0, 0);
		CHECK(r.kinds().size() == 0);
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds_.size() == 0);
	}

    TEST_CASE("scope with one unknown data") {
		scoped_resolver<size_t, size_t> r(0, 0);
		size_t x = 0;
		r.insert(x);
		CHECK(r.assign(x, r.unknown));
		CHECK(r.type_of(x) == r.unknown);
		CHECK(r.kinds().size() == 1);
		CHECK(r.kinds().at(x) == r.unknown);
	}

    TEST_CASE("scopes with one kinded data") {
		scoped_resolver<size_t, size_t> r(0, 0);
		size_t x = 0;
		size_t type = 1;
		r.insert(x);
		CHECK(r.assign(x, type));
		CHECK(r.type_of(x) == type);
		CHECK(r.kinds().size() == 1);
		CHECK(r.kinds().at(x) == type);
	}

    TEST_CASE("scopes with two untyped variable") {
		scoped_resolver<size_t, size_t> r(0, 0);
		size_t x = 0;
		size_t y = 1;
		r.insert(x);
		r.insert(y);
		CHECK(r.type_of(x) == r.unknown);
		CHECK(r.type_of(y) == r.unknown);
		CHECK(r.kinds().size() == 2);
		CHECK(r.kinds().at(x) == r.unknown);
		CHECK(r.kinds().at(y) == r.unknown);
	}

    TEST_CASE("scopes with two data, one known and the other one unknown") {
		scoped_resolver<size_t, size_t> r(0, 0);
		size_t x = 0;
		size_t y = 1;
		r.insert(x);
		r.insert(y);
		size_t type = 1;
		CHECK(r.assign(x, type));
		CHECK(r.type_of(x) == type);
		CHECK(r.type_of(y) == r.unknown);
		CHECK(r.kinds().size() == 2);
		CHECK(r.kinds().at(x) == type);
		CHECK(r.kinds().at(y) == r.unknown);
	}

	TEST_CASE("open scope with kinds") {
		scoped_resolver<size_t, size_t> r(0, 0);
		std::map<size_t, size_t> scoped = {{1, 1}, {2, 2}};
		r.open(scoped);
		CHECK(r.type_of(1) == 1);
		CHECK(r.type_of(2) == 2);
		CHECK(r.type_of(3) == 0);
		r.insert(3);
		CHECK(r.type_of(3) == 0);
	}

	TEST_CASE("assign and type_of") {
		scoped_resolver<size_t, size_t> r(0, 0);
		r.open({{1, 1}});
		CHECK(r.type_of(1) == 1);
		CHECK(!r.assign(1, 3)); // cannot assign conflicting kind
	}

	TEST_CASE("same_kind") {
		scoped_resolver<size_t, size_t> r(0, 0);
		r.open({{1, 1}, {2, 1}, {3, 2}});
		CHECK(r.same_kind(1, 2));
		CHECK(!r.same_kind(1, 3));
	}

	TEST_CASE("multiple scopes") {
		scoped_resolver<size_t, size_t> r(0, 0);
		r.open({{1, 1}});
		r.open({{1, 2}});
		CHECK(r.type_of(1) == 2);
		r.close();
		CHECK(r.type_of(1) == 1);
	}

	TEST_CASE("kinds map") {
		scoped_resolver<size_t, size_t> r(0, 0);
		r.open({{1, 1}, {2, 2}});
		r.insert(3);
		r.assign(3, 3);
		auto kinds_map = r.kinds();
		CHECK(kinds_map.size() == 3);
		CHECK(kinds_map[1] == 1);
		CHECK(kinds_map[2] == 2);
		CHECK(kinds_map[3] == 3);
	}
}

TEST_SUITE("merge_ba_types") {

	bool match(const type& type1, const type& type2, const type& result) {
		auto merged = merge_ba_types<node_t>(type1, type2);
		if (!merged) return false;
		return (result.first == merged.value().first
			&& result.second == merged.value().second);
	}

	bool nomatch(const type& type1, const type& type2) {
		auto merged = merge_ba_types<node_t>(type1, type2);
		return !merged.has_value();
	}

	TEST_CASE("untyped with untyped") {
		CHECK(match({0, nullptr}, {0, nullptr}, {0, nullptr}));
		CHECK(match({0, (tref)1}, {0, nullptr}, {0, nullptr}));
		CHECK(match({0, nullptr}, {0, (tref)1}, {0, nullptr}));
	}

	TEST_CASE("untyped with any type") {
		CHECK(match({0, nullptr}, {1, nullptr}, {1, nullptr}));
		CHECK(match({0, nullptr}, {1, (tref)1}, {1, (tref)1}));
		CHECK(match({1, nullptr}, {0, nullptr}, {1, nullptr}));
		CHECK(match({1, (tref)1}, {0, nullptr}, {1, (tref)1}));
		CHECK(match({0, nullptr}, {0, nullptr}, {0, nullptr}));
	}

	TEST_CASE("same types all possible subtypes") {
		CHECK(match({1, nullptr}, {1, nullptr}, {1, nullptr}));
		CHECK(match({1, (tref)1}, {1, (tref)1}, {1, (tref)1}));
		CHECK(match({1, nullptr}, {1, (tref)1}, {1, (tref)1}));
		CHECK(match({1, (tref)1}, {1, nullptr}, {1, (tref)1}));
	}

	TEST_CASE("different types") {
		CHECK(nomatch({1, nullptr}, {2, nullptr}));
		CHECK(nomatch({1, (tref)1}, {1, (tref)2}));
	}
}

TEST_SUITE("type_scoped_resolver") {

	TEST_CASE("merging in the same scope") {
		type_scoped_resolver<node_t> r;
		tref a = (tref)1;
		tref b = (tref)2;
		r.insert(a);
		r.insert(b);
		type t = {1, (tref)1};
		CHECK(r.assign(a, t));
		CHECK(r.merge(a, b));
		CHECK(r.type_of(a) == r.type_of(b));
		CHECK(r.type_of(a) == t);
	}

	TEST_CASE("merging in different scopes") {
		type_scoped_resolver<node_t> r;
		tref a = (tref)1;
		tref b = (tref)2;
		type t = {1, (tref)1};
		r.insert(a);
		r.open({{b, t}});
		CHECK(r.merge(a, b));
		CHECK(r.type_of(a) == r.type_of(b));
		CHECK(r.type_of(a) == t);
	}

	TEST_CASE("merging conflicting types in the same scope") {
		type_scoped_resolver<node_t> r;
		tref a = (tref)1;
		tref b = (tref)2;
		r.insert(a);
		r.insert(b);
		type t1 = {1, (tref)1};
		type t2 = {2, (tref)2};
		CHECK(r.assign(a, t1));
		CHECK(r.assign(b, t2));
		CHECK(!r.merge(a, b)); // conflicting types
	}

	TEST_CASE("merging conflicting types in different scopes") {
		type_scoped_resolver<node_t> r;
		tref a = (tref)1;
		tref b = (tref)2;
		type t1 = {1, (tref)1};
		type t2 = {2, (tref)2};
		r.insert(a);
		CHECK(r.assign(a, t1));
		r.open({{b, t2}});
		CHECK(!r.merge(a, b)); // conflicting types
	}
}

TEST_SUITE("new_infer_ba_types") {}