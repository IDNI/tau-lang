// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "test_helpers.h"
#include "base_bas/cvc5_widths.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;
using namespace boost::log;

namespace testing = doctest;

TEST_SUITE("annotate bv widths") {

	TEST_CASE("no bitvector vars: case 1") {
		const char* sample = "x = 1";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_widths(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::type, Bool>).size() == 0 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::type, Bool>).size() == 0 );
		CHECK( annotated == expr );
		
		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}

	TEST_CASE("no bitvector vars: case 2") {
		const char* sample = "all x x = 1";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_widths(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::type, Bool>).size() == 0 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::type, Bool>).size() == 0 );
		CHECK( annotated == expr );

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}

	TEST_CASE("no bitvector vars: case 1") {
		const char* sample = "all x x = 1 && y = 1";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_widths(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::type, Bool>).size() == 0 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::type, Bool>).size() == 0 );
		CHECK( annotated == expr );

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}

	TEST_CASE("simple free bitvector var") {
		const char* sample = "x =_ 1:bv16";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_widths(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::type, Bool>).size() == 0 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::type, Bool>).size() == 1 );
		
		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}

	TEST_CASE("two free bitvector vars") {
		const char* sample = "x =_ y:bv16";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_widths(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::type, Bool>).size() == 1 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::type, Bool>).size() == 2 );

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}

	TEST_CASE("two free bitvector vars") {
		const char* sample = "x + y =_ 0:bv16";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_widths(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::type, Bool>).size() == 0 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::type, Bool>).size() == 2 );

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}

	TEST_CASE("simple bounded bitvector var: case 1") {
		const char* sample = "all x x =_ 1:bv16";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_widths(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::type, Bool>).size() == 0 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::type, Bool>).size() == 2 );
		
		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		print_tau_tree(std::cout, expr);
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		print_tau_tree(std::cout, annotated);
		#endif // DEBUG
	}

	TEST_CASE("simple bounded bitvector var: case 2") {
		const char* sample = "all x:bv16 x =_ 1";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_widths(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::type, Bool>).size() == 1 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::type, Bool>).size() == 2 );
		
		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}

	TEST_CASE("one bounded and one free bitvector vars: case 1") {
		const char* sample = "all x:bv16 x =_ y";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_widths(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::type, Bool>).size() == 1 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::type, Bool>).size() == 3 );
		
		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}

	TEST_CASE("one bounded and one free bitvector vars: case 2") {
		const char* sample = "all x x =_ y:bv16";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_widths(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::type, Bool>).size() == 1 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::type, Bool>).size() == 3 );
		
		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}

	TEST_CASE("one bounded and one free bitvector vars: case 1") {
		const char* sample = "all x x =_ 1:bv16 && y =_ 0";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_widths(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::type, Bool>).size() == 0 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::type, Bool>).size() == 2 );
		
		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}
}

TEST_SUITE("annotate bv ctes") {

	TEST_CASE("no bitvector ctes: case 1") {
		const char* sample = "x = 1";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_ctes(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::bv_type, Bool>).size() == 0 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::bv_type, Bool>).size() == 0 );
		CHECK( annotated == expr );
		
		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}

	TEST_CASE("no bitvector ctes: case 2") {
		const char* sample = "all x x = 1";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_ctes(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::bv_type, Bool>).size() == 0 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::bv_type, Bool>).size() == 0 );
		CHECK( annotated == expr );

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}

	TEST_CASE("no bitvector ctes: case 1") {
		const char* sample = "all x x = 1 && y = 1";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_ctes(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::bv_type, Bool>).size() == 0 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::bv_type, Bool>).size() == 0 );
		CHECK( annotated == expr );

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}

	TEST_CASE("simple free bitvector var") {
		const char* sample = "x:bv16 =_ 1";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_ctes(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::bv_type, Bool>).size() == 1 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::bv_type, Bool>).size() == 2 );

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}

	TEST_CASE("simple two bitvector ctes") {
		const char* sample = "1:bv16 =_ 1";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_ctes(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::bv_type, Bool>).size() == 1 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::bv_type, Bool>).size() == 2 );

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}

	TEST_CASE("simple bounded bitvector var") {
		const char* sample = "all x:bv16 x:bv16 =_ 1";
		auto src = make_tau_source(sample, { .start = tau_parser::wff });
		auto expr = make_statement(src);
		auto annotated = annotate_bv_ctes(expr);
		CHECK( select_all(expr, is_non_terminal<tau_parser::bv_type, Bool>).size() == 2 );
		CHECK( select_all(annotated, is_non_terminal<tau_parser::bv_type, Bool>).size() == 3 );

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "Expr: " << expr;
		BOOST_LOG_TRIVIAL(info) << "Annotated: " << annotated;
		#endif // DEBUG
	}

}