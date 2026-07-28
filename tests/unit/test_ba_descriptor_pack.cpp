// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Exercises the generic, descriptor-driven dispatcher.
//
// `node<tau_ba<sbf_ba>, sbf_ba>` is the smallest pack whose BAs all have a
// ba_descriptor and for which no explicit base_ba_dispatcher specialization
// exists, so it is the only pack that reaches the primary template in
// base_ba_dispatcher.tmpl.h.  The default build takes a specialization and
// never instantiates that code; without this file a break in it surfaces only
// in a reduced-pack or out-of-tree-BA build.
//
// The explicit instantiations below are the substance -- they force every
// member of the generic dispatcher and the descriptor-driven constant parser
// to compile and link for a descriptor-complete pack.

#include "test_init.h"
#include "tau.h"

namespace idni::tau_lang {

using mini_node = node<tau_ba<sbf_ba>, sbf_ba>;
using mini_dispatcher = base_ba_dispatcher<tau_ba<sbf_ba>, sbf_ba>;

static_assert(assert_pack_descriptors_complete<mini_node>(),
	"sbf/tau descriptors incomplete for the minimal pack");

template struct base_ba_dispatcher<tau_ba<sbf_ba>, sbf_ba>;
template std::optional<typename mini_node::constant_with_type>
ba_constants<mini_node>::get(const std::string&, tref, const std::string);

// Every converted BA joins this pack, so its descriptor is type-checked by the
// default build rather than only by a reduced-pack configure.  A descriptor no
// pack names is never instantiated, and compiles as text however broken it is.
// Add each newly converted BA here and extend the expected types() below.
using conv_node = node<tau_ba<sbf_ba, qint, qlt, hsb, nlang_ba, bv>, sbf_ba, qint, qlt, hsb, nlang_ba, bv>;
using conv_dispatcher =
	base_ba_dispatcher<tau_ba<sbf_ba, qint, qlt, hsb, nlang_ba, bv>, sbf_ba, qint, qlt, hsb, nlang_ba, bv>;

static_assert(assert_pack_descriptors_complete<conv_node>(),
	"a converted BA's descriptor is incomplete");

template struct base_ba_dispatcher<tau_ba<sbf_ba, qint, qlt, hsb, nlang_ba, bv>, sbf_ba, qint, qlt, hsb, nlang_ba, bv>;
template std::optional<typename conv_node::constant_with_type>
ba_constants<conv_node>::get(const std::string&, tref, const std::string);

} // namespace idni::tau_lang

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("generic dispatcher over a descriptor-complete pack") {

	TEST_CASE("types() folds the descriptors in pack order") {
		CHECK( mini_dispatcher::types()
			== std::vector<std::string>{ "tau", "sbf" } );
	}

	TEST_CASE("default_type() resolves by descriptor priority") {
		tref t = mini_dispatcher::default_type();
		REQUIRE(t != nullptr);
		CHECK( ba_descriptor<tau_ba<sbf_ba>, mini_node>::matches_type(t) );
	}
}

TEST_SUITE("generic dispatcher over the converted-BA pack") {

	TEST_CASE("types() folds the descriptors in pack order") {
		CHECK( conv_dispatcher::types()
			== std::vector<std::string>{ "tau", "sbf", "qint", "qlt", "hsb", "nlang", "bv" } );
	}

	TEST_CASE("the base BAs' priorities leave the default type to tau") {
		tref t = conv_dispatcher::default_type();
		REQUIRE(t != nullptr);
		CHECK( ba_descriptor<tau_ba<sbf_ba, qint, qlt, hsb, nlang_ba>, conv_node>
			::matches_type(t) );
	}

	TEST_CASE("one/zero route to the qint descriptor's literals") {
		tref t = qint_type<conv_node>();
		REQUIRE(t != nullptr);
		CHECK( conv_dispatcher::one(t) == "top" );
		CHECK( conv_dispatcher::zero(t) == "bot" );
	}

	TEST_CASE("one/zero route to the qlt descriptor's literals") {
		tref t = qlt_type<conv_node>();
		REQUIRE(t != nullptr);
		CHECK( conv_dispatcher::one(t) == "top" );
		CHECK( conv_dispatcher::zero(t) == "bot" );
	}

	TEST_CASE("one/zero route to the hsb descriptor's literals") {
		tref t = hsb_type<conv_node>();
		REQUIRE(t != nullptr);
		CHECK( conv_dispatcher::one(t) == "top" );
		CHECK( conv_dispatcher::zero(t) == "bot" );
	}

	TEST_CASE("one/zero route to the nlang descriptor's literals") {
		tref t = nlang_type<conv_node>();
		REQUIRE(t != nullptr);
		CHECK( conv_dispatcher::one(t) == "everything" );
		CHECK( conv_dispatcher::zero(t) == "nothing" );
	}

	TEST_CASE("bv is the one parameterized type: subtype round-trips") {
		tref t8  = ba_descriptor<bv, conv_node>::type_tree_for(8);
		tref t16 = ba_descriptor<bv, conv_node>::type_tree_for(16);
		REQUIRE(t8 != nullptr);
		REQUIRE(t16 != nullptr);
		CHECK( t8 != t16 );
		CHECK( ba_descriptor<bv, conv_node>::type_param(t8)
			== std::optional<unsigned short>(8) );
		CHECK( ba_descriptor<bv, conv_node>::type_param(t16)
			== std::optional<unsigned short>(16) );
		// every other BA is unparameterized and yields nullopt
		CHECK( ba_descriptor<qint, conv_node>::type_param(qint_type<conv_node>())
			== std::nullopt );
	}

	TEST_CASE("bv literals are width-dependent, unlike every other BA's") {
		CHECK( conv_dispatcher::one(ba_descriptor<bv, conv_node>::type_tree_for(8))
			!= conv_dispatcher::one(
				ba_descriptor<bv, conv_node>::type_tree_for(16)) );
	}

	TEST_CASE("an unowned type falls back to the Boolean carrier's literals") {
		// bf_t/bf_f carry the untyped type; the per-pack chains answered such a
		// type from their trailing clause, which returned sbf's "1"/"0".
		tref t = untyped_type<conv_node>();
		REQUIRE(t != nullptr);
		CHECK( conv_dispatcher::one(t) == "1" );
		CHECK( conv_dispatcher::zero(t) == "0" );
	}

	TEST_CASE("qlt is classified as a non-aba omcat, unlike qint") {
		CHECK_FALSE( ba_descriptor<qlt, conv_node>::atomless );
		CHECK( ba_descriptor<qlt, conv_node>::non_aba_omcat );
		CHECK( ba_descriptor<qint, conv_node>::atomless );
		CHECK_FALSE( ba_descriptor<qint, conv_node>::non_aba_omcat );
	}
}
