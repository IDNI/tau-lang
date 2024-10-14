// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <iostream>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "doctest.h"
#include "runner.h"
#include "bdd_binding.h"

using namespace boost::log;

std::string random_file(const std::string& extension = ".out", const std::string prefix = "/tmp/") {
    // define the characters to use in the random string
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	// length of the random string
    const size_t length = 10;
    // random number generator
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
    std::ostringstream oss;
	// add default location
	oss << prefix;
    // generate random string
    for (size_t i = 0; i < length; ++i) oss << charset[dist(generator)];
    // append the file extension and return the file name
    oss << extension;
    return oss.str();
}

template<typename...BAs>
struct output_bdd_console {

	bool write(const assignment<BAs...>& outputs) {
		// for each stream in out.streams, write the value from the solution
		for (const auto& [var, value]: outputs)
			std::cout << var << " <- " << value << "\n";
		return true; // success (always)
	}

	std::optional<type> type_of(const nso<BAs...>&) {
		return { "bdd" }; // bdd (always)
	}
};

template<typename...BAs>
struct input_bdd_vector {

	input_bdd_vector() = default;
	input_bdd_vector(std::vector<assignment<BAs...>>& inputs): inputs(inputs) {}

	std::optional<assignment<BAs...>> read() {
		if (inputs.empty()) return { assignment<BAs...>{} };
		if (current == inputs.size()) return {};
		return inputs[current++];
	}

	std::optional<type> type_of(const nso<BAs...>&) {
		return { "bdd" }; // bdd (always)
	}

	std::vector<assignment<BAs...>> inputs;
	size_t current = 0;
};

assignment<tau_ba<bdd_binding>, bdd_binding> run_test(const char* sample,
		input_bdd_vector<tau_ba<bdd_binding>, bdd_binding>& inputs,
		output_bdd_console<tau_ba<bdd_binding>, bdd_binding>& outputs,
		const size_t& times) {
	auto sample_src = make_tau_source(sample);
	auto phi_inf = make_nso_rr_using_factory<tau_ba<bdd_binding>, bdd_binding>(sample_src).main;

	#ifdef DEBUG
	std::cout << "run_test/------------------------------------------------------\n";
	std::cout << "run_test/sample: " << sample << "\n";
	#endif // DEBUG

	auto runner = make_interpreter(phi_inf, inputs, outputs).value();

	// we read the inputs only once (they are always empty in this test suite)

	for (size_t i = 0; i < times; ++i) {
		// we execute the i-th step
		auto in = inputs.read();

		#ifdef DEBUG
		std::cout << "run_test/input[" << i << "]: ";
		if (in.has_value()) {
			for (const auto& [var, value]: in.value())
				std::cout << var << " <- " << value << " ... ";
			std::cout << "\n";
		} else std::cout << "{}\n"; // no input
		#endif // DEBUG

		auto out = runner.step(in.value());

		if (out.size() == 0) {
			runner.memory.clear();
			#ifdef DEBUG
			std::cout << "run_test/output[" << i << "]: {}\n"; // no output
			#endif // DEBUG
			break;
		}

		#ifdef DEBUG
		std::cout << "run_test/output[" << i << "]: ";
		for (const auto& [var, value]: out) {
			std::cout << var << " <- " << value << " ... ";
			if (auto io_vars = find_top(value, is_non_terminal<tau_parser::io_var, tau_ba<bdd_binding>, bdd_binding>); io_vars) {
				std::cout << "run_test/output[" << i << "]: unexpected io_var " << io_vars.value() << "\n";
				runner.memory.clear();
				break;
			}
		}
		std::cout << "\n";
		#endif // DEBUG

		if (runner.memory.empty()) break;
	}

	return runner.memory;
}

assignment<tau_ba<bdd_binding>, bdd_binding> run_test(const char* sample,
		const size_t& times) {
	input_bdd_vector<tau_ba<bdd_binding>, bdd_binding> inputs;
	output_bdd_console<tau_ba<bdd_binding>, bdd_binding> outputs;
	return run_test(sample, inputs, outputs, times);
}

assignment<tau_ba<bdd_binding>, bdd_binding> run_test(const char* sample,
		input_bdd_vector<tau_ba<bdd_binding>, bdd_binding>& inputs,
		const size_t& times) {
	output_bdd_console<tau_ba<bdd_binding>, bdd_binding> outputs;
	return run_test(sample, inputs, outputs, times);
}

