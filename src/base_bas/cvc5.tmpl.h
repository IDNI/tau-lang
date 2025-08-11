// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

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

template <NodeType node>
Term bv_eval_node(const typename tree<node>::traverser& form, subtree_map<node, Term> vars,
		subtree_map<node, Term>& free_vars, bool checked) {
	using tau = tree<node>;
	using tt = tau::traverser;
	using type = tau_parser::nonterminal;

	auto nt = form | tt::nt;

	auto bv_sort = [](tref var) -> Sort {
		size_t bv_size;
		if (auto num = tt(var) | type::type | type::bv_type | type::num; num) {
			bv_size = tt(num) | tt::only_child | tt::num;
		} else {
			// default size is cvc5_default_size
			bv_size = bv_default_size;
		}
		return cvc5_solver.mkBitVectorSort(bv_size); // cvc5_default_size-bit bitvector
	};

	switch (nt) {
		case type::wff_always:
		case type::wff_sometimes: {
			return bv_eval_node(form | tt::first, vars, free_vars, checked);
		}
		// due to hooks we should consider wff_t or bf_t
		case type::wff:
		case type::bv: {
			auto expr = bv_eval_node(form | tt::first, vars, free_vars, checked);
			return expr;
		}
		case type::wff_neg: {
			return mkNot(bv_eval_node(form | tt::first, vars, free_vars, checked));
		}
		case type::wff_and: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkAnd(l, r);
		}
		case type::wff_or: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkOr(l, r);
		}
		case type::wff_all: {
			cvc5_solver.push();

			std::vector<Term> cvc5_var_list;
			for (const auto& v: (form | tt::first | tt::Tree).select_top(is<node, tau::variable>)) {
				auto vn = tau::get(v).to_str();
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/vn: " << vn;
				auto x = cvc5_solver.mkVar(bv_sort(v), vn.c_str());
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/x: " << x;
				vars.emplace(v, x);
				cvc5_var_list.push_back(x);
			}
			auto f = bv_eval_node(form | tt::second, vars, free_vars, checked);
			auto res = mkForall(cvc5_var_list, f);

			cvc5_solver.pop();

			//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/f: " << res;

			return res;
		}
		case type::wff_ex: {
			cvc5_solver.push();

			std::vector<Term> cvc5_var_list;
			for (const auto& v: (form | tt::first | tt::Tree).select_top(is<node, tau::variable>)) {
				auto vn = tau::get(v).to_str();
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/vn: " << vn;
				auto x = cvc5_solver.mkVar(bv_sort(v), vn.c_str());
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/x: " << x;
				vars.emplace(v, x);
				cvc5_var_list.push_back(x);
			}

			auto f = bv_eval_node(form | tt::second, vars, free_vars, checked);
			auto res = mkExists(cvc5_var_list, f);

			cvc5_solver.pop();

			//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/f: " << res;
			return res;
		}
		case type::variable: {
			// check if the variable is alr
			if (auto it = vars.find(form | tt::ref); it != vars.end()) return it->second;
			if (auto it = free_vars.find(form | tt::ref); it != free_vars.end()) return it->second;
			auto vn = (form | tt::Tree).to_str();
			// create a new constant according to the type and added to the map
			//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/vn: " << vn;
			auto x = cvc5_solver.mkConst(bv_sort(form | tt::ref), vn.c_str());
			//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/x: " << x;
			//vars.emplace(form, x);
			free_vars.emplace(form | tt::ref, x);
			return x;
		}
		case type::bv_checked: {
			return bv_eval_node(form | tt::first, vars, free_vars, true);
		}
		case type::bv_eq: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkEqual(l, r);
		}
		case type::bv_neq: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkDisticnt(l, r);
		}
		case type::bv_less_equal: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkLessEqual(l, r);
		}
		case type::bv_nleq: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkNot(mkLessEqual(l, r));
		}
		case type::bv_greater: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkGreater(l, r);
		}
		case type::bv_ngreater: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkNot(mkGreater(l, r));
		}
		case type::bv_greater_equal: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkGreaterEqual(l, r);
		}
		case type::bv_ngeq: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkNot(mkGreaterEqual(l, r));
		}
		case type::bv_less: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkLess(l, r);
		}
		case type::bv_nless: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkNot(mkLess(l, r));
		}
		case type::bv_neg: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			return mkBitVectorNot(l);
		}
		case type::bv_add: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkBitVectorAdd(l, r);
		}
		case type::bv_sub: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkBitVectorSub(l, r);
		}
		case type::bv_mul: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkBitVectorMul(l, r);
		}
		case type::bv_div: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkBitVectorDiv(l, r);
		}
		case type::bv_mod: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkBitVectorMod(l, r);
		}
		case type::bv_and: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkBitVectorAnd(l, r);
		}
		case type::bv_nand: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkBitVectorNand(l, r);
		}
		case type::bv_or: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkBitVectorOr(l, r);
		}
		case type::bv_nor: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkBitVectorNor(l, r);
		}
		case type::bv_xor: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkBitVectorXor(l, r);
		}
		case type::bv_xnor: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkBitVectorXnor(l, r);
		}
		/*case type::bv_min: {
			auto l = bv_eval_node(form->child[0], vars, free_vars, checked);
			auto r = bv_eval_node(form->child[1], vars, free_vars, checked);
			return min(l, r);
		}
		case type::bv_max: {
			auto l = bv_eval_node(form->child[0], vars, free_vars, checked);
			auto r = bv_eval_node(form->child[1], vars, free_vars, checked);
			return max(l, r);
		}*/
		case type::bv_rotate_left: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkBitVectorRotateLeft(l, r);
		}
		case type::bv_rotate_right: {
			auto l = bv_eval_node(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node(form | tt::second, vars, free_vars, checked);
			return mkBitVectorRotateRight(l, r);
		}
		case type::bitvector: {
			// TODO (HIGH) fix when we introduce Terms in nodes
			// auto expr = std::get<Term>(form | tt::first | tt::template extractor<Term>());
			auto expr = cvc5_solver.mkConst(bv_sort(form | tt::ref));
			return expr;
		}
		default: {
			#ifdef DEBUG
			//print_tau_tree(std::cout, form);
			#endif // DEBUG
		}
	}
	throw cvc5::CVC5ApiException("unsupported bitvector operation");
}

