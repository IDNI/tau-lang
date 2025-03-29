// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentiTd cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cassert>

#include "doctest.h"
#include "boolean_algebras/sbf_ba.h"
#include "normalizer.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("bf operator hooks") {

	// we should get an error during parsing and hence return true if we get an error
	bool check_unbinded_hook(const char* sample) {
		auto tau_sample_src = make_tau_source(sample, { .start = tau_parser::bf });
		auto tau_sample = make_tau_code<tau_ba<sbf_ba>, sbf_ba>(tau_sample_src);

		#ifdef DEBUG
		std::string str(sample);
		if (tau_sample)
			std::cout << "sample: " << str << " expected error, got : " << tau_sample << "\n";
		else
			std::cout << "sample: " << str << " expected error, got it\n";
		#endif // DEBUG

		return tau_sample == nullptr;
	}

	// we should be able to parse the sample and the expected result should be the same
	bool check_hook(const char* sample, const char* expected) {
		auto tau_sample = make_nso_using_factory<
			tau_ba<sbf_ba>, sbf_ba>(sample, { .start = tau_parser::bf }).value();
		auto tau_expected = make_nso_using_factory<
			tau_ba<sbf_ba>, sbf_ba>(expected, { .start = tau_parser::bf }).value();

		#ifdef DEBUG
		std::string str(sample);
		std::cout << "sample: " << str << " expected: " << tau_expected << " got: " << tau_sample << "\n";
		#endif // DEBUG

		return tau_sample == tau_expected;
	}

	bool check_type(const char* sample, const char* type) {
		auto type_sample = make_nso_using_factory<
					tau_ba<sbf_ba>, sbf_ba>(
				sample, { .start = tau_parser::bf }).value()
			| tau_parser::bf_constant
			| tau_parser::type
			| optional_value_extractor<tau_depreciating<tau_ba<sbf_ba>, sbf_ba>>;
		auto type_expected = make_nso_using_factory<
				tau_ba<sbf_ba>, sbf_ba>(
			type, { .start = tau_parser::type }).value();

		#ifdef DEBUG
		std::string str(sample);
		if (type_sample)
			std::cout << "sample: " << str << " expected type: " << type_expected << " got: " << type_sample << "\n";
		else
			std::cout << "sample: " << str << " expected type: " << type_expected << " got: tau\n";
		#endif // DEBUG

		std::stringstream ss_sample, ss_expected;

		if (type_sample) ss_sample << type_sample;
		else ss_sample << "tau";

		ss_expected << type_expected;
		return ss_sample.str() == ss_expected.str();
	}

	TEST_CASE("conversion to 1/0") {
		CHECK( check_hook("{T}", "1:tau") );
		CHECK( check_hook("{F}", "0:tau") );

		CHECK( check_hook("{T}:tau", "1:tau") );
		CHECK( check_hook("{F}:tau", "0:tau") );

		CHECK( check_hook("{1}:sbf", "1:sbf") );
		CHECK( check_hook("{0}:sbf", "0:sbf") );
	}

	TEST_CASE("'") {
		CHECK( check_hook("0'", "1") );
		CHECK( check_hook("1'", "0") );

		CHECK( check_hook("0:sbf'", "1:sbf") );
		CHECK( check_hook("1:sbf'", "0:sbf") );

		CHECK( check_hook("0:tau'", "1:tau") );
		CHECK( check_hook("1:tau'", "0:tau") );

		CHECK( check_hook("x''", "x") );

		CHECK( check_type("{a}:sbf'", "sbf") );
		CHECK( check_type("{o1[t] = 0}'", "tau") );
		CHECK( check_type("{o1[t] = 0}:tau'", "tau") );
	}

	TEST_CASE("|") {
		CHECK( check_hook("1|0", "1") );
		CHECK( check_hook("0|1", "1") );
		CHECK( check_hook("1|1", "1") );
		CHECK( check_hook("0|0", "0") );

		CHECK( check_hook("1:sbf|0:sbf", "1:sbf") );
		CHECK( check_hook("0:sbf|1:sbf", "1:sbf") );
		CHECK( check_hook("1:sbf|1:sbf", "1:sbf") );
		CHECK( check_hook("0:sbf|0:sbf", "0:sbf") );

		CHECK( check_hook("1:sbf|0", "1:sbf") );
		CHECK( check_hook("0:sbf|1", "1:sbf") );
		CHECK( check_hook("1:sbf|1", "1:sbf") );
		CHECK( check_hook("0:sbf|0", "0:sbf") );

		CHECK( check_hook("1|0:sbf", "1:sbf") );
		CHECK( check_hook("0|1:sbf", "1:sbf") );
		CHECK( check_hook("1|1:sbf", "1:sbf") );
		CHECK( check_hook("0|0:sbf", "0:sbf") );

		CHECK( check_hook("1:tau|0:tau", "1:tau") );
		CHECK( check_hook("0:tau|1:tau", "1:tau") );
		CHECK( check_hook("1:tau|1:tau", "1:tau") );
		CHECK( check_hook("0:tau|0:tau", "0:tau") );

		CHECK( check_hook("1|0:tau", "1:tau") );
		CHECK( check_hook("0|1:tau", "1:tau") );
		CHECK( check_hook("1|1:tau", "1:tau") );
		CHECK( check_hook("0|0:tau", "0:tau") );

		CHECK( check_hook("1:tau|0", "1:tau") );
		CHECK( check_hook("0:tau|1", "1:tau") );
		CHECK( check_hook("1:tau|1", "1:tau") );
		CHECK( check_hook("0:tau|0", "0:tau") );

		CHECK( !check_hook("1:sbf|0:tau", "1") );
		CHECK( !check_hook("0:sbf|1:tau", "1") );
		CHECK( !check_hook("1:sbf|1:tau", "1") );
		CHECK( !check_hook("0:sbf|0:tau", "0") );

		CHECK( !check_hook("1:tau|0:sbf", "1") );
		CHECK( !check_hook("0:tau|1:sbf", "1") );
		CHECK( !check_hook("1:tau|1:sbf", "1") );
		CHECK( !check_hook("0:tau|0:sbf", "0") );

		CHECK( !check_hook("{1}:sbf|0:tau", "1") );
		CHECK( !check_hook("{0}:sbf|1:tau", "1") );
		CHECK( !check_hook("{1}:sbf|1:tau", "1") );
		CHECK( !check_hook("{0}:sbf|0:tau", "0") );

		CHECK( !check_hook("1:tau|{0}:sbf", "1") );
		CHECK( !check_hook("0:tau|{1}:sbf", "1") );
		CHECK( !check_hook("1:tau|{1}:sbf", "1") );
		CHECK( !check_hook("0:tau|{0}:sbf", "0") );

		CHECK( !check_hook("1:sbf|{F}:tau", "1") );
		CHECK( !check_hook("0:sbf|{T}:tau", "1") );
		CHECK( !check_hook("1:sbf|{T}:tau", "1") );
		CHECK( !check_hook("0:sbf|{F}:tau", "0") );

		CHECK( !check_hook("{T}:tau|1:sbf", "1") );
		CHECK( !check_hook("{F}:tau|0:sbf", "0") );
		CHECK( !check_hook("{T}:tau|1:sbf", "1") );
		CHECK( !check_hook("{F}:tau|0:sbf", "0") );

		CHECK( check_unbinded_hook("{1}:sbf|{F}:tau") );
		CHECK( check_unbinded_hook("{0}:sbf|{T}:tau") );
		CHECK( check_unbinded_hook("{1}:sbf|{T}:tau") );
		CHECK( check_unbinded_hook("{0}:sbf|{F}:tau") );

		CHECK( check_unbinded_hook("{T}:tau|{1}:sbf") );
		CHECK( check_unbinded_hook("{F}:tau|{0}:sbf") );
		CHECK( check_unbinded_hook("{T}:tau|{1}:sbf") );
		CHECK( check_unbinded_hook("{F}:tau|{0}:sbf") );

		CHECK( check_hook("1|x", "1") );
		CHECK( check_hook("x|1", "1") );
		CHECK( check_hook("0|x", "x") );
		CHECK( check_hook("x|0", "x") );
		CHECK( check_hook("x|x", "x") );
		CHECK( check_hook("x|x'", "1") );
		CHECK( check_hook("x'|x", "1") );

		CHECK( check_type("{a}:sbf|{b}:sbf", "sbf") );
		CHECK( check_type("{o1[t] = 0}|{o2[t] = 0}", "tau") );
		CHECK( check_type("{o1[t] = 0}:tau|{o2[t] = 0}", "tau") );
		CHECK( check_type("{o1[t] = 0}|{o2[t] = 0}:tau", "tau") );

	}

	TEST_CASE("&") {
		CHECK( check_hook("0&1", "0") );
		CHECK( check_hook("1&0", "0") );
		CHECK( check_hook("1&1", "1") );
		CHECK( check_hook("0&0", "0") );

		CHECK( check_hook("1:sbf&0:sbf", "0:sbf") );
		CHECK( check_hook("0:sbf&1:sbf", "0:sbf") );
		CHECK( check_hook("1:sbf&1:sbf", "1:sbf") );
		CHECK( check_hook("0:sbf&0:sbf", "0:sbf") );

		CHECK( check_hook("1:sbf&0", "0:sbf") );
		CHECK( check_hook("0:sbf&1", "0:sbf") );
		CHECK( check_hook("1:sbf&1", "1:sbf") );
		CHECK( check_hook("0:sbf&0", "0:sbf") );

		CHECK( check_hook("1&0:sbf", "0:sbf") );
		CHECK( check_hook("0&1:sbf", "0:sbf") );
		CHECK( check_hook("1&1:sbf", "1:sbf") );
		CHECK( check_hook("0&0:sbf", "0:sbf") );

		CHECK( check_hook("1:tau&0:tau", "0:tau") );
		CHECK( check_hook("0:tau&1:tau", "0:tau") );
		CHECK( check_hook("1:tau&1:tau", "1:tau") );
		CHECK( check_hook("0:tau&0:tau", "0:tau") );

		CHECK( check_hook("1&0:tau", "0:tau") );
		CHECK( check_hook("0&1:tau", "0:tau") );
		CHECK( check_hook("1&1:tau", "1:tau") );
		CHECK( check_hook("0&0:tau", "0:tau") );

		CHECK( check_hook("1:tau&0", "0:tau") );
		CHECK( check_hook("0:tau&1", "0:tau") );
		CHECK( check_hook("1:tau&1", "1:tau") );
		CHECK( check_hook("0:tau&0", "0:tau") );

		CHECK( !check_hook("1:sbf&0:tau", "0") );
		CHECK( !check_hook("0:sbf&1:tau", "0") );
		CHECK( !check_hook("1:sbf&1:tau", "1") );
		CHECK( !check_hook("0:sbf&0:tau", "0") );

		CHECK( !check_hook("1:tau&0:sbf", "0") );
		CHECK( !check_hook("0:tau&1:sbf", "0") );
		CHECK( !check_hook("1:tau&1:sbf", "1") );
		CHECK( !check_hook("0:tau&0:sbf", "0") );

		CHECK( !check_hook("{1}:sbf&0:tau", "0") );
		CHECK( !check_hook("{0}:sbf&1:tau", "0") );
		CHECK( !check_hook("{1}:sbf&1:tau", "1") );
		CHECK( !check_hook("{0}:sbf&0:tau", "0") );

		CHECK( !check_hook("1:tau&{0}:sbf", "0") );
		CHECK( !check_hook("0:tau&{1}:sbf", "0") );
		CHECK( !check_hook("1:tau&{1}:sbf", "1") );
		CHECK( !check_hook("0:tau&{0}:sbf", "0") );

		CHECK( !check_hook("1:sbf&{F}:tau", "0") );
		CHECK( !check_hook("0:sbf&{T}:tau", "0") );
		CHECK( !check_hook("1:sbf&{T}:tau", "1") );
		CHECK( !check_hook("0:sbf&{F}:tau", "0") );

		CHECK( !check_hook("{T}:tau&0:sbf", "0") );
		CHECK( !check_hook("{F}:tau&1:sbf", "0") );
		CHECK( !check_hook("{T}:tau&1:sbf", "1") );
		CHECK( !check_hook("{F}:tau&0:sbf", "0") );

		CHECK( check_unbinded_hook("{1}:sbf&{F}:tau") );
		CHECK( check_unbinded_hook("{0}:sbf&{T}:tau") );
		CHECK( check_unbinded_hook("{1}:sbf&{T}:tau") );
		CHECK( check_unbinded_hook("{0}:sbf&{F}:tau") );

		CHECK( check_unbinded_hook("{T}:tau&{0}:sbf") );
		CHECK( check_unbinded_hook("{F}:tau&{1}:sbf") );
		CHECK( check_unbinded_hook("{T}:tau&{1}:sbf") );
		CHECK( check_unbinded_hook("{F}:tau&{0}:sbf") );

		CHECK( check_hook("1&x", "x") );
		CHECK( check_hook("x&1", "x") );
		CHECK( check_hook("0&x", "0") );
		CHECK( check_hook("x&0", "0") );
		CHECK( check_hook("x&x", "x") );
		CHECK( check_hook("x&x'", "0") );
		CHECK( check_hook("x'&x", "0") );

		CHECK( check_type("{a}:sbf&{b}:sbf", "sbf") );
		CHECK( check_type("{o1[t] = 0}&{o2[t] = 0}", "tau") );
		CHECK( check_type("{o1[t] = 0}:tau&{o2[t] = 0}", "tau") );
		CHECK( check_type("{o1[t] = 0}&{o2[t] = 0}:tau", "tau") );
	}

	TEST_CASE("+") {
		CHECK( check_hook("0+1", "1") );
		CHECK( check_hook("1+0", "1") );
		CHECK( check_hook("1+1", "0") );
		CHECK( check_hook("0+0", "0") );

		CHECK( check_hook("1:sbf+0:sbf", "1:sbf") );
		CHECK( check_hook("0:sbf+1:sbf", "1:sbf") );
		CHECK( check_hook("1:sbf+1:sbf", "0:sbf") );
		CHECK( check_hook("0:sbf+0:sbf", "0:sbf") );

		CHECK( check_hook("1:sbf+0", "1:sbf") );
		CHECK( check_hook("0:sbf+1", "1:sbf") );
		CHECK( check_hook("1:sbf+1", "0:sbf") );
		CHECK( check_hook("0:sbf+0", "0:sbf") );

		CHECK( check_hook("1+0:sbf", "1:sbf") );
		CHECK( check_hook("0+1:sbf", "1:sbf") );
		CHECK( check_hook("1+1:sbf", "0:sbf") );
		CHECK( check_hook("0+0:sbf", "0:sbf") );

		CHECK( check_hook("1:tau+0:tau", "1:tau") );
		CHECK( check_hook("0:tau+1:tau", "1:tau") );
		CHECK( check_hook("1:tau+1:tau", "0:tau") );
		CHECK( check_hook("0:tau+0:tau", "0:tau") );

		CHECK( check_hook("1+0:tau", "1:tau") );
		CHECK( check_hook("0+1:tau", "1:tau") );
		CHECK( check_hook("1+1:tau", "0:tau") );
		CHECK( check_hook("0+0:tau", "0:tau") );

		CHECK( check_hook("1:tau+0", "1:tau") );
		CHECK( check_hook("0:tau+1", "1:tau") );
		CHECK( check_hook("1:tau+1", "0:tau") );
		CHECK( check_hook("0:tau+0", "0:tau") );

		CHECK( !check_hook("1:sbf+0:tau", "1") );
		CHECK( !check_hook("0:sbf+1:tau", "1") );
		CHECK( !check_hook("1:sbf+1:tau", "0") );
		CHECK( !check_hook("0:sbf+0:tau", "0") );

		CHECK( !check_hook("1:tau+0:sbf", "1") );
		CHECK( !check_hook("0:tau+1:sbf", "1") );
		CHECK( !check_hook("1:tau+1:sbf", "0") );
		CHECK( !check_hook("0:tau+0:sbf", "0") );

		CHECK( !check_hook("{1}:sbf+0:tau", "1") );
		CHECK( !check_hook("{0}:sbf+1:tau", "1") );
		CHECK( !check_hook("{1}:sbf+1:tau", "0") );
		CHECK( !check_hook("{0}:sbf+0:tau", "0") );

		CHECK( !check_hook("1:tau+{0}:sbf", "1") );
		CHECK( !check_hook("0:tau+{1}:sbf", "1") );
		CHECK( !check_hook("1:tau+{1}:sbf", "0") );
		CHECK( !check_hook("0:tau+{0}:sbf", "0") );

		CHECK( !check_hook("1:sbf+{F}:tau", "1") );
		CHECK( !check_hook("0:sbf+{T}:tau", "1") );
		CHECK( !check_hook("1:sbf+{T}:tau", "0") );
		CHECK( !check_hook("0:sbf+{F}:tau", "0") );

		CHECK( !check_hook("{T}:tau+0:sbf", "1") );
		CHECK( !check_hook("{F}:tau+1:sbf", "1") );
		CHECK( !check_hook("{T}:tau+1:sbf", "0") );
		CHECK( !check_hook("{F}:tau+0:sbf", "0") );

		CHECK( check_unbinded_hook("{1}:sbf+{F}:tau") );
		CHECK( check_unbinded_hook("{0}:sbf+{T}:tau") );
		CHECK( check_unbinded_hook("{1}:sbf+{T}:tau") );
		CHECK( check_unbinded_hook("{0}:sbf+{F}:tau") );

		CHECK( check_unbinded_hook("{T}:tau+{0}:sbf") );
		CHECK( check_unbinded_hook("{F}:tau+{1}:sbf") );
		CHECK( check_unbinded_hook("{T}:tau+{1}:sbf") );
		CHECK( check_unbinded_hook("{F}:tau+{0}:sbf") );

		CHECK( check_hook("1+x", "x'") );
		CHECK( check_hook("x+1", "x'") );
		CHECK( check_hook("0+x", "x") );
		CHECK( check_hook("x+0", "x") );
		CHECK( check_hook("x+x", "0") );
		CHECK( check_hook("x+x'", "1") );
		CHECK( check_hook("x'+x", "1") );

		CHECK( check_type("{a}:sbf+{b}:sbf", "sbf") );
		CHECK( check_type("{o1[t] = 0}+{o2[t] = 0}", "tau") );
		CHECK( check_type("{o1[t] = 0}:tau+{o2[t] = 0}", "tau") );
		CHECK( check_type("{o1[t] = 0}+{o2[t] = 0}:tau", "tau") );
	}
}
