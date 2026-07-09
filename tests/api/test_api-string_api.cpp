// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"

using tau_api = api<node_t>;

#include "test_api-fixture.h"

TEST_SUITE("Tau API - string") {
	TEST_CASE_FIXTURE(api_fixture, "Querying") {
		for (const auto& term : terms) {
			CHECK(tau_api::is_term(term));
			CHECK(!tau_api::is_formula(term));
		}
		for (const auto& formula : formulas) {
			CHECK(!tau_api::is_term(formula));
			CHECK(tau_api::is_formula(formula));
		}
	}
}

// AP-8: only get_interpreter (below) had negative/malformed-input tests;
// every other string-API parse entry point was untested for the malformed
// or empty-string case. Each of these mirrors the style of the two
// get_interpreter negative tests: feed a syntactically invalid (or empty)
// string in and check the function fails gracefully (no crash, no value)
// instead of asserting/crashing on the null tref that the underlying parse
// produces.
TEST_SUITE("Tau API - string - malformed input") {
	static const strings malformed = { "", "x ) ( invalid !!!", "o[t] =" };

	TEST_CASE_FIXTURE(api_fixture, "is_term / is_formula reject malformed input") {
		for (const auto& s : malformed) {
			CHECK(!tau_api::is_term(s));
			CHECK(!tau_api::is_formula(s));
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "apply_def/apply_defs/apply_all_defs reject malformed input") {
		for (const auto& s : malformed) {
			CHECK(!tau_api::apply_def(s, "x + 1").has_value());
			CHECK(!tau_api::apply_def("f(x) := x + 1", s).has_value());
			CHECK(!tau_api::apply_defs(std::set<std::string>{ s }, "x + 1")
				.has_value());
			CHECK(!tau_api::apply_all_defs(s).has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "substitute rejects malformed input") {
		for (const auto& s : malformed) {
			CHECK(!tau_api::substitute(s, "x", "y").has_value());
			CHECK(!tau_api::substitute("x", s, "y").has_value());
			CHECK(!tau_api::substitute("x", "x", s).has_value());
			CHECK(!tau_api::substitute(
				s, std::map<std::string, std::string>{ { "x", "y" } })
				.has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "normal forms reject malformed input") {
		for (const auto& s : malformed) {
			CHECK(!tau_api::boole_normal_form(s).has_value());
			CHECK(!tau_api::dnf(s).has_value());
			CHECK(!tau_api::cnf(s).has_value());
			CHECK(!tau_api::nnf(s).has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "procedures reject malformed input") {
		for (const auto& s : malformed) {
			CHECK(!tau_api::syntactic_term_simplification(s).has_value());
			CHECK(!tau_api::syntactic_formula_simplification(s).has_value());
			CHECK(!tau_api::normalize_term(s).has_value());
			CHECK(!tau_api::normalize_formula(s).has_value());
			CHECK(!tau_api::anti_prenex(s).has_value());
			CHECK(!tau_api::eliminate_quantifiers(s).has_value());
			CHECK(!tau_api::simplify(s).has_value());
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "formula checks reject malformed input") {
		for (const auto& s : malformed) {
			// realizable/valid/valid_spec/sat all bottom out on a null
			// tref, which every one of them treats as "not satisfied".
			CHECK(!tau_api::realizable(s));
			CHECK(!tau_api::valid(s));
			CHECK(!tau_api::valid_spec(s));
			CHECK(!tau_api::sat(s));
			// unrealizable/unsat are defined as the negation of the
			// above, so a malformed input reports true here.
			CHECK(tau_api::unrealizable(s));
			CHECK(tau_api::unsat(s));
		}
	}

	TEST_CASE_FIXTURE(api_fixture, "solve/lgrs reject malformed input") {
		for (const auto& s : malformed) {
			CHECK(!tau_api::solve(s, solver_mode::general).has_value());
			CHECK(!tau_api::lgrs(s).has_value());
		}
	}
}

TEST_SUITE("Tau API - string - execution") {

	TEST_CASE("handle syntax error") {
		auto maybe_i = tau_api::get_interpreter("o[t] =");
		CHECK(!maybe_i.has_value());
	}

	TEST_CASE("handle type error") {
		auto maybe_i = tau_api::get_interpreter("o[t]:tau = i[t]:sbf");
		CHECK(!maybe_i.has_value());
	}


	TEST_CASE("using get_inputs_for_step") {

		// Make the interpreter for a given specification as a string
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].");
		CHECK(maybe_i.has_value());
		auto& i = maybe_i.value();

		std::vector<std::string> collected_outputs;

		size_t step = 0;
		while (step < 3) {

			DBG(TAU_LOG_TRACE << "Step " << step;)
			step++;

			// Find out what inputs are expected for the next step
			auto inputs = tau_api::get_inputs_for_step(i);
#ifdef DEBUG
			TAU_LOG_TRACE << "Inputs:";
			if (inputs.empty()) TAU_LOG_TRACE << "No inputs";
			for (auto& input_at : inputs)
				TAU_LOG_TRACE << input_at.name << "[" << input_at.time_point<< "]";
#endif

			// Assign values to the inputs (spec has only `i` input stream)
			std::map<stream_at, std::string> assigned_inputs;
			for (auto& input_at : inputs) {
				assigned_inputs[input_at] = step % 2 == 0 ? "T." : "F.";
				DBG(TAU_LOG_TRACE << "Assigned " << input_at.name << "[" << input_at.time_point << "] = `" << assigned_inputs[input_at] << "`";)
			}

			// Step the interpreter with the assigned values
			// and retrieve outputs
			auto maybe_outputs = tau_api::step(i, assigned_inputs);
			CHECK(maybe_outputs.has_value());
			auto outputs = maybe_outputs.value();

#ifdef DEBUG
			TAU_LOG_TRACE << "Outputs:";
			if (outputs.empty()) TAU_LOG_TRACE << "No outputs";
#endif
			// Collect the outputs (we have only one output stream `o`)
			for (auto& [output_at, value] : outputs) {
				collected_outputs.push_back(value);
				DBG(TAU_LOG_TRACE << output_at.name << "[" << output_at.time_point << "] = `" << value << "`";)
			}
		}
#ifdef DEBUG
		std::stringstream ss;
		for (size_t i = 0; i < collected_outputs.size(); ++i)
			ss << (i > 0 ? ", " : " ") << collected_outputs[i];
		TAU_LOG_TRACE << "Collected outputs: " << ss.str();
#endif
		CHECK(collected_outputs == std::vector<std::string>({ "F", "T", "F" }));
	}

	TEST_CASE("with remapped streams") {

		// Remap input and output streams from default console to custom streams
		std::vector<std::string> i_values = { "T", "F", "T" };
		auto i_stream = std::make_shared<vector_input_stream>(i_values);
		auto o_stream = std::make_shared<vector_output_stream>();
		interpreter_options opts;
		opts.input_remaps["i"] = i_stream;
		opts.output_remaps["o"] = o_stream;

		// Make the interpreter for a given specification as string
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].", opts);
		CHECK(maybe_i.has_value());
		auto& i = maybe_i.value();

		size_t step = 0;
		while (step < 3) {

			DBG(TAU_LOG_TRACE << "Step " << step;)
			step++;

			// Step the interpreter and retrieve outputs
			auto maybe_outputs = tau_api::step(i);
			CHECK(maybe_outputs.has_value());
			auto outputs = maybe_outputs.value();

#ifdef DEBUG
			TAU_LOG_TRACE << "Outputs:";
			if (outputs.empty()) TAU_LOG_TRACE << "No outputs";
			for (auto& [output_at, value] : outputs)
				TAU_LOG_TRACE << output_at.name << "[" << output_at.time_point << "] = `" << value << "`";
#endif
		}

		auto o_values = o_stream->get_values();
#ifdef DEBUG
		std::stringstream ss;
		ss << "Input values: [ ";
		for (size_t t = 0; t < i_values.size(); ++t)
			ss << (t > 0 ? ", " : " ") << i_values[t];
		ss << " ]";
		TAU_LOG_TRACE << ss.str();
		ss.str("");
		ss << "Output values: [ ";
		for (size_t t = 0; t < o_values.size(); ++t)
			ss << (t > 0 ? ", " : " ") << o_values[t];
		ss << " ]";
		TAU_LOG_TRACE << ss.str();
#endif
		CHECK( o_values.size() == i_values.size() );
		CHECK( o_values == i_values );
	}
}
