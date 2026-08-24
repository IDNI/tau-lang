// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/bv/bv_ba.h" // Only for IDE resolution, not really needed.
#include "boolean_algebras/bv/parser/bitvector_parser.generated.h"

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
			// Same rationale as the ba_constant case above: a non-bv
			// (e.g. sbf or tau) T/F node can reach here once every
			// variable has been substituted away, since
			// is_bv_solvable_formula only inspects variable nodes.
			// get_bv_size requires an explicit bitwidth; fail
			// gracefully instead of asserting/crashing.
			tref c = form | tt::ref;
			tref type_tree = tau::get(c).get_ba_type_tree();
			if (!is_bv_type_family<node>(tau::get(c).get_ba_type())
				|| !(tt(type_tree) | tau::subtype))
				return std::nullopt;
			auto bv_size = get_bv_size<node>(type_tree);
			return make_bitvector_top_elem(bv_size);
		}
		case tau::bf_f: {
			tref c = form | tt::ref;
			tref type_tree = tau::get(c).get_ba_type_tree();
			if (!is_bv_type_family<node>(tau::get(c).get_ba_type())
				|| !(tt(type_tree) | tau::subtype))
				return std::nullopt;
			auto bv_size = get_bv_size<node>(type_tree);
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
 * every variable must have an explicitly sized bitvector type, and at least
 * one bv-typed variable or ba_constant must actually be present. Mixed-type
 * formulas (e.g. with sbf or tau variables) cannot be translated to cvc5,
 * and a formula with no bv content at all (e.g. an already-resolved
 * T/F literal) is not this solver's to claim.
 *
 * Also rejects formulas carrying a non-bv-typed ba_constant (e.g. a `qlt`
 * constant like `{1/3}:qlt`): such a constant can appear in an otherwise
 * bv-only clause once its variable has already been substituted by a
 * concrete value (e.g. during interpretation), so checking only `variable`
 * nodes is not enough to catch the mixed-type case.
 *
 * A `variable` node is treated as an opaque leaf: its children are not
 * descended into. An io_var (`o1[t]`, `i1[t-3]`) is itself a `variable`
 * node whose children carry the time offset/shift bookkeeping, including
 * an inner, untyped `variable` node for the bare time symbol `t` -- that
 * bookkeeping is not a data variable and bv_eval_node never looks past the
 * outer variable's own type when translating it, so inspecting it here
 * would reject bv-only formulas over their own opaque type mismatch.
 *
 * @tparam node Node type
 * @param form The formula to check
 * @return true if all variables/constants are (explicitly sized) bitvectors
 * and at least one bv-typed variable/constant was seen
 */
template <NodeType node>
bool is_bv_solvable_formula(tref form) {
	using tau = tree<node>;
	using tt = tau::traverser;

	// Temporal operators have no cvc5 encoding; bail out before translating.
	if (tau::get(form).find_top(is_temporal_quantifier<node>))
		return false;

	bool solvable = true;
	bool has_bv_content = false;
	auto check = [&](tref n) {
		if (is<node>(n, tau::variable)) {
			size_t t = tau::get(n).get_ba_type();
			if (!is_bv_type_family<node>(t)) return solvable = false;
			// the solver requires an explicit bitwidth
			if (!(tt(tau::get(n).get_ba_type_tree()) | tau::subtype))
				return solvable = false;
			has_bv_content = true;
		} else if (is<node>(n, tau::ba_constant)) {
			size_t t = tau::get(n).get_ba_type();
			if (t != 0 && !is_bv_type_family<node>(t))
				return solvable = false;
			if (t != 0) has_bv_content = true;
		}
		return solvable;
	};
	// Do not descend into a variable node's own children: its offset/shift
	// bookkeeping (e.g. the untyped `t` inside an io_var) is not data.
	auto skip_variable_children = [](tref, tref parent = nullptr) {
		return !(parent && is<node>(parent, tau::variable));
	};
	auto up = [](tref) {};
	pre_order<node>(form).search_unique(check, skip_variable_children, up);
	return solvable && has_bv_content;
}

namespace detail {

/**
 * @brief Reusable cvc5::Solver for the bv satisfiability/solve path
 * (bv_formula_sat_status, solve_bv). Constructing a cvc5::Solver is
 * expensive -- finishInit sets up TheoryArith etc. from scratch every time
 * -- while resetAssertions() only clears the assertion stack and keeps the
 * once-configured options/logic (SMT-LIB reset-assertions semantics), so
 * one process-wide instance can serve every independent query. Solvers are
 * single-use per query here (no push/pop), matching how each call site
 * already treats its solver as scoped to one formula.
 *
 * Single-threaded only, like every other user of cvc5_term_manager in this
 * codebase; must not be shared across threads.
 */
class bv_solver_holder {
	std::optional<cvc5::Solver> solver;
public:
	/** @brief Return a solver ready for a new, independent query. */
	cvc5::Solver& get() {
		if (solver) {
			try { solver->resetAssertions(); return *solver; }
			catch (...) { solver.reset(); } // fall through and rebuild
		}
		solver.emplace(cvc5_term_manager);
		config_cvc5_solver(*solver);
		return *solver;
	}
	/**
	 * @brief Drop the solver so the next get() rebuilds it from scratch.
	 * Call after a query throws: cvc5's post-exception solver state is
	 * unspecified, so the shared instance must not be reused as-is.
	 */
	void invalidate() { solver.reset(); }
};

inline bv_solver_holder& shared_bv_solver() {
	static bv_solver_holder holder;
	return holder;
}

} // namespace detail

template <NodeType node>
std::optional<bv_sat_status> bv_formula_sat_status(tref form) {
	using tau = tree<node>;
	using tt = tau::traverser;

	subtree_map<node, bv> vars, free_vars;
	auto expr = bv_eval_node<node>(tt(form), vars, free_vars);
	if (!expr) {
		LOG_DEBUG << "Failed to translate the formula to cvc5: " << LOG_FM(form);
		DBG(LOG_TRACE << LOG_FM_TREE(form) << "\n";)
		return std::nullopt;
	}
	DBG( LOG_TRACE << "CVC5 translated formula: " << expr.value(); )
	auto& solver = detail::shared_bv_solver().get();
	try {
		solver.assertFormula(expr.value());
		auto result = solver.checkSat();
		if (result.isSat()) return bv_sat_status::sat;
		if (result.isUnknown()) {
			LOG_DEBUG << "cvc5 could not decide satisfiability (unknown) for: " << expr.value();
			return bv_sat_status::unknown;
		}
		return bv_sat_status::unsat;
	} catch (...) {
		detail::shared_bv_solver().invalidate();
		throw;
	}
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
	auto expr = bv_eval_node<node>(tt(form), vars, free_vars);
	if (!expr) {
		LOG_DEBUG << "Failed to translate the formula to cvc5: " << LOG_FM(form);
		DBG(LOG_TRACE << LOG_FM_TREE(form) << "\n";)
		return std::nullopt;
	}
	DBG( LOG_TRACE << "CVC5 translated formula: " << expr.value(); )

	auto& solver = detail::shared_bv_solver().get();
	try {
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
	} catch (...) {
		detail::shared_bv_solver().invalidate();
		throw;
	}
}

template<NodeType node>
std::optional<solution<node>> solve_bv(const trefs& lits) {
	using tau = tree<node>;

	return solve_bv<node>(tau::build_wff_and(lits));
}

} // namespace idni::tau_lang