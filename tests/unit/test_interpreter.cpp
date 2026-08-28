// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// AP-22: this file used to be a single empty TEST_CASE with 0 assertions.
// Coverage below focuses on the tref-based api<node>::get_interpreter()/
// step() overloads and on interpreter<node>'s public state, at the unit
// level. The string-based get_interpreter()/step()/get_inputs_for_step()
// path (with and without remapped streams) is already covered end-to-end
// by tests/api/test_api-string_api.cpp and by the full multi-time-step
// executions in tests/integration/test_integration-interpreter.cpp, so it
// is intentionally not duplicated here.

#include "test_init.h"
#include "test_tau_helpers.h"
// for the AP2-1 gc-pinning regression at the end of this file
#include "repl_evaluator.h"

using tau_api = api<node_t>;

TEST_SUITE("interpreter") {

	TEST_CASE("get_interpreter(tref) builds a working interpreter and "
		  "time_point starts at 0")
	{
		tref fm = tau_api::get_formula("o[t]:tau = i[t]:tau");
		REQUIRE(fm);
		auto maybe_i = tau_api::get_interpreter(fm);
		REQUIRE(maybe_i.has_value());
		auto& i = maybe_i.value();
		CHECK(i.time_point == 0);

		auto inputs = tau_api::get_inputs_for_step(i);
		REQUIRE(inputs.size() == 1);
		CHECK(inputs[0].name == "i");
		auto maybe_outputs = tau_api::step(i, {
			{ inputs[0], "T" }
		});
		REQUIRE(maybe_outputs.has_value());
		auto outputs = maybe_outputs.value();
		// the output for the input consumed at the input's time point is
		// reported one time point ahead
		REQUIRE(outputs.contains({ "o", 1 }));
		CHECK(outputs.at({ "o", 1 }) == "T");
		// after a successful step the interpreter has moved on to the
		// next time point
		CHECK(i.time_point == 1);
	}

	TEST_CASE("get_interpreter(tref) rejects a formula with a genuinely "
		  "free variable")
	{
		// `x` is neither an input/output stream nor bound by any
		// quantifier, so the resulting normalized formula still has a
		// free variable and get_interpreter must refuse to build an
		// interpreter for it.
		tref fm = tau_api::get_formula("o[t]:tau = i[t]:tau && x = 0");
		REQUIRE(fm);
		auto maybe_i = tau_api::get_interpreter(fm);
		CHECK(!maybe_i.has_value());
	}

	TEST_CASE("get_interpreter(tref) rejects an unsatisfiable formula") {
		tref fm = tau_api::get_formula(
			"o[t]:tau = i[t]:tau && o[t]:tau != i[t]:tau");
		REQUIRE(fm);
		auto maybe_i = tau_api::get_interpreter(fm);
		CHECK(!maybe_i.has_value());
	}

	TEST_CASE("get_inputs_for_step reports every input stream referenced "
		  "by the specification")
	{
		// bf-level `&` (not wff-level `&&`) combines the two tau-typed
		// input streams into a single bf value the equality can compare
		// against.
		tref fm = tau_api::get_formula(
			"o[t]:tau = i1[t]:tau & i2[t]:tau");
		REQUIRE(fm);
		auto maybe_i = tau_api::get_interpreter(fm);
		REQUIRE(maybe_i.has_value());
		auto& i = maybe_i.value();

		auto inputs = tau_api::get_inputs_for_step(i);
		std::vector<std::string> names;
		for (auto& in : inputs) names.push_back(in.name);
		std::sort(names.begin(), names.end());
		CHECK(names == std::vector<std::string>({ "i1", "i2" }));

		auto maybe_outputs = tau_api::step(i, {
			{ { "i1", 0 }, "T" },
			{ { "i2", 0 }, "T" }
		});
		REQUIRE(maybe_outputs.has_value());
		CHECK(maybe_outputs.value().at({ "o", 1 }) == "T");
	}
}

