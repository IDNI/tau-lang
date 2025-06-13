// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

#include "nso_rr.h"

namespace idni::tau_lang {

using namespace cvc5;

static auto BV = cvc5_solver.mkBitVectorSort(64); // 64-bit bitvector
static auto BOOL = cvc5_solver.getBooleanSort();

Term mkNot(const Term& t) {
	return cvc5_solver.mkTerm(Kind::NOT, {t});
}

Term mkAnd(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::AND, {l, r});
}

Term mkOr(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::OR, {l, r});
}

Term mkForall(const Term& var, const Term& body) {
	auto vars = cvc5_solver.mkTerm(Kind::VARIABLE_LIST, {var});
	return cvc5_solver.mkTerm(Kind::FORALL, {vars, body});
}

Term mkForall(const std::vector<Term>& vars, const Term& body) {
	auto cvc5_var_list = cvc5_solver.mkTerm(Kind::VARIABLE_LIST, vars);
	return cvc5_solver.mkTerm(Kind::FORALL, {cvc5_var_list, body});
}

Term mkExists(const Term& var, const Term& body) {
	auto vars = cvc5_solver.mkTerm(Kind::VARIABLE_LIST, {var});
	return cvc5_solver.mkTerm(Kind::EXISTS, {vars, body});
}

Term mkExists(const std::vector<Term>& vars, const Term& body) {
	auto cvc5_var_list = cvc5_solver.mkTerm(Kind::VARIABLE_LIST, vars);
	return cvc5_solver.mkTerm(Kind::EXISTS, {cvc5_var_list, body});
}

Term mkEqual(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::EQUAL, {l, r});
}

Term mkDisticnt(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::DISTINCT, {l, r});
}

Term mkLessEqual(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::LEQ, {l, r});
}

Term mkGreater(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::GT, {l, r});
}

Term mkGreaterEqual(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::GEQ, {l, r});
}

Term mkLess(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::LT, {l, r});
}

Term mkBitVectorNot(const Term& t) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_NOT, {t});
}

Term mkBitVectorAnd(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_AND, {l, r});
}

Term mkBitVectorOr(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_OR, {l, r});
}

Term mkBitVectorNor(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_NOR, {l, r});
}

Term mkBitVectorXor(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_XOR, {l, r});
}

Term mkBitVectorXnor(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_XNOR, {l, r});
}

Term mkBitVectorNand(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_NAND, {l, r});
}

Term mkBitVectorAdd(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_ADD, {l, r});
}

Term mkBitVectorSub(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_SUB, {l, r});
}

Term mkBitVectorMul(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_MULT, {l, r});
}

Term mkBitVectorDiv(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_UDIV, {l, r});
}

Term mkBitVectorMod(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_SMOD, {l, r});
}

Term mkBitVectorRotateLeft(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_SHL, {l, r});
}

Term mkBitVectorRotateRight(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_LSHR, {l, r});
}


