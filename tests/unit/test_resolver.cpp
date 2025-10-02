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
    TEST_CASE("empty scope is empty") {
		scoped_resolver<size_t, size_t> r(0);
		CHECK(r.kinds().size() == 0);
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 0);
	}

	TEST_CASE("closing outer scope") {
		scoped_resolver<size_t, size_t> r(0);
		r.close();
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 0);
	}

	TEST_CASE("opening inner scope") {
		scoped_resolver<size_t, size_t> r(0);
		r.open({});
		CHECK(r.scopes_.size() == 2);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 1);
	}

	TEST_CASE("opening clossing scope") {
		scoped_resolver<size_t, size_t> r(0);
		r.open({});
		CHECK(r.scopes_.size() == 2);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 1);
		r.close();
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 1);
	}

	TEST_CASE("opening two nested inner scopes") {
		scoped_resolver<size_t, size_t> r(0);
		r.open({});
		CHECK(r.scopes_.size() == 2);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 1);
		r.open({});
		CHECK(r.scopes_.size() == 3);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 2);
	}

	TEST_CASE("closing two nested inner scopes") {
		scoped_resolver<size_t, size_t> r(0);
		r.open({});
		CHECK(r.scopes_.size() == 2);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 1);
		r.open({});
		CHECK(r.scopes_.size() == 3);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 2);
		r.close();
		CHECK(r.scopes_.size() == 2);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 2);
		r.close();
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 2);
	}

	TEST_CASE("opening/closing two sibling inner scopes") {
		scoped_resolver<size_t, size_t> r(0);
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 0);
		r.open({});
		CHECK(r.scopes_.size() == 2);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 1);
		r.close();
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 1);
		r.open({});
		CHECK(r.scopes_.size() == 2);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 2);
		r.close();
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 2);
	}

    TEST_CASE("adding unkinded element to empty scope") {
		scoped_resolver<size_t, size_t> r(0);
		size_t x = 0;
		r.insert(x);
		CHECK(r.kinds().size() == 1);
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds_.size() == 1);
		CHECK(r.type_of(x) == r.unknown);
	}

    TEST_CASE("assigning to non-existing element in empty scope") {
		scoped_resolver<size_t, size_t> r(0);
		size_t x = 0;
		size_t type = 1;
		CHECK(!r.assign(x, type));
		CHECK(r.kinds().size() == 0);
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds_.size() == 0);
	}

    TEST_CASE("adding and assign element to empty scope") {
		scoped_resolver<size_t, size_t> r(0);
		size_t x = 0;
		size_t type = 1;
		r.insert(x);
		r.assign(x, type);
		CHECK(r.type_of(x) == type);
		CHECK(r.kinds().size() == 1);
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds_.size() == 1);
	}

	TEST_CASE("assigning unkinded element to a non-empty scope") {
		scoped_resolver<size_t, size_t> r(0);
		size_t x = 0;
		size_t y = 1;
		r.insert(x);
		r.insert(y);
		CHECK(r.kinds().size() == 2);
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds_.size() == 2);
		CHECK(r.type_of(x) == r.unknown);
		CHECK(r.type_of(y) == r.unknown);
	}

	TEST_CASE("assigning kinded element to a non-empty scope: existing unkinded") {
		scoped_resolver<size_t, size_t> r(0);
		size_t x = 0;
		size_t y = 1;
		size_t type = 2;
		r.insert(x);
		r.insert(y);
		CHECK(r.assign(y, type));
		CHECK(r.kinds().size() == 2);
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds_.size() == 2);
		CHECK(r.type_of(x) == r.unknown);
		CHECK(r.type_of(y) == type);
	}

	TEST_CASE("assigning kinded element to a non-empty scope: existing kinded") {
		scoped_resolver<size_t, size_t> r(0);
		size_t x = 0;
		size_t y = 1;
		size_t type1 = 2;
		size_t type2 = 3;
		r.insert(x);
		r.insert(y);
		CHECK(r.assign(y, type1));
		CHECK(r.assign(x, type2)); // cannot assign conflicting kind
		CHECK(r.kinds().size() == 2);
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds_.size() == 2);
		CHECK(r.type_of(x) == type2);
		CHECK(r.type_of(y) == type1);
	}

	TEST_CASE("assigning unkinded element to a nested scope") {
		scoped_resolver<size_t, size_t> r(0);
		size_t x = 0;
		size_t y = 1;
		r.insert(x);
		r.open({});
		r.insert(y);
		CHECK(r.kinds().size() == 2);
		CHECK(r.scopes_.size() == 2);
		CHECK(r.kinds_.size() == 2);
		CHECK(r.type_of(x) == r.unknown);
		CHECK(r.type_of(y) == r.unknown);
	}

	TEST_CASE("assigning kinded element to a nested scope (y1)") {
		scoped_resolver<size_t, size_t> r(0);
		size_t x = 0;
		size_t y = 1;
		size_t type = 2;
		r.insert(x);
		r.open({});
		r.insert(y);
		CHECK(r.assign(y, type));
		CHECK(r.kinds().size() == 2);
		CHECK(r.scopes_.size() == 2);
		CHECK(r.kinds_.size() == 2);
		CHECK(r.type_of(x) == r.unknown);
		CHECK(r.type_of(y) == type);
	}

	TEST_CASE("assigning kinded element to a nested scope (y2)") {
		scoped_resolver<size_t, size_t> r(0);
		size_t x = 0;
		size_t y = 1;
		size_t type = 2;
		r.insert(x);
		CHECK(r.assign(x, type));
		r.open({});
		r.insert(y);
		CHECK(r.kinds().size() == 2);
		CHECK(r.scopes_.size() == 2);
		CHECK(r.kinds_.size() == 2);
		CHECK(r.type_of(x) == type);
		CHECK(r.type_of(y) == r.unknown);
	}

	TEST_CASE("overwriting kinded element kind in a nested scope") {
		scoped_resolver<size_t, size_t> r(0);
		size_t x = 0;
		size_t type1 = 2;
		size_t type2 = 3;
		r.insert(x);
		CHECK(r.assign(x, type1));
		r.open({{x, type2}});
		CHECK(r.kinds().size() == 1);
		CHECK(r.scopes_.size() == 2);
		CHECK(r.kinds_.size() == 2);
		CHECK(r.type_of(x) == type2);
		r.close();
		CHECK(r.kinds().size() == 1);
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds_.size() == 2);
		CHECK(r.type_of(x) == type1);
	}

	TEST_CASE("overwriting unkinded element kind in a nested scope") {
		scoped_resolver<size_t, size_t> r(0);
		size_t x = 0;
		size_t type1 = 2;
		r.insert(x);
		r.open({{x, type1}});
		CHECK(r.kinds().size() == 1);
		CHECK(r.scopes_.size() == 2);
		CHECK(r.kinds_.size() == 2);
		CHECK(r.type_of(x) == type1);
		r.close();
		CHECK(r.kinds().size() == 1);
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds_.size() == 2);
		CHECK(r.type_of(x) == r.unknown);
	}
}

TEST_SUITE("merge_ba_types") {

	using type_t = tree<node_t>::type_t;

	bool match(const type_t& type1, const type_t& type2, const type_t& result) {
		auto merged = merge_ba_types<node_t>(type1, type2);
		if (!merged) return false;
		return (result.first == merged.value().first
			&& result.second == merged.value().second);
	}

	bool nomatch(const type_t& type1, const type_t& type2) {
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
