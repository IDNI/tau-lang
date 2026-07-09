// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// BA-3: dedicated unit tests for variant_ba's mixed-alternative behavior.
//
// variant_ba.h lifts the BA operators (&, |, ^, +, ~, == bool) to
// std::variant<BAs...> by dispatching to the currently-active alternative.
// Per review finding BA-12, mismatched alternatives are expected to throw
// std::logic_error("wrong types") by design -- this file exercises both the
// same-alternative success path and the throws-on-mismatch path.
//
// variant_ba's operators require BAsPack<BAs...>, which in turn requires
// cvc5::Term (aliased as `bv`) to be literally one of the alternatives
// (see tau_tree.h's BAsPack concept), so the variant under test must be
// std::variant<bv, sbf_ba> -- the same pack the real base_ba_dispatcher<bv,
// sbf_ba> specialization (base_ba_dispatcher.tmpl.h) is built around.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "boolean_algebras/variant_ba.h"
#include "boolean_algebras/cvc5/cvc5.h"

namespace {

using V = std::variant<bv, sbf_ba>;

V make_bv(bool top) {
	return top ? make_bitvector_top_elem(4) : make_bitvector_bottom_elem(4);
}

V make_sbf(const char* src) {
	auto parsed = parse_sbf<bv, sbf_ba>(src);
	assert(parsed.has_value());
	return std::get<sbf_ba>(parsed.value().first);
}

// bv (cvc5::Term) operators build a new symbolic term (e.g. "bvand x y")
// rather than eagerly computing a concrete value, so raw structural
// equality against a bitvector constant only holds after running cvc5's
// simplifier (normalize_bv, as used by base_ba_dispatcher::normalize).
bool bv_equals(const V& v, bool top) {
	return normalize_bv(std::get<bv>(v))
		== (top ? make_bitvector_top_elem(4) : make_bitvector_bottom_elem(4));
}

} // namespace

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("variant boolean algebra") {

	TEST_CASE("same-alternative operators dispatch to the active bv type") {
		V t = make_bv(true), f = make_bv(false);

		CHECK( std::holds_alternative<bv>(t & f) );
		CHECK( bv_equals(t & f, false) );
		CHECK( bv_equals(t & t, true) );

		CHECK( bv_equals(t | f, true) );
		CHECK( bv_equals(f | f, false) );

		CHECK( bv_equals(t ^ f, true) );
		CHECK( bv_equals(t ^ t, false) );

		// operator+ is defined as a synonym for operator^
		CHECK( bv_equals(t + f, true) );
		CHECK( bv_equals(t + t, false) );

		CHECK( bv_equals(~t, false) );
		CHECK( bv_equals(~f, true) );
	}

	TEST_CASE("same-alternative operators work for the sbf_ba alternative too") {
		V a = make_sbf("1"), b = make_sbf("0");

		CHECK( std::holds_alternative<sbf_ba>(a & b) );
		CHECK( (a & b) == b );
		CHECK( (a | b) == a );
		CHECK( (a ^ a) == b );
		CHECK( (~a) == b );
	}

	TEST_CASE("bool comparisons dispatch to the active alternative") {
		V t = make_bv(true), f = make_bv(false);

		CHECK( t == true );
		CHECK( f == false );
		CHECK( true == t );
		CHECK( false == f );

		CHECK( t != false );
		CHECK( f != true );
		CHECK( false != t );
		CHECK( true != f );

		V s1 = make_sbf("1"), s0 = make_sbf("0");
		CHECK( s1 == true );
		CHECK( s0 == false );
	}

	TEST_CASE("mismatched alternatives throw std::logic_error (BA-12)") {
		V a = make_bv(true);
		V b = make_sbf("1");

		CHECK_THROWS_AS( a & b, std::logic_error );
		CHECK_THROWS_AS( a | b, std::logic_error );
		CHECK_THROWS_AS( a ^ b, std::logic_error );
		CHECK_THROWS_AS( a + b, std::logic_error );

		// order shouldn't matter
		CHECK_THROWS_AS( b & a, std::logic_error );
		CHECK_THROWS_AS( b | a, std::logic_error );
	}
}
