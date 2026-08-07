// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Per-block bail-out to the legacy anti-prenex during interpreter
// normalization (run path).
// The bail-out changes ROUTING, not semantics: with the switch on, the same
// spec must produce the same outputs as the stock pipeline. The sample is the
// nested-conditionals shape from the mixed tau/bv regression test.
// A spec with no bv content at all is covered by its own test binary
// (test_integration-interpreter_block_bailout_tau) -- one spec family per
// process, so neither measurement inherits the other's interpreter state.

#include "test_integration-interpreter_helper.h"

#include "api.h"

using tau_api = api<node_t>;

static const char* mixed_sample =
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

static std::optional<assignment<node_t>> run_mixed() {
	io_context<node_t> ctx;
	strings i1_values = { "T", "T" };
	strings i2_values = { "{ #x01 }:bv[8]", "{ #x01 }:bv[8]" };
	ctx.add_input("i1", tau_type_id<node_t>(),
		std::make_shared<vector_input_stream>(i1_values));
	ctx.add_input("i2", bv_type_id<node_t>(8),
		std::make_shared<vector_input_stream>(i2_values));
	return run_test(mixed_sample, ctx, 1);
}

TEST_SUITE("interpreter block bail-out") {
	TEST_CASE("off by default: stock routing works") {
		bdd_init<Bool>();
		auto memory = run_mixed();
		REQUIRE( memory.has_value() );
		CHECK ( !memory.value().empty() );
	}
	TEST_CASE("routing is per block: a tau-only block keeps its own route") {
		// The switch is applied per quantifier block, so a spec whose bv
		// content sits in one block must still agree with stock routing.
		auto stock = run_mixed();
		REQUIRE( stock.has_value() );
		tau_api::set_run_block_bailout(true);
		auto routed = run_mixed();
		tau_api::set_run_block_bailout(false);
		REQUIRE( routed.has_value() );
		CHECK ( stock.value() == routed.value() );
	}
	TEST_CASE("bail-out routing produces the same outputs") {
		auto stock = run_mixed();
		REQUIRE( stock.has_value() );
		tau_api::set_run_block_bailout(true);
		auto bailed = run_mixed();
		tau_api::set_run_block_bailout(false);
		REQUIRE( bailed.has_value() );
		// hash-consed trees: equal content means equal trefs, so the
		// assignments compare directly
		CHECK ( stock.value() == bailed.value() );
	}
}
