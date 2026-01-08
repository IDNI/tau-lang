// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"

#include "api.h"

TEST_SUITE("String API") {

	TEST_CASE("using get_inputs_for_step") {
		using tau_api = api<node_t>;

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
				TAU_LOG_TRACE << input_at.first << "[" << input_at.second << "]";
#endif

			// Assign values to the inputs (spec has only `i` input stream)
			std::map<stream_at, std::string> assigned_inputs;
			for (auto& input_at : inputs) {
				assigned_inputs[input_at] = step % 2 == 0 ? "T." : "F.";
				DBG(TAU_LOG_TRACE << "Assigned " << input_at.first << "[" << input_at.second << "] = `" << assigned_inputs[input_at] << "`";)
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
				DBG(TAU_LOG_TRACE << output_at.first << "[" << output_at.second << "] = `" << value << "`";)
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
		using tau_api = api<node_t>;

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
				TAU_LOG_TRACE << output_at.first << "[" << output_at.second << "] = `" << value << "`";
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
