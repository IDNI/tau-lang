// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "union_find.h"

using namespace idni::tau_lang;

TEST_SUITE("union_find") {

	TEST_CASE("insert and contains") {
		union_find_by_rank<int> uf;
		uf.insert(1);
		uf.insert(2);
		uf.insert(3);
		CHECK(uf.contains(1));
		CHECK(uf.contains(2));
		CHECK(uf.contains(3));
		CHECK(!uf.contains(4));
		CHECK(uf.size() == 3);
	}

	TEST_CASE("merge and connected") {
		union_find_by_rank<int> uf;
		uf.insert(1);
		uf.insert(2);
		uf.insert(3);
		uf.merge(1, 2);
		CHECK(uf.connected(1, 2));
		CHECK(!uf.connected(1, 3));
		uf.merge(2, 3);
		CHECK(uf.connected(1, 3));
	}

	TEST_CASE("root and path compression") {
		union_find_by_rank<int> uf;
		uf.insert(1);
		uf.insert(2);
		uf.insert(3);
		uf.merge(1, 2);
		uf.merge(2, 3);
		int r1 = uf.root(1);
		int r2 = uf.root(2);
		int r3 = uf.root(3);
		CHECK(r1 == r2);
		CHECK(r2 == r3);
	}

	TEST_CASE("size after merges and inserts") {
		union_find_by_rank<int> uf;
		uf.insert(1);
		uf.insert(2);
		uf.insert(3);
		CHECK(uf.size() == 3);
		uf.merge(1, 2);
		CHECK(uf.size() == 3); // size does not decrease
		uf.insert(4);
		CHECK(uf.size() == 4);
	}

	TEST_CASE("connected for non-inserted elements") {
		union_find_by_rank<int> uf;
		CHECK(!uf.connected(1, 2));
		uf.insert(1);
		CHECK(!uf.connected(1, 2));
		uf.insert(2);
		CHECK(!uf.connected(1, 2));
		uf.merge(1, 2);
		CHECK(uf.connected(1, 2));
	}

}

TEST_SUITE("scoped_union_find") {
    TEST_CASE("empty scope is empty") {
		scoped_union_find<size_t, size_t> r(0);
		CHECK(r.kinds().size() == 0);
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 0);
	}

	TEST_CASE("closing outer scope") {
		scoped_union_find<size_t, size_t> r(0);
		r.close();
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 0);
	}

	TEST_CASE("opening inner scope") {
		scoped_union_find<size_t, size_t> r(0);
		r.open({});
		CHECK(r.scopes_.size() == 2);
		CHECK(r.kinds().size() == 0);
		CHECK(r.kinds_.size() == 0);
		CHECK(r.current == 1);
	}

	TEST_CASE("opening clossing scope") {
		scoped_union_find<size_t, size_t> r(0);
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
		scoped_union_find<size_t, size_t> r(0);
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
		scoped_union_find<size_t, size_t> r(0);
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
		scoped_union_find<size_t, size_t> r(0);
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
		scoped_union_find<size_t, size_t> r(0);
		size_t x = 0;
		r.insert(x);
		CHECK(r.kinds().size() == 1);
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds_.size() == 1);
		CHECK(r.type_of(x) == r.unknown);
	}

    TEST_CASE("assigning to non-existing element in empty scope") {
		scoped_union_find<size_t, size_t> r(0);
		size_t x = 0;
		size_t type = 1;
		CHECK(!r.assign(x, type));
		CHECK(r.kinds().size() == 0);
		CHECK(r.scopes_.size() == 1);
		CHECK(r.kinds_.size() == 0);
	}

    TEST_CASE("adding and assign element to empty scope") {
		scoped_union_find<size_t, size_t> r(0);
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
		scoped_union_find<size_t, size_t> r(0);
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
		scoped_union_find<size_t, size_t> r(0);
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
		scoped_union_find<size_t, size_t> r(0);
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
		scoped_union_find<size_t, size_t> r(0);
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
		scoped_union_find<size_t, size_t> r(0);
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
		scoped_union_find<size_t, size_t> r(0);
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
		scoped_union_find<size_t, size_t> r(0);
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
		scoped_union_find<size_t, size_t> r(0);
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
