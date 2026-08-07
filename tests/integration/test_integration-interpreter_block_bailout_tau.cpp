// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Per-block bail-out, applied to a spec with NO bv content.
// The routing predicate is the interpreter context alone, not what the block
// contains, so this shape is routed as well and must keep its outputs.
// Own binary on purpose: this spec family and the mixed one from
// test_integration-interpreter_block_bailout are kept in separate processes.

#include "test_integration-interpreter_helper.h"

#include "api.h"

using tau_api = api<node_t>;

static const char* tau_only_sample =
	"o0seal[0]:tau = 1 && o0law[0]:tau = 1 && "
	"( (i2[t]:tau != 0) "
	"  ? ( (o0seal[t]:tau = o0law[t-1]:tau) "
	"      && (o0law[t]:tau = o0law[t-1]:tau) ) "
	"  : ( (o0seal[t]:tau = o0seal[t-1]:tau) "
	"      && (o0law[t]:tau = o0law[t-1]:tau & i1[t]:tau) ) ).";

static std::optional<assignment<node_t>> run_tau_only() {
	io_context<node_t> ctx;
	strings i1_values = { "T", "T" };
	strings i2_values = { "F", "F" };
	ctx.add_input("i1", tau_type_id<node_t>(),
		std::make_shared<vector_input_stream>(i1_values));
	ctx.add_input("i2", tau_type_id<node_t>(),
		std::make_shared<vector_input_stream>(i2_values));
	return run_test(tau_only_sample, ctx, 1);
}

TEST_SUITE("interpreter block bail-out, no bv content") {
	TEST_CASE("off by default: stock routing works") {
		bdd_init<Bool>();
		auto memory = run_tau_only();
		REQUIRE( memory.has_value() );
		CHECK ( !memory.value().empty() );
	}
	TEST_CASE("a spec without bv content is routed too, and agrees") {
		auto stock = run_tau_only();
		REQUIRE( stock.has_value() );
		tau_api::set_run_block_bailout(true);
		auto routed = run_tau_only();
		tau_api::set_run_block_bailout(false);
		REQUIRE( routed.has_value() );
		CHECK ( stock.value() == routed.value() );
	}
}
