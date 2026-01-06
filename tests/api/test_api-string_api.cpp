// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"

#include "api.h"

TEST_SUITE("String API") {

	TEST_CASE("using get_inputs_for_step") {
		using tau_api = api<node_t>;
		// Make the interpreter for a given specification as string
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].");
		CHECK(maybe_i.has_value());
		auto& i = maybe_i.value();
		std::vector<std::string> collected_outputs;

		size_t step = 0;
		while (step < 3) {
			step++;

			TAU_LOG_INFO << "Step " << step << "\n";

			auto inputs = tau_api::get_inputs_for_step(i);
			TAU_LOG_INFO << "Inputs:\n";
			if (inputs.empty()) TAU_LOG_INFO << "No inputs\n";
			for (auto& in : inputs)
				TAU_LOG_INFO << in.first << "[" << in.second << "]\n";

			std::map<stream_at, std::string> assigned_inputs;
			for (auto& in : inputs) {
				assigned_inputs[in] = step % 2 == 0 ? "T." : "F.";
				TAU_LOG_INFO << "Assigned " << in.first << "[" << in.second << "] = `" << assigned_inputs[in] << "`";
			}

			auto maybe_outputs = tau_api::step(i, assigned_inputs);
			CHECK(maybe_outputs.has_value());
			auto outputs = maybe_outputs.value();

			TAU_LOG_INFO << "Outputs:\n";
			if (outputs.empty()) TAU_LOG_INFO << "No outputs\n";
			for (auto& [stream, value] : outputs) {
				collected_outputs.push_back(value);
				TAU_LOG_INFO << stream.first << "[" << stream.second << "] = `" << value << "`\n";
			}

		}
		std::stringstream ss;
		for (auto& output : collected_outputs) ss << output << " ";
		TAU_LOG_TRACE << "Collected outputs: " << ss.str();
		CHECK(collected_outputs == std::vector<std::string>({ "0", "1", "0" }));
	}

	TEST_CASE("with remapped streams") {
		using tau_api = api<node_t>;
		std::vector<std::string> i_values = { "T", "F", "T" };
		// Remap input and output streams from default console to custom streams
		interpreter_options opts;
		auto o_stream = std::make_shared<vector_output_stream>();
		opts.input_remaps["i"] = std::make_shared<vector_input_stream>(
			i_values);
		opts.output_remaps["o"] = o_stream;
		// Make the interpreter for a given specification as string
		auto maybe_i = tau_api::get_interpreter("o[t] = i[t].", opts);
		CHECK(maybe_i.has_value());
		auto& i = maybe_i.value();

		size_t step = 0;
		while (step < 3) {
			step++;

			TAU_LOG_INFO << "Step " << step << "\n";

			auto maybe_outputs = tau_api::step(i);
			CHECK(maybe_outputs.has_value());
			auto outputs = maybe_outputs.value();

			TAU_LOG_INFO << "Outputs:\n";
			if (outputs.empty()) TAU_LOG_INFO << "No outputs\n";
			for (auto& [stream, value] : outputs)
				TAU_LOG_INFO << stream.first << "[" << stream.second << "] = `" << value << "`\n";


			auto o_values = o_stream->get_values();

			for (size_t t = 0; t < i_values.size(); t++)
				TAU_LOG_TRACE << "i_values[" << t << "]: " << i_values[t];
			if (i_values.empty()) TAU_LOG_TRACE << "No inputs";
			for (size_t t = 0; t < o_values.size(); t++)
				TAU_LOG_TRACE << "o_values[" << t << "]: " << o_values[t];
			if (o_values.empty()) TAU_LOG_TRACE << "No outputs";
			CHECK( o_values.size() == i_values.size() );
			CHECK( o_values == i_values );
		}
	}

}
