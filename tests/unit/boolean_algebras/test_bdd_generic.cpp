// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "backends/bdds/bdd_handle.h"

// The four-element Boolean algebra, as the subsets of a two-element set. It
// lives outside idni::tau_lang, as an algebra of another library would.
struct b4 {
	unsigned v = 0;
	static const b4& zero() { static b4 z{ 0 }; return z; }
	static const b4& one() { static b4 o{ 3 }; return o; }
	b4 operator&(const b4& x) const { return { v & x.v }; }
	b4 operator|(const b4& x) const { return { v | x.v }; }
	b4 operator^(const b4& x) const { return { v ^ x.v }; }
	b4 operator+(const b4& x) const { return { v ^ x.v }; }
	b4 operator~() const { return { ~v & 3u }; }
	auto operator<=>(const b4&) const = default;
	bool operator==(const b4&) const = default;
	bool operator==(bool b) const { return b ? v == 3 : v == 0; }
};

std::ostream& operator<<(std::ostream& os, const b4& b) { return os << b.v; }

template<> struct std::hash<b4> {
	size_t operator()(const b4& b) const { return b.v; }
};

using namespace idni::tau_lang;

namespace {

using h4 = hbdd<b4>;
using env = std::map<int_t, b4>;

h4 x(uint_t v) { return bdd_handle<b4>::bit(true, v); }
h4 elem(unsigned v) { return bdd_handle<b4>::get(b4{ v }); }

// Calls @p check with every assignment of the variables 1 and 2.
template<typename F> void for_each_env(F check) {
	for (unsigned a = 0; a != 4; ++a) for (unsigned b = 0; b != 4; ++b) {
		env m{ { 1, b4{ a } }, { 2, b4{ b } } };
		check(m);
	}
}

// x1 a | x1' x2, with the leaf a = {1}.
h4 sample() { return (x(1) & elem(1)) | (~x(1) & x(2)); }

b4 sample_value(const env& m) {
	const b4 x1 = m.at(1), x2 = m.at(2);
	return (x1 & b4{ 1 }) | (~x1 & x2);
}

} // namespace

TEST_SUITE("Configuration") {
	TEST_CASE("bdd init") {
		bdd_init<b4>();
	}
}

TEST_SUITE("bdd over a four-element algebra") {

	TEST_CASE("the constants are the interned one and zero") {
		CHECK( bdd_handle<b4>::one()->is_one() );
		CHECK( bdd_handle<b4>::zero()->is_zero() );
		CHECK( elem(3) == bdd_handle<b4>::one() );
		CHECK( elem(0) == bdd_handle<b4>::zero() );
		CHECK( !elem(1)->is_zero() );
		CHECK( !elem(1)->is_one() );
	}

	TEST_CASE("a leaf keeps its element") {
		for (unsigned v = 0; v != 4; ++v) {
			env m;
			CHECK( elem(v)->eval(m) == b4{ v } );
		}
	}

	TEST_CASE("and, or and not evaluate pointwise") {
		const h4 f = sample();
		const h4 g = x(2) | elem(2);
		for_each_env([&](env m) {
			const b4 fv = sample_value(m), gv = m.at(2) | b4{ 2 };
			CHECK( f->eval(m) == fv );
			CHECK( g->eval(m) == gv );
			CHECK( (f & g)->eval(m) == (fv & gv) );
			CHECK( (f | g)->eval(m) == (fv | gv) );
			CHECK( (~f)->eval(m) == ~fv );
			CHECK( (f ^ g)->eval(m) == (fv ^ gv) );
		});
	}

	TEST_CASE("equal functions share one handle") {
		const h4 f = sample();
		CHECK( f == ((x(1) & elem(1)) | (~x(1) & x(2))) );
		CHECK( ~~f == f );
		CHECK( (f & ~f)->is_zero() );
		CHECK( (f | ~f)->is_one() );
		CHECK( (elem(1) & elem(2))->is_zero() );
		CHECK( (elem(1) | elem(2))->is_one() );
	}

	TEST_CASE("the cofactors fix a variable at zero and at one") {
		const h4 f = sample();
		CHECK( f->sub0(1) == x(2) );
		CHECK( f->sub1(1) == elem(1) );
		for_each_env([&](env m) {
			env m0 = m, m1 = m;
			m0[2] = b4::zero(), m1[2] = b4::one();
			CHECK( f->sub0(2)->eval(m) == sample_value(m0) );
			CHECK( f->sub1(2)->eval(m) == sample_value(m1) );
		});
	}

	TEST_CASE("the quantifiers join and meet the cofactors") {
		const h4 f = sample();
		CHECK( f->ex(1) == (f->sub0(1) | f->sub1(1)) );
		CHECK( f->all(1) == (f->sub0(1) & f->sub1(1)) );
		CHECK( f->ex(2) == (f->sub0(2) | f->sub1(2)) );
		CHECK( f->all(2) == (f->sub0(2) & f->sub1(2)) );
	}

	TEST_CASE("substitution puts a function in place of a variable") {
		const h4 f = sample();
		const h4 g = f->subst(2, x(1) | elem(2));
		for_each_env([&](env m) {
			env s = m;
			s[2] = m.at(1) | b4{ 2 };
			CHECK( g->eval(m) == sample_value(s) );
		});
	}

	TEST_CASE("eliminating every variable meets or joins the leaves") {
		const h4 f = x(1) & elem(1);
		CHECK( f->get_eelim() == b4{ 1 } );
		CHECK( f->get_uelim() == b4::zero() );
		const h4 g = x(1) | elem(1);
		CHECK( g->get_eelim() == b4::one() );
		CHECK( g->get_uelim() == b4{ 1 } );
	}

	TEST_CASE("a zero witness makes the function zero") {
		// x1 x2 a is zero at x1 = 0; x1 a | x1' x2 needs x2 below a'.
		for (const h4& f : { sample(), x(1) & x(2) & elem(1),
			x(1) ^ x(2), (x(1) | elem(1)) & (x(2) ^ elem(2)) })
		{
			auto z = f->get_one_zero();
			REQUIRE( z.has_value() );
			env m = z.value();
			m.emplace(1, b4::zero()), m.emplace(2, b4::zero());
			CHECK( f->eval(m) == b4::zero() );
		}
	}

	TEST_CASE("a function that is zero nowhere has no zero witness") {
		CHECK_FALSE( elem(1)->get_one_zero().has_value() );
		CHECK_FALSE( (x(1) | elem(1))->get_one_zero().has_value() );
		CHECK_FALSE( bdd_handle<b4>::one()->get_one_zero().has_value() );
	}

	TEST_CASE("the general solution solves the equation for every parameter") {
		for (const h4& f : { sample(), x(1) ^ x(2),
			(x(1) | elem(1)) & (x(2) ^ elem(2)) })
		{
			auto s = f->lgrs();
			REQUIRE( s.has_value() );
			CHECK( f->compose(s.value())->is_zero() );
		}
	}
}
