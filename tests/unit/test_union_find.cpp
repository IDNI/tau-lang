// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt


#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "union_find.h"

using namespace idni::tau_lang;

TEST_SUITE("union_find") {

TEST_CASE("insert and contains") {
	union_find<int> uf;
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
	union_find<int> uf;
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
	union_find<int> uf;
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

TEST_CASE("get_sets returns correct sets") {
	union_find<int> uf;
	uf.insert(1);
	uf.insert(2);
	uf.insert(3);
	uf.insert(4);
	uf.merge(1, 2);
	uf.merge(3, 4);
	auto sets = uf.get_sets();
	CHECK(sets.size() == 2);
	bool found12 = false, found34 = false;
	for (const auto& [root, members] : sets) {
		if (members.contains(1) && members.contains(2)) found12 = true;
		if (members.contains(3) && members.contains(4)) found34 = true;
	}
	CHECK(found12);
	CHECK(found34);
}

TEST_CASE("size after merges and inserts") {
	union_find<int> uf;
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
	union_find<int> uf;
	CHECK(!uf.connected(1, 2));
	uf.insert(1);
	CHECK(!uf.connected(1, 2));
	uf.insert(2);
	CHECK(!uf.connected(1, 2));
	uf.merge(1, 2);
	CHECK(uf.connected(1, 2));
}

}