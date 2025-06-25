// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include <cvc5/cvc5.h>
#include "../../../src/init_log.h"
//#include "../unit/test_helpers.h"

using namespace boost::log;
using namespace cvc5;

namespace testing = doctest;

TEST_SUITE("sample cvc5 programs") {

	TEST_CASE("forall sample") {
		/*
		(assert
				(forall ((x (_ BitVec 4)))
					(= x #b0101)
				)
		)
		(check-sat)
		*/
		Solver solver;
		auto bv4 = solver.mkBitVectorSort(4);
		auto x = solver.mkVar(bv4, "x");
		auto b0101 = solver.mkBitVector(4, 3);
		auto eq = solver.mkTerm(Kind::EQUAL, {x, b0101});
		auto q_x = solver.mkTerm(Kind::VARIABLE_LIST, {x});
		auto fml = solver.mkTerm(Kind::FORALL, {q_x, eq});
		solver.assertFormula(fml);
		auto result = solver.checkSat();

		BOOST_LOG_TRIVIAL(info) << "Fml: " << fml;
		BOOST_LOG_TRIVIAL(info) << "Result: " << result;

		CHECK(!result.isSat());
	}

	TEST_CASE("exists sample") {
		/*
		(assert
				(forall ((x (_ BitVec 4)))
					(= x #b0101)
				)
		)
		(check-sat)
		*/
		Solver solver;
		auto bv4 = solver.mkBitVectorSort(4);
		auto x = solver.mkVar(bv4, "x");
		auto b0101 = solver.mkBitVector(4, 3);
		auto eq = solver.mkTerm(Kind::EQUAL, {x, b0101});
		auto q_x = solver.mkTerm(Kind::VARIABLE_LIST, {x});
		auto fml = solver.mkTerm(Kind::EXISTS, {q_x, eq});
		solver.assertFormula(fml);
		auto result = solver.checkSat();

		BOOST_LOG_TRIVIAL(info) << "Fml: " << fml;
		BOOST_LOG_TRIVIAL(info) << "Result: " << result;

		CHECK(result.isSat());
	}

	TEST_CASE("r o1[t] =_ i1[t]") {
		/*
		(set-logic BV)
		(assert
			(not
				(and
					(forall ((|i1[0]| (_ BitVec 64)))
						(forall ((|o1[0]| (_ BitVec 64)))
							(or
								(distinct |i1[0]| |o1[0]|)
								(= |i1[0]| |o1[0]|))))
					(forall ((|i1[0]| (_ BitVec 64)))
						(or
							(forall ((|o1[0]| (_ BitVec 64)))
								(distinct |i1[0]| |o1[0]|))
							(forall ((|i1[1]| (_ BitVec 64)))
								(exists ((|o1[1]| (_ BitVec 64)))
									(= |i1[1]| |o1[1]|)
								)
							)
						)
					)
				)
			)
		)
		(check-sat)
		*/
		Solver solver;
		auto bvSort = solver.mkBitVectorSort(4);
		auto o1_t = solver.mkVar(bvSort, "o1_t");
		auto i1_t = solver.mkVar(bvSort, "i1_t");

		auto fml =
			solver.mkTerm(Kind::NOT, {
				solver.mkTerm(Kind::AND, {
					solver.mkTerm(Kind::FORALL, {
						solver.mkTerm(Kind::VARIABLE_LIST, {i1_t}),
						solver.mkTerm(Kind::FORALL, {
							solver.mkTerm(Kind::VARIABLE_LIST, {o1_t}),
							solver.mkTerm(Kind::OR,	{
								solver.mkTerm(Kind::DISTINCT, {i1_t, o1_t}),
								solver.mkTerm(Kind::EQUAL, {i1_t, o1_t})
							})
						})
					}),
					solver.mkTerm(Kind::FORALL, {
						solver.mkTerm(Kind::VARIABLE_LIST, {i1_t}),
						solver.mkTerm(Kind::OR, {
							solver.mkTerm(Kind::FORALL, {
								solver.mkTerm(Kind::VARIABLE_LIST, {o1_t}),
								solver.mkTerm(Kind::DISTINCT, {i1_t, o1_t})
							}),
							solver.mkTerm(Kind::FORALL, {
								solver.mkTerm(Kind::VARIABLE_LIST, {i1_t}),
								solver.mkTerm(Kind::EXISTS, {
									solver.mkTerm(Kind::VARIABLE_LIST, {o1_t}),
									solver.mkTerm(Kind::EQUAL, {i1_t, o1_t})
								})
							})
						})
					})
				})
			});

		solver.assertFormula(fml);
		auto result = solver.checkSat();

		BOOST_LOG_TRIVIAL(info) << "Fml: " << fml;
		BOOST_LOG_TRIVIAL(info) << "Result: " << result;

		CHECK(result.isUnsat());
	}

}
