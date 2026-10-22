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

	TEST_CASE("type_names() spells the descriptors' names in pack order") {
		constexpr auto names = mini_dispatcher::type_names();
		CHECK( names.size() == 2 );
		CHECK( names[0] == "tau" );
		CHECK( names[1] == "sbf" );
	}

	TEST_CASE("default_type() resolves by descriptor priority") {
		tref t = mini_dispatcher::default_type();
		REQUIRE(t != nullptr);
		CHECK( ba_descriptor<tau_ba<sbf_ba>, mini_node>::matches_type(t) );
	}
}

TEST_SUITE("the Bool descriptor over the minimal pack") {

	TEST_CASE("is_one, is_zero, normalize and simplify_term answer for Bool") {
		using D = ba_descriptor<Bool, mini_node>;
		CHECK( D::is_one(Bool(true)).value() );
		CHECK( !D::is_one(Bool(false)).value() );
		CHECK( D::is_zero(Bool(false)).value() );
		CHECK( !D::is_zero(Bool(true)).value() );
		CHECK( D::normalize(Bool(true)).value().is_one() );
		CHECK( D::normalize(Bool(false)).value().is_zero() );
		CHECK( D::simplify_term(nullptr).value() == nullptr );
	}

	TEST_CASE("a Bool prints as 1 or 0") {
		std::ostringstream os;
		os << Bool(true) << Bool(false);
		CHECK( os.str() == "10" );
	}
}

TEST_SUITE("Bool constants and the constant source text") {

	using bool_node = node<Bool>;

	TEST_CASE("a Bool literal parses to its value, another text is an error") {
		using D = ba_descriptor<Bool, bool_node>;
		for (const char* src : { "0", "false", "F" }) {
			auto c = D::parse(src, nullptr);
			REQUIRE( c.has_value() );
			CHECK( std::get<Bool>(c.value().first).is_zero() );
		}
		for (const char* src : { "1", "true", "T" }) {
			auto c = D::parse(src, nullptr);
			REQUIRE( c.has_value() );
			CHECK( std::get<Bool>(c.value().first).is_one() );
		}
		CHECK( D::parse("maybe", nullptr).has_error() );
	}

	TEST_CASE("a BA without a splitter splits an element into itself") {
		auto s = base_ba_dispatcher<Bool>::splitter(
			std::variant<Bool>{ Bool(true) }, splitter_type::upper);
		REQUIRE( s.has_value() );
		CHECK( std::get<Bool>(s.value()).is_one() );
	}

	TEST_CASE("the source of a constant loses its braces, blanks and quotes") {
		CHECK( strip_ba_constant_source(" { a b } ") == "a b" );
		CHECK( strip_ba_constant_source("{ \"a b\" }", true) == "a b" );
		CHECK( strip_ba_constant_source("{ 'a b' }", true) == "a b" );
		CHECK( strip_ba_constant_source("{ 'a b' }") == "'a b'" );
	}
}
