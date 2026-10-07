// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv/bv_ba.h"


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
	tref src = tree<node_t>::get(sample, opts).value_or(nullptr);
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

TEST_SUITE("bv term helpers") {

	TEST_CASE("get_bv_size reads the annotated width") {
		tref fm = parse("X:bv[8] = { 0 }:bv[8]");
		REQUIRE( fm != nullptr );
		trefs vars = get_free_vars<node_t>(fm);
		REQUIRE( vars.size() == 1 );
		// get_bv_size expects a "type tree" (typed > type > subtype),
		// not the bare variable node -- get_ba_type_tree() synthesizes
		// that from the variable's ba_type id.
		CHECK( get_bv_size<node_t>(
			tau::get(vars[0]).get_ba_type_tree().value()).value() == 8 );
		tref fm16 = parse("Y:bv[16] = { 0 }:bv[16]");
		REQUIRE( fm16 != nullptr );
		trefs vars16 = get_free_vars<node_t>(fm16);
		REQUIRE( vars16.size() == 1 );
		CHECK( get_bv_size<node_t>(
			tau::get(vars16[0]).get_ba_type_tree().value()).value() == 16 );
	}

	TEST_CASE("normalize_bv is idempotent and cache-stable") {
		cvc5::Term one = cvc5_term_manager.mkBitVector(8, 1);
		cvc5::Term two = cvc5_term_manager.mkBitVector(8, 2);
		cvc5::Term sum = cvc5_term_manager.mkTerm(
			cvc5::Kind::BITVECTOR_ADD, { one, two });
		cvc5::Term n1 = normalize_bv(sum);
		// Idempotent: normalizing a normal form is the identity.
		CHECK( normalize_bv(n1) == n1 );
		// Cache-stable: the same input gives the same term again.
		CHECK( normalize_bv(sum) == n1 );
	}
}

TEST_SUITE("bv width completion") {

	// x:bv = {5}:bv[8] propagates the constant's width to the variable via unify()'s children_size()==1 branch.
	TEST_CASE("variable infers width from a widthful constant: x:bv = { 5 }:bv[8]") {
		auto src = parse("x:bv = { 5 }:bv[8]");
		CHECK( src != nullptr );
		auto var = tau::get(src).find_top(is<node_t, tau::variable>);
		CHECK( var != nullptr );
		CHECK( tau::get(var).get_ba_type() == bv8_type_id<node_t> );
	}

	// { 5 }:bv = x:bv[8]: same propagation, the other direction.
	TEST_CASE("constant infers width from a widthful variable: { 5 }:bv = x:bv[8]") {
		auto src = parse("{ 5 }:bv = x:bv[8]");
		CHECK( src != nullptr );
		auto cst = tau::get(src).find_top(is<node_t, tau::ba_constant>);
		CHECK( cst != nullptr );
		CHECK( tau::get(cst).get_ba_type() == bv8_type_id<node_t> );
	}

	// x:bv = {5}:bv has no width anywhere and no cast to complete it from: a type error, never a pack default.
	TEST_CASE("no width anywhere is a type error: x:bv = { 5 }:bv") {
		auto src = parse("x:bv = { 5 }:bv");
		CHECK( src == nullptr );
	}

	// Inference leaves an explicit subtype, so get_bv_size never sees a null traverser here.
	TEST_CASE("inference leaves an explicit subtype for the accessor to read") {
		auto src = parse("(bv) x:bv[8] = 0");
		CHECK( src != nullptr );
		auto var = tau::get(src).find_top(is<node_t, tau::variable>);
		tref type_tree = get_ba_type_tree<node_t>(tau::get(var).get_ba_type());
		using tt = tau::traverser;
		CHECK( (tt(type_tree) | tau::type | tau::subtype | tt::ref) != nullptr );
	}

	// The checked accessor reports a widthless type instead of reading past it.
	TEST_CASE("get_bv_size reports a type_error on a genuinely widthless bv type tree") {
		tref widthless_type = tau::get(tau::typed,
			tau::get(tau::type, "bv"));
		auto r = get_bv_size<node_t>(widthless_type);
		CHECK_FALSE( r.has_value() );
		CHECK( report_has_code(r.report(), code::type_error) );
	}
}