assignment<tau_ba<bdd_binding>, bdd_binding> run_test(const char* sample,
		output_bdd_console<tau_ba<bdd_binding>, bdd_binding>& outputs,
		const size_t& times) {
	input_bdd_vector<tau_ba<bdd_binding>, bdd_binding> inputs;
	return run_test(sample, inputs, outputs, times);
}

TEST_SUITE("configuration") {

	TEST_CASE("configuration") {
		// initialize the bdd library
		bdd_init<Bool>();
		// configuring boost logging only for errors
		core::get()->set_filter(trivial::severity >= trivial::error);
		add_console_log(cout, keywords::format =
			expressions::stream << expressions::smessage);
	}
}

TEST_SUITE("only outputs") {

	// In this test suite we test the interpreter with only outputs in the Tau
	// specifications, no inputs are provided at all. The outputs are written
	// to the console.

	// coonstant test cases

	TEST_CASE("o1[t] = 0") {
		const char* sample = "o1[t] = 0.";
		auto memory = run_test(sample, 2);
		CHECK ( !memory.empty() );
	}

	TEST_CASE("o1[t] = {bdd: a}") {
		const char* sample = "o1[t] = {bdd: a}.";
		auto memory = run_test(sample, 2);
		CHECK ( !memory.empty() );
	}

	TEST_CASE("o1[0] = 1") {
		const char* sample = "o1[0] = 1.";
		auto memory = run_test(sample, 3);
		CHECK ( !memory.empty() );
	}

	TEST_CASE("o1[0] = {bdd: a}") {
		const char* sample = "o1[0] = {bdd: a}.";
		auto memory = run_test(sample, 3);
		CHECK ( !memory.empty() );
	}

	// TODO (HIGH) check with Ohad the proper output in this case.
	// The spec is T, i.e. all outputs are valid. However, due to our inner
	// implementation, we are not defining outputs as we have T in the spec.
	// and hence, we are not able to provide outputs.
	// One possibility is return an optional instead of an assignment and
	// return an optional with an empty assigment in this case.
	TEST_CASE("o1[t] & o1[t]'= 0") {
		const char* sample = "o1[t] & o1[t]' = 0.";
		auto memory = run_test(sample, 2);
		CHECK ( memory.empty() );
	}

	TEST_CASE("o1[0] = 1 && o1[t] = o1[t-1]") {
		const char* sample = "o1[0] = 1 && o1[t] = o1[t-1].";
		auto memory = run_test(sample, 2);
		CHECK ( !memory.empty() );
	}

	TEST_CASE("o1[0] = {bdd: a} && o1[t] = o1[t-1]") {
		const char* sample = "o1[0] = {bdd: a} && o1[t] = o1[t-1].";
		auto memory = run_test(sample, 3);
		CHECK ( !memory.empty() );
	}

	TEST_CASE("o1[0] = {bdd: a} && o1[t] != o1[t-1]") {
		const char* sample = "o1[0] = {bdd: a} && o1[t] = o1[t-1].";
		auto memory = run_test(sample, 3);
		CHECK ( !memory.empty() );
	}

	TEST_CASE("o1[t] | o2[t]= 0") {
		const char* sample = "o1[t] | o2[t]= 0.";
		auto memory = run_test(sample, 2);
		CHECK ( !memory.empty() );
	}

	TEST_CASE("o1[t] & o1[t-1] = 1") {
		const char* sample = "o1[t] & o1[t-1] = 1.";
		auto memory = run_test(sample, 2);
		CHECK ( !memory.empty() );
	}

	TEST_CASE("o1[t] + o1[t-1] = 1") {
		const char* sample = "o1[t] + o1[t-1] = 1.";
		auto memory = run_test(sample, 2);
		CHECK ( !memory.empty() );
	}

	TEST_CASE("o1[0] = {bdd:a} && o1[t] < o1[t-1] && o1[t] != 0") {
		const char* sample = "o1[0] = {bdd:a} && o1[t] < o1[t-1] && o1[t] != 0.";
		auto memory = run_test(sample, 4);
		CHECK ( !memory.empty() );
	}

	// increasing monotonicity (2)
	TEST_CASE("o1[0] = {bdd:a} && o1[t] > o1[t-1] && o1[t] != 1") {
		const char* sample = "o1[0] = {bdd:a} && o1[t] > o1[t-1] && o1[t] != 1.";
		auto memory = run_test(sample, 4);
		CHECK ( !memory.empty() );
	}

	// increasing monotonicity (3)
	TEST_CASE("o1[0] = 0 && o1[t] > o1[t-1] && o1[t] != 1") {
		const char* sample = "o1[0] = {bdd:a} && o1[t] > o1[t-1] && o1[t] != 1.";
		auto memory = run_test(sample, 4);
		CHECK ( !memory.empty() );
	}

	// increasing monotonicity (4)
	TEST_CASE("o1[t] > o1[t-1] && o1[t] != 1") {
		const char* sample = "o1[0] = {bdd:a} && o1[t] > o1[t-1] && o1[t] != 1.";
		auto memory = run_test(sample, 4);
		CHECK ( !memory.empty() );
	}

	TEST_CASE("<:a> o1[t] + <:b> o1[t]' = 0") {
		const char* sample = "<:a> o1[t] + <:b> o1[t]' = 0.";
		auto memory = run_test(sample, 8);
		// execution of this test should fail, i.e. memory should be empty
		CHECK ( memory.empty() );
	}

	TEST_CASE("o1[0] = <:c> && o1[t] = <:a> o1[t-1] + <:b> o1[t-1]'") {
		const char* sample = "o1[0] = <:c> && o1[t] = <:a> o1[t-1] + <:b> o1[t-1]'.";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.empty() );
	}

	// f(f(f(x))) = f(x) using uninterpreted constants
	TEST_CASE("o1[t] = <:a> o1[t-1] + <:b> o1[t-1]'") {
		const char* sample = "o1[t] = <:a> o1[t-1] + <:b> o1[t-1]'.";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.empty() );
	}

	// f(f(f(x))) = f(x) using constants
	TEST_CASE("o1[t] = {bdd:a} o1[t-1] + {bdd:b} o1[t-1]'") {
		const char* sample = "o1[t] = {bdd:a} o1[t-1] + {bdd:b} o1[t-1]'.";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.empty() );
	}
}


