// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <random>

#include "test_init.h"
#include "boolean_algebras/bdds/bdd_handle.h"
#include "boolean_algebras/sbf_ba.h"

TEST_SUITE("operator==") {

	TEST_CASE("hbdd == bool") {
		CHECK( get_one<Bool>() == true );
		CHECK( get_zero<Bool>() == false );
		CHECK( get_one<Bool>() != false );
		CHECK( get_zero<Bool>() != true );
	}
}

TEST_SUITE("operator&") {

	TEST_CASE("hbdd & hbdd") {
		CHECK( (get_one<Bool>() & get_one<Bool>()) == true );
		CHECK( (get_one<Bool>() & get_zero<Bool>()) == false );
		CHECK( (get_zero<Bool>() & get_one<Bool>()) == false );
		CHECK( (get_zero<Bool>() & get_zero<Bool>()) == false );
	}
}

TEST_SUITE("operator|") {

	TEST_CASE("hbdd | hbdd") {
		CHECK( (get_one<Bool>() | get_one<Bool>()) == true );
		CHECK( (get_one<Bool>() | get_zero<Bool>()) == true );
		CHECK( (get_zero<Bool>() | get_one<Bool>()) == true );
		CHECK( (get_zero<Bool>() | get_zero<Bool>()) == false );
	}
}

TEST_SUITE("operator^") {

	TEST_CASE("hbdd ^ hbdd") {
		CHECK( (get_one<Bool>() ^ get_one<Bool>()) == false );
		CHECK( (get_one<Bool>() ^ get_zero<Bool>()) == true );
		CHECK( (get_zero<Bool>() ^ get_one<Bool>()) == true );
		CHECK( (get_zero<Bool>() ^ get_zero<Bool>()) == false );
	}
}

TEST_SUITE("operator+") {

	TEST_CASE("hbdd + hbdd") {
		CHECK( (get_one<Bool>() + get_one<Bool>()) == false );
		CHECK( (get_one<Bool>() + get_zero<Bool>()) == true );
		CHECK( (get_zero<Bool>() + get_one<Bool>()) == true );
		CHECK( (get_zero<Bool>() + get_zero<Bool>()) == false );
	}
}

TEST_SUITE("operator~") {

	TEST_CASE("hbdd ~") {
		CHECK( (~get_one<Bool>()) == get_zero<Bool>() );
		CHECK( (~get_zero<Bool>()) == get_one<Bool>() );
	}
}

TEST_SUITE("Bool helpers") {
	TEST_CASE("zero/one/normalize/hash") {
		CHECK(Bool::zero().is_zero());
		CHECK(Bool::one().is_one());
		CHECK(normalize_bool(Bool(false)) == Bool(false));
		CHECK(normalize_bool(Bool(true)) == Bool(true));
		CHECK(std::hash<Bool>{}(Bool(false)) == 0);
		CHECK(std::hash<Bool>{}(Bool(true)) == 1);
	}
}

TEST_SUITE("BDD_Splitter") {
	TEST_CASE("DNF_clause_deletion1") {
		bdd_init<Bool>();
		auto a1 = bdd_handle<Bool>::bit(true, 1);
		auto a2 = bdd_handle<Bool>::bit(true, 2);
		auto a3 = bdd_handle<Bool>::bit(true, 3);

		auto d1 = a1 | a2 | a3;
		CHECK(sbf_splitter(d1, splitter_type::upper) == (a1 | a2));
	}

	TEST_CASE("DNF_clause_deletion2") {
		bdd_init<Bool>();
		auto a1 = bdd_handle<Bool>::bit(false, 1);
		auto a2 = bdd_handle<Bool>::bit(false, 2);
		auto a3 = bdd_handle<Bool>::bit(false, 3);

		auto d1 = a1 | a2 | a3;
		CHECK(sbf_splitter(d1, splitter_type::upper) == (a1 | a2));
	}

	TEST_CASE("single_DNF_clause") {
		bdd_init<Bool>();
		auto a1 = bdd_handle<Bool>::bit(true, 1);
		auto a2 = bdd_handle<Bool>::bit(true, 2);
		auto a3 = bdd_handle<Bool>::bit(true, 3);
		auto a4 = bdd_handle<Bool>::bit(true, 4);

		auto d1 = a1 & a2 & a3;
		CHECK(sbf_splitter(d1, splitter_type::upper) == (d1 & a4));
	}

	TEST_CASE("splitter strategies produce proper refinements") {
		bdd_init<Bool>();
		auto a1 = bdd_handle<Bool>::bit(true, 1);
		auto a2 = bdd_handle<Bool>::bit(true, 2);
		auto a3 = bdd_handle<Bool>::bit(true, 3);
		auto d1 = a1 | a2 | a3;
		auto lower = sbf_splitter(d1, splitter_type::lower);
		auto middle = sbf_splitter(d1, splitter_type::middle);
		auto upper = sbf_splitter(d1, splitter_type::upper);
		CHECK(lower != d1);
		CHECK(middle != d1);
		CHECK(upper == (a1 | a2));
		CHECK((lower & ~d1) == false);
		CHECK((middle & ~d1) == false);
		CHECK((upper & ~d1) == false);
	}

	TEST_CASE("bad splitter adds a fresh clause on unsplittable input") {
		bdd_init<Bool>();
		auto a1 = bdd_handle<Bool>::bit(true, 1);
		auto a2 = bdd_handle<Bool>::bit(true, 2);
		auto bad = sbf_splitter(a1 & a2, splitter_type::bad);
		CHECK(bad != (a1 & a2));
		CHECK((bad & ~(a1 & a2)) == false);
		CHECK(((a1 & a2) & ~bad) != false);
	}
}

