// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The trace checks a tau constant goes through when a running specification
// receives it: the validity of the constant and of its complement, asked by
// the Tau algebra while the step is solved. A constant holding a conjunction
// of many parts over shared input streams must not have its whole formula
// decided again with the inputs read as outputs: the parts are decided one
// by one, and a formula already known realizable has a trace.

#include "test_integration-interpreter_helper.h"

namespace {

// Part k of a shared specification, as the account rules benchmark writes
// it: an amount over a limit needs the approver of that limit, else the
// part's output is 0.
std::string part(size_t k) {
	std::ostringstream o;
	o << "( (i1[t]:bv[24] > { 100000 }:bv[24] && !(i20[t]:bv[16] = { "
	  << 3 * k + 3 << " }:bv[16]))"
	  << " || (i1[t]:bv[24] > { 10000 }:bv[24] && !(i19[t]:bv[16] = { "
	  << 3 * k + 2 << " }:bv[16]))"
	  << " || (i1[t]:bv[24] > { 1000 }:bv[24] && !(i18[t]:bv[16] = { "
	  << 3 * k + 1 << " }:bv[16])) )"
	  << " ? (o" << 1000 + k << "[t]:bv[24] = { 0 }:bv[24])"
	  << " : (o" << 1000 + k << "[t]:bv[24] = { 1 }:bv[24])";
	return o.str();
}

std::string unit(size_t parts) {
	std::string body;
	for (size_t k = 1; k <= parts; ++k)
		body += (k > 1 ? " && (" : "(") + part(k) + ")";
	return "always ( " + body + " ).";
}

struct router_run {
	strings o0;
	bool has_error = true;
	size_t decisions = 0;
	size_t decided_nodes = 0;
};

// Runs the router of the benchmark for two steps: step 0 delivers the base
// rule on i0, step 1 a unit of @p parts parts. The trace checks run while
// the unit's step is solved are counted.
router_run run_router(size_t parts) {
	auto spec = spec_or_fail("((!(i0[t]:tau = 0)) ? "
		"( u[t]:tau = i0[t]:tau && o0[t]:tau = 0 ) : o0[t]:tau = 1).");
	io_context<node_t> ctx;
	ctx.add_input("i0", tau_type_id<node_t>(),
		std::make_shared<vector_input_stream>(strings{
			"always ( o1[t]:bv[24] = i1[t]:bv[24] ).", unit(parts), "F" }));
	const size_t bv24 = bv_type_id<node_t>(24), bv16 = bv_type_id<node_t>(16);
	for (const char* in : { "i1" })
		ctx.add_input(in, bv24, std::make_shared<vector_input_stream>(
			strings{ "0", "0", "0" }));
	for (const char* in : { "i18", "i19", "i20" })
		ctx.add_input(in, bv16, std::make_shared<vector_input_stream>(
			strings{ "0", "0", "0" }));
	auto o0 = std::make_shared<vector_output_stream>();
	ctx.add_output("o0", tau_type_id<node_t>(), o0);
	ctx.add_output("u", tau_type_id<node_t>(),
		std::make_shared<vector_output_stream>());
	ctx.add_output("o1", bv24, std::make_shared<vector_output_stream>());
	for (size_t k = 1; k <= parts; ++k)
		ctx.add_output("o" + std::to_string(1000 + k), bv24,
			std::make_shared<vector_output_stream>());
	const size_t d0 = trace_check_decisions;
	const size_t n0 = trace_check_decided_nodes;
	auto maybe_i = run<node_t>(spec, ctx, 2);
	router_run res;
	res.decisions = trace_check_decisions - d0;
	res.decided_nodes = trace_check_decided_nodes - n0;
	res.has_error = !maybe_i.has_value() || maybe_i.report().has_error();
	res.o0 = o0->get_values();
	return res;
}

} // namespace

TEST_SUITE("trace checks of a tau constant fed to a router") {

	// The cost of the checks does not grow with the parts of the unit:
	// the whole unit is never decided with its inputs read as outputs,
	// neither for its own validity nor for the validity of its complement.
	TEST_CASE("a unit of many parts costs what a unit of few parts costs") {
		auto few = run_router(4);
		CHECK_FALSE( few.has_error );
		CHECK( few.o0 == strings{ "F", "F" } );
		auto many = run_router(32);
		CHECK_FALSE( many.has_error );
		CHECK( many.o0 == strings{ "F", "F" } );
		INFO( "few: " << few.decisions << " decisions over "
			<< few.decided_nodes << " nodes, many: " << many.decisions
			<< " decisions over " << many.decided_nodes << " nodes" );
		CHECK( many.decisions <= few.decisions );
		CHECK( many.decided_nodes <= 2 * few.decided_nodes );
	}
}
