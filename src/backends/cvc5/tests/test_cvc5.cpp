// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <cvc5/cvc5.h>

#include "test_init.h"
#include "boolean_algebras/bv/bv_ba.h"

using namespace cvc5;

TEST_SUITE("cvc5 solver") {

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


TEST_SUITE("cvc5 simplification") {

	TEST_CASE("x - ( 1 + x)") {
		Solver cvc5_solver(cvc5_term_manager);
		auto bv4 = cvc5_term_manager.mkBitVectorSort(4);
		auto x = cvc5_term_manager.mkVar(bv4, "x");
		auto y = cvc5_term_manager.mkVar(bv4, "y");
		auto y_plus_x = cvc5_term_manager.mkTerm(Kind::BITVECTOR_ADD, {	y, x });
		auto x_minus_one_plus_x = cvc5_term_manager.mkTerm(Kind::BITVECTOR_SUB, {x, y_plus_x });
		auto result = cvc5_solver.simplify(x_minus_one_plus_x);
		BOOST_LOG_TRIVIAL(info) << "x - (y + x) = " << result;
		CHECK(result.getKind() == Kind::BITVECTOR_NEG);
		CHECK(result[0] == y);
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

TEST_SUITE("config_cvc5_solver option sets") {

	// Configure a fresh solver under `set` and return the value of one
	// cvc5 option. Restores the global before returning.
	static std::string opt_under(cvc5_option_set set, const char* name,
		bool decision_only = false)
	{
		struct cvc5_options_guard {
			cvc5_option_set saved = cvc5_options;
			~cvc5_options_guard() { cvc5_options = saved; }
		} guard;
		cvc5_options = set;
		cvc5::Solver s(cvc5_term_manager);
		config_cvc5_solver(s, decision_only);
		std::string v = s.getOption(name);
		return v;
	}

	TEST_CASE("baseline keeps models and proofs off") {
		CHECK( opt_under(cvc5_option_set::baseline, "produce-models")
			== "true" );
		CHECK( opt_under(cvc5_option_set::baseline, "produce-proofs")
			== "false" );
	}
	TEST_CASE("miniscope_agg") {
		CHECK( opt_under(cvc5_option_set::miniscope_agg,
			"miniscope-quant") == "agg" );
	}
	TEST_CASE("ext_rewrite_quant") {
		CHECK( opt_under(cvc5_option_set::ext_rewrite_quant,
			"ext-rewrite-quant") == "true" );
	}
	TEST_CASE("pre_skolem_agg") {
		CHECK( opt_under(cvc5_option_set::pre_skolem_agg,
			"pre-skolem-quant") == "agg" );
	}
	TEST_CASE("sygus_inst turns incremental off") {
		CHECK( opt_under(cvc5_option_set::sygus_inst, "sygus-inst")
			== "true" );
		CHECK( opt_under(cvc5_option_set::sygus_inst, "incremental")
			== "false" );
	}
	TEST_CASE("mbqi") {
		CHECK( opt_under(cvc5_option_set::mbqi, "mbqi") == "true" );
	}
	TEST_CASE("enum_inst") {
		CHECK( opt_under(cvc5_option_set::enum_inst, "enum-inst")
			== "true" );
	}
	TEST_CASE("cegqi_bv_ineq_keep") {
		CHECK( opt_under(cvc5_option_set::cegqi_bv_ineq_keep,
			"cegqi-bv-ineq") == "keep" );
	}
	TEST_CASE("non_incremental") {
		CHECK( opt_under(cvc5_option_set::non_incremental,
			"incremental") == "false" );
	}
	TEST_CASE("ext_rewrite_no_models (the shipped default)") {
		CHECK( opt_under(cvc5_option_set::ext_rewrite_no_models,
			"ext-rewrite-quant") == "true" );
		CHECK( opt_under(cvc5_option_set::ext_rewrite_no_models,
			"incremental") == "false" );
		// decision_only=false: models stay ON even in a no-models set.
		CHECK( opt_under(cvc5_option_set::ext_rewrite_no_models,
			"produce-models") == "true" );
	}
	TEST_CASE("combined_best") {
		CHECK( opt_under(cvc5_option_set::combined_best,
			"ext-rewrite-quant") == "true" );
		CHECK( opt_under(cvc5_option_set::combined_best,
			"cegqi-bv-ineq") == "keep" );
	}

	TEST_CASE("decision_only drops models exactly in the no-models sets") {
		CHECK( opt_under(cvc5_option_set::decision_no_models,
			"produce-models", true) == "false" );
		CHECK( opt_under(cvc5_option_set::ext_rewrite_no_models,
			"produce-models", true) == "false" );
		CHECK( opt_under(cvc5_option_set::combined_best,
			"produce-models", true) == "false" );
		// And ONLY in those: any other set keeps models even when the
		// caller flags decision-only.
		CHECK( opt_under(cvc5_option_set::baseline,
			"produce-models", true) == "true" );
		CHECK( opt_under(cvc5_option_set::miniscope_agg,
			"produce-models", true) == "true" );
		// The no-models demotion also forces non-incremental.
		CHECK( opt_under(cvc5_option_set::decision_no_models,
			"incremental", true) == "false" );
	}
}