// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <cvc5/cvc5.h>

#include "test_init.h"

using namespace cvc5;

TEST_SUITE("sample cvc5 programs") {

	static TermManager cvc5_term_manager;
	static Solver cvc5_solver(cvc5_term_manager);

	TEST_CASE("forall sample (static)") {
		/*
		(assert
				(forall ((x (_ BitVec 4)))
					(= x #b0101)
				)
		)
		(check-sat)
		*/
		auto bv4 = cvc5_term_manager.mkBitVectorSort(4);
		auto x = cvc5_term_manager.mkVar(bv4, "x");
		auto b0101 = cvc5_term_manager.mkBitVector(4, 3);
		auto eq = cvc5_term_manager.mkTerm(Kind::EQUAL, {x, b0101});
		auto q_x = cvc5_term_manager.mkTerm(Kind::VARIABLE_LIST, {x});
		auto fml = cvc5_term_manager.mkTerm(Kind::FORALL, {q_x, eq});
		cvc5_solver.assertFormula(fml);
		auto result = cvc5_solver.checkSat();

		BOOST_LOG_TRIVIAL(info) << "Fml: " << fml;
		BOOST_LOG_TRIVIAL(info) << "Result: " << result;

		CHECK(!result.isSat());
	}

	TEST_CASE("forall sample") {
		/*
		(assert
				(forall ((x (_ BitVec 4)))
					(= x #b0101)
				)
		)
		(check-sat)
		*/
		auto bv4 = cvc5_term_manager.mkBitVectorSort(4);
		auto x = cvc5_term_manager.mkVar(bv4, "x");
		auto b0101 = cvc5_term_manager.mkBitVector(4, 3);
		auto eq = cvc5_term_manager.mkTerm(Kind::EQUAL, {x, b0101});
		auto q_x = cvc5_term_manager.mkTerm(Kind::VARIABLE_LIST, {x});
		auto fml = cvc5_term_manager.mkTerm(Kind::FORALL, {q_x, eq});
		cvc5_solver.assertFormula(fml);
		auto result = cvc5_solver.checkSat();

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
		auto bv4 = cvc5_term_manager.mkBitVectorSort(4);
		auto x = cvc5_term_manager.mkVar(bv4, "x");
		auto b0101 = cvc5_term_manager.mkBitVector(4, 3);
		auto eq = cvc5_term_manager.mkTerm(Kind::EQUAL, {x, b0101});
		auto q_x = cvc5_term_manager.mkTerm(Kind::VARIABLE_LIST, {x});
		auto fml = cvc5_term_manager.mkTerm(Kind::EXISTS, {q_x, eq});
		cvc5_solver.assertFormula(fml);
		auto result = cvc5_solver.checkSat();

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
		auto bvSort = cvc5_term_manager.mkBitVectorSort(4);
		auto o1_t = cvc5_term_manager.mkVar(bvSort, "o1_t");
		auto i1_t = cvc5_term_manager.mkVar(bvSort, "i1_t");

		auto fml =
			cvc5_term_manager.mkTerm(Kind::NOT, {
				cvc5_term_manager.mkTerm(Kind::AND, {
					cvc5_term_manager.mkTerm(Kind::FORALL, {
						cvc5_term_manager.mkTerm(Kind::VARIABLE_LIST, {i1_t}),
						cvc5_term_manager.mkTerm(Kind::FORALL, {
							cvc5_term_manager.mkTerm(Kind::VARIABLE_LIST, {o1_t}),
							cvc5_term_manager.mkTerm(Kind::OR,	{
								cvc5_term_manager.mkTerm(Kind::DISTINCT, {i1_t, o1_t}),
								cvc5_term_manager.mkTerm(Kind::EQUAL, {i1_t, o1_t})
							})
						})
					}),
					cvc5_term_manager.mkTerm(Kind::FORALL, {
						cvc5_term_manager.mkTerm(Kind::VARIABLE_LIST, {i1_t}),
						cvc5_term_manager.mkTerm(Kind::OR, {
							cvc5_term_manager.mkTerm(Kind::FORALL, {
								cvc5_term_manager.mkTerm(Kind::VARIABLE_LIST, {o1_t}),
								cvc5_term_manager.mkTerm(Kind::DISTINCT, {i1_t, o1_t})
							}),
							cvc5_term_manager.mkTerm(Kind::FORALL, {
								cvc5_term_manager.mkTerm(Kind::VARIABLE_LIST, {i1_t}),
								cvc5_term_manager.mkTerm(Kind::EXISTS, {
									cvc5_term_manager.mkTerm(Kind::VARIABLE_LIST, {o1_t}),
									cvc5_term_manager.mkTerm(Kind::EQUAL, {i1_t, o1_t})
								})
							})
						})
					})
				})
			});

		// TODO (MEDIUM) correct the above formula (using scopes)
		CHECK_THROWS(cvc5_solver.assertFormula(fml));

		//auto result = solver.checkSat();
		//BOOST_LOG_TRIVIAL(info) << "Fml: " << fml;
		//BOOST_LOG_TRIVIAL(info) << "Result: " << result;
		//CHECK_THROWS(result.isUnsat());
	}

}
