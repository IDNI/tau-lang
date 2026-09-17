// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-interpreter_helper.h"

#include "boolean_algebras/bv/heuristics/bv_widening.h"

namespace {

// Sets bv_widening for the lifetime of the enclosing scope and restores
// whatever value it had before, even on a REQUIRE/CHECK-failure stack
// unwind.
struct bv_widening_scope {
	bool prev;
	bv_widening_scope() : prev(bv_widening) { bv_widening = true; }
	~bv_widening_scope() { bv_widening = prev; }
};

// Same RAII shape for bv_max_width (the widening cap).
struct bv_max_width_scope {
	size_t prev;
	explicit bv_max_width_scope(size_t w) : prev(bv_max_width) {
		bv_max_width = w;
	}
	~bv_max_width_scope() { bv_max_width = prev; }
};

// GitHub #126: a run whose step forks on values its own definitions
// determine, with and without the definitional propagation.
strings run_forking_spec(bool on) {
	const bool prev = interpreter<node_t>::definitional_propagation;
	interpreter<node_t>::definitional_propagation = on;
	bdd_init<Bool>();
	auto spec = create_spec(
		"(o1[t]:bv[8] = (i1[t]:bv[8] % { 6 }:bv[8]) + { 1 }:bv[8])"
		" && ((o1[t]:bv[8] + { 40 }:bv[8] > { 42 }:bv[8])"
		"   ? (o2[t]:bv[8] = { 42 }:bv[8]) : (o2[t]:bv[8] = o1[t]:bv[8] + { 40 }:bv[8]))"
		" && ((o1[t]:bv[8] + { 2 }:bv[8] > { 4 }:bv[8])"
		"   ? (o3[t]:bv[8] = { 1 }:bv[8]) : (o3[t]:bv[8] = { 0 }:bv[8])).");
	io_context<node_t> ctx;
	strings i1_values = { "1", "5", "2", "7" };
	ctx.add_input("i1", bv_type_id<node_t>(8),
		std::make_shared<vector_input_stream>(i1_values));
	auto o2 = std::make_shared<vector_output_stream>();
	ctx.add_output("o2", bv_type_id<node_t>(8), o2);
	auto o3 = std::make_shared<vector_output_stream>();
	ctx.add_output("o3", bv_type_id<node_t>(8), o3);
	auto maybe_i = run<node_t>(spec, ctx, 4);
	interpreter<node_t>::definitional_propagation = prev;
	REQUIRE(maybe_i.has_value());
	strings out = o2->get_values();
	for (const auto& v : o3->get_values()) out.push_back(v);
	return out;
}

} // namespace

