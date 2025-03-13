// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <cassert>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "doctest.h"

#include "test_integration_helpers.h"

using namespace std;
using namespace idni::rewriter;
using namespace idni::tau_lang;
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
