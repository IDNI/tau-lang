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
}

} // TEST_SUITE
