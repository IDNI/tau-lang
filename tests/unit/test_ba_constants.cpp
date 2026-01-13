// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"

TEST_SUITE("Bool BA") {
	TEST_CASE("Bool") {
		tref t_ref = tau::get_ba_constant(Bool(true),  bool_type());
		tref f_ref = tau::get_ba_constant(Bool(false), bool_type());
		size_t t = tau::get(t_ref).get_ba_constant_id();
		size_t f = tau::get(f_ref).get_ba_constant_id();
		CHECK( t == 1 );
		CHECK( f == 2 );
		CHECK( bac::get(t) != bac::get(f) );
		CHECK( bac::get(t) == variant<bv, Bool>(Bool(true)) );
		CHECK( bac::get(t) != variant<bv, Bool>(Bool(false)) );
		CHECK( bac::get(f) == variant<bv, Bool>(Bool(false)) );
		CHECK( bac::get(f) != variant<bv, Bool>(Bool(true)) );
	}
}

struct ba_constants_fixture {
	tref true_binding, false_binding;

	ba_constants_fixture() :

	true_binding( tau::get_ba_constant(Bool(true),  bool_type())),
	false_binding(tau::get_ba_constant(Bool(false), bool_type()))

	{}
};

TEST_SUITE("constants from factory") {

	TEST_CASE_FIXTURE(ba_constants_fixture,
		"binding: given one statement with one non-matching binding, "
		"the binding process returns the original statement.")
	{
		const char* sample = "{ some_source_code } : nonbool.";
		tref t = tau::get(sample);
		CHECK( t == nullptr );
	}
}

namespace idni::tau_lang {

struct sbf_ba_Bool_constants_fixture {
	sbf_ba _0, _1, _1_and_1, _0_and_1, _0_and_0,
			_1_or_1, _0_or_1, _0_or_0;
	sbf_ba sbf_t, sbf_f;
	template <typename BA>
	BA get_sbf_Bool(const std::string& src, const std::string&) {
		if (auto opt = parse_sbf<bv, sbf_ba>(src); opt)
			return std::get<BA>(opt.value().first);
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

} // namespace idni::tau_lang

TEST_SUITE("sbf_ba and Bool BAs") {
	TEST_CASE_FIXTURE(sbf_ba_Bool_constants_fixture, "sbf_ba, Bool") {
		using node = node<bv, sbf_ba, Bool>;
		using tau = tree<node>;
		using bac = ba_constants<node>;

		tref t_ref = tau::get_ba_constant(Bool(true),  bool_type());
		tref f_ref = tau::get_ba_constant(Bool(false), bool_type());
		size_t t = tau::get(t_ref).get_ba_constant_id();
		size_t f = tau::get(f_ref).get_ba_constant_id();
		CHECK( t == 1 );
		CHECK( f == 2 );
		CHECK( bac::get(t) != bac::get(f) );
		CHECK( bac::get(t) == variant<bv, sbf_ba, Bool>(Bool(true)) );
		CHECK( bac::get(t) != variant<bv, sbf_ba, Bool>(Bool(false)) );
		CHECK( bac::get(f) == variant<bv, sbf_ba, Bool>(Bool(false)) );
		CHECK( bac::get(f) != variant<bv, sbf_ba, Bool>(Bool(true)) );

		t_ref = tau::get_ba_constant(sbf_t, sbf_type<node>());
		f_ref = tau::get_ba_constant(sbf_f, sbf_type<node>());
		t = tau::get(t_ref).get_ba_constant_id();
		f = tau::get(f_ref).get_ba_constant_id();
		CHECK( t == 3 );
		CHECK( f == 4 );
		CHECK( bac::get(t) != bac::get(f) );
		CHECK( bac::get(t) == variant<bv, sbf_ba, Bool>(sbf_t) );
		CHECK( bac::get(t) != variant<bv, sbf_ba, Bool>(sbf_f) );
		CHECK( bac::get(f) == variant<bv, sbf_ba, Bool>(sbf_f) );
		CHECK( bac::get(f) != variant<bv, sbf_ba, Bool>(sbf_t) );
	}
}