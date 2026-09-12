// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Every converted BA joins one pack here, so each descriptor is type-checked
// by the default build rather than only by a reduced-pack configure: a
// descriptor no pack names is never instantiated, and compiles as text
// however broken it is. Needs every algebra, so a reduced pack skips this
// file and runs test_ba_descriptor_pack.cpp's minimal-pack half instead.
// Add each newly converted BA here and extend the expected types() below.

#include "test_init.h"
#include "tau.h"

namespace idni::tau_lang {

// The smallest descriptor-complete pack, the foil the per-pack checks below
// compare against; test_ba_descriptor_pack.cpp exercises it on its own.
using mini_node = node<tau_ba<sbf_ba>, sbf_ba>;
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
		// every other BA is unparameterized and declares none of the trio
		static_assert(!ba_has_type_tree_for<conv_node, qint>);
		static_assert(!ba_has_type_tree_for<conv_node, sbf_ba>);
	}

	TEST_CASE("bv literals are width-dependent, unlike every other BA's") {
		CHECK( conv_dispatcher::one(ba_descriptor<bv, conv_node>::type_tree_for(8))
			!= conv_dispatcher::one(
				ba_descriptor<bv, conv_node>::type_tree_for(16)) );
	}

	TEST_CASE("an unowned type falls back to the carrier's own literals") {
		// bf_t/bf_f carry the untyped type; the per-pack chains answered such a
		// type from their trailing clause, which returned sbf's "1"/"0".  The
		// carrier here is bv[1], whose literals are the same two strings -- it
		// is bv[16] that would answer "65535".
		tref t = untyped_type<conv_node>();
		REQUIRE(t != nullptr);
		CHECK( conv_dispatcher::one(t) == "1" );
		CHECK( conv_dispatcher::zero(t) == "0" );
	}

	TEST_CASE("the Boolean carrier resolves per pack, from one configured order") {
		// TAU_BOOL_CARRIERS defaults to bv,sbf,bool: a pack holding bv gets
		// bv[1] whatever its pack order, and one without bv falls to sbf.
		CHECK( pack_bool_carrier_type<conv_node>()
			== ba_descriptor<bv, conv_node>::type_tree_for(1) );
		CHECK( pack_bool_carrier_type<mini_node>()
			== ba_descriptor<sbf_ba, mini_node>::type_tree() );
	}

	TEST_CASE("qlt is classified as a non-aba omcat, unlike qint") {
		CHECK_FALSE( ba_descriptor<qlt, conv_node>::atomless );
		CHECK( ba_descriptor<qlt, conv_node>::non_aba_omcat );
		CHECK( ba_descriptor<qint, conv_node>::atomless );
		CHECK_FALSE( ba_descriptor<qint, conv_node>::non_aba_omcat );
	}
}

