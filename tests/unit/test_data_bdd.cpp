// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "backends/bdds/data_bdd.h"

#include <bit>
#include <random>

using namespace idni::tau_lang;

namespace {

using id = data_bdd::id;

constexpr uint32_t nv = 10;

// The value of `n` under the assignment whose bit v is variable v.
bool eval(const data_bdd& bdd, id n, uint32_t assignment) {
	while (n > data_bdd::T) {
		const auto& x = bdd.nodes[n];
		n = assignment >> x.var & 1 ? x.hi : x.lo;
	}
	return n == data_bdd::T;
}

std::vector<bool> truth_table(const data_bdd& bdd, id n) {
	std::vector<bool> t;
	for (uint32_t a = 0; a < (1u << nv); ++a) t.push_back(eval(bdd, n, a));
	return t;
}

// Applies a random operation to a pool of functions, replacing one of
// them; `quantified` holds the variables a quantification removes.
struct random_ops {
	std::mt19937 rng;
	explicit random_ops(unsigned seed) : rng(seed) {}
	size_t pick(size_t n) { return rng() % n; }

	struct op { size_t kind, a, b, c, to; uint32_t v; };
	op next(size_t pool) {
		return { pick(7), pick(pool), pick(pool), pick(pool), pick(pool),
			(uint32_t)pick(nv) };
	}
};

id run(data_bdd& bdd, const random_ops::op& o, const std::vector<id>& p) {
	switch (o.kind) {
	case 0: return bdd.conj(p[o.a], p[o.b]);
	case 1: return bdd.disj(p[o.a], p[o.b]);
	case 2: return bdd.exor(p[o.a], p[o.b]);
	case 3: return bdd.ite(p[o.a], p[o.b], p[o.c]);
	case 4: return bdd.neg(p[o.a]);
	case 5: {
		std::vector<bool> qs(nv, false);
		qs[o.v] = true;
		return bdd.quantify(p[o.a], qs, o.b % 2 == 0);
	}
	default: return bdd.exor(p[o.a], bdd.var(o.v, o.b % 2 == 0));
	}
}

} // namespace

TEST_SUITE("data_bdd collection") {

	// A table of 600 nodes against one that is never filled: collected
	// once half full and when full, or only when full.
	void agree_with_large_table(bool at_half) {
		constexpr size_t pool = 8;
		data_bdd small(600), large(size_t{1} << 20);
		std::vector<id> ps, pl;
		for (uint32_t v = 0; v < pool; ++v) {
			ps.push_back(small.var(v));
			pl.push_back(large.var(v));
		}
		auto roots = [&](auto&& mark) { for (id x : ps) mark(x); };
		random_ops gen(20260928);
		size_t redone = 0;
		for (size_t step = 0; step < 4000; ++step) {
			const auto o = gen.next(pool);
			if (at_half && small.wants_collect()) small.collect(roots);
			id rs = run(small, o, ps);
			if (small.full) {
				small.collect(roots);
				++redone;
				rs = run(small, o, ps);
			}
			REQUIRE( !small.full );
			ps[o.to] = rs;
			pl[o.to] = run(large, o, pl);
			if (step % 50 == 0)
				for (size_t i = 0; i < pool; ++i)
					REQUIRE( truth_table(small, ps[i])
						== truth_table(large, pl[i]) );
		}
		for (size_t i = 0; i < pool; ++i)
			CHECK( truth_table(small, ps[i]) == truth_table(large, pl[i]) );
		CHECK( small.collections > 10 );
		if (!at_half) CHECK( redone == small.collections );
		CHECK( small.nodes.size() <= 600 );
		CHECK( large.collections == 0 );
	}

	TEST_CASE("results agree with a table that never collects") {
		agree_with_large_table(true);
	}

	TEST_CASE("results agree when collected only after a full table") {
		agree_with_large_table(false);
	}

	TEST_CASE("a collection keeps the roots and the hash-consing") {
		data_bdd bdd(size_t{1} << 16);
		const id f = bdd.conj(bdd.var(0), bdd.disj(bdd.var(3), bdd.var(5)));
		const id g = bdd.exor(bdd.var(1), bdd.var(2));
		const auto tf = truth_table(bdd, f), tg = truth_table(bdd, g);
		id garbage = data_bdd::F;
		for (uint32_t v = 0; v + 1 < nv; ++v)
			garbage = bdd.exor(garbage, bdd.conj(bdd.var(v), bdd.var(v + 1)));
		const size_t before = bdd.size();
		const size_t freed = bdd.collect([&](auto&& mark) {
			mark(f);
			mark(g);
		});
		CHECK( freed > 0 );
		CHECK( bdd.size() == before - freed );
		CHECK( bdd.unique_size() == bdd.size() - 2 );
		CHECK( truth_table(bdd, f) == tf );
		CHECK( truth_table(bdd, g) == tg );
		// no remembered operation names a freed node
		size_t remembered = 0;
		for (const auto& e : bdd.memo_slots) {
			if (e.op == data_bdd::leaf) continue;
			++remembered;
			CHECK( bdd.nodes[e.a].var != data_bdd::freed );
			CHECK( bdd.nodes[e.b].var != data_bdd::freed );
			CHECK( bdd.nodes[e.r].var != data_bdd::freed );
		}
		CHECK( remembered == bdd.memo_size() );
		// the same functions built again are the same nodes
		CHECK( bdd.conj(bdd.var(0), bdd.disj(bdd.var(3), bdd.var(5))) == f );
		CHECK( bdd.exor(bdd.var(2), bdd.var(1)) == g );
		// freed ids are taken again before the table grows
		const size_t slots = bdd.nodes.size();
		id again = data_bdd::F;
		for (uint32_t v = 0; v + 1 < nv; ++v)
			again = bdd.exor(again, bdd.conj(bdd.var(v), bdd.var(v + 1)));
		CHECK( bdd.nodes.size() == slots );
		CHECK( again != data_bdd::F );
	}

	TEST_CASE("after a full table the work is redone") {
		// the parity of ten variables, from the last one up
		auto parity = [](data_bdd& bdd) {
			id p = data_bdd::F;
			for (uint32_t v = nv; v-- > 0; )
				p = bdd.exor(bdd.var(v), p);
			return p;
		};
		data_bdd bdd(120);
		id junk = data_bdd::T;
		for (uint32_t v = 0; v < nv && !bdd.full; ++v)
			junk = bdd.conj(junk, bdd.disj(bdd.var(v), bdd.var((v + 3) % nv)));
		parity(bdd);
		REQUIRE( bdd.full );
		bdd.collect([](auto&&) {});
		CHECK( !bdd.full );
		CHECK( bdd.memo_size() == 0 );
		CHECK( bdd.size() == 2 );
		const id p = parity(bdd);
		REQUIRE( !bdd.full );
		for (uint32_t a = 0; a < (1u << nv); ++a)
			CHECK( eval(bdd, p, a) == (std::popcount(a) % 2 == 1) );
	}

	TEST_CASE("a bound on the growth fills the table early") {
		data_bdd bdd(size_t{1} << 16);
		bdd.grow_at_most(5);
		id p = data_bdd::F;
		for (uint32_t v = nv; v-- > 0 && !bdd.full; )
			p = bdd.exor(bdd.var(v), p);
		CHECK( bdd.full );
		CHECK( bdd.size() == 2 + 5 );
		bdd.collect([](auto&&) {});
		bdd.grow_freely();
		p = data_bdd::F;
		for (uint32_t v = nv; v-- > 0; ) p = bdd.exor(bdd.var(v), p);
		CHECK( !bdd.full );
		CHECK( eval(bdd, p, 1) );
	}
}
