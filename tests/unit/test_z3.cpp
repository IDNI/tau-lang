// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "../unit/test_helpers.h"
#include "z3.h"


using namespace idni::rewriter;
using namespace idni::tau_lang;
using namespace boost::log;
using namespace z3;

namespace testing = doctest;

TEST_SUITE("configuration") {

	TEST_CASE("z3_config") {
		z3_config();
	}
}

TEST_SUITE("sample z3 programs") {

	TEST_CASE("sample") {
		// (declare-const x (_ BitVec 32))
		// (declare-const y (_ BitVec 32))
		// (assert (= (bvadd x y) #x00000004))
		// (assert (= x #x00000002))
		// (check-sat)
		// (get-model)
		//
		// ; output
		//
		// ;sat
		// ;(
		// ;  (define-fun x () (_ BitVec 32) #x00000002)
		// ;  (define-fun y () (_ BitVec 32) #x00000002)
		// ;)
		context c;
		expr x = c.bv_const("x", 32);
		expr y = c.bv_const("y", 32);
		solver s(c);
		s.add(((x * y) == 4) && (x  == 2));
		BOOST_LOG_TRIVIAL(info)
			<< s << "\n"
			<< s.check() << "\n"
			<< s.get_model() << "\n"
			<< "x <- " << s.get_model().eval(x).get_numeral_int() << "\n"
			<< "y <- " << s.get_model().eval(y).get_numeral_int() << "\n";
	}

	TEST_CASE("evaluating expr in the model") {
		// (declare-fun f (Int Int) Int)
		// (assert (forall ((a Int) (b Int)) (= (f a b) (+ a b))))
		// (check-sat)
		// (get-model)
		// (declare-const a Int)
		// (declare-const b Int)
		// (eval (f a b))
		//
		// ; output
		//
		// ;sat
		// ;(
		// ;  (define-fun f ((x!0 Int) (x!1 Int)) Int
		// ;    (+ x!0 x!1))
		// ;)
		// ;(+ a b)

		context c;
		expr a = c.int_const("a");
		expr b = c.int_const("b");
		sort I = c.int_sort();
		func_decl f = function("f", I, I, I);
		solver s(c);
		s.add(forall(a, b, f(a, b) == a + b));

		BOOST_LOG_TRIVIAL(info)
			<< s << "\n"
			<< s.check() << "\n"
			<< s.get_model().eval(f(a, b)) << "\n";
	}

	TEST_CASE("eliminating one variable") {
		//	(declare-fun f ((_ BitVec 2)) Bool)
		//	(assert (forall ((y (_ BitVec 2))) (ite (exists ((x (_ BitVec 2))) (= (bvor y x) #b11)) (= (f y) true) (= (f y) false))))
		//	(check-sat)
		//	(get-model)
		//	(reset)
		//
		//	; output
		//
		//	;sat
		//	;(
		//	;  (define-fun f ((x!0 (_ BitVec 2))) Bool
		//	;    true)
		//	;)

		context c;
		expr x = c.bv_const("x", 2);
		expr y = c.bv_const("y", 2);
		sort BV = c.bv_sort(2);
		sort B = c.bool_sort();
		func_decl f = function("f", BV, B);
		solver s(c);
		s.add(forall(y,
			ite(
				exists(x, (y | x) == c.bv_val(3, 2)),
				f(y) == c.bool_val(true),
				f(y) == c.bool_val(false))));

		BOOST_LOG_TRIVIAL(info)
			<< s << "\n"
			<< s.check() << "\n"
			<< s.get_model().eval(f(y)) << "\n";

		CHECK( s.get_model().eval(f(y)).bool_value() == true );
	}

	TEST_CASE("eliminating one variable (y2)") {
		//	(declare-fun f ((_ BitVec 2)) Bool)
		//	(assert (forall ((y (_ BitVec 2))) (ite (exists ((x (_ BitVec 2))) (= (bvor y x) #b11)) (= (f y) true) (= (f y) false))))
		//	(check-sat)
		//	(get-model)
		//	(reset)
		//
		//	; output
		//
		//	;sat
		//	;(
		//	;  (define-fun f ((x!0 (_ BitVec 2))) Bool
		//	;    true)
		//	;)

		context c;
		expr x = c.bv_const("x", 2);
		expr y = c.bv_const("y", 2);
		sort BV = c.bv_sort(2);
		sort B = c.bool_sort();
		func_decl f = function("f", BV, B);
		solver s(c);
		s.add(forall(y,
			ite(
				exists(x, (y & x) == c.bv_val(1, 2)),
				f(y) == c.bool_val(true),
				f(y) == c.bool_val(false))));

		BOOST_LOG_TRIVIAL(info)
			<< s << "\n"
			<< s.check() << "\n"
			<< s.get_model().eval(f(y)) << "\n";

		CHECK( s.get_model().eval(f(y)).bool_value() == false );
	}

	TEST_CASE("eliminating one variable (y3)") {
		//	(declare-fun f_y ((_ BitVec 2)) Bool)
		//	(assert (forall ((y (_ BitVec 2))) (ite (forall ((x (_ BitVec 2))) (= (bvand y x) #b00)) (= (f_y y) true) (= (f_y y) false))))
		//	(check-sat)
		//	(get-model)
		//	(reset)
		//
		//	; output
		//
		//	;sat
		//	;(
		//	;  (define-fun f ((x!0 (_ BitVec 2))) Bool
		//	;    (and (= x!0 #b00) (not (= x!0 #b10))))
		//	;)
		//	;
		//	; x!0 stands for the first argument of f, x!1 for the second and so on...
		//	; the above solution is equivalent to equivalent to (= x!0 #b00)

		context c;
		expr x = c.bv_const("x", 2);
		expr y = c.bv_const("y", 2);
		sort BV = c.bv_sort(2);
		sort B = c.bool_sort();
		func_decl f_y = function("f_y", BV, B);
		solver s(c);
		s.add(forall(y,
			ite(
				exists(x, (y & x) == c.bv_val(0, 2)),
				f_y(y) == c.bool_val(true),
				f_y(y) == c.bool_val(false))));

		BOOST_LOG_TRIVIAL(info)
			<< s << "\n"
			<< s.check() << "\n"
			<< s.get_model().eval(f_y(y)) << "\n";

			CHECK( s.get_model().eval(f_y(y)) != c.bool_val(false) );
			CHECK( s.get_model().eval(f_y(y)) != c.bool_val(true) );
		}

	TEST_CASE("eliminating two variables") {
		//	(declare-fun f_y_z ((_ BitVec 2)) Bool)
		//	(assert (forall ((y (_ BitVec 2))) (ite (forall ((x (_ BitVec 2))) (= (bvand y x z) #b00)) (= (f_y_z y) true) (= (f_y_z y) false))))
		//	(check-sat)
		//	(get-model)
		//	(reset)
		//
		//	; output
		//
		//	;sat
		//	;(
		//	;  (define-fun f ((x!0 (_ BitVec 2))) Bool
		//	;    (and (= x!0 #b00) (not (= x!0 #b10))))
		//	;)
		//	;
		//	; x!0 stands for the first argument of f, x!1 for the second and so on...
		//	; the above solution is equivalent to equivalent to (= x!0 #b00)
		//
		//	(declare-fun f_y_z ((_ BitVec 2)) Bool)
		//	(assert (forall ((y (_ BitVec 2))) (ite (forall ((x (_ BitVec 2))) (= (bvand y x z) #b00)) (= (f_y_z y) true) (= (f_y_z y) false))))
		//	(check-sat)
		//	(get-model)
		//	(reset)

		context c;
		expr x = c.bv_const("x", 2);
		expr y = c.bv_const("y", 2);
		expr z = c.bv_const("z", 2);
		sort BV = c.bv_sort(2);
		sort B = c.bool_sort();
		func_decl f_y_z = function("f_y_z", BV, BV, B);
		solver s(c);
		s.add(forall(y, z,
			ite(
				exists(x, (y & x & z) == c.bv_val(0, 2)),
				f_y_z(y, z) == c.bool_val(true),
				f_y_z(y, z) == c.bool_val(false))));

		BOOST_LOG_TRIVIAL(info)
			<< s << "\n"
			<< s.check() << "\n"
			<< s.get_model().eval(f_y_z(y, z)) << "\n";

		auto f_y_z_val = s.get_model().eval(f_y_z(y, z));
		func_decl f_z = function("f_z", BV, B);
		s.add(forall(z,
			ite(
				exists(y, f_z(y) == f_y_z_val),
				f_z(z) == c.bool_val(true),
				f_z(z) == c.bool_val(false))));

		BOOST_LOG_TRIVIAL(info)
			<< s << "\n"
			<< s.check() << "\n"
			<< s.get_model().eval(f_z(z)) << "\n";

		CHECK( s.get_model().eval(f_z(z)) == c.bool_val(false) );

	}

	TEST_CASE("eliminating two variables (y2)") {
		//	(declare-fun f_y_z ((_ BitVec 2)) Bool)
		//	(assert (forall ((y (_ BitVec 2))) (ite (forall ((x (_ BitVec 2))) (= (bvor y x z) #b00)) (= (f_y_z y) true) (= (f_y_z y) false))))
		//	(check-sat)
		//	(get-model)
		//	(reset)
		//
		//	; output
		//
		//	;sat
		//	;(
		//	;  (define-fun f ((x!0 (_ BitVec 2))) Bool
		//	;    (and (= x!0 #b00) (not (= x!0 #b10))))
		//	;)
		//	;
		//	; x!0 stands for the first argument of f, x!1 for the second and so on...
		//	; the above solution is equivalent to equivalent to (= x!0 #b00)
		//
		//	(declare-fun f_y_z ((_ BitVec 2)) Bool)
		//	(assert (forall ((y (_ BitVec 2))) (ite (forall ((x (_ BitVec 2))) (= (bvor y x z) #b00)) (= (f_y_z y) true) (= (f_y_z y) false))))
		//	(check-sat)
		//	(get-model)
		//	(reset)

		context c;
		expr x = c.bv_const("x", 2);
		expr y = c.bv_const("y", 2);
		expr z = c.bv_const("z", 2);
		sort BV = c.bv_sort(2);
		sort B = c.bool_sort();
		func_decl f_y_z = function("f_y_z", BV, BV, B);
		solver s(c);
		s.add(forall(y, z,
			ite(
				exists(x, (y | x | z) == c.bv_val(3, 2)),
				f_y_z(y, z) == c.bool_val(true),
				f_y_z(y, z) == c.bool_val(false))));

		BOOST_LOG_TRIVIAL(info)
			<< s << "\n"
			<< s.check() << "\n"
			<< s.get_model().eval(f_y_z(y, z)) << "\n";

		auto f_y_z_val = s.get_model().eval(f_y_z(y, z));
		func_decl f_z = function("f_z", BV, B);
		s.add(forall(z,
			ite(
				exists(y, f_z(y) == f_y_z_val),
				f_z(z) == c.bool_val(true),
				f_z(z) == c.bool_val(false))));

		BOOST_LOG_TRIVIAL(info)
			<< s << "\n"
			<< s.check() << "\n"
			<< s.get_model().eval(f_z(z)) << "\n";
	}
}