// Coverage-driven additions (2026-08-01). interpreter.tmpl.h measured 79.6%
// line coverage. A large share of the gap is its I/O failure handling in
// read()/write(): what happens when an input stream errors outright, yields a
// value that is not a parseable BA constant, or an output stream refuses a
// write. No test supplied a misbehaving stream, so none of those branches ran.
//
// interpreter_options::input_remaps / output_remaps make these reachable from
// the public API with purpose-built streams, which is what the suite below
// does. Note the distinction the interpreter draws between the two ways an
// input stream can decline to produce a value: an EMPTY STRING means "no more
// inputs" and ends the run gracefully, whereas NULLOPT is a read error.
namespace {

// get() fails outright, as opposed to signalling end-of-input with "".
struct failing_input_stream : serialized_constant_input_stream {
	std::shared_ptr<serialized_constant_input_stream> rebuild() override {
		return std::make_shared<failing_input_stream>();
	}
	std::optional<std::string> get() override { return std::nullopt; }
};

// get() yields a value that is not a parseable BA constant.
struct garbage_input_stream : serialized_constant_input_stream {
	std::shared_ptr<serialized_constant_input_stream> rebuild() override {
		return std::make_shared<garbage_input_stream>();
	}
	std::optional<std::string> get() override {
		return std::string{ "! ) ( not a constant" };
	}
};

// put() reports failure for every write.
struct failing_output_stream : serialized_constant_output_stream {
	std::shared_ptr<serialized_constant_output_stream> rebuild() override {
		return std::make_shared<failing_output_stream>();
	}
	bool put(const std::string&) override { return false; }
};

} // namespace

TEST_SUITE("interpreter: misbehaving streams") {

	TEST_CASE("an input stream that errors makes the step fail") {
		interpreter_options opts;
		opts.input_remaps["i"] = std::make_shared<failing_input_stream>();
		opts.output_remaps["o"] = std::make_shared<vector_output_stream>();
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].", opts);
		REQUIRE(maybe_i.has_value());
		CHECK(!tau_api::step(maybe_i.value()).has_value());
	}

	TEST_CASE("an unparseable input value makes the step fail") {
		interpreter_options opts;
		opts.input_remaps["i"] = std::make_shared<garbage_input_stream>();
		opts.output_remaps["o"] = std::make_shared<vector_output_stream>();
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].", opts);
		REQUIRE(maybe_i.has_value());
		CHECK(!tau_api::step(maybe_i.value()).has_value());
	}

	TEST_CASE("an output stream that refuses the write makes the step fail") {
		interpreter_options opts;
		opts.input_remaps["i"] = std::make_shared<vector_input_stream>(
			std::vector<std::string>{ "T", "F" });
		opts.output_remaps["o"] = std::make_shared<failing_output_stream>();
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].", opts);
		REQUIRE(maybe_i.has_value());
		CHECK(!tau_api::step(maybe_i.value()).has_value());
	}

	// Contrast: an EMPTY value is the graceful "no more inputs" signal rather
	// than an error, so the run ends without the step reporting failure the
	// way the error cases above do.
	TEST_CASE("an exhausted vector input stream ends the run gracefully") {
		auto in = std::make_shared<vector_input_stream>(
			std::vector<std::string>{ "T" });
		auto out = std::make_shared<vector_output_stream>();
		interpreter_options opts;
		opts.input_remaps["i"] = in;
		opts.output_remaps["o"] = out;
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].", opts);
		REQUIRE(maybe_i.has_value());
		auto& i = maybe_i.value();
		// First step consumes the only value and produces an output.
		CHECK(tau_api::step(i).has_value());
		CHECK(out->get_values().size() == 1);
		// The stream is now exhausted and returns "", ending the run.
		CHECK(!tau_api::step(i).has_value());
	}
}