TEST_SUITE("with inputs") {

	input_bdd_vector<tau_ba<bdd_binding>, bdd_binding> build_i1_inputs(
			std::vector<nso<tau_ba<bdd_binding>, bdd_binding>> values) {
		std::vector<assignment<tau_ba<bdd_binding>, bdd_binding>> assignments;
		for (const auto& value: values) {
			assignment<tau_ba<bdd_binding>, bdd_binding> assignment;
			assignment[build_in_var_name<tau_ba<bdd_binding>, bdd_binding>(1)] = value;
			assignments.push_back(assignment);
		}
		input_bdd_vector<tau_ba<bdd_binding>, bdd_binding> ins(assignments);
		return ins;
	}

	TEST_CASE("i1[t] = o1[t]") {
		const char* sample = "i1[t] = o1[t].";
		auto ins = build_i1_inputs({
			_1<tau_ba<bdd_binding>, bdd_binding>,
			_0<tau_ba<bdd_binding>, bdd_binding>,
			_0<tau_ba<bdd_binding>, bdd_binding> });
		auto memory = run_test(sample, ins, 3);
		CHECK ( !memory.empty() );
	}

	// In this case, we get an error as the output should be equal to the
	// input all the time, but the output is set to 0 at the beginning.
	TEST_CASE("i1[t] = o1[t] && o1[0] = 0") {
		const char* sample = "i1[t] = o1[t] && o1[0] = 0.";
		auto ins = build_i1_inputs({
			_1<tau_ba<bdd_binding>, bdd_binding>,
			_1<tau_ba<bdd_binding>, bdd_binding>,
			_1<tau_ba<bdd_binding>, bdd_binding> });
		auto memory = run_test(sample, ins, 3);
		CHECK ( memory.empty() );
	}

	// In this case we get the sequence 0, 1, 1 as the output is set to 1
	// at the beginning.
	TEST_CASE("i1[t-1] = o1[t] && o1[0] = 0") {
		const char* sample = "i1[t-1] = o1[t] && o1[0] = 0.";
		auto ins = build_i1_inputs({
			_1<tau_ba<bdd_binding>, bdd_binding>,
			_1<tau_ba<bdd_binding>, bdd_binding>,
			_1<tau_ba<bdd_binding>, bdd_binding> });
		auto memory = run_test(sample, ins, 2);
		CHECK ( !memory.empty() );
	}

}

TEST_SUITE("with inputs and outputs") {

}

TEST_SUITE("test inputs") {

}

TEST_SUITE("test outputs") {

}
