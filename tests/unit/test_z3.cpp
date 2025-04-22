// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "../unit/test_helpers.h"
#include "z3.h"


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

	TEST_CASE("z3_config") {
		z3_config();
	}
}

TEST_SUITE("sample z3 programs") {

	TEST_CASE("sample") {
		context c;
		expr x = c.bv_const("x", 32);
		expr y = c.bv_const("y", 32);
		solver s(c);
		// In C++, the operator == has higher precedence than ^.
		s.add(((x * y) == 4) && (x  == 2));
		std::cout << s << "\n";
		std::cout << s.check() << "\n";
		std::cout << s.get_model() << "\n";
		std::cout << s.get_model().eval(x) << "\n";
		std::cout << s.get_model().eval(y) << "\n";
	}
}
