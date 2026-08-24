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
		CHECK( (tt(type_tree) | tau::subtype | tt::ref) != nullptr );
	}

	// A genuinely widthless type tree reaching the accessor is a loud, defined failure, never UB.
	TEST_CASE("get_bv_size throws on a genuinely widthless bv type tree") {
		tref widthless_type = tau::get(tau::typed, tau::get(tau::type, "bv"));
		CHECK_THROWS_AS( get_bv_size<node_t>(widthless_type), std::logic_error );
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
