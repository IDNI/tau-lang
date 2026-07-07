// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/bv_ba.h" // Only for IDE resolution, not really needed.
#include "../parser/bitvector_parser.generated.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba_solver"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template <NodeType node>
std::optional<bv> bv_eval_node(const typename tree<node>::traverser& form, subtree_map<node, bv>& vars,
			       subtree_map<node, bv>& free_vars) {
	using tau = tree<node>;
	using tt = tree<node>::traverser;

	auto nt = form | tt::nt;

	switch (nt) {
		case tau::wff_always:
		case tau::wff_sometimes: {
			return bv_eval_node<node>(form | tt::first, vars, free_vars);
		}
		// Hooks normalize these wrappers to their contained bitvector formulas.
		case tau::wff: case tau::bf:
		/*case tau::bv:*/ {
			return bv_eval_node<node>(form | tt::first, vars, free_vars);
		}
		case tau::wff_t: return make_bitvector_true();
		case tau::wff_f: return make_bitvector_false();
		case tau::wff_neg: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			return l ? std::optional<bv>(make_term_not(l.value())) : std::nullopt;
		}
		case tau::wff_and: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_and(l.value(), r.value())) : std::nullopt;
		}
		case tau::wff_or: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_or(l.value(), r.value())) : std::nullopt;
		}
		case tau::wff_all: {
			tref v = (form | tt::first | tt::ref);
			// If the bound "variable" is not actually a variable (e.g., a constant
			// due to variable capture in substitution), just evaluate the body.
			if (!is<node>(v, tau::variable))
				return bv_eval_node<node>(form | tt::second, vars, free_vars);
			size_t bv_size = get_bv_size<node>(tau::get(v).get_ba_type_tree());
			bv x = cvc5_term_manager.mkVar(cvc5_term_manager.mkBitVectorSort(bv_size), tau::get(v).to_str());
			// vars is now shared by reference across the whole recursion, so a
			// shadowed outer binding of the same tref (nested quantifiers
			// sharing the same variable tref due to caching) must be saved and
			// restored -- unlike the previous by-value vars, an unconditional
			// erase here would now also delete that outer binding for the
			// caller instead of just for this call's own local copy.
			auto prev = vars.find(v);
			std::optional<bv> outer = prev != vars.end() ? std::optional(prev->second) : std::nullopt;
			vars[v] = x;

			auto f = bv_eval_node<node>(form | tt::second, vars, free_vars);
			if (outer) vars[v] = *outer; else vars.erase(v);
			if (!f) return std::nullopt;
			return std::optional<bv>(make_term_forall({x}, f.value()));
		}
		case tau::wff_ex: {
			tref v = (form | tt::first | tt::ref);
			// If the bound "variable" is not actually a variable, just evaluate the body.
			if (!is<node>(v, tau::variable))
				return bv_eval_node<node>(form | tt::second, vars, free_vars);
			size_t bv_size = get_bv_size<node>(tau::get(v).get_ba_type_tree());
			bv x = cvc5_term_manager.mkVar(cvc5_term_manager.mkBitVectorSort(bv_size), tau::get(v).to_str());
			// See the wff_all case above for why the outer binding must be
			// saved and restored now that vars is passed by reference.
			auto prev = vars.find(v);
			std::optional<bv> outer = prev != vars.end() ? std::optional(prev->second) : std::nullopt;
			vars[v] = x;

			auto f = bv_eval_node<node>(form | tt::second, vars, free_vars);
			if (outer) vars[v] = *outer; else vars.erase(v);
			if (!f) return std::nullopt;
			return std::optional<bv>(make_term_exists({x}, f.value()));
		}
		case tau::variable: {
			// check if the variable is alr
			tref v = form | tt::ref;
			if (auto it = vars.find(v); it != vars.end()) return it->second;
			if (auto it = free_vars.find(form | tt::ref); it != free_vars.end()) return it->second;
			auto vn = (form | tt::Tree).to_str();
			// create a new constant according to the type and added to the map
			size_t bv_size = get_bv_size<node>(tau::get(v).get_ba_type_tree());
			auto x = cvc5_term_manager.mkConst(cvc5_term_manager.mkBitVectorSort(bv_size), vn.c_str());
			free_vars.emplace(form | tt::ref, x);
			return std::optional<bv>(x);
		}
		case tau::bf_eq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_equal(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_neq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_distinct(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_lteq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_less_equal(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_nlteq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_less_equal(l.value(), r.value()))) : std::nullopt;
		}
		case tau::bf_gt: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_greater(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_ngt: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_greater(l.value(), r.value()))) : std::nullopt;
		}
		case tau::bf_gteq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_greater_equal(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_ngteq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_greater_equal(l.value(), r.value()))) : std::nullopt;
		}
		case tau::bf_lt: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_less(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_nlt: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_less(l.value(), r.value()))) : std::nullopt;
		}
		case tau::bf_neg: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			return (l) ? std::optional<bv>(make_bitvector_not(l.value())) : std::nullopt;
		}
		case tau::bf_cast: {
			tref c = form | tt::ref;
			size_t target_size = get_bv_size<node>(tau::get(c).get_ba_type_tree());
			auto src = bv_eval_node<node>(form | tt::first, vars, free_vars);
			if (!src) return std::nullopt;
			size_t src_size = src.value().getSort().getBitVectorSize();
			if (target_size > src_size)
				return std::optional<bv>(make_bitvector_zero_extend(src.value(), target_size - src_size));
			if (target_size < src_size)
				return std::optional<bv>(make_bitvector_extract(src.value(), target_size - 1, 0));
			return src;
		}
		case tau::bf_add: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_add(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_sub: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_sub(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_mul: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_mul(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_div: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_div(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_mod: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_mod(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_and: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_and(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_nand: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_nand(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_or: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_or(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_nor: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_nor(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_xor: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_xor(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_xnor: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_xnor(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_shl: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_shl(l.value(), r.value())) : std::nullopt;
		}
		case tau::bf_shr: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_shr(l.value(), r.value())) : std::nullopt;
		}
		case tau::ba_constant: {
			// is_bv_solvable_formula only inspects variable nodes, so a
			// formula whose variables are all bitvectors but which also
			// contains a non-bv (e.g. sbf or tau) constant passes that gate;
			// std::get would then throw std::bad_variant_access in release
			// (the DBG-only assert doesn't guard it there). Fail gracefully
			// instead, same as any other untranslatable node.
			auto cte = form | tt::ba_constant;
			if (!std::holds_alternative<bv>(cte)) return std::nullopt;
			return std::optional<bv>(std::get<bv>(cte));
		}
		case tau::bf_t: {
			tref c = form | tt::ref;
			auto bv_size = get_bv_size<node>(tau::get(c).get_ba_type_tree());
			return make_bitvector_top_elem(bv_size);
		}
		case tau::bf_f: {
			tref c = form | tt::ref;
			auto bv_size = get_bv_size<node>(tau::get(c).get_ba_type_tree());
			return make_bitvector_bottom_elem(bv_size);
		}
		case tau::ctnvar: {
			return bv_eval_node<node>(form | tt::first, vars, free_vars);
		}
		default:
			return std::nullopt;
	}
}

template<NodeType node>
std::optional<bv> bv_eval_node(tref form, subtree_map<node, bv>& vars,
	subtree_map<node, bv>& free_vars) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto t = tt(form);
	return bv_eval_node(t, vars, free_vars);
}


/**
 * @brief Checks that the formula can be decided by the bitvector solver:
 * every variable must have an explicitly sized bitvector type. Mixed-type
 * formulas (e.g. with sbf or tau variables) cannot be translated to cvc5.
 *
 * @tparam node Node type
 * @param form The formula to check
 * @return true if all variables are explicitly sized bitvectors
 */
template <NodeType node>
bool is_bv_solvable_formula(tref form) {
	using tau = tree<node>;
	using tt = tau::traverser;

	bool solvable = true;
	auto check = [&](tref n) {
		if (is<node>(n, tau::variable)) {
			size_t t = tau::get(n).get_ba_type();
			if (!is_bv_type_family<node>(t)) return solvable = false;
			// the solver requires an explicit bitwidth
			if (!(tt(tau::get(n).get_ba_type_tree()) | tau::subtype))
				return solvable = false;
		}
		return solvable;
	};
	pre_order<node>(form).search_unique(check);
	return solvable;
}

template <NodeType node>
std::optional<bv_sat_status> bv_formula_sat_status(tref form) {
	using tau = tree<node>;
	using tt = tau::traverser;

	subtree_map<node, bv> vars, free_vars;
	cvc5::Solver solver(cvc5_term_manager);
	config_cvc5_solver(solver);

	auto expr = bv_eval_node<node>(tt(form), vars, free_vars);
	if (!expr) {
		LOG_ERROR << "Failed to translate the formula to cvc5: " << LOG_FM(form);
		DBG(LOG_TRACE << LOG_FM_TREE(form) << "\n";)
		return std::nullopt;
	}
	DBG( LOG_TRACE << "CVC5 translated formula: " << expr.value(); )
	solver.assertFormula(expr.value());
	auto result = solver.checkSat();
	if (result.isSat()) return bv_sat_status::sat;
	if (result.isUnknown()) {
		LOG_DEBUG << "cvc5 could not decide satisfiability (unknown) for: " << expr.value();
		return bv_sat_status::unknown;
	}
	return bv_sat_status::unsat;
}

template <NodeType node>
bool is_bv_formula_sat(tref form) {
	// Collapses unknown and translation failure into false, same as unsat.
	// Callers that would otherwise assert the formula is definitely false
	// on a false return here must use bv_formula_sat_status instead and
	// treat unknown/nullopt as "cannot decide".
	return bv_formula_sat_status<node>(form) == bv_sat_status::sat;
}

template <NodeType node>
bool is_bv_formula_unsat(tref form) {
	return !is_bv_formula_sat<node>(form);
}

template <NodeType node>
bool is_bv_formula_valid(tref form) {
	using tau = tree<node>;
	return is_bv_formula_unsat<node>(tau::build_wff_neg(form));
}

template <NodeType node>
std::optional<solution<node>> solve_bv(const tref form) {
	using tau = tree<node>;
	using tt = tau::traverser;

	subtree_map<node, bv> vars, free_vars;
	cvc5::Solver solver(cvc5_term_manager);
	config_cvc5_solver(solver);

	auto expr = bv_eval_node<node>(tt(form), vars, free_vars);
	if (!expr) {
		LOG_ERROR << "Failed to translate the formula to cvc5: " << LOG_FM(form);
		DBG(LOG_TRACE << LOG_FM_TREE(form) << "\n";)
		return std::nullopt;
	}
	DBG( LOG_TRACE << "CVC5 translated formula: " << expr.value(); )

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
				tau::get(tau::bf, tau::get_ba_constant(cte,
					bv_type<node>(cte.getSort().getBitVectorSize()))));
		}
		return s;
	}
	// Callers of this overload (solve_bv(trefs) -> solver.tmpl.h) already
	// treat "no solution" uniformly as "skip this clause" regardless of
	// the reason, which is sound for unknown as well as unsat (neither
	// asserts a definite truth value), so nullopt is returned for both;
	// only the diagnostic differs.
	if (result.isUnknown())
		LOG_DEBUG << "cvc5 could not decide satisfiability (unknown) for: " << expr.value();
	else
		LOG_DEBUG << "Bitvector system is unsat.";
	return {};
}

template<NodeType node>
std::optional<solution<node>> solve_bv(const trefs& lits) {
	using tau = tree<node>;

	return solve_bv<node>(tau::build_wff_and(lits));
}

} // namespace idni::tau_lang