// Task 8: interpreter-side grouped stream construction for tuple-typed
// (ADT) io streams (private/2026-08-05-adt-design.md, section 4). A
// tuple-typed stream stays ONE physical stream: rebuild_inputs/
// rebuild_outputs (interpreter.tmpl.h) group a stream's flattened members
// by their ctx.adt_streams root (populated by the ADT flattener,
// src/adt/adt_flatten.tmpl.h, Task 7's io_context.h) and route them through
// one shared adt_tuple_reader/writer instead of giving each member its own
// private stream. read()/write() themselves are untouched: they still just
// look up each flattened member io var (`i.a`, `i.b`, ...) in inputs/
// outputs, unaware the object behind it is a shared adapter.
TEST_SUITE("adt interpreter") {

	TEST_CASE("tuple input distributes and tuple output collects") {
		auto in = std::make_shared<vector_input_stream>(
			std::vector<std::string>{
				"{ a: \"0\", b: \"1\" }",
				"{ a: \"1\", b: \"0\" }"
			});
		auto out = std::make_shared<vector_output_stream>();
		interpreter_options opts;
		opts.input_remaps["i"] = in;
		opts.output_remaps["o"] = out;
		auto maybe_i = tau_api::get_interpreter(
			"type Point = {a: sbf, b: sbf}. "
			"i:Point := in console. o:Point := out console. "
			"o[t] = i[t].", opts);
		REQUIRE(maybe_i.has_value());
		auto& i = maybe_i.value();

		// Two steps, each consuming ONE tuple literal: the shared reader
		// reads the physical stream once per time point (memoized across
		// both i.a's and i.b's own adapter calls), matching the design's
		// "one prompt/read per stream per step".
		REQUIRE(tau_api::step(i).has_value());
		REQUIRE(tau_api::step(i).has_value());

		// The output stream received exactly the two formatted tuple
		// literals -- one put() per step (both o.a and o.b collected
		// through the shared writer before it writes), not four separate
		// per-member writes.
		auto values = out->get_values();
		REQUIRE(values.size() == 2);
		CHECK(values[0] == "{ a: \"0\", b: \"1\" }");
		CHECK(values[1] == "{ a: \"1\", b: \"0\" }");
	}

	TEST_CASE("malformed tuple input aborts the step") {
		// Missing member "b": adt_tuple_reader::leaf validates the parsed
		// literal against the layout's shape and fails both i.a's and i.b's
		// read, aborting the step exactly like today's unparsable console
		// input (see "an unparseable input value makes the step fail" above).
		auto in = std::make_shared<vector_input_stream>(
			std::vector<std::string>{ "{ a: \"0\" }" });
		auto out = std::make_shared<vector_output_stream>();
		interpreter_options opts;
		opts.input_remaps["i"] = in;
		opts.output_remaps["o"] = out;
		auto maybe_i = tau_api::get_interpreter(
			"type Point = {a: sbf, b: sbf}. "
			"i:Point := in console. o:Point := out console. "
			"o[t] = i[t].", opts);
		REQUIRE(maybe_i.has_value());
		auto& i = maybe_i.value();

		CHECK(!tau_api::step(i).has_value());
		CHECK(out->get_values().empty());
	}

	TEST_CASE("REPL-style console input surfaces one awaiting tuple prompt "
		  "per time point")
	{
		// Regression test for a Critical review finding: the REPL's
		// continue_running (repl_evaluator.tmpl.h) used to find an
		// awaiting console stream by dynamic_pointer_cast<
		// repl_pending_input_stream> directly over interpreter::inputs'
		// values. For a tuple-typed member that value is an
		// adt_member_input_stream, never a repl_pending_input_stream --
		// the cast always failed, so a `run` of any spec with a
		// tuple-typed console input silently never prompted and fell
		// straight into the continue-or-quit branch. Fixed by
		// find_repl_pending_input<node> (interpreter.tmpl.h), which sees
		// through the adapter chain; this test exercises that helper the
		// same way continue_running now does, with a console_input_factory
		// wired exactly like the REPL wires one (repl_evaluator.tmpl.h's
		// constructor: a factory returning a fresh repl_pending_input_stream,
		// so a step needing a value suspends instead of blocking on stdin).
		auto& ctx = *definitions<node_t>::instance().get_io_context();
		auto saved_factory = ctx.console_input_factory;
		ctx.console_input_factory = [](const std::string&) {
			return std::make_shared<repl_pending_input_stream>();
		};
		// Restore the global io_context's console_input_factory on scope
		// exit, even if a REQUIRE below fails/throws -- it is a
		// process-wide singleton every other test in this binary relies on
		// staying unset (the default: blocking console_prompt_input_stream).
		struct restore_factory {
			io_context<node_t>& ctx;
			std::function<std::shared_ptr<serialized_constant_input_stream>(
				const std::string&)> saved;
			~restore_factory() { ctx.console_input_factory = saved; }
		} restore{ ctx, saved_factory };

		auto out = std::make_shared<vector_output_stream>();
		interpreter_options opts;
		opts.output_remaps["o"] = out; // "i" deliberately unmapped: falls
						// through to console_input_factory
		auto maybe_i = tau_api::get_interpreter(
			"type Point = {a: sbf, b: sbf}. "
			"i:Point := in console. o:Point := out console. "
			"o[t] = i[t].", opts);
		REQUIRE(maybe_i.has_value());
		auto& i = maybe_i.value();

		// First step: no value available yet -- the non-blocking stream
		// suspends the step (returns "") rather than blocking on stdin.
		CHECK(!tau_api::step(i).has_value());

		// Find the awaiting stream exactly as continue_running now does,
		// for every member var registered for the tuple stream, and
		// confirm every one resolves to the SAME shared instance: one
		// physical stream (and therefore exactly one pending request) for
		// the whole tuple, no matter how many flattened members
		// interpreter::inputs lists separately.
		REQUIRE(i.inputs.size() == 2); // i.a, i.b
		std::shared_ptr<repl_pending_input_stream> found;
		for (auto& [var, stream] : i.inputs) {
			(void)var; // every entry checked below, name unused
			auto rp = find_repl_pending_input<node_t>(stream);
			REQUIRE(rp != nullptr);
			CHECK(rp->awaiting());
			CHECK(rp->awaiting_time_point() == 0);
			if (!found) found = rp;
			else CHECK(found == rp); // identical shared physical stream
		}
		REQUIRE(found != nullptr);

		// set() the whole tuple literal (as the REPL would, once the user
		// answers the prompt) and confirm it unblocks the step: the SAME
		// repl_pending_input_stream backs every member, so one set() is
		// enough for the group.
		found->set("{ a: \"0\", b: \"1\" }");
		auto maybe_out = tau_api::step(i);
		REQUIRE(maybe_out.has_value());
		REQUIRE(out->get_values().size() == 1);
		CHECK(out->get_values()[0] == "{ a: \"0\", b: \"1\" }");
	}
}

