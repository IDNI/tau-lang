// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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
		scoped_union_find<size_t, size_t> r;
		CHECK(r.scopes.size() == 1);
		CHECK(r.current == 0);
	}

	TEST_CASE("closing outer scope") {
		scoped_union_find<size_t, size_t> r;
		r.close();
		CHECK(r.scopes.size() == 1);
		CHECK(r.current == 0);
	}

	TEST_CASE("opening inner scope") {
		scoped_union_find<size_t, size_t> r;
		r.open();
		CHECK(r.scopes.size() == 2);
		CHECK(r.current == 1);
	}

	TEST_CASE("opening clossing scope") {
		scoped_union_find<size_t, size_t> r;
		r.open();
		CHECK(r.scopes.size() == 2);
		CHECK(r.current == 1);
		r.close();
		CHECK(r.scopes.size() == 1);
		CHECK(r.current == 1);
	}

	TEST_CASE("opening two nested inner scopes") {
		scoped_union_find<size_t, size_t> r;
		r.open();
		CHECK(r.scopes.size() == 2);
		CHECK(r.current == 1);
		r.open();
		CHECK(r.scopes.size() == 3);
		CHECK(r.current == 2);
	}

	TEST_CASE("closing two nested inner scopes") {
		scoped_union_find<size_t, size_t> r;
		r.open();
		CHECK(r.scopes.size() == 2);
		CHECK(r.current == 1);
		r.open();
		CHECK(r.scopes.size() == 3);
		CHECK(r.current == 2);
		r.close();
		CHECK(r.scopes.size() == 2);
		CHECK(r.current == 2);
		r.close();
		CHECK(r.scopes.size() == 1);
		CHECK(r.current == 2);
	}

	TEST_CASE("opening/closing two sibling inner scopes") {
		scoped_union_find<size_t, size_t> r;
		CHECK(r.scopes.size() == 1);
		CHECK(r.current == 0);
		r.open();
		CHECK(r.scopes.size() == 2);
		CHECK(r.current == 1);
		r.close();
		CHECK(r.scopes.size() == 1);
		CHECK(r.current == 1);
		r.open();
		CHECK(r.scopes.size() == 2);
		CHECK(r.current == 2);
		r.close();
		CHECK(r.scopes.size() == 1);
		CHECK(r.current == 2);
	}
}
