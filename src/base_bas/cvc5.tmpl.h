// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

#include "nso_rr.h"

namespace idni::tau_lang {

using namespace cvc5;

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
	return cvc5_solver.mkTerm(Kind::BITVECTOR_ULE, {l, r});
}

Term mkGreater(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_UGT, {l, r});
}

Term mkGreaterEqual(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_UGE, {l, r});
}

Term mkLess(const Term& l, const Term& r) {
	return cvc5_solver.mkTerm(Kind::BITVECTOR_ULT, {l, r});
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
Term eval_bv(const tau<BAs...>& form, std::map<tau<BAs...>, Term> vars, std::map<tau<BAs...>, Term>& free_vars, bool checked) {
	auto nt = std::get<tau_source_sym>(form->value).n();

	//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " eval_bv/from: " << form;

	// control overflow/underflow if requested
	//if (checked) {
	//	BOOST_LOG_TRIVIAL(warning) << "(Warning) overflow/underflow checking is deactivated";
	//}

	auto bv_sort = [](const tau<BAs...> var) -> Sort {
		size_t bv_size;
		if (auto num = var | tau_parser::type | tau_parser::bv_type | tau_parser::num; num) {
			bv_size = num.value() 
				| only_child_extractor<BAs...> 
				| size_t_extractor<BAs...>
				| optional_value_extractor<size_t>;
		} else {
			// default size is cvc5_default_size
			bv_size = bv_default_size;
		}
		return cvc5_solver.mkBitVectorSort(bv_size); // cvc5_default_size-bit bitvector
	};

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
			cvc5_solver.push();

			std::vector<Term> cvc5_var_list;
			for (const auto& v : select_top(form->child[0], is_non_terminal<tau_parser::variable, BAs...>)) {
				auto vn = make_string(v);
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " eval_bv/vn: " << vn;
				auto x = cvc5_solver.mkVar(bv_sort(v), vn.c_str());
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " eval_bv/x: " << x;
				vars.emplace(v, x);
				cvc5_var_list.push_back(x);
			}
			auto f = eval_bv(form->child[1], vars, free_vars, checked);
			auto res = mkForall(cvc5_var_list, f);

			cvc5_solver.pop();

			//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " eval_bv/f: " << res;

			return res;
		}
		case tau_parser::wff_ex: {
			cvc5_solver.push();

			std::vector<Term> cvc5_var_list;
			for (const auto& v : select_top(form->child[0], is_non_terminal<tau_parser::variable, BAs...>)) {
				auto vn = make_string(v);
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " eval_bv/vn: " << vn;
				auto x = cvc5_solver.mkVar(bv_sort(v), vn.c_str());
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " eval_bv/x: " << x;
				vars.emplace(v, x);
				cvc5_var_list.push_back(x);
			}

			auto f = eval_bv(form->child[1], vars, free_vars, checked);
			auto res = mkExists(cvc5_var_list, f);

			cvc5_solver.pop();

			//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " eval_bv/f: " << res;
			return res;
		}
		case tau_parser::variable: {
			// check if the variable is alr
			if (auto it = vars.find(form); it != vars.end()) return it->second;
			if (auto it = free_vars.find(form); it != free_vars.end()) return it->second;
			auto vn = make_string(form);
			// create a new constant according to the type and added to the map
			//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " eval_bv/vn: " << vn;
			auto x = cvc5_solver.mkConst(bv_sort(form), vn.c_str());
			//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " eval_bv/x: " << x;
			//vars.emplace(form, x);
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
			return mkBitVectorNor(l, r);
		}
		case tau_parser::bv_xor: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkBitVectorXor(l, r);
		}
		case tau_parser::bv_xnor: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkBitVectorXnor(l, r);
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
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkBitVectorRotateLeft(l, r);
		}
		case tau_parser::bv_rotate_right: {
			auto l = eval_bv(form->child[0], vars, free_vars, checked);
			auto r = eval_bv(form->child[1], vars, free_vars, checked);
			return mkBitVectorRotateRight(l, r);
		}
		case tau_parser::bitvector: {
			auto expr = std::get<cvc5::Term>(form->child[0]->value);
			return expr;
		}
		default: {
			#ifdef DEBUG
			//BOOST_LOG_TRIVIAL(error) << "(Error) unknow bv non-terminal: " << nt;
			#endif // DEBUG
		}
	}
	assert(false);
}

