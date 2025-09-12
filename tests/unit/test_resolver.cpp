// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

#include "resolver.h"


TEST_SUITE("Configuration") {
	TEST_CASE("logging") {
		logging::trace();
	}
}

TEST_SUITE("scopes") {
    TEST_CASE("empty scopes") {
		scoped_resolver<size_t, size_t> r(0, 0);
		CHECK(r.kinds().size() == 0);
		CHECK(r.scopes.size() == 1);
		CHECK(r.kinds_.size() == 0);
	}

    TEST_CASE("scope with one unknown data") {
		scoped_resolver<size_t, size_t> r(0, 0);
		size_t x = 0;
		r.insert(x);
		CHECK(r.assign(x, 0));
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
		CHECK(r.assign(x, 1));
		CHECK(r.type_of(x) == 1);
		CHECK(r.type_of(y) == r.unknown);
		CHECK(r.kinds().size() == 2);
		CHECK(r.kinds().at(x) == 1);
		CHECK(r.kinds().at(y) == r.unknown);
	}

    TEST_CASE("scopes with two data, one known and the other one unknown") {
		scoped_resolver<size_t, size_t> r(0, 0);
		size_t x = 0;
		size_t y = 1;
		r.insert(x);
		r.insert(y);
		CHECK(r.assign(x, 1));
		CHECK(r.type_of(x) == 1);
		CHECK(r.type_of(y) == r.unknown);
		CHECK(r.kinds().size() == 2);
		CHECK(r.kinds().at(x) == 1);
		CHECK(r.kinds().at(y) == r.unknown);
	}

}
