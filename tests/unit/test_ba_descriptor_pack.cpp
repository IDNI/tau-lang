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
using conv_node = node<tau_ba<sbf_ba, qint, qlt, hsb, nlang_ba>, sbf_ba, qint, qlt, hsb, nlang_ba>;
using conv_dispatcher =
	base_ba_dispatcher<tau_ba<sbf_ba, qint, qlt, hsb, nlang_ba>, sbf_ba, qint, qlt, hsb, nlang_ba>;

static_assert(assert_pack_descriptors_complete<conv_node>(),
	"a converted BA's descriptor is incomplete");

template struct base_ba_dispatcher<tau_ba<sbf_ba, qint, qlt, hsb, nlang_ba>, sbf_ba, qint, qlt, hsb, nlang_ba>;
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
			== std::vector<std::string>{ "tau", "sbf", "qint", "qlt", "hsb", "nlang" } );
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

	TEST_CASE("qlt is classified as a non-aba omcat, unlike qint") {
		CHECK_FALSE( ba_descriptor<qlt, conv_node>::atomless );
		CHECK( ba_descriptor<qlt, conv_node>::non_aba_omcat );
		CHECK( ba_descriptor<qint, conv_node>::atomless );
		CHECK_FALSE( ba_descriptor<qint, conv_node>::non_aba_omcat );
	}
}
