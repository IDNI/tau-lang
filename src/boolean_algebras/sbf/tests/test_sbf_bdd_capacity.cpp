// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// A full bdd node table must raise bdd_node_table_exhausted, intern nothing
// past its capacity, and leave the table, its memos and its handle table
// describing exactly what they did before, so work that fits stays correct
// once the flag is lowered. A dedicated 16-entry table (idW = 4) is filled
// for real here; it is separate from the default hbdd<Bool> one.

#include "test_init.h"
#include "backends/bdds/bdd_handle.h"

using namespace idni::tau_lang;

namespace {

constexpr auto tiny = bdd_options<>::create(4, 12);
using tbdd = bdd<Bool, tiny>;
using th = bdd_handle<Bool, tiny>;
using thbdd = hbdd<Bool, tiny>;

thbdd tiny_lit(uint_t v) { return th::bit(true, v); }

// x_i == y_i for i <= n, variables grouped (all x before all y): the bdd
// needs about 2^n nodes, so the tiny table runs out for a small n.
thbdd equal_pairs(uint_t n) {
	thbdd f = th::htrue;
	for (uint_t i = 1; i <= n; ++i) {
		auto x = tiny_lit(i), y = tiny_lit(n + i);
		f = f & ((x & y) | (~x & ~y));
	}
	return f;
}

size_t capacity() { return size_t{1} << tiny.idW; }

// A second table of 32 entries for the interning cases, so their nodes
// do not depend on what the cases above left in the tiny one.
constexpr auto small = bdd_options<>::create(5, 12);
using sbdd = bdd<Bool, small>;
using sh = bdd_handle<Bool, small>;

hbdd<Bool, small> small_equal_pairs(uint_t n) {
	hbdd<Bool, small> f = sh::htrue;
	for (uint_t i = 1; i <= n; ++i) {
		auto x = sh::bit(true, i), y = sh::bit(true, n + i);
		f = f & ((x & y) | (~x & ~y));
	}
	return f;
}

// Every case restores the process-wide flag it may raise.
struct flag_guard {
	~flag_guard() { bdd_node_table_exhausted = false; }
};

} // namespace

TEST_SUITE("bdd node table exhaustion") {

TEST_CASE("a full table raises the flag and interns nothing more") {
	flag_guard g;
	bdd_init<Bool, tiny>();
	auto small = tiny_lit(1) & tiny_lit(2);
	REQUIRE(small != th::hfalse);
	// Quantifying 150 out of wide needs a node for 101 & 199, whose
	// variable distance no other function here has, so it does not fit
	// once the table is full.
	auto wide = tiny_lit(101) & tiny_lit(150) & tiny_lit(199);
	auto not_wide = ~wide;
	REQUIRE(wide != th::hfalse);
	REQUIRE_FALSE(bdd_node_table_exhausted);
	uint_t n = 1;
	for (; n <= 8 && !bdd_node_table_exhausted; ++n)
		(void) equal_pairs(n);
	REQUIRE(bdd_node_table_exhausted);
	CHECK(tbdd::V.size() == capacity());

	bdd_node_table_exhausted = false;

	SUBCASE("functions built before the overflow stay exact") {
		CHECK((small & ~small) == th::hfalse);
		CHECK((small | ~small) == th::htrue);
		CHECK((small & tiny_lit(1)) == small);
		CHECK((small | tiny_lit(1)) == tiny_lit(1));
		CHECK((tiny_lit(1) & tiny_lit(2)) == small);
		CHECK_FALSE(bdd_node_table_exhausted);
	}

	SUBCASE("a later overflow raises the flag again") {
		(void) equal_pairs(n + 2);
		CHECK(bdd_node_table_exhausted);
		CHECK(tbdd::V.size() == capacity());
	}

	SUBCASE("a quantifier on a raised flag computes and memoizes nothing") {
		bdd_node_table_exhausted = true;
		auto ex_entries = tbdd::ex_memo.size();
		auto all_entries = tbdd::all_memo.size();
		CHECK(not_wide->all(150) == th::hfalse);
		auto e = wide->ex(150);
		CHECK((e == th::hfalse || e == th::htrue));
		CHECK(tbdd::ex_memo.size() == ex_entries);
		CHECK(tbdd::all_memo.size() == all_entries);
		CHECK(bdd_node_table_exhausted);
	}

	SUBCASE("a quantifier that fills the table memoizes no result") {
		(void) wide->ex(150);
		REQUIRE(bdd_node_table_exhausted);
		bdd_node_table_exhausted = false;
		// not answered from a memo: the same call fills the table again
		(void) wide->ex(150);
		CHECK(bdd_node_table_exhausted);
		bdd_node_table_exhausted = false;
		(void) not_wide->all(150);
		CHECK(bdd_node_table_exhausted);
		bdd_node_table_exhausted = false;
		(void) not_wide->all(150);
		CHECK(bdd_node_table_exhausted);
		CHECK(tbdd::V.size() == capacity());
	}
}

TEST_CASE("a handle made while the table is full is the interned one") {
	flag_guard g;
	bdd_init<Bool, small>();
	// an engine node that no handle stands for yet
	auto r = sbdd::add(3, sbdd::T, sbdd::F);
	REQUIRE(r != sbdd::F);
	bdd_node_table_exhausted = true;
	auto during = sh::get(r);
	bdd_node_table_exhausted = false;
	auto after = sh::get(r);
	CHECK(during.get() == after.get());
	CHECK(during == after);
}

TEST_CASE("a node the full table cannot hold gets the interned zero") {
	flag_guard g;
	bdd_init<Bool, small>();
	auto x = sbdd::bit(77);
	for (uint_t n = 1; n <= 8 && !bdd_node_table_exhausted; ++n)
		(void) small_equal_pairs(n);
	REQUIRE(bdd_node_table_exhausted);
	bdd_node_table_exhausted = false;
	// 60 ? x77 : 0 needs a skeleton no function above has
	auto h = sh::get(typename sh::bdd_node_t(60, x, sbdd::F));
	CHECK(bdd_node_table_exhausted);
	CHECK(h.get() == sh::hfalse.get());
	CHECK(h == sh::hfalse);
}

} // TEST_SUITE
