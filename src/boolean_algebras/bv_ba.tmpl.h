// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "boolean_algebras/bv_ba.h" // Only for IDE resolution, not really needed.
#include "../parser/bitvector_parser.generated.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
size_t get_bv_size(const tref t) {
	using tau = tree<node>;
	using tt = tau::traverser;
	if (auto subtype = tt(t) | tau::subtype | tt::ref; subtype) {
		return tau::get(subtype)[0].get_num();
	}
	return default_bv_size;
}

template <NodeType node>
std::optional<bv> bv_eval_node(cvc5::Solver& solver, const typename tree<node>::traverser& form, subtree_map<node, bv> vars,
		subtree_map<node, bv>& free_vars, tref type_tree) {
	using tau = tree<node>;
	using tt = tree<node>::traverser;

	auto nt = form | tt::nt;

	switch (nt) {
		case node::type::wff_always:
		case node::type::wff_sometimes: {
			return bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
		}
		// due to hooks we should consider wff_t or bf_t
		case node::type::wff: case node::type::bf:
		// TODO (HIGH) deal with those eq appropiately
		/*case node::type::bv:*/ {
			return bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
		}
		case node::type::wff_neg: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			return l ? std::optional<bv>(make_term_not(l.value())) : std::nullopt;
		}
		case node::type::wff_and: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_term_and(l.value(), r.value())) : std::nullopt;
		}
		case node::type::wff_or: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_term_or(l.value(), r.value())) : std::nullopt;
		}
		case node::type::wff_all: {
			tref v = (form | tt::first | tt::ref);
			DBG(assert(is<node>(v, tau::variable));)
			size_t bv_size = get_bv_size<node>(type_tree);
			bv x = cvc5_term_manager.mkVar(cvc5_term_manager.mkBitVectorSort(bv_size), tau::get(v).to_str());
			vars.emplace(v, x);

			auto f = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			if (!f) return std::nullopt;
			auto res = std::optional<bv>(make_term_forall({x}, f.value()));
			vars.erase(v);
			return res;
		}
		case node::type::wff_ex: {
			tref v = (form | tt::first | tt::ref);
			DBG(assert(is<node>(v, tau::variable));)
			size_t bv_size = get_bv_size<node>(type_tree);
			bv x = cvc5_term_manager.mkVar(cvc5_term_manager.mkBitVectorSort(bv_size), tau::get(v).to_str());
			vars.emplace(v, x);

			auto f = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			if (!f) return std::nullopt;
			auto res = std::optional<bv>(make_term_exists({x}, f.value()));
			vars.erase(v);
			return res;
		}
		case node::type::variable: {
			// check if the variable is alr
			if (auto it = vars.find(form | tt::ref); it != vars.end()) return it->second;
			if (auto it = free_vars.find(form | tt::ref); it != free_vars.end()) return it->second;
			auto vn = (form | tt::Tree).to_str();
			// create a new constant according to the type and added to the map
			size_t bv_size = get_bv_size<node>(type_tree);
			auto x = cvc5_term_manager.mkConst(cvc5_term_manager.mkBitVectorSort(bv_size), vn.c_str());
			free_vars.emplace(form | tt::ref, x);
			return std::optional<bv>(x);
		}
		case node::type::bf_eq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_term_equal(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_neq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_term_distinct(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_lteq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_term_less_equal(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_nlteq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_less_equal(l.value(), r.value()))) : std::nullopt;
		}
		case node::type::bf_gt: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_term_greater(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_ngt: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_greater(l.value(), r.value()))) : std::nullopt;
		}
		case node::type::bf_gteq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_term_greater_equal(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_ngteq: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_greater_equal(l.value(), r.value()))) : std::nullopt;
		}
		case node::type::bf_lt: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_term_less(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_nlt: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_less(l.value(), r.value()))) : std::nullopt;
		}
		case node::type::bf_neg: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			return (l) ? std::optional<bv>(make_bitvector_not(l.value())) : std::nullopt;
		}
		case node::type::bf_add: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_bitvector_add(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_sub: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_bitvector_sub(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_mul: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_bitvector_mul(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_div: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_bitvector_div(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_mod: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_bitvector_mod(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_and: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_bitvector_and(l.value(), r.value())) : std::nullopt;
		}
		/*case node::type::bv_nand: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_bitvector_nand(l.value(), r.value())) : std::nullopt;
		}*/
		case node::type::bf_or: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_bitvector_or(l.value(), r.value())) : std::nullopt;
		}
		/*case node::type::bv_nor: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_bitvector_nor(l.value(), r.value())) : std::nullopt;
		}*/
		case node::type::bf_xor: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_bitvector_xor(l.value(), r.value())) : std::nullopt;
		}
		/*case node::type::bv_xnor: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_bitvector_xnor(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bv_min: {
			auto l = bv_eval_node<node>(solver, form->child[0], vars, free_vars);
			auto r = bv_eval_node<node>(solver, form->child[1], vars, free_vars);
			return (l && r) ? min(l.value(), r.value()) : std::nullopt;
		}
		case node::type::bv_max: {
			auto l = bv_eval_node<node>(solver, form->child[0], vars, free_vars);
			auto r = bv_eval_node<node>(solver, form->child[1], vars, free_vars);
			return (l && r) ? max(l.value(), r.value()) : std::nullopt;
		}*/
		case node::type::bf_shl: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_bitvector_shr(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_shr: {
			auto l = bv_eval_node<node>(solver, form | tt::first, vars, free_vars, type_tree);
			auto r = bv_eval_node<node>(solver, form | tt::second, vars, free_vars, type_tree);
			return (l && r) ? std::optional<bv>(make_bitvector_shr(l.value(), r.value())) : std::nullopt;
		}
		case node::type::ba_constant: {
			auto cte = form | tt::ba_constant;
			DBG(assert(std::holds_alternative<bv>(cte));)
			return std::optional<bv>(std::get<bv>(cte));
		}
		default:
			return std::nullopt;
	}
}

