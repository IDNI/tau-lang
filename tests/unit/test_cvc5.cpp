// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <cvc5/cvc5.h>

#include "test_init.h"

using namespace cvc5;

static TermManager cvc5_term_manager;

TEST_SUITE("sample cvc5 programs") {

	TEST_CASE("forall sample (static)") {
		/*
		(assert
				(forall ((x (_ BitVec 4)))
					(= x #b0101)
				)
		)
		(check-sat)
		*/
		Solver cvc5_solver(cvc5_term_manager);
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

		CHECK(result.isUnsat());
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
		Solver cvc5_solver(cvc5_term_manager);
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

		CHECK(result.isUnsat());
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
		Solver cvc5_solver(cvc5_term_manager);
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

		CHECK(result.isUnsat());
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
		Solver cvc5_solver(cvc5_term_manager);
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
		// CHECK_THROWS(cvc5_solver.assertFormula(fml));

		auto result = cvc5_solver.checkSat();
		BOOST_LOG_TRIVIAL(info) << "Fml: " << fml;
		BOOST_LOG_TRIVIAL(info) << "Result: " << result;
		CHECK(result.isSat());
	}
}

TEST_SUITE("bitwise operations with cvc5") {

	TEST_CASE("bitwise not") {
		Solver cvc5_solver(cvc5_term_manager);
		auto b01 = cvc5_term_manager.mkBitVector(2, 1);
		auto b10 = cvc5_term_manager.mkBitVector(2, 2);
		auto not_term = cvc5_term_manager.mkTerm(Kind::BITVECTOR_NOT, {b01});
		auto neq_term = cvc5_term_manager.mkTerm(Kind::DISTINCT, {not_term, b10});

		cvc5_solver.assertFormula(neq_term);
		auto result = cvc5_solver.checkSat();
		CHECK(result.isUnsat());
	}

	TEST_CASE("bitwise and") {
		Solver cvc5_solver(cvc5_term_manager);
		auto b01 = cvc5_term_manager.mkBitVector(2, 1);
		auto b10 = cvc5_term_manager.mkBitVector(2, 2);
		auto b00 = cvc5_term_manager.mkBitVector(2, 0);
		auto and_term = cvc5_term_manager.mkTerm(Kind::BITVECTOR_AND, {b01, b10});
		auto neq_term = cvc5_term_manager.mkTerm(Kind::DISTINCT, {and_term, b00});

		cvc5_solver.assertFormula(neq_term);
		auto result = cvc5_solver.checkSat();
		CHECK(result.isUnsat());
	}

	TEST_CASE("bitwise nand") {
		Solver cvc5_solver(cvc5_term_manager);
		auto b01 = cvc5_term_manager.mkBitVector(2, 1);
		auto b10 = cvc5_term_manager.mkBitVector(2, 2);
		auto b11 = cvc5_term_manager.mkBitVector(2, 3);
		auto nand_term = cvc5_term_manager.mkTerm(Kind::BITVECTOR_NAND, {b01, b10});
		auto neq_term = cvc5_term_manager.mkTerm(Kind::DISTINCT, {nand_term, b11});

		cvc5_solver.assertFormula(neq_term);
		auto result = cvc5_solver.checkSat();
		CHECK(result.isUnsat());
	}

	TEST_CASE("bitwise or") {
		Solver cvc5_solver(cvc5_term_manager);
		auto b01 = cvc5_term_manager.mkBitVector(2, 1);
		auto b10 = cvc5_term_manager.mkBitVector(2, 2);
		auto b11 = cvc5_term_manager.mkBitVector(2, 3);
		auto or_term = cvc5_term_manager.mkTerm(Kind::BITVECTOR_OR, {b01, b10});
		auto neq_term = cvc5_term_manager.mkTerm(Kind::DISTINCT, {or_term, b11});

		cvc5_solver.assertFormula(neq_term);
		auto result = cvc5_solver.checkSat();
		CHECK(result.isUnsat());
	}

	TEST_CASE("bitwise nor") {
		Solver cvc5_solver(cvc5_term_manager);
		auto b01 = cvc5_term_manager.mkBitVector(2, 1);
		auto b10 = cvc5_term_manager.mkBitVector(2, 2);
		auto b00 = cvc5_term_manager.mkBitVector(2, 0);
		auto nor_term = cvc5_term_manager.mkTerm(Kind::BITVECTOR_NOR, {b01, b10});
		auto neq_term = cvc5_term_manager.mkTerm(Kind::DISTINCT, {nor_term, b00});

		cvc5_solver.assertFormula(neq_term);
		auto result = cvc5_solver.checkSat();
		CHECK(result.isUnsat());
	}

	TEST_CASE("bitwise xor") {
		Solver cvc5_solver(cvc5_term_manager);
		auto b01 = cvc5_term_manager.mkBitVector(2, 1);
		auto b10 = cvc5_term_manager.mkBitVector(2, 2);
		auto b11 = cvc5_term_manager.mkBitVector(2, 3);
		auto xor_term = cvc5_term_manager.mkTerm(Kind::BITVECTOR_XOR, {b01, b10});
		auto neq_term = cvc5_term_manager.mkTerm(Kind::DISTINCT, {xor_term, b11});

		cvc5_solver.assertFormula(neq_term);
		auto result = cvc5_solver.checkSat();
		CHECK(result.isUnsat());
	}

	TEST_CASE("bitwise xnor") {
		Solver cvc5_solver(cvc5_term_manager);
		auto b01 = cvc5_term_manager.mkBitVector(2, 1);
		auto b10 = cvc5_term_manager.mkBitVector(2, 2);
		auto b00 = cvc5_term_manager.mkBitVector(2, 0);
		auto xnor_term = cvc5_term_manager.mkTerm(Kind::BITVECTOR_XNOR, {b01, b10});
		auto neq_term = cvc5_term_manager.mkTerm(Kind::DISTINCT, {xnor_term, b00});

		cvc5_solver.assertFormula(neq_term);
		auto result = cvc5_solver.checkSat();
		CHECK(result.isUnsat());
	}

	TEST_CASE("add") {
		Solver cvc5_solver(cvc5_term_manager);
		auto b01 = cvc5_term_manager.mkBitVector(2, 1);
		auto b10 = cvc5_term_manager.mkBitVector(2, 2);
		auto b11 = cvc5_term_manager.mkBitVector(2, 3);
		auto add_term = cvc5_term_manager.mkTerm(Kind::BITVECTOR_ADD, {b01, b10});
		auto neq_term = cvc5_term_manager.mkTerm(Kind::DISTINCT, {add_term, b11});

		cvc5_solver.assertFormula(neq_term);
		auto result = cvc5_solver.checkSat();
		CHECK(result.isUnsat());
	}

	TEST_CASE("sub") {
		Solver cvc5_solver(cvc5_term_manager);
		auto b11 = cvc5_term_manager.mkBitVector(2, 3);
		auto b10 = cvc5_term_manager.mkBitVector(2, 2);
		auto b01 = cvc5_term_manager.mkBitVector(2, 1);
		auto sub_term = cvc5_term_manager.mkTerm(Kind::BITVECTOR_SUB, {b11, b10});
		auto neq_term = cvc5_term_manager.mkTerm(Kind::DISTINCT, {sub_term, b01});

		cvc5_solver.assertFormula(neq_term);
		auto result = cvc5_solver.checkSat();
		CHECK(result.isUnsat());
	}

	TEST_CASE("mul") {
		Solver cvc5_solver(cvc5_term_manager);
		auto b10 = cvc5_term_manager.mkBitVector(2, 2);
		auto b11 = cvc5_term_manager.mkBitVector(2, 3);
		auto b10_2 = cvc5_term_manager.mkBitVector(2, 2);
		auto mul_term = cvc5_term_manager.mkTerm(Kind::BITVECTOR_MULT, {b10, b11});
		auto neq_term = cvc5_term_manager.mkTerm(Kind::DISTINCT, {mul_term, b10_2});

		cvc5_solver.assertFormula(neq_term);
		auto result = cvc5_solver.checkSat();
		CHECK(result.isUnsat());
	}

	TEST_CASE("div") {
		Solver cvc5_solver(cvc5_term_manager);
		auto b10 = cvc5_term_manager.mkBitVector(2, 2);
		auto b11 = cvc5_term_manager.mkBitVector(2, 3);
		auto b01 = cvc5_term_manager.mkBitVector(2, 1);
		auto div_term = cvc5_term_manager.mkTerm(Kind::BITVECTOR_UDIV, {b11, b10});
		auto neq_term = cvc5_term_manager.mkTerm(Kind::DISTINCT, {div_term, b01});

		cvc5_solver.assertFormula(neq_term);
		auto result = cvc5_solver.checkSat();
		CHECK(result.isUnsat());
	}

	TEST_CASE("mod") {
		// {139}:bv[8] % {3}:bv[8] = {1}:bv[8]
		Solver cvc5_solver(cvc5_term_manager);
		auto b139 = cvc5_term_manager.mkBitVector(8, 139);
		auto b003 = cvc5_term_manager.mkBitVector(8, 3);
		auto b001 = cvc5_term_manager.mkBitVector(8, 1);
		auto mod_term = cvc5_term_manager.mkTerm(Kind::BITVECTOR_UREM, {b139, b003});
		auto neq_term = cvc5_term_manager.mkTerm(Kind::DISTINCT, {mod_term, b001});
		cvc5_solver.assertFormula(neq_term);
		auto result = cvc5_solver.checkSat();
		CHECK(result.isUnsat());
	}

	TEST_CASE("shift left") {
		Solver cvc5_solver(cvc5_term_manager);
		auto b01 = cvc5_term_manager.mkBitVector(4, 1);
		auto b10 = cvc5_term_manager.mkBitVector(4, 2);
		auto shl_term = cvc5_term_manager.mkTerm(Kind::BITVECTOR_SHL, {b01, b10});
		auto b04 = cvc5_term_manager.mkBitVector(4, 4);
		auto neq_term = cvc5_term_manager.mkTerm(Kind::DISTINCT, {shl_term, b04});

		cvc5_solver.assertFormula(neq_term);
		auto result = cvc5_solver.checkSat();
		CHECK(result.isUnsat());
	}

	TEST_CASE("shift right") {
		Solver cvc5_solver(cvc5_term_manager);
		auto b10 = cvc5_term_manager.mkBitVector(4, 2);
		auto b01 = cvc5_term_manager.mkBitVector(4, 1);
		auto shr_term = cvc5_term_manager.mkTerm(Kind::BITVECTOR_LSHR, {b10, b01});
		auto b01_2 = cvc5_term_manager.mkBitVector(4, 1);
		auto neq_term = cvc5_term_manager.mkTerm(Kind::DISTINCT, {shr_term, b01_2});

		cvc5_solver.assertFormula(neq_term);
		auto result = cvc5_solver.checkSat();
		CHECK(result.isUnsat());
	}
}