// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

TEST_SUITE("Bool BA") {
	TEST_CASE("Bool") {
		tref t_ref = tau::get_ba_constant(Bool(true),  "bool");
		tref f_ref = tau::get_ba_constant(Bool(false), "bool");
		size_t t = tau::get(t_ref).get_ba_constant_id();
		size_t f = tau::get(f_ref).get_ba_constant_id();
		CHECK( t == 1 );
		CHECK( f == 2 );
		CHECK( bac::get(t) != bac::get(f) );
		CHECK( bac::get(t) == variant<Bool>(Bool(true)) );
		CHECK( bac::get(t) != variant<Bool>(Bool(false)) );
		CHECK( bac::get(f) == variant<Bool>(Bool(false)) );
		CHECK( bac::get(f) != variant<Bool>(Bool(true)) );
	}
}

struct ba_constants_fixture {
	tref true_binding, false_binding;
	tau::get_options parse_lib, parse_lib_with_named;

	ba_constants_fixture() :

	true_binding( tau::get_ba_constant(Bool(true),  "bool")),
	false_binding(tau::get_ba_constant(Bool(false), "bool")),
	parse_lib(parse_library()),
	parse_lib_with_named(parse_library())

	{
		parse_lib_with_named.named_constants = {
			{ "true_binding",  true_binding  },
			{ "false_binding", false_binding }};
		parse_lib_with_named.infer_ba_types = true;
		parse_lib.infer_ba_types = true;
	}
};

TEST_SUITE("named constants") {

	// TEST_CASE_FIXTURE(ba_constants_fixture,
	// 	"binding: given one statement with no bindigns, the binding "
	// 	"process returns the same statement.")
	// {
	// 	const char* sample = "$X := $X.";
	// 	tref t1 = tau::get(sample, parse_lib);
	// 	tref t2 = tau::get(sample, parse_lib_with_named);
	// 	CHECK( t1 == t2 );
	// }

	// TEST_CASE_FIXTURE(ba_constants_fixture,
	// 	"binding: given one statement with one binding, the binding "
	// 	"process returns the statement with the binding replaced.")
	// {
	// 	const char* sample = "{ true_binding } := { true_binding }.";
	// 	tref bound = tau::get(sample, parse_lib_with_named);
	// 	if (bound) tau::get(bound).dump(std::cout);
	// 	if (bound) tau::get(bound).print_tree(std::cout << "result: ") << "\n";
	// 	auto rul = tt(bound) | tau::rules | tau::rule | tau::bf_rule;
	// 	auto c1 = rul | tau::bf_matcher | tau::bf | tt::Tree;
	// 	auto c2 = rul | tau::bf_body | tau::bf | tt::Tree;
	// 	CHECK(c1 == c2);
	// }

	// TEST_CASE_FIXTURE(ba_constants_fixture,
	// 	"binding: given one statement with one non-matching binding, "
	// 	"the binding process returns the original statement.")
	// {
	// 	const char* sample = "{ nonmatching } := { nonmatching }.";
	// 	tref bound = tau::get(sample, parse_lib_with_named);
	// 	CHECK( bound == nullptr );
	// }
}

TEST_SUITE("constants from factory") {

	// TEST_CASE_FIXTURE(ba_constants_fixture,
	// 	"binding: given one statement with no bindigns, the binding "
	// 	"process returns the same statement.")
	// {
	// 	const char* sample = "$X := $X.";
	// 	tref t = tau::get(sample, parse_lib);
	// 	CHECK( tau::get(t).is(tau::library) );
	// }

	// TEST_CASE_FIXTURE(ba_constants_fixture,
	// 	"binding: given one statement with one binding, the binding "
	// 	"process returns the statement with the binding replaced.")
	// {
	// 	const char* sample = "$X := { true } : bool.";
	// 	tref t = tau::get(sample, parse_lib);
	// 	CHECK( tau::get(t).is(tau::library) );
	// }

	TEST_CASE_FIXTURE(ba_constants_fixture,
		"binding: given one statement with one non-matching binding, "
		"the binding process returns the original statement.")
	{
		const char* sample = "$X := { some_source_code } : nonbool.";
		tref t = tau::get(sample, parse_lib);
		CHECK( t == nullptr );
	}
}

