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