TEST_SUITE("step definitional propagation (bv)") {

	TEST_CASE("a step forking on values its own definitions determine: same outputs") {
		auto off = run_forking_spec(false);
		auto on = run_forking_spec(true);
		CHECK(off == on);
		CHECK(on == strings{ "42", "42", "42", "42", "0", "1", "1", "0" });
	}
}

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

	// A specification whose widened arithmetic would exceed bv_max_width
	// fails its first normalization (nullptr, with the pass's own error
	// line) and make_interpreter must report "cannot run" instead of
	// enumerating the paths of a null spec: i1*i1 at bv[8] needs W = 16.
	TEST_CASE("spec exceeding the widening cap cannot be run") {
		bdd_init<Bool>();
		bv_widening_scope widen;
		bv_max_width_scope cap(12);
		auto spec = create_spec("o1[t]:bv[8] = i1[t]:bv[8] * i1[t]:bv[8].");
		io_context<node_t> ctx;
		strings i1_values = { "3", "4" };
		ctx.add_input("i1", bv_type_id<node_t>(8),
			std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto maybe_i = run<node_t>(spec, ctx, 2);
		CHECK( !maybe_i.has_value() );
		CHECK( o1->get_values().empty() );
	}

	// An update proposal whose widened arithmetic would exceed the cap is
	// rejected ("No update performed") and the running spec stays as it
	// was: o7 keeps echoing i9 after the rejected step exactly as before
	// it, and the run itself does not fail. The stream names are fresh
	// (o7/i8/i9): the tau-typed o1/i1 of the update cases above linger in
	// the process-wide stream type registry and make a bv-typed o1 update
	// proposal unparseable when the suite runs in file order.
	TEST_CASE("update exceeding the widening cap is rejected, spec kept") {
		bdd_init<Bool>();
		bv_widening_scope widen;
		bv_max_width_scope cap(12);
		auto spec = create_spec("u[t] = i8[t] && o7[t]:bv[8] = i9[t]:bv[8].");
		strings i1_values = {
			"F", "o7[t]:bv[8] = i9[t]:bv[8] * i9[t]:bv[8]", "F"
		};
		strings i2_values = { "1", "2", "3" };
		io_context<node_t> ctx;
		ctx.add_input("i8", tau_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i9", bv_type_id<node_t>(8),
			std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto u  = std::make_shared<vector_output_stream>();
		ctx.add_output("o7", bv_type_id<node_t>(8), o1);
		ctx.add_output("u",  tau_type_id<node_t>(), u);
		auto maybe_i = run<node_t>(spec, ctx, 3);
		CHECK( maybe_i.has_value() );
		CHECK( o1->get_values() == strings{ "1", "2", "3" } );
	}

	// Task 8 (bv-widening): prove the widened semantics through a live
	// execution run, not just direct widen_bv_arithmetic/normalizer calls
	// (Tasks 4-6). `min(i1[t] + i2[t], {200}:bv[8])` is the brief's
	// saturating-add idiom: at step 0, i1=200/i2=100 overflows bv[8]
	// (200+100=300). Steps 1-3 use small, non-overflowing pairs so the
	// two semantics agree there, isolating the divergence to index 0.
	//
	// Modular (bv_widening off, cvc5 native bv[8] add): 300 mod 256 = 44;
	// min(44, 200) = 44.
	// Exact (bv_widening on): needed_width(add) = max(8,8)+1 = 9, so
	// 200+100 = 300 fits (no wrap); min(300, 200) = 200; the atom's
	// bare-storage truncating cast back to bv[8] is lossless (200 <=
	// 255).
	TEST_CASE("always o1 = min(i1+i2, 200): wraps to 44 when bv_widening is off") {
		bdd_init<Bool>();
		REQUIRE( !bv_widening ); // default; no scope guard turns it on here
		auto spec = create_spec(
			"always o1[t]:bv[8] = min(i1[t] + i2[t], { 200 }:bv[8]).");
		io_context<node_t> ctx;
		strings i1_values = { "200", "10", "5", "0" };
		strings i2_values = { "100", "20", "5", "0" };
		ctx.add_input("i1", bv_type_id<node_t>(8),
			std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", bv_type_id<node_t>(8),
			std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK( maybe_i.has_value() );
		CHECK( o1->get_values() == strings{ "44", "30", "10", "0" } );
	}

	TEST_CASE("always o1 = min(i1+i2, 200): saturates to 200 when bv_widening is on") {
		bdd_init<Bool>();
		bv_widening_scope widen;
		auto spec = create_spec(
			"always o1[t]:bv[8] = min(i1[t] + i2[t], { 200 }:bv[8]).");
		io_context<node_t> ctx;
		strings i1_values = { "200", "10", "5", "0" };
		strings i2_values = { "100", "20", "5", "0" };
		ctx.add_input("i1", bv_type_id<node_t>(8),
			std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", bv_type_id<node_t>(8),
			std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK( maybe_i.has_value() );
		CHECK( o1->get_values() == strings{ "200", "30", "10", "0" } );
	}

	// Task 8 pwr probe: does an UPDATE rule containing bv arithmetic get
	// elaborated the same way a compile-time spec does? `i1` stays the
	// tau-typed update-submission channel (the `u[t] = i1[t]` idiom used
	// throughout this suite's "u[t] = i1[t]: ..." cases); the update text
	// references a SEPARATE bv[8] stream `i2` for the arithmetic operand
	// -- mirroring the "dec_seq" case above, which uses a second stream
	// (there, tau-typed) referenced from inside a submitted update rather
	// than i1 itself, because i1 is already pinned to tau (u's type) by
	// the submission clause and cannot also carry bv[8] values without a
	// type conflict once the update clause is merged into the running
	// spec.
	//
	// The original spec leaves o1 UNCONSTRAINED (unlike "u[t] = i1[t]:
	// spec_replace" above, which pins a baseline for o1 to replace).
	// Confirmed by a trace-level investigation (see the Task 8 report):
	// pinning a conflicting baseline instead routes pointwise_revision
	// through its documented "I1" last-resort-alternative path
	// (interpreter.tmpl.h ~1806-1823) rather than the plain "append as a
	// new spec part" path (~1526-1540) this test now takes -- an earlier
	// version of this test pinned o1 to a baseline and observed a stuck
	// value there, which turned out to be an artifact of that alternate
	// path, not a widening defect (flagged separately, out of this task's
	// scope). The update submitted at step 1 introduces `min(i2*3, 100)`
	// for o1, effective from step 2 onward (one-step lag, same as every
	// other update case in this suite).
	//
	// i2's programmatic stream is only read starting the first step it
	// becomes an active input, i.e. from step 2 onward (positional
	// consumption from the stream's own cursor, not indexed by absolute
	// time) -- confirmed by a trace-level rerun. So `i2_values` need only
	// the two entries actually consumed at steps 2-3, not four
	// time-aligned ones.
	//
	// i2 = 90: 90*3 = 270 overflows bv[8] (270 mod 256 = 14) -- modular
	// would give min(14,100) = 14. Exact (widened): needed_width(mul(8,8))
	// = 16, 270 fits, min(270,100) = 100, truncated losslessly to bv[8].
	// With bv_widening on for the whole run, o1 must read 100 at steps
	// 2-3, not 14.
	TEST_CASE("pwr: an update rule with bv arithmetic is elaborated under bv_widening") {
		bdd_init<Bool>();
		// The type of a stream name is global to the process
		// (definitions<node_t>::instance(), see the same fix and comment
		// in tests/integration/test_integration-bv_stress_check.cpp).
		// `o1` and `i2` were already pinned to :tau by dozens of earlier
		// "u[t] = i1[t]: ..." cases above in this same suite (none of
		// which annotate them, so they default to tau); without clearing
		// here, the update text below fails to parse as a stream value
		// with "Incompatible type information ..., expected tau, found
		// bv[8]" the first time o1/i2 are read back as bv[8] through
		// interpreter::read() -> ba_constants<node>::get(line, ...) (the
		// two direct-execution tests above never hit this: their o1/i1/i2
		// are only ever parsed once, through create_spec's own
		// self-contained inference, never re-parsed from a runtime
		// stream-value string).
		definitions<node_t>::instance().clear();
		bv_widening_scope widen;
		auto spec = create_spec("u[t] = i1[t].");
		strings i1_values = {
			"F",
			"always o1[t]:bv[8] = min(i2[t]:bv[8] * { 3 }:bv[8], { 100 }:bv[8])",
			"F", "F"
		};
		strings i2_values = { "90", "90" };
		io_context<node_t> ctx;
		ctx.add_input("i1", tau_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", bv_type_id<node_t>(8),
			std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto u  = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		ctx.add_output("u",  tau_type_id<node_t>(), u);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK( maybe_i.has_value() );
		auto o1_values = o1->get_values();
		// o1 is unconstrained before the update, so it produces no value
		// (is not part of the executed spec) at steps 0-1 at all; it only
		// becomes an active output once the revision (submitted at step 1,
		// effective from step 2) introduces it. get_values() therefore
		// collects exactly two entries -- one per produced step (2 and 3),
		// not four time-indexed slots.
		REQUIRE( o1_values.size() == 2 );
		CHECK( o1_values[0] == "100" );
		CHECK( o1_values[1] == "100" );
	}

}
