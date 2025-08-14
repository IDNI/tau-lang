// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "bv_ba.h" // Only for IDE resolution, not really needed.

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

/*template <NodeType node>
bv get_bv_var_sort(const tref var) {
	using tt = tree<node>::traverser;

	size_t bv_size;
	if (auto num = tt(var)
			| node::type::type
			| node::type::bv_type
			| node::type::num; num) {
		bv_size = tt(num) | tt::only_child | tt::num;
	} else {
		// default size is cvc5_default_size
		bv_size = bv_default_size;
	}
	return bv_solver.mkBitVectorSort(bv_size); // cvc5_default_size-bit bitvector
};*/

template <NodeType node>
bv bv_eval_node(const typename tree<node>::traverser& form, subtree_map<node, bv> vars,
		subtree_map<node, bv>& free_vars, bool checked) {
	using tau = tree<node>;
	using tt = tree<node>::traverser;

	auto nt = form | tt::nt;

	switch (nt) {
		case node::type::wff_always:
		case node::type::wff_sometimes: {
			return bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
		}
		// due to hooks we should consider wff_t or bf_t
		case node::type::wff:
		case node::type::bv: {
			return bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
		}
		case node::type::wff_neg: {
			return make_term_not(bv_eval_node<node>(form | tt::first, vars, free_vars, checked));
		}
		case node::type::wff_and: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_term_and(l, r);
		}
		case node::type::wff_or: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_term_or(l, r);
		}
		case node::type::wff_all: {
			bv_solver.push();

			std::vector<bv> cvc5_var_list;
			for (const auto& v: (form | tt::first | tt::Tree).select_top(is<node, tau::variable>)) {
				auto vn = tau::get(v).to_str();
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/vn: " << vn;
				auto bv_size = get_bv_size<node>(v);
				auto x = bv_solver.mkVar(bv_solver.mkBitVectorSort(bv_size), vn.c_str());
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/x: " << x;
				vars.emplace(v, x);
				cvc5_var_list.push_back(x);
			}
			auto f = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			auto res = make_term_forall(cvc5_var_list, f);

			bv_solver.pop();

			//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/f: " << res;

			return res;
		}
		case node::type::wff_ex: {
			bv_solver.push();

			std::vector<bv> cvc5_var_list;
			for (const auto& v: (form | tt::first | tt::Tree).select_top(is<node, tau::variable>)) {
				auto vn = tau::get(v).to_str();
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/vn: " << vn;
				auto bv_size = get_bv_size<node>(v);
				auto x = bv_solver.mkVar(bv_solver.mkBitVectorSort(bv_size), vn.c_str());
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/x: " << x;
				vars.emplace(v, x);
				cvc5_var_list.push_back(x);
			}

			auto f = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			auto res = make_term_exists(cvc5_var_list, f);

			bv_solver.pop();

			//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/f: " << res;
			return res;
		}
		case node::type::variable: {
			// check if the variable is alr
			if (auto it = vars.find(form | tt::ref); it != vars.end()) return it->second;
			if (auto it = free_vars.find(form | tt::ref); it != free_vars.end()) return it->second;
			auto vn = (form | tt::Tree).to_str();
			// create a new constant according to the type and added to the map
			//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/vn: " << vn;
			auto bv_size = get_bv_size<node>(form | tt::ref);
			auto x = bv_solver.mkConst(bv_solver.mkBitVectorSort(bv_size), vn.c_str());
			//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/x: " << x;
			//vars.emplace(form, x);
			free_vars.emplace(form | tt::ref, x);
			return x;
		}
		case node::type::bv_checked: {
			return bv_eval_node<node>(form | tt::first, vars, free_vars, true);
		}
		case node::type::bv_eq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_term_equal(l, r);
		}
		case node::type::bv_neq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_term_distinct(l, r);
		}
		case node::type::bv_less_equal: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_term_less_equal(l, r);
		}
		case node::type::bv_nleq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_term_not(make_term_less_equal(l, r));
		}
		case node::type::bv_greater: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_term_greater(l, r);
		}
		case node::type::bv_ngreater: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_term_not(make_term_greater(l, r));
		}
		case node::type::bv_greater_equal: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_term_greater_equal(l, r);
		}
		case node::type::bv_ngeq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_term_not(make_term_greater_equal(l, r));
		}
		case node::type::bv_less: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_term_less(l, r);
		}
		case node::type::bv_nless: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_term_not(make_term_less(l, r));
		}
		case node::type::bv_neg: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			return make_bv_not(l);
		}
		case node::type::bv_add: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_bv_add(l, r);
		}
		case node::type::bv_sub: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_bv_sub(l, r);
		}
		case node::type::bv_mul: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_bv_mul(l, r);
		}
		case node::type::bv_div: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_bv_div(l, r);
		}
		case node::type::bv_mod: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_bv_mod(l, r);
		}
		case node::type::bv_and: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_bv_and(l, r);
		}
		case node::type::bv_nand: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_bv_nand(l, r);
		}
		case node::type::bv_or: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_bv_or(l, r);
		}
		case node::type::bv_nor: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_bv_nor(l, r);
		}
		case node::type::bv_xor: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_bv_xor(l, r);
		}
		case node::type::bv_xnor: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_bv_xnor(l, r);
		}
		/*case node::type::bv_min: {
			auto l = bv_eval_node<node>(form->child[0], vars, free_vars, checked);
			auto r = bv_eval_node<node>(form->child[1], vars, free_vars, checked);
			return min(l, r);
		}
		case node::type::bv_max: {
			auto l = bv_eval_node<node>(form->child[0], vars, free_vars, checked);
			auto r = bv_eval_node<node>(form->child[1], vars, free_vars, checked);
			return max(l, r);
		}*/
		case node::type::bv_rotate_left: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_bv_shr(l, r);
		}
		case node::type::bv_rotate_right: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars, checked);
			return make_bv_shr(l, r);
		}
		case node::type::bitvector: {
			// TODO (HIGH) fix when we introduce bvs in nodes
			return bv_solver.mkConst(bv_solver.mkBitVectorSort(2));
			// by replacing with
			//auto cte = form | tt::first | tt::ba_constant;
			//DBG( assert(std::holds_alternative<bv>(cte)); )
			//return std::get<bv>(cte);
		}
		default: {
			throw cvc5::CVC5ApiException("unsupported bitvector operation");
		}
	}
}


