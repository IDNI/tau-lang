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

// Each converted BA joins a pack here, so its descriptor is type-checked by
// the default build rather than only by a reduced-pack configure.
using qint_node = node<tau_ba<sbf_ba, qint>, sbf_ba, qint>;
using qint_dispatcher = base_ba_dispatcher<tau_ba<sbf_ba, qint>, sbf_ba, qint>;

static_assert(assert_pack_descriptors_complete<qint_node>(),
	"qint descriptor incomplete");

template struct base_ba_dispatcher<tau_ba<sbf_ba, qint>, sbf_ba, qint>;
template std::optional<typename qint_node::constant_with_type>
ba_constants<qint_node>::get(const std::string&, tref, const std::string);

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

TEST_SUITE("generic dispatcher over a pack containing qint") {

	TEST_CASE("types() folds the descriptors in pack order") {
		CHECK( qint_dispatcher::types()
			== std::vector<std::string>{ "tau", "sbf", "qint" } );
	}

	TEST_CASE("qint's priority leaves the default type to tau") {
		tref t = qint_dispatcher::default_type();
		REQUIRE(t != nullptr);
		CHECK( ba_descriptor<tau_ba<sbf_ba, qint>, qint_node>
			::matches_type(t) );
	}

	TEST_CASE("one/zero route to the qint descriptor's literals") {
		tref t = qint_type<qint_node>();
		REQUIRE(t != nullptr);
		CHECK( qint_dispatcher::one(t) == "top" );
		CHECK( qint_dispatcher::zero(t) == "bot" );
	}
}