template <typename...BAs>
bool is_bv_formula_sat(const tau<BAs...>& form) {
	std::map<tau<BAs...>, cvc5::Term> vars, free_vars;
	
	cvc5_solver.resetAssertions();
	cvc5_solver.push();
	auto expr = eval_bv(form, vars, free_vars);

	BOOST_LOG_TRIVIAL(trace) << "cvc5.tml.h:" << __LINE__ << " is_bv_formula_sat/form: " << form;
	BOOST_LOG_TRIVIAL(trace) << "cvc5.tml.h:" << __LINE__ << " is_bv_formula_sat/expr: " << expr;

	cvc5_solver.assertFormula(expr);
	auto result = cvc5_solver.checkSat().isSat();
	cvc5_solver.pop();

	BOOST_LOG_TRIVIAL(trace) << "cvc5.tml.h:" << __LINE__ << " is_bv_formula_sat/result: " << result;
	BOOST_LOG_TRIVIAL(info) << "(Info) bv system is " << result;

	return result;
}

template <typename...BAs>
bool is_bv_formula_unsat(const tau<BAs...>& form) {
	return !is_bv_formula_sat(form);
}

template<typename...BAs>
bool is_bv_formula_valid(const tau<BAs...>& form) {
	return is_bv_formula_unsat(build_wff_neg(form));
}

template<typename...BAs>
std::optional<solution<BAs...>> solve_bv(const tau<BAs...>& form) {
	std::map<tau<BAs...>, cvc5::Term> vars;
	std::map<tau<BAs...>, cvc5::Term> free_vars;
	cvc5_solver.resetAssertions();
	cvc5_solver.push();
	auto expr = eval_bv(form, vars, free_vars, false);

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
				std::make_shared<rewriter::depreciating::node<tau_sym<BAs...>>>(
					tau_parser::instance().literal(tau_parser::bitvector), bvv));
		}
		cvc5_solver.pop();
		return s;
	} else {

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "(Info) bv system is unsat";
		#endif // DEBUG
	}
	cvc5_solver.pop();
	return {};
}

/*template <typename...BAs>
std::optional<tau_nso<BAs...>> bv_ba_factory<BAs...>::parse(const std::string& src) {
	// parse source
	auto source = make_tau_source(src, {
			.start = tau_parser::bitvector });
	if (!source) return std::optional<tau_nso_t>{};
	auto rr = make_nso_rr_using_factory<tau_ba_t, BAs...>(source);
	if (!rr) return std::optional<tau_nso_t>{};
	// cvompute final result
	return std::optional<tau_nso_t>{
		rewriter::depreciating::make_node<tau_sym<tau_ba_t, BAs...>>(t, {}) };
}*/

template<typename...BAs>
std::optional<solution<BAs...>> solve_bv(const std::vector<tau<BAs...>>& lits) {
	return solve_bv(build_wff_and<BAs...>(lits));
}

template <typename...BAs>
std::optional<tau<BAs...>> bv_ba_factory<BAs...>::parse(const std::string& src) {
	// check source cache
	if (auto cn = cache.find(src); cn != cache.end())
		return cn->second;
/*		auto result = sbf_parser::instance().parse(src.c_str(), src.size());
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(tau_parser::error::info_lvl::INFO_BASIC);
		BOOST_LOG_TRIVIAL(error) << "(Error) " << msg << "\n";
		return std::optional<tau<BAs...>>{}; // Syntax error
	}
	using parse_symbol = sbf_parser::node_type;
	using namespace rewriter;
	auto root = make_node_from_tree<sbf_parser,
		drop_location_t<parse_symbol, sbf_sym>,
		sbf_sym>(
			drop_location<parse_symbol, sbf_sym>,
			result.get_shaped_tree());
	auto t = sbf_traverser_t(root) | sbf_parser::sbf;
	auto b = t.has_value()? eval_node(t): bdd_handle<Bool>::hfalse;
	std::variant<BAs...> vp {b};
	auto n = rewriter::depreciating::make_node<tau_sym<BAs...>>(vp, {});
	return cache.emplace(src, n).first->second;*/
	//return {};
	auto bv = make_nso_using_factory<BAs...>(src, { .start = tau_parser::bitvector });
	if (!bv) return std::optional<tau<BAs...>>{};
	// cvompute final result
	//return std::optional<tau<BAs...>>{ cache.emplace(src, bv.value() ).first->second };
	return bv;

}

} // namespace idni::tau_lang