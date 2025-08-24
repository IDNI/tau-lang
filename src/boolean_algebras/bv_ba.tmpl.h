// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "boolean_algebras/bv_ba.h" // Only for IDE resolution, not really needed.

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
size_t get_bv_size(const tref t) {
	using tt = typename tree<node>::traverser;
	if (auto num = tt(t)
			| node::type::type
			| node::type::bv_type
			| node::type::num; num) {
		return tt(num) | tt::only_child | tt::num;
	}
	return cvc5_default_bv_size;
}

template <NodeType node>
bv bv_eval_node(cvc5::Solver& solver, const typename tree<node>::traverser& form, subtree_map<node, bv> vars,
		subtree_map<node, bv>& free_vars, bool checked) {
	using tau = tree<node>;
	using tt = tree<node>::traverser;

	auto nt = form | tt::nt;

	switch (nt) {
		case node::type::wff_always:
		case node::type::wff_sometimes: {
			return bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
		}
		// due to hooks we should consider wff_t or bf_t
		case node::type::wff:
		case node::type::bv: {
			return bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
		}
		case node::type::wff_neg: {
			return make_term_not(bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked));
		}
		case node::type::wff_and: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_term_and(l, r);
		}
		case node::type::wff_or: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_term_or(l, r);
		}
		case node::type::wff_all: {
			solver.push();

			std::vector<bv> cvc5_var_list;
			for (const auto& v: (form | tt::first | tt::Tree).select_top(is<node, tau::variable>)) {
				auto vn = tau::get(v).to_str();
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/vn: " << vn;
				size_t bv_size = get_bv_size<node>(v);
				auto x = cvc5_term_manager.mkVar(cvc5_term_manager.mkBitVectorSort(bv_size), vn.c_str());
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/x: " << x;
				vars.emplace(v, x);
				cvc5_var_list.push_back(x);
			}
			auto f = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			auto res = make_term_forall(cvc5_var_list, f);

			solver.pop();

			//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/f: " << res;

			return res;
		}
		case node::type::wff_ex: {
			solver.push();

			std::vector<bv> cvc5_var_list;
			for (const auto& v: (form | tt::first | tt::Tree).select_top(is<node, tau::variable>)) {
				auto vn = tau::get(v).to_str();
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/vn: " << vn;
				size_t bv_size = get_bv_size<node>(v);
				auto x = cvc5_term_manager.mkVar(cvc5_term_manager.mkBitVectorSort(bv_size), vn.c_str());
				//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/x: " << x;
				vars.emplace(v, x);
				cvc5_var_list.push_back(x);
			}

			auto f = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			auto res = make_term_exists(cvc5_var_list, f);

			solver.pop();

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
			size_t bv_size = get_bv_size<node>(form | tt::ref);
			auto x = cvc5_term_manager.mkConst(cvc5_term_manager.mkBitVectorSort(bv_size), vn.c_str());
			//BOOST_LOG_TRIVIAL(trace) << "cvc5.tmpl.h:" << __LINE__ << " bv_eval_node/x: " << x;
			//vars.emplace(form, x);
			free_vars.emplace(form | tt::ref, x);
			return x;
		}
		case node::type::bv_checked: {
			return bv_eval_node<node>(solver, form | tt::first, vars, free_vars, true);
		}
		case node::type::bv_eq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_term_equal(l, r);
		}
		case node::type::bv_neq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_term_distinct(l, r);
		}
		case node::type::bv_lteq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_term_less_equal(l, r);
		}
		case node::type::bv_nlteq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_term_not(make_term_less_equal(l, r));
		}
		case node::type::bv_gt: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_term_greater(l, r);
		}
		case node::type::bv_ngt: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_term_not(make_term_greater(l, r));
		}
		case node::type::bv_gteq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_term_greater_equal(l, r);
		}
		case node::type::bv_ngteq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_term_not(make_term_greater_equal(l, r));
		}
		case node::type::bv_lt: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_term_less(l, r);
		}
		case node::type::bv_nlt: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_term_not(make_term_less(l, r));
		}
		case node::type::bv_neg: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			return make_bitvector_not(l);
		}
		case node::type::bv_add: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_bitvector_add(l, r);
		}
		case node::type::bv_sub: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_bitvector_sub(l, r);
		}
		case node::type::bv_mul: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_bitvector_mul(l, r);
		}
		case node::type::bv_div: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_bitvector_div(l, r);
		}
		case node::type::bv_mod: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_bitvector_mod(l, r);
		}
		case node::type::bv_and: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_bitvector_and(l, r);
		}
		case node::type::bv_nand: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_bitvector_nand(l, r);
		}
		case node::type::bv_or: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_bitvector_or(l, r);
		}
		case node::type::bv_nor: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_bitvector_nor(l, r);
		}
		case node::type::bv_xor: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_bitvector_xor(l, r);
		}
		case node::type::bv_xnor: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_bitvector_xnor(l, r);
		}
		/*case node::type::bv_min: {
			auto l = bv_eval_node<node>(solver, form->child[0], vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form->child[1], vars, free_vars, checked);
			return min(l, r);
		}
		case node::type::bv_max: {
			auto l = bv_eval_node<node>(solver, form->child[0], vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form->child[1], vars, free_vars, checked);
			return max(l, r);
		}*/
		case node::type::bv_rotate_left: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_bitvector_shr(l, r);
		}
		case node::type::bv_rotate_right: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return make_bitvector_shr(l, r);
		}
		case node::type::bv_constant: {
			auto cte = form | tt::bv_constant;
			DBG( assert(std::holds_alternative<bv>(cte)); )
			return std::get<bv>(cte);
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
	cvc5::Solver solver(cvc5_term_manager);
	config_cvc5_solver(solver);

	solver.push();
	auto expr = bv_eval_node<node>(solver, tt(form), vars, free_vars, false);
	solver.assertFormula(expr);
	auto result = solver.checkSat().isSat();
	solver.pop();
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
	cvc5::Solver solver(cvc5_term_manager);
	config_cvc5_solver(solver);

	solver.push();
	auto expr = bv_eval_node<node>(solver, tt(form), vars, free_vars, false);

	// solve the equations
	solver.assertFormula(expr);

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(info) << "(Info) solving cvc5 formula: " << expr;
	#endif // DEBUG

	auto result = solver.checkSat();
	// extract the model and return the solution if sat
	if (result.isSat()) {

		solution<node> s;
		for (const auto& v: free_vars) {
			// create a new tau_sym node with the value of the variable
			// and an empty child vector from cvc5 values

			// TODO (HIGH) fix when we introduce bvs in nodes
			// auto bvn = make_node<tau_sym<BAs...>>(solver.getValue(v.second), {});
			// std::vector<tau<BAs...>> bvv{bvn};
			s.emplace(v.first,
				tau::get(type::bv_constant));
		}
		solver.pop();
		return s;
	} else {

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info) << "(Info) bv system is unsat";
		#endif // DEBUG
	}
	solver.pop();
	return {};
}

template<NodeType node>
std::optional<solution<node>> solve_bv(const trefs& lits) {
	using tau = tree<node>;

	return solve_bv<node>(tau::build_wff_and(lits));
}

template<typename...BAs>
requires BAsPack<BAs...>
std::optional<constant_with_type<BAs...>> parse_bv(const std::string& src,
		[[maybe_unused]] size_t size,
		[[maybe_unused]] size_t base) {
	using tau = tree<node<BAs...>>;
	using tt = tau::traverser;

	typename tau::get_options opts{
		.parse = { .start = tau::bv_constant },
		.infer_ba_types = false,
		.reget_with_hooks = false };
	auto result = tau::get(src, opts);
	if (!result) return {};

	auto cte = tt(result) | tt::only_child | tt::ba_constant;
	return constant_with_type<BAs...>{ cte, "bv" };
}

} // namespace idni::tau_lang