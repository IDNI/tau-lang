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

#include "doctest.h"
#include "runner.h"
#include "bdd_binding.h"

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

/*template<typename...BAs>
bool test_output(const outputs<bdd_binding>& outs,
		const std::map<stream_ba<BAs...>, vector<nso<BAs...>>>& expected) {
	// get the nso factory
	nso_factory<BAs...> factory;
	// read each output line by line and check the result
	for (const auto& [stream, ofile]: outs.streams) {
		//ofile.close();
		ifstream ifile(stream.file_name);
		std::string line;
		for (auto& current: expected) {
			std::getline(ifile, line);
			if (factory.parse(line) != current) return false;
		}
	}
	// TODO check the ouput just by substituting in the equations as in the solver,
	// no need to provide expected output
	return true;
}

TEST_SUITE("runner: only outputs") {

	TEST_CASE("o1[t] = 0") {
		const char* sample = "o1[t] = 0.";
		auto sample_src = make_tau_source(sample);
		nso<bdd_binding> sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src).main;
		auto o1 = sample_formula
			| tau_parser::bf_eq
			| tau_parser::bf
			| tau_parser::variable
			| tau_parser::io_var
			| tau_parser::out
			| tau_parser::out_var_name
			| optional_value_extractor<nso<bdd_binding>>;
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << sample_formula << "\n";
		std::cout << "out var: " << o1 << "\n";
		#endif // DEBUG
		stream_ba<bdd_binding> out_ba = {.file_name = random_file(), .name = o1, .type = "bdd"};
		inputs<bdd_binding> ins;
		outputs<bdd_binding> outs;
		std::vector<nso<bdd_binding>> expected = {_0<bdd_binding>};

		//CHECK ( test_output<bdd_binding>(outs, expected) );
	}

}

TEST_SUITE("runner: only inputs") {

	TEST_CASE("i1[t] = 0") {
		const char* sample = "i1[t] = 0.";
		auto sample_src = make_tau_source(sample);
		nso<bdd_binding> sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src).main;
		auto i1 = sample_formula | tau_parser::bf_eq | optional_value_extractor<nso<bdd_binding>>;
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << sample_formula << "\n";
		std::cout << "in var: " << i1 << "\n";
		#endif // DEBUG
		inputs<bdd_binding> ins;
		outputs<bdd_binding> outs;
		CHECK ( true );
	}

}

TEST_SUITE("runner: simple cases") {

	TEST_CASE("o1 = i1") {
		const char* sample = "o1 = i1.";
		auto sample_src = make_tau_source(sample);
		nso<bdd_binding> sample_formula = make_nso_rr_using_factory<bdd_binding>(sample_src).main;
		// get input variable
		// get output variable
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << sample_formula << "\n";
		std::cout << "in var: " << "input variable" << "\n";
		std::cout << "out var: " << "output variable" << "\n";
		#endif // DEBUG
		inputs<bdd_binding> ins;
		outputs<bdd_binding> outs;
		CHECK ( true );
	}

}

TEST_SUITE("runner: only outputs") {

	TEST_CASE("o1[t] = 0") {
		const char* phi_inf_str = "o1[t] = 0.";
		auto phi_inf_src = make_tau_source(phi_inf_str);
		nso<tau_ba<bdd_binding>, bdd_binding> phi_inf = make_nso_rr_using_factory<tau_ba<bdd_binding>, bdd_binding>(phi_inf_src).main;
		auto o1 = phi_inf
			| tau_parser::bf_eq
			| tau_parser::bf
			| tau_parser::variable
			| tau_parser::io_var
			| tau_parser::out
			| tau_parser::out_var_name
			| optional_value_extractor<nso<tau_ba<bdd_binding>, bdd_binding>>;
		auto o1_0 = build_out_var(o1, 0);
		#ifdef DEBUG
		std::cout << "------------------------------------------------------\n";
		std::cout << "sample: " << phi_inf << "\n";
		std::cout << "out var: " << o1 << "\n";
		#endif // DEBUG
		interpreter<tau_ba<bdd_binding>, bdd_binding> interp;
		auto output = interp.timed_step({});
		CHECK ( output[o1_0] == _0<tau_ba<bdd_binding>, bdd_binding> );
	}

}*/