template <NodeType node>
bool is_bv_formula_sat(tref form) {
	using tt = tree<node>::traverser;

	subtree_map<node, Term> vars, free_vars;

	cvc5_solver.resetAssertions();
	cvc5_solver.push();
//	auto tform = tt(form);
	auto expr = bv_eval_node(tt(form), vars, free_vars, false);
	cvc5_solver.assertFormula(expr);
	auto result = cvc5_solver.checkSat().isSat();
	cvc5_solver.pop();
	return result;
}

template <NodeType node>
bool is_bv_formula_unsat(tref form) {
	return !is_bv_formula_sat<node>(form);
}

template <NodeType node>
bool is_bv_formula_valid(tref form) {
	return is_bv_formula_unsat<node>(build_wff_neg<node>(form));
}

template <NodeType node>
std::optional<solution<node>> solve_bv(const tref form) {
	using tau = tree<node>;
	using tt = tau::traverser;
	using type = tau_parser::nonterminal;

	subtree_map<node, Term> vars, free_vars;

	cvc5_solver.resetAssertions();
	cvc5_solver.push();
	auto expr = bv_eval_node(tt(form), vars, free_vars, false);

	// solve the equations
	cvc5_solver.assertFormula(expr);

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(info) << "(Info) solving cvc5 formula: " << expr;
	#endif // DEBUG

	auto result = cvc5_solver.checkSat();
	// extract the model and return the solution if sat
	if (result.isSat()) {

		solution<node> s;
		for (const auto& v: free_vars) {
			// create a new tau_sym node with the value of the variable
			// and an empty child vector from cvc5 values

			// TODO (HIGH) fix when we introduce Terms in nodes
			// auto bvn = make_node<tau_sym<BAs...>>(cvc5_solver.getValue(v.second), {});
			// std::vector<tau<BAs...>> bvv{bvn};
			s.emplace(v.first,
				// TODO (HIGH) fix when we introduce Terms in nodes
				//std::make_shared<rewriter::depreciating::node<tau_sym<BAs...>>>(
				//	tau_parser::instance().literal(tau_parser::bitvector), bvv));
				tau::get(type::bitvector));
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
}

template<typename...BAs>
std::optional<solution<BAs...>> solve_bv(const std::vector<tau<BAs...>>& lits) {
	return solve_bv(build_wff_and<BAs...>(lits));
}

template <typename...BAs>
std::optional<tau<BAs...>> bv_ba_factory<BAs...>::parse(const std::string& src) {
	// check source cache
	if (auto cn = cache.find(src); cn != cache.end())
		return cn->second;
		auto result = sbf_parser::instance().parse(src.c_str(), src.size());
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
	return cache.emplace(src, n).first->second;
	//return {};
	auto bv = make_nso_using_factory<BAs...>(src, { .start = tau_parser::bitvector });
	if (!bv) return std::optional<tau<BAs...>>{};
	// cvompute final result
	//return std::optional<tau<BAs...>>{ cache.emplace(src, bv.value() ).first->second };
	return bv;

}*/

} // namespace idni::tau_lang