template <NodeType node>
bool is_bv_formula_sat(tref form, tref type_tree) {
	using tt = tree<node>::traverser;

	subtree_map<node, bv> vars, free_vars;
	cvc5::Solver solver(cvc5_term_manager);
	config_cvc5_solver(solver);

	auto expr = bv_eval_node<node>(solver, tt(form), vars, free_vars, type_tree);
	// TODO (MEDIUM) handle this case at an upper level (maybe return an optional)
	if (!expr) {
		LOG_ERROR << "Failed to translate the formula to cvc5: " << LOG_FM(form);
		return false;
	}
	solver.assertFormula(expr.value());
	return solver.checkSat().isSat();
}

template <NodeType node>
bool is_bv_formula_unsat(tref form, tref type_tree) {
	return !is_bv_formula_sat<node>(form, type_tree);
}

template <NodeType node>
bool is_bv_formula_valid(tref form, tref type_tree) {
	using tau = tree<node>;
	return is_bv_formula_unsat<node>(tau::build_wff_neg(form), type_tree);
}

template <NodeType node>
std::optional<solution<node>> solve_bv(const tref form, tref type_tree) {
	using tau = tree<node>;
	using tt = tau::traverser;

	subtree_map<node, bv> vars, free_vars;
	cvc5::Solver solver(cvc5_term_manager);
	config_cvc5_solver(solver);

	auto expr = bv_eval_node<node>(solver, tt(form), vars, free_vars, type_tree);
	if (!expr) {
		LOG_ERROR << "Failed to translate the formula to cvc5: " << LOG_FM(form);
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
			bv cte = solver.getValue(bv_var);
			s.emplace(tau::get(tau::bf, tau_var),
				tau::get(tau::bf, tau::get_ba_constant(cte, type_tree)));
		}
		return s;
	} else
		LOG_DEBUG << "Bitvector system is unsat.";
	return {};
}

template<NodeType node>
std::optional<solution<node>> solve_bv(const trefs& lits, tref type_tree) {
	using tau = tree<node>;

	return solve_bv<node>(tau::build_wff_and(lits), type_tree);
}

template<typename ... BAs> requires BAsPack<BAs...>
std::optional<bv> bv_constant_from_parse_tree(tref parse_tree, tref type_tree) {
	using tt = bitvector_parser::tree::traverser;
	using bv_parser = bitvector_parser::nonterminal;
	if (!parse_tree) return std::nullopt;
	auto t = bitvector_parser::tree::traverser(parse_tree)
					| bitvector_parser::bitvector;

	auto type = t | tt::only_child | tt::nonterminal;
	size_t base;
	switch (type) {
		case bv_parser::decimal: { base = 10; break; }
		case bv_parser::binary: { base = 2; break; }
		case bv_parser::hexadecimal: { base = 16; break; }
		default: {
			DBG(assert(false);)
			return std::nullopt;
		}
	}
	DBG(assert(base > 0 );)
	size_t bv_size = get_bv_size<node<BAs...>>(type_tree);
	auto str = t | tt::terminals;
	return make_bitvector_cte(bv_size, str, base);
}

template<typename...BAs>
requires BAsPack<BAs...>
std::optional<typename node<BAs...>::constant_with_type> parse_bv(const std::string& src,
		tref type_tree) {

	auto result = bitvector_parser::instance().parse(src.c_str(), src.size());
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(bitvector_parser::error::info_lvl::INFO_BASIC);
		LOG_ERROR << "[bv] " << msg << "\n";
		return {}; // Syntax error
	}
	auto cte = bv_constant_from_parse_tree<BAs...>(result.get_shaped_tree2(), type_tree);
	if (!cte) {
		LOG_ERROR << "Failed to parse bitvector constant: " << src;
		return {};
	}
	return typename node<BAs...>::constant_with_type{ cte.value(), type_tree };
}

} // namespace idni::tau_lang