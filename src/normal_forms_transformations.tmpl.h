// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "normal_forms_transformations.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "normal_forms"

/** @internal @copydoc unequal_to_not_equal */
template <NodeType node>
tref unequal_to_not_equal(tref fm) {
	using tau = tree<node>;
	LOG_TRACE << "unequal_to_not_equal: " << LOG_FM(fm);
	auto neq_to_not_eq = [](tref n) {
		//$X != 0 ::= !($X = 0)
		if (is<node>(n, tau::bf_neq)) {
			return tau::trim(tau::build_wff_neg(
				tau::build_bf_eq(
					tau::get(n).first(),
					tau::get(n).second())));
		}
		return n;
	};
	tref result = pre_order<node>(fm)
				.apply_unique(neq_to_not_eq, visit_wff<node>);
	LOG_TRACE << "unequal_to_not_equal result: " << LOG_FM(result);
	return result;
}


// Convert X =(!=) Y to X + Y =(!=) 0
/** @internal @copydoc norm_equation */
template<NodeType node>
tref norm_equation(tref eq) {
	using tau = tree<node>;
	tau e = tau::get(eq);
	if (e.child_is(tau::bf_eq)) {
		return tau::build_bf_eq_0(tau::build_bf_xor(e[0].first(), e[0].second()));
	} else if (e.child_is(tau::bf_neq)) {
		return tau::build_bf_neq_0(tau::build_bf_xor(e[0].first(), e[0].second()));
	} else return eq;
}


// Convert all occurrences of X =(!=) Y to X + Y =(!=) 0 in fm
/** @internal @copydoc norm_all_equations */
template <NodeType node>
tref norm_all_equations (tref fm) {
	return pre_order<node>(fm).apply_unique(norm_equation<node>,
						visit_wff<node>);
}


/** @internal @copydoc apply_all_xor_def */
template<NodeType node>
tref apply_all_xor_def(tref fm) {
	return pre_order<node>(fm).apply_unique(apply_xor_def<node>);
}


/** @internal @copydoc push_negation_in */
template <NodeType node, bool is_wff>
tref push_negation_in(tref fm) {
	auto pn = [](tref n) {
		return push_negation_one_in<node, is_wff>(n);
	};
	if constexpr (is_wff) return pre_order<node>(fm)
		.template apply_unique<MemorySlotPre::push_negation_in_m>(
							pn, visit_wff<node>);
	else return pre_order<node>(fm)
		.template apply_unique<MemorySlotPre::push_negation_in_m>(pn);
}


/**
 * @brief Shift non-initial IO variable time indices in `fm` by `shift`.
 *
 * For each IO variable in `io_vars` that is **not** an initial condition,
 * increases its lookback offset by `shift`. Returns `fm` unchanged when
 * `shift <= 0`.
 * @tparam node Tree node type.
 * @param fm Formula whose IO variables are to be shifted.
 * @param io_vars Collection of IO variable nodes to consider.
 * @param shift Time offset to add to non-initial IO variable shifts.
 * @return Formula with adjusted IO variable time indices.
 * @internal
 */
template <NodeType node>
tref shift_io_vars_in_fm(tref fm, const auto& io_vars, const int_t shift) {
	using tau = tree<node>;
	if (shift <= 0) return fm;
	subtree_map<node, tref> changes;
	for (tref io_var : io_vars) {
		// Skip initial conditions
		if (is_io_initial<node>(io_var)) continue;
		int_t var_shift = get_io_var_shift<node>(io_var);
		size_t type = tau::get(io_var).get_ba_type();
		changes[io_var] = tau::get(io_var).is_input_variable()
			? tau::trim(build_in_var_at_t_minus<node>(get_var_name_node<node>(
				io_var), var_shift + shift, type))
			: tau::trim(build_out_var_at_t_minus<node>(get_var_name_node<node>(
				io_var), var_shift + shift, type));
	}
	return rewriter::replace<node>(fm, changes);
}


/**
 * @brief Shift initial-condition IO variables in `fm` by `shift`.
 *
 * Adds `shift` to the absolute time point of every `io_var` that *is* an
 * initial condition. Returns `F` if any resulting time point would be negative.
 * Does nothing if `shift <= 0`.
 * @tparam node Tree node type.
 * @param fm Formula whose initial IO variables are to be shifted.
 * @param io_vars Collection of IO variable nodes to consider.
 * @param shift Time offset to add.
 * @return Formula with adjusted initial IO variable time points, or `F`.
 * @internal
 */
