// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "test_helpers.h"

using bac = ba_constants<Bool>;
using bacb = ba_constants_binder<Bool>;
using bac_sbf_Bool = ba_constants<sbf_ba, Bool>;
using bacb_sbf_Bool = ba_constants_binder<sbf_ba, Bool>;
using btree_sbf_Bool = tree<node<sbf_ba, Bool>>;

struct sbf_ba_Bool_constants_fixture {
	sbf_ba _0, _1, _1_and_1, _0_and_1, _0_and_0,
			_1_or_1, _0_or_1, _0_or_0;
	sbf_ba sbf_t, sbf_f;
	template <typename BA>
	BA get_sbf_Bool(const std::string& src, const std::string&) {
		if (auto opt = sbf_ba_factory<sbf_ba>::instance().parse(src);
				opt.has_value())
			return std::get<BA>(opt.value());
		assert(false);
		return BA();
	}
	sbf_ba_Bool_constants_fixture() {
		bdd_init<Bool>();
		sbf_t = bdd_handle<Bool>::htrue;
		sbf_f = bdd_handle<Bool>::hfalse;
		_0       = get_sbf_Bool<sbf_ba>("0", "sbf");
		_1       = get_sbf_Bool<sbf_ba>("1", "sbf");
		_1_and_1 = get_sbf_Bool<sbf_ba>("1&1", "sbf");
		_0_and_1 = get_sbf_Bool<sbf_ba>("0&1", "sbf");
		_0_and_0 = get_sbf_Bool<sbf_ba>("0&0", "sbf");
		_1_or_1  = get_sbf_Bool<sbf_ba>("1|1", "sbf");
		_0_or_1  = get_sbf_Bool<sbf_ba>("0|1", "sbf");
		_0_or_0  = get_sbf_Bool<sbf_ba>("0|0", "sbf");
	}
};

// std::variant<Bool> bv(Bool x) { return x; }

TEST_SUITE("Bool boolean algebra") {

	TEST_CASE("Bool") {
		tref t_ref = bacb::instance().bind(Bool(true),  "bool");
		tref f_ref = bacb::instance().bind(Bool(false), "bool");
		size_t t = btree::get(t_ref).get_ba_constant_id();
		size_t f = btree::get(f_ref).get_ba_constant_id();
		CHECK( t == 1 );
		CHECK( f == 2 );
		CHECK( bac::is<Bool>(t) );
		CHECK( bac::is<Bool>(f) );
		CHECK( bac::get(t) != bac::get(f) );
		CHECK( bac::get(t) == variant<Bool>(Bool(true)) );
		CHECK( bac::get(t) != variant<Bool>(Bool(false)) );
		CHECK( bac::get(f) == variant<Bool>(Bool(false)) );
		CHECK( bac::get(f) != variant<Bool>(Bool(true)) );
		CHECK( bac::get<Bool>(t) == Bool(true) );
		CHECK( bac::get<Bool>(f) == Bool(false) );
	}

	TEST_CASE_FIXTURE(sbf_ba_Bool_constants_fixture, "sbf_ba_Bool") {

		tref t_ref = bacb_sbf_Bool::instance().bind(Bool(true),  "bool");
		tref f_ref = bacb_sbf_Bool::instance().bind(Bool(false), "bool");
		size_t t = btree_sbf_Bool::get(t_ref).get_ba_constant_id();
		size_t f = btree_sbf_Bool::get(f_ref).get_ba_constant_id();


		CHECK( t == 1 );
		CHECK( f == 2 );
		CHECK( bac_sbf_Bool::is<Bool>(t) );
		CHECK( bac_sbf_Bool::is<Bool>(f) );
		CHECK( !bac_sbf_Bool::is<sbf_ba>(t) );
		CHECK( !bac_sbf_Bool::is<sbf_ba>(f) );
		CHECK( bac_sbf_Bool::get(t) != bac_sbf_Bool::get(f) );
		CHECK( bac_sbf_Bool::get(t) == variant<sbf_ba, Bool>(Bool(true)) );
		CHECK( bac_sbf_Bool::get(t) != variant<sbf_ba, Bool>(Bool(false)) );
		CHECK( bac_sbf_Bool::get(f) == variant<sbf_ba, Bool>(Bool(false)) );
		CHECK( bac_sbf_Bool::get(f) != variant<sbf_ba, Bool>(Bool(true)) );
		CHECK( bac_sbf_Bool::get<Bool>(t) == Bool(true) );
		CHECK( bac_sbf_Bool::get<Bool>(f) == Bool(false) );

		t_ref = bacb_sbf_Bool::instance().bind(sbf_t, "sbf");
		f_ref = bacb_sbf_Bool::instance().bind(sbf_f, "sbf");
		t = btree_sbf_Bool::get(t_ref).get_ba_constant_id();
		f = btree_sbf_Bool::get(f_ref).get_ba_constant_id();

		CHECK( t == 3 );
		CHECK( f == 4 );
		CHECK( bac_sbf_Bool::is<sbf_ba>(t) );
		CHECK( bac_sbf_Bool::is<sbf_ba>(f) );
		CHECK( !bac_sbf_Bool::is<Bool>(t) );
		CHECK( !bac_sbf_Bool::is<Bool>(f) );
		CHECK( bac_sbf_Bool::get(t) != bac_sbf_Bool::get(f) );
		CHECK( bac_sbf_Bool::get(t) == variant<sbf_ba, Bool>(sbf_t) );
		CHECK( bac_sbf_Bool::get(t) != variant<sbf_ba, Bool>(sbf_f) );
		CHECK( bac_sbf_Bool::get(f) == variant<sbf_ba, Bool>(sbf_f) );
		CHECK( bac_sbf_Bool::get(f) != variant<sbf_ba, Bool>(sbf_t) );
		CHECK( bac_sbf_Bool::get<sbf_ba>(t) == sbf_t );
		CHECK( bac_sbf_Bool::get<sbf_ba>(f) == sbf_f );
	}
}