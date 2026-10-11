// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Exercises the generic, descriptor-driven dispatcher over the smallest
// descriptor-complete pack, `node<tau_ba<sbf_ba>, sbf_ba>`. This file needs
// only sbf and tau, so it builds and runs in every pack that has them; the
// checks over the full converted-BA pack live in
// test_ba_descriptor_pack_full.cpp, which needs every algebra.
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
template result<typename mini_node::constant_with_type>
ba_constants<mini_node>::get(const std::string&, tref, const std::string);

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