template <NodeType node>
tref shift_const_io_vars_in_fm(tref fm, const auto& io_vars, const int_t shift){
	using tau = tree<node>;
	if (shift <= 0) return fm;
	subtree_map<node, tref> changes;
	for (tref io_var : io_vars) {
		if (!is_io_initial<node>(io_var)) continue;
		int_t tp = get_io_time_point<node>(io_var);
		// Make sure that the resulting time point is positive
		if (tp + shift < 0) return tau::_F();
		size_t type = tau::get(io_var).get_ba_type();
		changes.emplace(io_var, tau::get(io_var).is_input_variable()
			? tau::trim(build_in_var_at_n<node>(
				get_var_name_node<node>(io_var), tp + shift, type))
			: tau::trim(build_out_var_at_n<node>(
				get_var_name_node<node>(io_var), tp + shift, type)));
	}
	return rewriter::replace<node>(fm, changes);
}


/**
 * @brief Conjoin two `always`-quantified formulas, aligning their lookbacks.
 *
 * Strips the `always` wrapper from `fm1_aw` and `fm2_aw`, determines their
 * maximum shift, and adjusts the formula with the shorter lookback by adding
 * the difference, then returns their conjunction.
 * @tparam node Tree node type.
 * @param fm1_aw First (possibly `always`-wrapped) formula.
 * @param fm2_aw Second (possibly `always`-wrapped) formula.
 * @return Conjunction of the two formulas with aligned lookbacks.
 * @internal
 */
template <NodeType node>
tref always_conjunction(tref fm1_aw, tref fm2_aw) {
	using tau = tree<node>;
	// Trim the always node if present
	auto fm1 = is_child<node>(fm1_aw, tau::wff_always)
				? tau::trim2(fm1_aw) : fm1_aw;
	auto fm2 = is_child<node>(fm2_aw, tau::wff_always)
				? tau::trim2(fm2_aw) : fm2_aw;
	auto io_vars1 = tau::get(fm1)
		.select_top(is_child<node, tau::io_var>);
	auto io_vars2 = tau::get(fm2)
		.select_top(is_child<node, tau::io_var>);
	// Get lookbacks
	int_t lb1 = get_max_shift<node>(io_vars1);
	int_t lb2 = get_max_shift<node>(io_vars2);
	if (lb1 < lb2) {
		// adjust fm1 by lb2 - lb1
		return tau::build_wff_and(
			shift_io_vars_in_fm<node>(fm1, io_vars1, lb2 - lb1),
			fm2);
	} else if (lb2 < lb1) {
		// adjust fm2 by lb1 - lb2
		return tau::build_wff_and(fm1,
			shift_io_vars_in_fm<node>(fm2, io_vars2, lb1 - lb2));
	} else {
		// no adjustment needed
		return tau::build_wff_and(fm1, fm2);
	}
}


/**
 * @brief Collect all positive equalities in `n` of the given BA type and merge them.
 *
 * Finds all `bf_eq` atoms whose BA type matches `type_id`, normalizes each
 * to `f = 0` form, and combines their left-hand sides into a single disjunction
 * `f1 | f2 | ...`. Returns the resulting BF, or `nullptr` if no match is found.
 * @tparam node Tree node type.
 * @param n Formula to search for equalities.
 * @param type_id BA type identifier selecting which equalities to merge.
 * @return Disjunction of all matching left-hand sides, or `nullptr`.
 * @internal
 */
template <NodeType node>
tref squeeze_positives(tref n, size_t type_id) {
	using tau = tree<node>;
	using tt = tau::traverser;
	LOG_TRACE << "squeeze_positives: " << LOG_FM(n);
	auto match = [&type_id](tref n) {
		return is<node, tau::bf_eq>(n) &&
			find_ba_type<node>(n) == type_id;
	};
	if (trefs eqs = tau::get(n).select_top(match);
		eqs.size() > 0)
	{
		for (tref& eq : eqs) {
			eq = norm_trimmed_equation<node>(eq);
		}
		eqs = tt(eqs) | tt::children | tt::refs;
		tref res = tau::build_bf_or(eqs, find_ba_type<node>(eqs[0]));
		LOG_TRACE << "squeeze_positives result: " << LOG_FM(res);
		return res;
	}
	LOG_TRACE << "(I) squeeze_positives result: none";
	return nullptr;
}


/**
 * @brief Replace all free variables in `fm` with `val`.
 *
 * Collects every free variable in `fm` and substitutes each with `val`.
 * Returns `fm` unchanged if it has no free variables.
 * @tparam node Tree node type.
 * @param fm Formula whose free variables are to be replaced.
 * @param val Replacement value; must not be a bare `bf` node.
 * @return Formula with all free variables replaced by `val`.
 * @internal
 */
