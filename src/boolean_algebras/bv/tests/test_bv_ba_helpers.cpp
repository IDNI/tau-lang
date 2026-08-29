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
			tau::get(vars[0]).get_ba_type_tree()) == 8 );
		tref fm16 = parse("Y:bv[16] = { 0 }:bv[16]");
		REQUIRE( fm16 != nullptr );
		trefs vars16 = get_free_vars<node_t>(fm16);
		REQUIRE( vars16.size() == 1 );
		CHECK( get_bv_size<node_t>(
			tau::get(vars16[0]).get_ba_type_tree()) == 16 );
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

TEST_SUITE("bv default width") {

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

	// x:bv = {5}:bv has no width anywhere; inference defaults both operands to the same concrete width.
	TEST_CASE("both operands default when no width appears anywhere: x:bv = { 5 }:bv") {
		auto src = parse("x:bv = { 5 }:bv");
		CHECK( src != nullptr );
		auto var = tau::get(src).find_top(is<node_t, tau::variable>);
		auto cst = tau::get(src).find_top(is<node_t, tau::ba_constant>);
		CHECK( var != nullptr );
		CHECK( cst != nullptr );
		CHECK( tau::get(var).get_ba_type() == bv16_type_id<node_t> );
		CHECK( tau::get(cst).get_ba_type() == bv16_type_id<node_t> );
		auto solution = solve_bv<node_t>(src);
		CHECK( solution.has_value() );
	}

	// Inference leaves an explicit subtype, so get_bv_size never sees a null traverser here.
	TEST_CASE("inference leaves an explicit subtype for the accessor to read") {
		auto src = parse("x:bv = { 5 }:bv");
		auto var = tau::get(src).find_top(is<node_t, tau::variable>);
		tref type_tree = get_ba_type_tree<node_t>(tau::get(var).get_ba_type());
		using tt = tau::traverser;
		CHECK( (tt(type_tree) | tau::type | tau::subtype | tt::ref) != nullptr );
	}

	// A genuinely widthless type tree reaching the accessor is a loud, defined failure, never UB.
	TEST_CASE("get_bv_size throws on a genuinely widthless bv type tree") {
		tref widthless_type = tau::get(tau::typed,
			tau::get(tau::type, "bv"));
		CHECK_THROWS_AS( get_bv_size<node_t>(widthless_type), std::logic_error );
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
