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
