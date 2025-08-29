// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "boolean_algebras/bv_ba.h" // Only for IDE resolution, not really needed.

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
size_t get_bv_size(const tref t) {
	using tau = tree<node>;

	static std::map<tref, size_t> bv_size_cache;
	using tt = typename tree<node>::traverser;
	if (auto type_ref = tt(t) | tau::type; type_ref) {
		if (auto it = bv_size_cache.find(type_ref.value()); it != bv_size_cache.end()) {
			return it->second;
		}
		auto type_name = type_ref.value_tree().get_string();
		auto bv_size = std::strtoull(type_name.substr(2).c_str(), nullptr, 10);
		bv_size_cache[type_ref.value()] = bv_size;
		return bv_size;
	}
	return cvc5_default_bv_size;
}

template <NodeType node>
std::optional<bv> bv_eval_node(cvc5::Solver& solver, const typename tree<node>::traverser& form, subtree_map<node, bv> vars,
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
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			return l ? std::optional<bv>(make_term_not(l.value())) : std::nullopt;
		}
		case node::type::wff_and: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_term_and(l.value(), r.value())) : std::nullopt;
		}
		case node::type::wff_or: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_term_or(l.value(), r.value())) : std::nullopt;
		}
		case node::type::wff_all: {
			solver.push();
			std::vector<bv> cvc5_var_list;
			for (const auto& v: (form | tt::first | tt::Tree).select_top(is<node, tau::variable>)) {
				auto vn = tau::get(v).to_str();
				size_t bv_size = get_bv_size<node>(v);
				auto x = cvc5_term_manager.mkVar(cvc5_term_manager.mkBitVectorSort(bv_size), vn.c_str());
				vars.emplace(v, x);
				cvc5_var_list.push_back(x);
			}
			auto f = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			if (!f) return std::nullopt;
			auto res = std::optional<bv>(make_term_forall(cvc5_var_list, f.value()));
			solver.pop();
			return res;
		}
		case node::type::wff_ex: {
			solver.push();
			std::vector<bv> cvc5_var_list;
			for (const auto& v: (form | tt::first | tt::Tree).select_top(is<node, tau::variable>)) {
				auto vn = tau::get(v).to_str();
				size_t bv_size = get_bv_size<node>(v);
				auto x = cvc5_term_manager.mkVar(cvc5_term_manager.mkBitVectorSort(bv_size), vn.c_str());
				vars.emplace(v, x);
				cvc5_var_list.push_back(x);
			}
			auto f = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			if (!f) return std::nullopt;
			auto res = std::optional<bv>(make_term_exists(cvc5_var_list, f.value()));
			solver.pop();
			return res;
		}
		case node::type::variable: {
			// check if the variable is alr
			if (auto it = vars.find(form | tt::ref); it != vars.end()) return it->second;
			if (auto it = free_vars.find(form | tt::ref); it != free_vars.end()) return it->second;
			auto vn = (form | tt::Tree).to_str();
			// create a new constant according to the type and added to the map
			size_t bv_size = get_bv_size<node>(form | tt::ref);
			auto x = cvc5_term_manager.mkConst(cvc5_term_manager.mkBitVectorSort(bv_size), vn.c_str());
			free_vars.emplace(tau::get(node::type::bv, form | tt::ref), x);
			return std::optional<bv>(x);
		}
		case node::type::bv_checked: {
			return bv_eval_node<node>(solver, form | tt::first, vars, free_vars, true);
		}
		case node::type::bv_eq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_term_equal(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_neq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_term_distinct(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_lteq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_term_less_equal(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_nlteq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_less_equal(l.value(), r.value()))) : std::nullopt;
		}
		case node::type::bv_gt: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_term_greater(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_ngt: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_greater(l.value(), r.value()))) : std::nullopt;
		}
		case node::type::bv_gteq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_term_greater_equal(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_ngteq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_greater_equal(l.value(), r.value()))) : std::nullopt;
		}
		case node::type::bv_lt: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_term_less(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_nlt: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_less(l.value(), r.value()))) : std::nullopt;
		}
		case node::type::bv_neg: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			return (l) ? std::optional<bv>(make_bitvector_not(l.value())) : std::nullopt;
		}
		case node::type::bv_add: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_bitvector_add(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_sub: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_bitvector_sub(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_mul: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_bitvector_mul(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_div: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_bitvector_div(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_mod: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_bitvector_mod(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_and: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_bitvector_and(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_nand: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_bitvector_nand(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_or: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_bitvector_or(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_nor: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_bitvector_nor(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_xor: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_bitvector_xor(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_xnor: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_bitvector_xnor(l.value(), r.value())) : std::nullopt;
		}
		/*case node::type::bv_min: {
			auto l = bv_eval_node<node>(solver, form->child[0], vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form->child[1], vars, free_vars, checked);
			return (l && r) ? min(l.value(), r.value()) : std::nullopt;
		}
		case node::type::bv_max: {
			auto l = bv_eval_node<node>(solver, form->child[0], vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form->child[1], vars, free_vars, checked);
			return (l && r) ? max(l.value(), r.value()) : std::nullopt;
		}*/
		case node::type::bv_shl: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_bitvector_shr(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_shr: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, checked);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, checked);
			return (l && r) ? std::optional<bv>(make_bitvector_shr(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_constant: {
			auto cte = form | tt::bv_constant;
			DBG(assert(std::holds_alternative<bv>(cte));)
			return std::optional<bv>(std::get<bv>(cte));
		}
		default:
			return std::nullopt;
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
	// TODO (MEDIUM) handle this case at an upper level (maybe return an optional)
	if (!expr) {
		LOG_ERROR << "Failed to translate the formula to cvc5: " << LOG_FM(form);
		solver.pop();
		return false;
	}
	solver.assertFormula(expr.value());
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

	subtree_map<node, bv> vars, free_vars;
	cvc5::Solver solver(cvc5_term_manager);
	config_cvc5_solver(solver);

	solver.push();
	auto expr = bv_eval_node<node>(solver, tt(form), vars, free_vars, false);
	if (!expr) {
		LOG_ERROR << "Failed to translate the formula to cvc5: " << LOG_FM(form);
		solver.pop();
		return std::nullopt;
	}
	// solve the equations
	solver.assertFormula(expr.value());
	LOG_DEBUG << "Solving bitvector formula: " << expr.value();
	auto result = solver.checkSat();
	// extract the model and return the solution if sat
	if (result.isSat()) {
		LOG_DEBUG << "Bitvector system is sat.";
		solution<node> s;
		for (const auto& [tau_var, bv_var] : free_vars) {
			auto cte = solver.getValue(bv_var);
			s.emplace(tau_var, tau::get(tau::bv, tau::get_bv_constant({cte})));
		}
		solver.pop();
		return s;
	} else
		LOG_DEBUG << "Bitvector system is unsat.";
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
		.reget_with_hooks = true };
	auto result = tau::get(src, opts);
	if (!result) {
		LOG_ERROR << "Failed to parse bitvector constant: " << src;
		return {};
	}
	auto cte = tt(result) | tt::ba_constant;
	return constant_with_type<BAs...>{ cte, "bv" };
}

} // namespace idni::tau_lang