template<typename...BAs>
Term eval_bv(const tau<BAs...>& form, std::map<tau<BAs...>, Term>& vars, std::map<tau<BAs...>, Term>& free_vars, bool checked) {
	auto nt = std::get<tau_source_sym>(form->value).n();

	BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " eval_bv/from: " << form;

	// control overflow/underflow if requested
	if (checked) {
		BOOST_LOG_TRIVIAL(warning)
			<< "(Warning) overflow/underflow checking is deactivated";
	}

	switch (nt) {
		// due to hooks we should consider wff_t or bf_t
		case tau_parser::wff:
		case tau_parser::bv: {
			auto expr = eval_bv(form->child[0], vars, free_vars, checked);
			return expr;
		}
		case tau_parser::wff_neg: {
			return mkNot(eval_bv(form->child[0], vars, free_vars, checked));
		}
		case tau_parser::wff_and: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkAnd(l, r);
		}
		case tau_parser::wff_or: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkOr(l, r);
		}
		case tau_parser::wff_all: {
			std::vector<Term> cvc5_var_list;
			for (const auto& v : select_top(form->child[0], is_non_terminal<tau_parser::variable, BAs...>)) {
				auto vn = make_string(tau_node_terminal_extractor<BAs...>, v);
				auto x = cvc5_solver.mkVar(BV, vn.c_str());
				vars.emplace(v, x);
				cvc5_var_list.push_back(x);
			}
			auto f = eval_bv(form->child[1], vars, free_vars, checked);
			BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " eval_bv/f: " << f;
			return mkForall(cvc5_var_list, f);
		}
		case tau_parser::wff_ex: {
			std::vector<Term> cvc5_var_list;
			for (const auto& v : select_top(form->child[0], is_non_terminal<tau_parser::variable, BAs...>)) {
				auto vn = make_string(tau_node_terminal_extractor<BAs...>, v);
				auto x = cvc5_solver.mkVar(BV, vn.c_str());
				vars.emplace(v, x);
				cvc5_var_list.push_back(x);
			}
			auto f = eval_bv(form->child[1], vars, free_vars, checked);
			BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " eval_bv/f: " << f;
			return mkExists(cvc5_var_list, f);
		}
		case tau_parser::variable: {
			// check if the variable is alr
			if (auto it = vars.find(form); it != vars.end()) return it->second;
			if (auto it = free_vars.find(form); it != free_vars.end()) return it->second;
			auto vn = make_string(tau_node_terminal_extractor<BAs...>, form);
			// create a new constant according to the type and added to the map
			auto x = cvc5_solver.mkConst(BV, vn.c_str());
			vars.emplace(form, x);
			free_vars.emplace(form, x);
			return x;
		}
		case tau_parser::bv_checked: {
			return eval_bv(form->child[0], vars, free_vars, true);
		}
		case tau_parser::bv_eq: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkEqual(l, r);
		};
		case tau_parser::bv_neq: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkDisticnt(l, r);
		}
		case tau_parser::bv_less_equal: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkLessEqual(l, r);
		}
		case tau_parser::bv_nleq: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkNot(mkLessEqual(l, r));
		}
		case tau_parser::bv_greater: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkGreater(l, r);
		}
		case tau_parser::bv_ngreater: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkNot(mkGreater(l, r));
		}
		case tau_parser::bv_greater_equal: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkGreaterEqual(l, r);
		}
		case tau_parser::bv_ngeq: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkNot(mkGreaterEqual(l, r));
		}
		case tau_parser::bv_less: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkLess(l, r);
		}
		case tau_parser::bv_nless: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkNot(mkLess(l, r));
		}
		case tau_parser::bv_neg: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			return mkBitVectorNot(l);
		}
		case tau_parser::bv_add: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkBitVectorAdd(l, r);
		}
		case tau_parser::bv_sub: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkBitVectorSub(l, r);
		}
		case tau_parser::bv_mul: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkBitVectorMul(l, r);
		}
		case tau_parser::bv_div: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkBitVectorDiv(l, r);
		}
		case tau_parser::bv_mod: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkBitVectorMod(l, r);
		}
		case tau_parser::bv_and: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkBitVectorAnd(l, r);
		}
		case tau_parser::bv_nand: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkBitVectorNand(l, r);
		}
		case tau_parser::bv_or: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkBitVectorOr(l, r);
		}
		case tau_parser::bv_nor: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkNot(mkBitVectorNor(l, r));
		}
		case tau_parser::bv_xor: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkBitVectorXor(l, r);
		}
		case tau_parser::bv_xnor: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkNot(mkBitVectorXnor(l, r));
		}
		/*case tau_parser::bv_min: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return min(l, r);
		}
		case tau_parser::bv_max: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return max(l, r);
		}*/
		case tau_parser::bv_rotate_left: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto value = make_string(tau_node_terminal_extractor<BAs...>, form->child[1]);
			// this is the type of rotate_left
			unsigned int ul = std::stoul(value);
			return mkBitVectorRotateLeft(l, cvc5_solver.mkBitVector(64, ul));
		}
		case tau_parser::bv_rotate_right: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto value = make_string(tau_node_terminal_extractor<BAs...>, form->child[1]);
			// this is the type of rotate_left
			unsigned int ul = std::stoul(value);
			return mkBitVectorRotateRight(l, cvc5_solver.mkBitVector(64, ul));
		}
		case tau_parser::bitvector: {
			auto expr = std::get<cvc5::Term>(form->child[0]->value);
			return expr;
		}
		default: {
			#ifdef DEBUG
			BOOST_LOG_TRIVIAL(error) << "(Error) unknow bv non-terminal: " << nt;
			#endif // DEBUG
		}
	}
	assert(false);
}


template <typename...BAs>
bool is_bv_formula_sat(const tau<BAs...>& form) {
	std::map<tau<BAs...>, cvc5::Term> vars;
	std::map<tau<BAs...>, cvc5::Term> free_vars;
	cvc5_solver.resetAssertions();
	// collect all variables and free variables
	auto expr = eval_bv(form, vars, free_vars);

	BOOST_LOG_TRIVIAL(trace) << "cvc5.tml.h:" << __LINE__ << " is_bv_formula_sat/expr: " << expr;

	// solve the equations
	cvc5_solver.assertFormula(expr);

	auto result = cvc5_solver.checkSat();

	BOOST_LOG_TRIVIAL(trace) << "cvc5.tml.h:" << __LINE__ << " is_bv_formula_sat/result: " << result;
	BOOST_LOG_TRIVIAL(info) << "(Info) bv system is " << result;

	return result.isSat();
}

template<typename...BAs>
std::optional<solution<BAs...>> solve_bv(const tau<BAs...>& form) {
	std::map<tau<BAs...>, cvc5::Term> vars;
	std::map<tau<BAs...>, cvc5::Term> free_vars;
	cvc5_solver.resetAssertions();
	auto expr = eval_bv(form, vars, free_vars);

	// solve the equations
	cvc5_solver.assertFormula(expr);

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(info) << "(Info) solving cvc5 formula: " << expr;
	#endif // DEBUG

	auto result = cvc5_solver.checkSat();
	// extract the model and return the solution if sat
	if (result.isSat()) {

		solution<BAs...> s;
		for (const auto& v: free_vars) {
			// create a new tau_sym node with the value of the variable
			// and an empty child vector from cvc5 values

			auto bvn = make_node<tau_sym<BAs...>>(cvc5_solver.getValue(v.second), {});
			std::vector<tau<BAs...>> bvv{bvn};
			s.emplace(v.first,
				std::make_shared<rewriter::node<tau_sym<BAs...>>>(
					tau_parser::instance().literal(tau_parser::bitvector), bvv));
		}
		return s;
	} else {

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "(Info) bv system is unsat";
		#endif // DEBUG
	}
	return {};
}


} // namespace idni::tau_lang