TEST_SUITE("and_many") {
	TEST_CASE("and_many of shifted copies of one node equals the binary fold") {
		// The slices x_i = y_i are shifted copies of one node, so their
		// cofactors share an id and differ in shift or input inverter alone.
		bdd_init<Bool>();
		std::vector<hbdd<Bool>> eqs;
		for (uint_t i = 0; i < 8; ++i) {
			// bit() takes new variables in sequence.
			const hbdd<Bool> x = bdd_handle<Bool>::bit(true, 2 * i + 1);
			const hbdd<Bool> y = bdd_handle<Bool>::bit(true, 2 * i + 2);
			eqs.push_back(~(x ^ y));
		}
		// and_many runs after the fold's first step, with that pair of
		// slices already in and_memo.
		hbdd<Bool> fold = eqs[0] & eqs[1];
		const hbdd<Bool> many = bdd_handle<Bool>::and_many(eqs);
		for (size_t i = 2; i < eqs.size(); ++i) fold = fold & eqs[i];
		CHECK((many == fold));
	}
}

TEST_SUITE("quantify") {
	TEST_CASE("one pass over a set equals one variable at a time") {
		using engine = bdd<Bool>;
		using handle = bdd_handle<Bool>;
		bdd_init<Bool>();
		const uint_t vars = 24, stride = 3;
		auto var = [](uint_t v) {
			return handle::get(engine::add(v, engine::T, engine::F));
		};
		// A random function over offset + 1 .. offset + 16; the same g at
		// two offsets gives two shifted copies of one skeleton.
		std::function<hbdd<Bool>(std::mt19937&, uint_t, int)> random_bdd =
			[&](std::mt19937& g, uint_t offset, int depth) {
			if (depth == 0) {
				const hbdd<Bool> l = var(offset + 1 + g() % 16);
				return g() % 2 ? l : ~l;
			}
			const hbdd<Bool> a = random_bdd(g, offset, depth - 1);
			const hbdd<Bool> b = random_bdd(g, offset, depth - 1);
			switch (g() % 3) {
				case 0: return a & b;
				case 1: return a | b;
				default: return a ^ b;
			}
		};
		std::mt19937 rng(20261009);
		for (int trial = 0; trial < 300; ++trial) {
			std::mt19937 g(rng()), g2 = g;
			const hbdd<Bool> f = random_bdd(g, 0, 4);
			const hbdd<Bool> x = trial % 3 == 0 ? f
				: trial % 3 == 1 ? f ^ random_bdd(g2, 1 + rng() % 8, 4)
				: f & ~random_bdd(g2, 1 + rng() % 8, 4);
			// Odd trials take the sweep's slice-major layout: one rank's
			// bits, `stride` apart; even ones a random set, unsorted and
			// reaching past the function's variables.
			std::vector<uint_t> vs;
			if (trial % 2) for (uint_t v = 1 + rng() % stride; v <= vars;
				v += stride) vs.push_back(v);
			else for (uint_t v = vars + 4; v >= 1; --v)
				if (rng() % 3 == 0) vs.push_back(v);
			for (const bool universal : { false, true }) {
				hbdd<Bool> one_by_one = x;
				for (const uint_t v : vs) one_by_one = universal
					? one_by_one->all(v) : one_by_one->ex(v);
				CHECK((x->quantify(vs, universal) == one_by_one));
			}
		}
	}
}

TEST_SUITE("descending variable order") {
	TEST_CASE("quantifying a variable above the top does not answer for one "
		"below it") {
		constexpr auto o = bdd_options<INV_IN | INV_OUT | VARSHIFT
			| INV_ORDER>::create();
		using engine = bdd<Bool, o>;
		using handle = bdd_handle<Bool, o>;
		bdd_init<Bool, o>();
		auto var = [](uint_t v) {
			return handle::get(engine::add(v, engine::T, engine::F));
		};
		// The top of x5·x3 is 5. Variable 7 lies above it and 3 below,
		// and their shifted cache keys differ.
		const hbdd<Bool, o> f = var(5) & var(3);
		CHECK((f->ex(7) == f));
		CHECK((f->ex(3) == var(5)));
		CHECK((f->all(7) == f));
		CHECK((f->all(3) == handle::hfalse));
	}

	TEST_CASE("variables as far above the top as below it keep apart") {
		constexpr auto o = bdd_options<INV_IN | INV_OUT | VARSHIFT
			| INV_ORDER>::create();
		using engine = bdd<Bool, o>;
		using handle = bdd_handle<Bool, o>;
		bdd_init<Bool, o>();
		auto var = [](uint_t v) {
			return handle::get(engine::add(v, engine::T, engine::F));
		};
		// The cache keys of x6·x2 count from 7: variable 8 lies one step
		// above it, the top 6 one step below.
		const hbdd<Bool, o> g = var(6) & var(2);
		CHECK((g->ex(8) == g));
		CHECK((g->ex(6) == var(2)));
		CHECK((g->all(8) == g));
		CHECK((g->all(6) == handle::hfalse));
	}
}
