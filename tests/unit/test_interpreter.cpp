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
}