// AP2-1: interpreter::step() calls maybe_gc(), and bintree<node>::gc()
// destroys every node that is neither reachable from a live htref nor in
// the keep set collect_live_refs() builds. The REPL keeps its rec-relation
// and I/O definitions as raw trefs, so a `run` in the same session freed
// nodes the REPL kept reading afterwards (defs listing, re-adding the defs
// to the next spec, the remaining commands of a multi-command line).
//
// The test reproduces the sweep directly instead of going through a run:
// what matters is that a definition the REPL is still holding survives a
// gc whose keep set does not mention it.
//
// ORDER-SENSITIVE: this suite clears the process-wide definitions table and
// sweeps the process-wide node table with an empty keep set, so anything a
// later case in this binary still held as a raw tref would be freed under it.
// Keep it last in the file, and add new cases above it.
TEST_SUITE("repl gc pinning") {

	TEST_CASE("a REPL rec-relation definition survives a gc sweep") {
		using repl_t = repl_evaluator<qint, qlt, nlang_ba, bv,
			sbf_ba, hsb>;
		auto& defs = definitions<node_t>::instance();
		defs.clear();
		repl_t::options o;
		o.status = o.colors = o.print_benchmarks = false;
		o.debug_repl = false;
		o.severity = boost::log::trivial::error;
		repl_t re(o);
		re.eval("p(X) := X = 0.");

		auto rules = defs.get_sym_defs();
		REQUIRE( rules.size() == 1 );
		// The node the REPL stored in rr_defs is the hash-consed parent
		// of the head/body pair it registered with `definitions`, so
		// rebuilding it must not allocate anything new. If this REQUIRE
		// fails the reconstruction no longer matches what def_rr_cmd
		// stores, not the pinning under test.
		const size_t m_before = tau::m_size();
		tref rr = tau::get(tau::rec_relation,
			rules[0].first->get(), rules[0].second->get());
		REQUIRE( rr != nullptr );
		REQUIRE( tau::m_size() == m_before );

		// gc() populates `keep` with every node that survived, but only
		// when it actually sweeps -- assert that it did.
		std::unordered_set<tref> keep;
		bintree<node_t>::gc(keep);
		REQUIRE( tau::m_size() < m_before );
		CHECK( keep.contains(rr) );
	}
}