TEST_SUITE("bv solver budgets and declines") {

	static tref closed_form(const char* sample) {
		auto r = get_nso_rr(sample);
		REQUIRE( r.has_value() );
		return r.value().main->get();
	}

	TEST_CASE("a zero bv-solve-timeout gives a question no deadline") {
		const size_t saved = bv_solve_timeout;
		bv_solve_timeout = 0;
		const auto none = bv_question_deadline();
		bv_solve_timeout = 60;
		const auto bounded = bv_question_deadline();
		bv_solve_timeout = saved;
		if (bounded_calls_available()) {
			CHECK( none == std::chrono::steady_clock::time_point::max() );
			CHECK( bounded < std::chrono::steady_clock::time_point::max() );
		} else
			CHECK( bounded == std::chrono::steady_clock::time_point::max() );
	}

	// The translator has no case for a reference, so both entry points
	// decline instead of answering.
	TEST_CASE("solve_bv declines a formula it cannot translate") {
		auto sol = solve_bv<node_t>(closed_form(
			"ex x:bv[8] (x + { 1 }:bv[8] = { 0 }:bv[8] && q(x))."));
		CHECK( !sol.has_error() );
		REQUIRE( sol.has_value() );
		CHECK( !sol.value().has_value() );
	}

	TEST_CASE("the quantifier-free decision declines an untranslatable matrix") {
		const bool saved = bv_quantifier_free_decision;
		bv_quantifier_free_decision = true;
		auto st = bv_formula_sat_status<node_t>(closed_form(
			"ex x:bv[8] (x + { 2 }:bv[8] = { 0 }:bv[8] && q(x))."));
		bv_quantifier_free_decision = saved;
		CHECK( !st.has_value() );
	}

	// An undecided question is neither sat, unsat nor valid.
	TEST_CASE("an untranslatable formula is neither unsat nor valid") {
		tref fm = closed_form(
			"ex x:bv[8] (x + { 1 }:bv[8] = { 0 }:bv[8] && q(x)).");
		CHECK( !bv_formula_sat_status<node_t>(fm).has_value() );
		CHECK( !is_bv_formula_unsat<node_t>(fm) );
		CHECK( !is_bv_formula_valid<node_t>(fm) );
	}

	TEST_CASE("an unknown verdict is neither unsat nor valid") {
		tref fm = closed_form("ex x:bv[8] (x + { 1 }:bv[8] = { 2 }:bv[8]).");
		{
			time_budget_handled scope(std::chrono::seconds(0));
			CHECK( bv_formula_sat_status<node_t>(fm)
				== bv_sat_status::unknown );
			CHECK( !is_bv_formula_sat<node_t>(fm) );
			CHECK( !is_bv_formula_unsat<node_t>(fm) );
			CHECK( !is_bv_formula_valid<node_t>(fm) );
		}
		CHECK( is_bv_formula_sat<node_t>(fm) );
		CHECK( !is_bv_formula_unsat<node_t>(fm) );
	}

	// The unknown of a spent budget answers for that budget only: the
	// formula is decided again once the scope closes, and nothing of the
	// scope's budget reaches a later question.
	TEST_CASE("a shared budget already spent leaves the question unknown") {
		tref fm = closed_form(
			"ex x:bv[8] (x + { 3 }:bv[8] = { 5 }:bv[8]).");
		{
			time_budget_handled scope(std::chrono::seconds(0));
			CHECK( bv_formula_sat_status<node_t>(fm)
				== bv_sat_status::unknown );
			CHECK( scope.ran_out() );
		}
		CHECK( bv_formula_sat_status<node_t>(fm) == bv_sat_status::sat );
		CHECK( time_budget_exhausted().empty() );
		CHECK( bv_formula_sat_status<node_t>(closed_form(
			"ex x:bv[8] (x + { 4 }:bv[8] = { 5 }:bv[8])."))
			== bv_sat_status::sat );
	}
}

// A bitvector literal reaches cvc5 only once it is known to fit: cvc5 throws
// on one that does not, and the library raises no exception.
TEST_SUITE("bitvector_literal_fits") {

	TEST_CASE("values at the edge of the width") {
		CHECK( bitvector_literal_fits(8, "255", 10) );
		CHECK( !bitvector_literal_fits(8, "256", 10) );
		CHECK( bitvector_literal_fits(8, "11111111", 2) );
		CHECK( !bitvector_literal_fits(8, "111111111", 2) );
		CHECK( bitvector_literal_fits(8, "000000000011", 2) );
		CHECK( bitvector_literal_fits(8, "ff", 16) );
		CHECK( bitvector_literal_fits(8, "00FF", 16) );
		CHECK( !bitvector_literal_fits(8, "100", 16) );
		CHECK( bitvector_literal_fits(1, "0", 10) );
		CHECK( bitvector_literal_fits(1, "1", 10) );
		CHECK( !bitvector_literal_fits(1, "2", 10) );
		CHECK( bitvector_literal_fits(64, "18446744073709551615", 10) );
		CHECK( !bitvector_literal_fits(64, "18446744073709551616", 10) );
	}

	TEST_CASE("malformed input and widths cvc5 rejects") {
		CHECK( !bitvector_literal_fits(0, "0", 10) );
		CHECK( !bitvector_literal_fits(8, "", 10) );
		CHECK( !bitvector_literal_fits(8, "12", 2) );
		CHECK( !bitvector_literal_fits(8, "1a", 10) );
		CHECK( !bitvector_literal_fits(8, "-1", 10) );
		CHECK( !bitvector_literal_fits(8, "1", 8) );
	}

	TEST_CASE("a constant that does not fit its type is a parse error") {
		CHECK( parse("{ 256 }:bv[8] = x") == nullptr );
		CHECK( parse("{ 255 }:bv[8] = x") != nullptr );
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