template <NodeType node>
tref replace_free_vars_by(tref fm, tref val) {
	DBG(using tau = tree<node>;)
	DBG(assert(!is<node>(val, tau::bf));)
	const trefs& free_vars = get_free_vars<node>(fm);
	if (free_vars.size()) {
		subtree_map<node, tref> free_var_assgm;
		for (tref free_var : free_vars)
			free_var_assgm.emplace(free_var, val);
		return rewriter::replace<node>(fm, free_var_assgm);
	} else return fm;
}


/**
 * @brief Function to apply syntactical simplifications to formula in almost
 * linear time in the formula size and the number of paths found in terms
 * @tparam node tree node type
 * @param formula The formula to simplify
 * @return The simplified formula
 * @internal
 */
template<NodeType node>
tref syntactic_formula_simplification(tref formula) {
	formula = simplify_using_equality<node>::on(formula);
	return syntactic_path_simplification<node>::on(formula);
}


// TODO: How to adjust for bitvector that are boolean?
/**
 * @brief Eliminate the existential quantifier scoping a clause.
 * @tparam node Tree node type
 * @param ex_clause Existentially quantified clause
 * @param quant_eliminated Indicates whether the quantifier was successfully removed
 * @return The resulting clause after removing the existential quantifier
 * @internal
 */
template <NodeType node>
tref treat_ex_quantified_clause(tref ex_clause, bool& quant_eliminated) {
	using tau = tree<node>;
	// Following Corollary 2.3 from Taba book from Ohad
	tref var = tau::trim2(ex_clause);
	tref formula = tau::get(ex_clause)[0].second();

	if (tau::get(formula).equals_T() || tau::get(formula).equals_F())
		return formula;
	tref new_fm = tau::_T();
	bool is_quant_removable_in_clause = true;
	trefs conjs = get_cnf_wff_clauses<node>(formula);
	// A conjunct containing the quantified variable that still holds a
	// binder or an unresolved reference anywhere blocks elimination:
	// equations under a surviving inner quantifier are not top-level
	// conjuncts of this clause, and the eliminator below would squeeze
	// them as if they were, silently dropping the inner binder and
	// leaking its variable free
	auto blocks_elimination = [](tref m) {
		return is_quantifier<node>(m) || is<node>(m, tau::wff_ref);
	};
	for (tref& conj : conjs) {
		if (!contains<node>(conj, var)) {
			new_fm = tau::build_wff_and(new_fm, conj);
			conj = tau::_T();
			continue;
		}
		if (tau::get(conj).find_top(blocks_elimination)) {
			// The quantified variable is entangled with a kept
			// quantifier or reference, so we cannot resolve the
			// quantifier in this clause
			is_quant_removable_in_clause = false;
		}
	}
	tref scoped_fm = tau::build_wff_and(conjs);
	if (!is_quant_removable_in_clause) {
		// Since we cannot remove the quantifier in this
		// clause it needs to be maintained
		quant_eliminated = false;
		return tau::build_wff_and(
			tau::build_wff_ex(var, scoped_fm, false), new_fm);
	}

	DBG(LOG_TRACE << "treat_ex_quantified_clause/scoped_fm: " << LOG_FM(scoped_fm) << "\n";)
	if (auto eliminated = ex_subs_based_elimination<node>(var, scoped_fm); eliminated != scoped_fm) {
		DBG(LOG_TRACE << "treat_ex_quantified_clause/eliminated: " << LOG_FM(eliminated) << "\n";)
		return tau::build_wff_and(eliminated, new_fm);
	}

	// Check that quantified variable appears
	if (tau::get(scoped_fm).equals_T()) return new_fm;
	// An existential over F is F, independently of the variable
	if (tau::get(scoped_fm).equals_F()) return tau::_F();

	// Check if quantified variable is bitvector
	if (is_bv_type_family<node>(tau::get(var).get_ba_type())) {
		if (const trefs& free_vars = get_free_vars<node>(scoped_fm);
			(free_vars.empty() || (free_vars.size() == 1 &&
			tau::get(free_vars[0]) == tau::get(var)))
			&& is_bv_solvable_formula<node>(scoped_fm)) {
				// By assumption quantifier is pushed in all the way
				// Closed bv formula, simplify to T/F
				if (is_bv_formula_sat<node>(tau::build_wff_ex(var, scoped_fm, false)))
					return new_fm;
				else return tau::_F();
			} else {
				// Non-closed BV quantifier: try predicate blasting to
				// convert the BV existential to Boolean bit quantifiers
				// that the atomless-BA path can then eliminate.
				if (bv_blasting) {
					tref ex_fm = tau::build_wff_ex(var, scoped_fm, false);
					if (auto blasted = bv_predicate_blasting<node>(ex_fm);
							blasted && blasted != ex_fm) {
						tref cont = anti_prenex_block<node>(blasted);
						return tau::build_wff_and(cont, new_fm);
					}
				}
				// Quantifier is not resolvable
				quant_eliminated = false;
				return tau::build_wff_and(
					tau::build_wff_ex(var, scoped_fm, false), new_fm);
			}
	}
	// Continue with quantifier elimination for atomless BA
	size_t type_v = find_ba_type<node>(var);
	tref f = squeeze_positives<node>(scoped_fm, type_v);
	tref f_0 = f ? rewriter::replace<node>(f, var, tau::_0_trimmed(type_v)) : tau::_0(type_v);
	// std::cout << "f_0: " << tau::get(f_0) << "\n";
	tref f_1 = f ? rewriter::replace<node>(f, var, tau::_1_trimmed(type_v)) : tau::_0(type_v);
	// std::cout << "f_1: " << tau::get(f_1) << "\n";
	// TODO: instead of != use !(=)
	trefs neqs = tau::get(scoped_fm).select_top(is<node, tau::bf_neq>);
	if (neqs.size()) {
		tref nneqs = tau::_T();
		for (tref neq : neqs) {
			// Convert to != 0
			neq = norm_trimmed_equation<node>(neq);
			// Get term
			tref g = tau::trim(neq);
			tref g_0 = rewriter::replace<node>(g, var,
						tau::_0_trimmed(find_ba_type<node>(var)));
			// std::cout << "g_0: " << tau::get(g_0) << "\n";
			tref g_1 = rewriter::replace<node>(g, var,
						tau::_1_trimmed(find_ba_type<node>(var)));
			// std::cout << "g_1: " << tau::get(g_1) << "\n";
			// If both are 1 then inequality is implied by f_0f_1 = 0
			if (tau::get(g_0).equals_1() && tau::get(g_1).equals_1())
				continue;
			// If f_0 is equal to f_1 we can use assumption f_0 = 0 and f_1 = 0
			if (tau::get(f_0) == tau::get(f_1)) {
				nneqs = tau::build_wff_and(nneqs,
					tau::build_bf_neq_0(
						tau::build_bf_or(g_0,
								 g_1)));
			} else if (tau::get(g_0) == tau::get(g_1)) {
				nneqs = tau::build_wff_and(nneqs,
					tau::build_bf_neq_0(g_0));
			} else nneqs = tau::build_wff_and(
				nneqs,
				tau::build_bf_neq_0(
					tau::build_bf_or(
						tau::build_bf_and(
						tau::build_bf_neg(f_1),
						g_1),
						tau::build_bf_and(
						tau::build_bf_neg(f_0),
						g_0)))
					);
		}
		new_fm = tau::build_wff_and(new_fm, tau::build_wff_and(
			tau::build_bf_eq_0(tau::build_bf_and(f_0, f_1)),
			nneqs));
	} else if (f) {
		new_fm = tau::build_wff_and(new_fm,
			tau::build_bf_eq_0(
			tau::build_bf_and(f_0, f_1)));
	}
	return term_boole_normal_form<node>(new_fm);
}