template <NodeType node>
bool is_bv_formula_sat(tref form) {
	using tt = tree<node>::traverser;

	subtree_map<node, bv> vars, free_vars;

	bv_solver.resetAssertions();
	bv_solver.push();
//	auto tform = tt(form);
	auto expr = bv_eval_node<node>(tt(form), vars, free_vars, false);
	bv_solver.assertFormula(expr);
	auto result = bv_solver.checkSat().isSat();
	bv_solver.pop();
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

	subtree_map<node, bv> vars, free_vars;

	bv_solver.resetAssertions();
	bv_solver.push();
	auto expr = bv_eval_node<node>(tt(form), vars, free_vars, false);

	// solve the equations
	bv_solver.assertFormula(expr);

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(info) << "(Info) solving cvc5 formula: " << expr;
	#endif // DEBUG

	auto result = bv_solver.checkSat();
	// extract the model and return the solution if sat
	if (result.isSat()) {

		solution<node> s;
		for (const auto& v: free_vars) {
			// create a new tau_sym node with the value of the variable
			// and an empty child vector from cvc5 values

			// TODO (HIGH) fix when we introduce bvs in nodes
			// auto bvn = make_node<tau_sym<BAs...>>(cvc5_solver.getValue(v.second), {});
			// std::vector<tau<BAs...>> bvv{bvn};
			s.emplace(v.first,
				// TODO (HIGH) fix when we introduce bvs in nodes
				//std::make_shared<rewriter::depreciating::node<tau_sym<BAs...>>>(
				//	tau_parser::instance().literal(tau_parser::bitvector), bvv));
				tau::get(type::bitvector));
		}
		bv_solver.pop();
		return s;
	} else {

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "(Info) bv system is unsat";
		#endif // DEBUG
	}
	bv_solver.pop();
	return {};
}

template<NodeType node>
std::optional<node> solve_bv(const trefs& lits) {
	using tau = tree<node>;

	return solve_bv(tau::build_wff_and(lits));
}

template<typename...BAs>
requires BAsPack<BAs...>
std::optional<constant_with_type<BAs...>> parse(const std::string& src) {
	using tau = tree<node<BAs...>>;
	using tt = tau::traverser;

	typename tau::get_options opts{
		.parse = { .start = tau::bitvector },
		.infer_ba_types = false,
		.reget_with_hooks = false };
	auto result = tau::get(src, opts);
	if (!result) return {};

	auto bv_constant = tt(result) | tt::only_child | tt::ba_constant;
	return { bv_constant };
}

} // namespace idni::tau_lang