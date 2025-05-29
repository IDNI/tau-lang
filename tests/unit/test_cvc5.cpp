// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include <cvc5/cvc5.h>

namespace testing = doctest;

using namespace cvc5;

TEST_SUITE("sample cvc5 programs") {

	struct TestParameters {
		size_t loopback;
		size_t n_inputs;
		size_t n_outputs;
		Solver solver;
		std::vector<Kind> bitvector_ops;
		std::vector<Kind> logical_ops;
		std::vector<Kind> comparators;
		size_t bitvector_size;
		size_t literal_depth;
		size_t clauses_depth;
	};

	Term mkBitvectorFormula(TestParameters& params, std::vector<Term> vars, size_t depth = 0) {
		// if depth equals zero, return a comparator of bitvectors
		if (depth == 0) {
			// choose a random comparator from the comparators
			Kind op = params.comparators[rand() % params.comparators.size()];
			auto left = mkBitvectorFormula(params, vars, depth + 1);
			auto right = mkBitvectorFormula(params, vars, depth + 1);
			return params.solver.mkTerm(op, {left, right});
		}
		// if depth == params.literal_depth, choose a random variable or a literal
		if (depth == params.literal_depth) {
			if (rand() % 2 == 0) {
				// choose a variable from vars
				return vars[rand() % vars.size()];
			} else {
				// choose a literal
				return params.solver.mkBitVector(params.bitvector_size, rand() % (1 << params.bitvector_size));
			}
		}
		// otherwise, choose a random operation from the bitvector_ops
		Kind op = params.bitvector_ops[rand() % params.bitvector_ops.size()];
		if (op == Kind::BITVECTOR_NOT) {
			return params.solver.mkTerm(op, { mkBitvectorFormula(params, vars, depth + 1) });
		}
		if (op == Kind::BITVECTOR_SHL) {
			// for shift operations, we need a second argument
			auto left = mkBitvectorFormula(params, vars, depth + 1);
			auto right = params.solver.mkBitVector(params.bitvector_size, rand() % (params.bitvector_size));
			return params.solver.mkTerm(op, {left, right});
		}
		auto left = mkBitvectorFormula(params, vars, depth + 1);
		auto right = mkBitvectorFormula(params, vars, depth + 1);
		return params.solver.mkTerm(op, {left, right});
	}

	Term mkQuantifiedFreeFormula(TestParameters& params, std::vector<Term> vars, size_t clauses_depth = 0) {
		// if clause_depth == params.clauses_depth, choose a random var from the vector of vars
		if (clauses_depth == params.clauses_depth) {
			return mkBitvectorFormula(params, vars);
		}
		// otherwise, choose a random operation from the logical_ops
		Kind op = params.logical_ops[rand() % params.logical_ops.size()];
		if (op == Kind::NOT) {
			return params.solver.mkTerm(op, { mkQuantifiedFreeFormula(params, vars, clauses_depth + 1) });
		}

		auto left = mkQuantifiedFreeFormula(params, vars, clauses_depth + 1);
		auto right = mkQuantifiedFreeFormula(params, vars, clauses_depth + 1);
		return params.solver.mkTerm(op, { left, right });
	}

	Term mkFormula(TestParameters& params) {
		// create a bitvector sort
		Sort bvSort = params.solver.mkBitVectorSort(params.bitvector_size);

		std::vector<std::pair<std::vector<Term>, std::vector<Term>>> ios;
		std::vector<Term> vars;

		for (size_t t = 0; t <= params.loopback; ++t) {
			std::vector<Term> is_t;
			for (size_t i = 1; i <= params.n_inputs; ++i) {
				auto i_t = params.solver.mkConst(bvSort, "i_" + std::to_string(i) + "_" + std::to_string(t));
				is_t.emplace_back(i_t);
				vars.emplace_back(i_t);
			}
			std::vector<Term> os_t;
			for (size_t j = 1; j <= params.n_outputs; ++j) {
				auto o_t = params.solver.mkConst(bvSort, "o_" + std::to_string(j) + "_" + std::to_string(t));
				os_t.emplace_back(o_t);
				vars.emplace_back(o_t);
			}
			ios.emplace_back(is_t, os_t);
		}

		auto qf_fml = mkQuantifiedFreeFormula(params, vars);

		auto fml = qf_fml;
		for (size_t i = params.loopback; i > 0; --i) {
			fml = params.solver.mkTerm(
				Kind::FORALL, {
					params.solver.mkTerm(Kind::VARIABLE_LIST, ios[i - 1].first /* inputs */),
					params.solver.mkTerm(Kind::EXISTS, {
						params.solver.mkTerm(Kind::VARIABLE_LIST, ios[i - 1].second /* outputs */),
						fml
					})
				});
		}

		return fml;
	}

	TEST_CASE("initialize random seed") {
		// This test is just to ensure that the random seed is initialized correctly.
		srand(static_cast<unsigned int>(time(nullptr)));
		CHECK(true);
	}

	TEST_CASE("bitvector sample 1") {
		TestParameters params = {
			.loopback = 0,
			.n_inputs = 2,
			.n_outputs = 1,
			.solver = Solver(),
			.bitvector_ops = {Kind::BITVECTOR_ADD, Kind::BITVECTOR_SUB, Kind::BITVECTOR_AND, Kind::BITVECTOR_OR, Kind::BITVECTOR_XOR},
			.logical_ops = {Kind::AND, Kind::OR, Kind::NOT},
			.comparators = {Kind::EQUAL, Kind::DISTINCT},
			.bitvector_size = 64,
			.literal_depth = 4,
			.clauses_depth = 2
		};

		// configuring solver
		params.solver.setOption("produce-models", "true");
		params.solver.setOption("produce-unsat-cores", "true");
		params.solver.setLogic("BV");

		Term fml = mkFormula(params);
		params.solver.assertFormula(fml);
		Result r1 = params.solver.checkSat();

		std::cout << "formula: " << fml << std::endl;
		std::cout << "result: " << r1 << std::endl;
		CHECK(true);
	}
}
