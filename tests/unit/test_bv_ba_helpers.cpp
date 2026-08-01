// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"


TEST_SUITE("Configuration") {

	TEST_CASE("logging") {
		logging::trace();
	}

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

tref parse(const std::string& sample) {
	auto opts = tau::get_options{
		.parse = { .start = tau::wff },
		.reget_with_hooks = true
	};
	tref src = tree<node_t>::get(sample, opts);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}

TEST_SUITE("bv to tau tree translation") {


}

// SO-2. is_bv_solvable_formula gates every solver shortcut in the normalizer
// and in anti-prenexing. It used to inspect `variable` nodes only, so a
// formula whose untranslatable part was something else -- an unresolved
// wff_ref, whose arguments are perfectly good bv-typed variables -- was
// declared solvable; bv_eval_node then had no case for it, returned nullopt and
// logged "Failed to translate the formula to cvc5" once per resolve pass plus
// the final gate (four times for a single normalize_non_temp), each time after
// a cvc5::Solver had been constructed and the tree walked.
TEST_SUITE("is_bv_solvable_formula") {

	static tref main_of(const char* sample) {
		auto r = get_nso_rr(sample);
		REQUIRE( r.has_value() );
		return r.value().main->get();
	}

	TEST_CASE("a plain sized bitvector formula is solvable") {
		CHECK( is_bv_solvable_formula<node_t>(
			main_of("ex x:bv[8] (x + { 1 }:bv[8] = { 0 }:bv[8]).")) );
	}

	TEST_CASE("an unresolved reference is not solvable") {
		// The translator has no case for `ref` (the nonterminal behind both
		// wff_ref and bf_ref), so the whole translation fails.
		CHECK( !is_bv_solvable_formula<node_t>(
			main_of("ex x:bv[8] (x + { 1 }:bv[8] = { 0 }:bv[8]"
				" && q(x)).")) );
	}

	TEST_CASE("a non-bv variable is not solvable") {
		// Control: the pre-existing variable check still holds.
		CHECK( !is_bv_solvable_formula<node_t>(
			main_of("ex x:bv[8] (x = { 0 }:bv[8] && y = 0).")) );
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