TEST_SUITE("capability concepts name what each descriptor declares") {

	TEST_CASE("bv, sbf, qint, qlt, hsb, nlang, tau") {
		using N = conv_node;
		using tau_t = tau_ba<sbf_ba, qint, qlt, hsb, nlang_ba, bv>;
		static_assert(ba_has_solve<N, bv>);
		static_assert(!ba_has_solve<N, sbf_ba> && !ba_has_solve<N, qlt>);
		static_assert(ba_has_can_solve<N, bv> && ba_has_sat_status<N, bv>);
		static_assert(!ba_has_can_solve<N, qlt> && !ba_has_sat_status<N, qlt>);
		static_assert(ba_has_preprocess<N, bv> && ba_has_set_preprocessing<N, bv>);
		static_assert(!ba_has_preprocess<N, sbf_ba>);
		static_assert(ba_has_formula_is_preprocessable<N, bv>);
		static_assert(ba_has_preprocessing_residue<N, bv>);
		static_assert(ba_has_term_is_blasteable<N, bv>);
		static_assert(ba_has_set_charvar<N, sbf_ba> && !ba_has_set_charvar<N, bv>);
		static_assert(ba_has_component_factoring<N, tau_t>);
		static_assert(!ba_has_component_factoring<N, sbf_ba>);
		static_assert(ba_has_zero_constant<N, bv> && ba_has_zero_constant<N, qlt>);
		static_assert(!ba_has_zero_constant<N, sbf_ba>);
		static_assert(ba_has_value_constant<N, bv> && ba_has_value_constant<N, sbf_ba>);
		static_assert(!ba_has_value_constant<N, qlt>);
		static_assert(ba_has_bool_carrier_type<N, bv> && !ba_has_bool_carrier_type<N, sbf_ba>);
		static_assert(ba_has_omcat_qe<N, qlt> && ba_has_semantic_pwr<N, qlt>);
		static_assert(!ba_has_omcat_qe<N, bv> && !ba_has_semantic_pwr<N, bv>);
		static_assert(ba_has_codegen_witness<N, bv> && ba_has_codegen_witness<N, qlt>);
		static_assert(!ba_has_codegen_witness<N, sbf_ba>);
		static_assert(ba_has_codegen_constant_expr<N, sbf_ba>);
		static_assert(ba_has_codegen_constant_expr<N, bv> && ba_has_codegen_constant_expr<N, qlt>);
		static_assert(!ba_has_codegen_constant_expr<N, hsb>);
		static_assert(ba_has_literal_incomplete<N, bv> && ba_has_literal_incomplete<N, sbf_ba>);
		static_assert(ba_has_literal_incomplete<N, qint> && ba_has_literal_incomplete<N, hsb>);
		static_assert(!ba_has_literal_incomplete<N, tau_t>);
		static_assert(ba_has_print_constant<N, bv> && !ba_has_print_constant<N, sbf_ba>);
		static_assert(ba_has_type_tree_for<N, bv>);
		static_assert(ba_arith_ops_v<N, bv> && !ba_arith_ops_v<N, sbf_ba>);
		static_assert(ba_can_host_bool_v<N, bv> && ba_can_host_bool_v<N, sbf_ba>);
		static_assert(!ba_can_host_bool_v<N, qlt>);
		static_assert(ba_uses_oracle_v<N, nlang_ba> && !ba_uses_oracle_v<N, bv>);
		static_assert(ba_output_always_satisfiable_v<N, nlang_ba>);
		static_assert(!ba_output_always_satisfiable_v<N, bv>);
		CHECK(true);
	}

	TEST_CASE("a type that is no BA declares nothing") {
		using N = conv_node;
		static_assert(!ba_has_solve<N, int> && !ba_has_literal_incomplete<N, int>);
		static_assert(!ba_arith_ops_v<N, int> && !ba_can_host_bool_v<N, int>);
		static_assert(!ba_uses_oracle_v<N, int>);
		static_assert(!ba_output_always_satisfiable_v<N, int>);
		CHECK(true);
	}
}

TEST_SUITE("carriers, solvers and the wrapper: what a pack resolves") {

	TEST_CASE("a Boolean carrier builds a plain value: Bool's value_constant") {
		using N = node<bv, Bool>;
		static_assert(ba_has_value_constant<N, Bool>);
		const size_t bid = ba_types<N>::id(ba_descriptor<Bool, N>::type_tree());
		tref one = pack_value_constant<N>(bid, 1);
		tref zero = pack_value_constant<N>(bid, 0);
		REQUIRE(one != nullptr);
		REQUIRE(zero != nullptr);
		CHECK(one != zero);
	}

	TEST_CASE("the arithmetic pipeline is on exactly when a BA has arith_ops and solve") {
		static_assert(pack_has_arithmetic_theory_v<conv_node>);
		static_assert(!pack_has_arithmetic_theory_v<mini_node>);
		static_assert(ba_has_arithmetic_theory_v<conv_node, bv>);
		static_assert(!ba_has_arithmetic_theory_v<conv_node, qlt>);
		static_assert(!ba_has_arithmetic_theory_v<conv_node, sbf_ba>);
		CHECK(true);
	}

	TEST_CASE("pack_solver_count counts the BAs declaring solve") {
		static_assert(pack_solver_count<conv_node>() == 1);
		static_assert(pack_solver_count<mini_node>() == 0);
		CHECK(true);
	}

	TEST_CASE("exactly one BA may claim propositional synthesis") {
		static_assert(pack_propositional_synthesizer_count<conv_node>() <= 1);
		static_assert(pack_propositional_synthesizer_count<mini_node>() == 0);
		CHECK(true);
	}

	TEST_CASE("pack_tau_ba is empty in a pack without the wrapper") {
		using N = node<bv, Bool>;
		static_assert(!pack_has_tau_ba_v<N>);
		static_assert(pack_has_tau_ba_v<conv_node>);
		static_assert(pack_has_tau_ba_v<mini_node>);
		CHECK_FALSE(N::ba::pack_tau_ba(nullptr).has_value());
	}
}
