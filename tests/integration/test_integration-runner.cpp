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

TEST_SUITE("only outputs") {

	// In this test suite we test the interpreter with only outputs in the Tau
	// specifications, no inputs are provided at all. The outputs are written
	// to the console.

	template<typename...BAs>
	struct output_bdd_console {

		bool write(const assignment<BAs...>& outputs) {
			// for each stream in out.streams, write the value from the solution
			for (const auto& [var, value]: outputs)
				std::cout << var << " <- " << value << "\n";
			return true; // success (always)
		}

		std::optional<type> type_of(const nso<BAs...>& var) {
			return { "bdd" }; // bdd (always)
		}
	};

	template<typename...BAs>
	struct input_bdd_vector {

		std::optional<assignment<BAs...>> read() {
			if (inputs.empty()) return { assignment<BAs...>{} };
			if (current == inputs.size()) return {};
			return inputs[current++];
		}

		std::optional<type> type_of(const nso<BAs...>& var) {
			return { "bdd" }; // bdd (always)
		}

		std::vector<assignment<BAs...>> inputs;
		size_t current = 0;
	};

	assignment<tau_ba<bdd_binding>, bdd_binding> run_test(const char* sample,
			const size_t& times) {
		auto sample_src = make_tau_source(sample);
		auto phi_inf = make_nso_rr_using_factory<tau_ba<bdd_binding>, bdd_binding>(sample_src).main;

		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << phi_inf << "\n";
		#endif // DEBUG

		// prepare inputs, outputs and the interpreter
		input_bdd_vector<tau_ba<bdd_binding>, bdd_binding> inputs;
		output_bdd_console<tau_ba<bdd_binding>, bdd_binding> outputs;
		auto runner = make_interpreter(phi_inf, inputs, outputs).value();

		// we read the inputs only once (they are always empty in this test suite)
		auto in = inputs.read();

		for (size_t i = 0; i < times; ++i) {
			// we execute the i-th step
			auto out = runner.step(in.value());

			if (out.size() == 0) {
				FAIL("empty output, check phi_inf formula: ", phi_inf); // no output
			}

			#ifdef DEBUG
			std::cout << "step " << i << ": ";
			for (const auto& [var, value]: out)
				std::cout << var << " <- " << value << " ... ";
			std::cout << "\n";
			#endif // DEBUG
		}

		return runner.memory;
	}

	TEST_CASE("configuration") {
		// initialize the bdd library
		bdd_init<Bool>();
		// configuring boost logging only for errors
		core::get()->set_filter(trivial::severity >= trivial::error);
		add_console_log(cout, keywords::format =
			expressions::stream << expressions::smessage);
	}

	TEST_CASE("o1[t] = 0") {
		const char* sample = "o1[t] = 0.";
		auto memory = run_test(sample, 2);
		// TODO (HIGH) add some checks
	}

	TEST_CASE("o1[t] = {bdd: a}") {
		const char* sample = "o1[t] = {bdd: a}.";
		auto memory = run_test(sample, 2);
		// TODO (HIGH) add some checks
	}

	TEST_CASE("o1[t] | o2[t]= 0") {
		const char* sample = "o1[t] | o2[t]= 0.";
		auto memory = run_test(sample, 2);
		// TODO (HIGH) add some checks
	}

	TEST_CASE("o1[t] + o1[t-1] = 1") {
		const char* sample = "o1[t] + o1[t-1] = 1.";
		auto memory = run_test(sample, 2);
		// TODO (HIGH) add some checks
	}

	TEST_CASE("o1[t] & o1[t-1] = 1") {
		const char* sample = "o1[t] & o1[t-1] = 1.";
		auto memory = run_test(sample, 2);
		// TODO (HIGH) add some checks
	}

//	TEST_CASE("o1[t] & o1[t-1]' = 0 && o1[t] & o1[t-1] != 0 && o1[t] != o1[t-1]") {
//		const char* sample = "o1[t] & o1[t-1]' = 0 && o1[t] & o1[t-1] != 0 && o1[t] != o1[t-1].";
//		auto memory = run_test(sample, 4);
//		// TODO (HIGH) add some checks
//	}

	TEST_CASE("o1[t] & o1[t-1]' = 0 && o1[t] & o1[t-1] != 0 && o1[t] != o1[t-1]") {
		const char* sample = "o1[t] & o1[t-1]' = 0 && o1[t] & o1[t-1] != 0 && o1[t] != o1[t-1].";
		auto memory = run_test(sample, 4);
		// TODO (HIGH) add some checks
	}
}