/** @internal @copydoc resolve_quantifiers */
template<NodeType node>
tref resolve_quantifiers(tref formula) {
using tau = tree<node>;
	subtree_set<node> excluded;
	auto down_resolver = [&](tref n) {
		if (is_child_quantifier<node>(n)) {
			// Check if the formula is closed and proceed to eliminate
			// the quantifier
			tref var = tau::trim2(n);
			if (is_bv_type_family<node>(tau::get(var).get_ba_type())) {
				// A closed, purely bitvector formula is decided
				// directly by the solver. This is checked before
				// blasting: the solver handles the bitvector
				// arithmetic natively, while deciding the blasted
				// form (with its many auxiliary quantifiers) is
				// much harder for it. Blasting does not close a
				// formula, so the check would not succeed later.
				if (get_free_vars<node>(n).empty()
					&& is_bv_solvable_formula<node>(n))
					return is_bv_formula_sat<node>(n)
						? tau::_T() : tau::_F();
				if (bv_blasting)
					if (auto blasted = bv_predicate_blasting<node>(n);
						blasted && blasted != n)
						return blasted;
				excluded.insert(n);
			}
		}
		return n;
	};
	auto visit = [&](tref n) {
		if (excluded.contains(n)) return false;
		return visit_wff<node>(n);
	};
	return pre_order<node>(formula).apply_unique(down_resolver, visit);
}

