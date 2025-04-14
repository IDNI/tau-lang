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
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "doctest.h"

#include "test_integration_helpers.h"

using namespace std;
using namespace idni::tau_lang;
using namespace idni::tau_lang::depreciating;
using namespace idni::rewriter::depreciating;

using namespace boost::log;

namespace testing = doctest;

TEST_SUITE("configuration") {


	TEST_CASE("logging") {
		core::get()->set_filter(trivial::severity >= trivial::trace);
		add_console_log(std::cout, keywords::format =
			expressions::stream << expressions::smessage);
	}

	TEST_CASE("bdd initialization") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("Tau_splitter_tau_coeff") {

	TEST_CASE("Tau_splitter_tau_coeff1") {
		const char *src = "{o1[t]o2[t] = 0.} v != 0.";
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::upper);
		stringstream ss; ss << s;

		#ifdef DEBUG
		std::cout << ss.str() << "\n";
		#endif

		CHECK(ss.str() == "{ o1[t]o2[t] = 0 } : tau ({ o1[t]o2[t] = 0 } : tau v)' = 0");
	}

	TEST_CASE("Tau_splitter_tau_coeff2") {
		const char *src = "{o1[t]|o2[t] = 0.}&v = 0.";
		auto fm = make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(src).value().main;
		auto s = tau_splitter(fm, splitter_type::upper);
		stringstream ss; ss << s;

		#ifdef DEBUG
		std::cout << ss.str() << "\n";
		#endif

		CHECK((ss.str() == "{ o1[t]|o2[t] = 0 } : tau v = 0 && v{ !(always o1[t] = 0 && o2[t] = 0) } : tau = 0"
			|| ss.str() == "{ o1[t]|o2[t] = 0 } : tau v = 0 && v{ !(always o2[t] = 0 && o1[t] = 0) } = 0"
			|| ss.str() == "{ o1[t]|o2[t] = 0 } v = 0 && v{ !(always o1[t] = 0 && o2[t] = 0) } : tau = 0"
			|| ss.str() == "{ o1[t]|o2[t] = 0 } v = 0 && v{ !(always o1[t] = 0 && o2[t] = 0) } = 0"));
	}
}
