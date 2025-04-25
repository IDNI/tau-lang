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

	/*TEST_CASE("logging") {
		core::get()->set_filter(trivial::severity >= trivial::trace);
		add_console_log(std::cout, keywords::format =
			expressions::stream << expressions::smessage);
	}*/

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
		BOOST_LOG_TRIVIAL(info)
			<< s << "\n"
			<< s.check() << "\n"
			<< s.get_model() << "\n"
			<< "x <- " << s.get_model().eval(x).get_numeral_int() << "\n"
			<< "y <- " << s.get_model().eval(y).get_numeral_int() << "\n";
	}

	TEST_CASE("function") {
		context c;

		expr x      = c.int_const("x");
		expr y      = c.int_const("y");

		sort I      = c.int_sort();

		func_decl g = function("g", I, I);

		solver s(c);

		expr conjecture1 = implies(x == y, g(x) == g(y));

		BOOST_LOG_TRIVIAL(info)
			<< "conjecture 1\n" << conjecture1 << "\n";
		s.add(!conjecture1);

		if (s.check() == unsat)
			BOOST_LOG_TRIVIAL(info)
				<< "proved" << "\n";
		else
			BOOST_LOG_TRIVIAL(info)
				<< "failed to prove" << "\n";

		s.reset(); // remove all assertions from solver s

		expr conjecture2 = implies(x == y, g(g(x)) == g(y));
		std::cout << "conjecture 2\n" << conjecture2 << "\n";
		s.add(!conjecture2);
		if (s.check() == unsat) {
			BOOST_LOG_TRIVIAL(info)
				<< "proved" << "\n";
		}
		else {
			model m = s.get_model();
			BOOST_LOG_TRIVIAL(info)
				<< "failed to prove" << "\n"
				<< "counterexample:\n" << m << "\n"
				<< "g(g(x)) = " << m.eval(g(g(x))) << "\n"
				<< "g(y)    = " << m.eval(g(y)) << "\n";
		}
	}
}
