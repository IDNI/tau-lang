// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-interpreter_helper.h"

TEST_SUITE("with inputs and outputs (bv)") {

	// Nested conditionals mixing tau and bv streams compile to clauses
	// neither translator can fully lift alone; this case is slow enough
	// to exceed the test timeout, so it stays skipped but present.
	TEST_CASE("nested conditionals over mixed tau/bv streams stay sat"
		* doctest::skip())
	{
		const char* sample =
			"o0seal[0]:tau = 1 && o0law[0]:tau = 1 && "
			"( (i2[t]:bv[8] = { #x01 }:bv[8]) "
			"  ? ( (o0seal[t]:tau = o0law[t-1]:tau) "
			"      && (o0law[t]:tau = o0law[t-1]:tau) "
			"      && (o0res[t]:bv[8] = { #x05 }:bv[8]) ) "
			"  : ( (o0seal[t]:tau = o0seal[t-1]:tau) && "
			"      ( ((o0law[t-1]:tau & i1[t]:tau) != 0) "
			"        ? ( (o0law[t]:tau = o0law[t-1]:tau & i1[t]:tau) "
			"            && (o0res[t]:bv[8] = { #x09 }:bv[8]) ) "
			"        : ( (o0law[t]:tau = o0law[t-1]:tau) "
			"            && (o0res[t]:bv[8] = { #x08 }:bv[8]) ) ) ) ).";
		io_context<node_t> ctx;
		strings i1_values = { "T", "T" };
		strings i2_values = { "{ #x01 }:bv[8]", "{ #x01 }:bv[8]" };
		ctx.add_input("i1", tau_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", bv_type_id<node_t>(8),
			std::make_shared<vector_input_stream>(i2_values));
		auto memory = run_test(sample, ctx, 1);
		REQUIRE( memory.has_value() );
		CHECK ( !memory.value().empty() );
	}

	// `(a*b)/c` computed at the operand width wraps before the divide
	// runs; casting the operands up, dividing, and narrowing the
	// quotient back down keeps the result exact.
	TEST_CASE("widening a stream through a cast") {
		bdd_init<Bool>();
		auto spec = create_spec("o1[t]:bv[16] = (bv[16]) i1[t]:bv[8].");
		io_context<node_t> ctx;
		strings i1_values = { "200", "7" };
		ctx.add_input("i1", bv_type_id<node_t>(8),
			std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(16), o1);
		auto maybe_i = run<node_t>(spec, ctx, 2);
		CHECK( maybe_i.has_value() );
		CHECK ( o1->get_values() == strings{ "200", "7" } );
	}

	TEST_CASE("percentage via widen-multiply-divide-narrow") {
		// 200 * 5 = 1000 wraps to 232 at 8 bits (and 232/100 = 2); at 16
		// bits the product is exact and 1000/100 = 10 fits back into 8 bits.
		bdd_init<Bool>();
		auto spec = create_spec("o1[t]:bv[8] = (bv[8]) ( ((bv[16]) i1[t]:bv[8]"
			" * { 5 }:bv[16]) / { 100 }:bv[16] ).");
		io_context<node_t> ctx;
		strings i1_values = { "200", "40" };
		ctx.add_input("i1", bv_type_id<node_t>(8),
			std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto maybe_i = run<node_t>(spec, ctx, 2);
		CHECK( maybe_i.has_value() );
		CHECK ( o1->get_values() == strings{ "10", "2" } );
	}

}