namespace idni::tau_lang {

// nso_factory for tests with <sbf_ba, Bool>
template<>
struct nso_factory<sbf_ba, Bool> {
	using node = tau_lang::node<sbf_ba, Bool>;

	optional<constant_with_type<sbf_ba, Bool>> parse_sbf_ba(
		const string& constant_source) const
	{
		auto r = sbf_ba_factory<sbf_ba, Bool>::instance()
						.parse(constant_source);
		if (!r) return {};
		return constant_with_type<sbf_ba, Bool>{
			std::get<sbf_ba>(r.value().first), "sbf" };
	}

	optional<constant_with_type<sbf_ba, Bool>> parse_Bool(
		const string& constant_source) const
	{
		auto r = nso_factory<Bool>::instance()
						.parse(constant_source, "bool");
		if (!r) return {};
		return constant_with_type<sbf_ba, Bool>{
			std::get<Bool>(r.value().first), "bool" };
	}

	optional<constant_with_type<sbf_ba, Bool>> parse(
		const string& constant_source, const string type_name) const
	{
		if (type_name == "sbf")
			return parse_sbf_ba(constant_source);
		else    return parse_Bool(constant_source);
	}

	vector<string> types() const { return { "sbf", "bool" }; }

	string default_type() const { return "sbf"; }

	string one(const string) const { throw logic_error("not implemented"); }

	string zero(const string) const { throw logic_error("not implemented");}

	tref splitter_one(const string) const {
		throw logic_error("not implemented");
	}

	// There is no tau_ba
	tref unpack_tau_ba(const variant<Bool>&) const { return nullptr; }

	std::variant<sbf_ba, Bool> pack_tau_ba (const tref) const {return {};}

	static nso_factory<sbf_ba, Bool>& instance() {
		static nso_factory<sbf_ba, Bool> factory;
		return factory;
	}
private:
	nso_factory() {};
};

struct sbf_ba_Bool_constants_fixture {
	sbf_ba _0, _1, _1_and_1, _0_and_1, _0_and_0,
			_1_or_1, _0_or_1, _0_or_0;
	sbf_ba sbf_t, sbf_f;
	template <typename BA>
	BA get_sbf_Bool(const std::string& src, const std::string&) {
		if (auto opt = sbf_ba_factory<sbf_ba>::instance().parse(src);
			opt) return std::get<BA>(opt.value().first);
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
		using node = node<sbf_ba, Bool>;
		using tau = tree<node>;
		using bac = ba_constants<node>;

		tref t_ref = tau::get_ba_constant(Bool(true),  "bool");
		tref f_ref = tau::get_ba_constant(Bool(false), "bool");
		size_t t = tau::get(t_ref).get_ba_constant_id();
		size_t f = tau::get(f_ref).get_ba_constant_id();
		CHECK( t == 1 );
		CHECK( f == 2 );
		CHECK( bac::get(t) != bac::get(f) );
		CHECK( bac::get(t) == variant<sbf_ba, Bool>(Bool(true)) );
		CHECK( bac::get(t) != variant<sbf_ba, Bool>(Bool(false)) );
		CHECK( bac::get(f) == variant<sbf_ba, Bool>(Bool(false)) );
		CHECK( bac::get(f) != variant<sbf_ba, Bool>(Bool(true)) );

		t_ref = tau::get_ba_constant(sbf_t, "sbf");
		f_ref = tau::get_ba_constant(sbf_f, "sbf");
		t = tau::get(t_ref).get_ba_constant_id();
		f = tau::get(f_ref).get_ba_constant_id();
		CHECK( t == 3 );
		CHECK( f == 4 );
		CHECK( bac::get(t) != bac::get(f) );
		CHECK( bac::get(t) == variant<sbf_ba, Bool>(sbf_t) );
		CHECK( bac::get(t) != variant<sbf_ba, Bool>(sbf_f) );
		CHECK( bac::get(f) == variant<sbf_ba, Bool>(sbf_f) );
		CHECK( bac::get(f) != variant<sbf_ba, Bool>(sbf_t) );
	}
}