// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "normal_forms_transformations.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "normal_forms"

/** @internal @copydoc unequal_to_not_equal @endinternal */
template <NodeType node>
tref unequal_to_not_equal(tref fm) {
	using tau = tree<node>;
	DBG(LOG_TRACE << "unequal_to_not_equal/fm: " << LOG_FM(fm);)
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
				.apply_unique(neq_to_not_eq, while_is_formula<node>);
	DBG(LOG_TRACE << "unequal_to_not_equal/result: " << LOG_FM(result);)
	return result;
}

/** @internal @copydoc order_atoms_to_literals @endinternal */
template <NodeType node>
tref order_atoms_to_literals(tref fm) {
	using tau = tree<node>;
	DBG(LOG_TRACE << "order_atoms_to_literals/fm: " << LOG_FM(fm);)
	auto to_literal = [](tref n) {
		if (!tau::get(n).is(tau::wff)) return n;
		const tau& c = tau::get(n)[0];
		switch (c.value.nt) {
		case tau::bf_nlt: return tau::build_wff_neg(
			tau::build_bf_lt(c.first(), c.second()));
		case tau::bf_nlteq: return tau::build_wff_neg(
			tau::build_bf_lteq(c.first(), c.second()));
		case tau::bf_gt: return tau::build_bf_lt(c.second(), c.first());
		case tau::bf_gteq: return tau::build_bf_lteq(c.second(), c.first());
		case tau::bf_ngt: return tau::build_wff_neg(
			tau::build_bf_lt(c.second(), c.first()));
		case tau::bf_ngteq: return tau::build_wff_neg(
			tau::build_bf_lteq(c.second(), c.first()));
		default: return n;
		}
	};
	tref result = pre_order<node>(fm)
				.apply_unique(to_literal, while_is_formula<node>);
	DBG(LOG_TRACE << "order_atoms_to_literals/result: " << LOG_FM(result);)
	return result;
}

// Convert X =(!=) Y to X + Y =(!=) 0
/** @internal @copydoc norm_equation @endinternal */
template<NodeType node>
tref norm_equation(tref eq) {
	using tau = tree<node>;
	tau e = tau::get(eq);
	// TODO (MEDIUM) check if the equation already equals/not equals 0, in which
	// case we don't need to transform it
	if (e.child_is(tau::bf_eq)) {
		return tau::build_bf_eq_0(tau::build_bf_xor(e[0].first(), e[0].second()));
	} else if (e.child_is(tau::bf_neq)) {
		return tau::build_bf_neq_0(tau::build_bf_xor(e[0].first(), e[0].second()));
	} else return eq;
}


// Convert all occurrences of X =(!=) Y to X + Y =(!=) 0 in fm
/** @internal @copydoc norm_all_equations @endinternal */
template <NodeType node>
tref norm_all_equations (tref fm) {
	return pre_order<node>(fm).apply_unique(norm_equation<node>,
						while_is_formula<node>);
}


/**
 * @internal
 * @brief Normalizes a trimmed (unwrapped) equation or disequality to the form `(X XOR Y) (!)= 0`.
 *
 * Unlike `norm_equation`, the input is not wrapped in `wff`.
 * @tparam node Tree node type.
 * @param eq Trimmed equation node (`bf_eq` or `bf_neq`).
 * @return Normalized equation with XOR of operands compared to zero.
 * @endinternal
 */
// Convert X =(!=) Y to X + Y =(!=) 0
template<NodeType node>
tref norm_trimmed_equation(tref eq) {
	using tau = tree<node>;
	tau e = tau::get(eq);
	if (e.is(tau::bf_eq)) {
		return tau::trim(tau::build_bf_eq_0(tau::build_bf_xor(e.first(), e.second())));
	} else if (e.is(tau::bf_neq)) {
		return tau::trim(tau::build_bf_neq_0(tau::build_bf_xor(e.first(), e.second())));
	} else return eq;
}

/**
 * @internal
 * @brief Pushes a single negation one level inward (De Morgan / dualisation).
 *
 * Handles both wff (`wff_neg`) and bf (`bf_neg`) cases controlled by the `is_wff` template parameter.
 * @tparam node Tree node type.
 * @tparam is_wff `true` to handle `wff_neg` (default), `false` to handle `bf_neg`.
 * @param fm Formula node whose outermost negation is to be pushed inward.
 * @return Formula with the negation pushed one level deeper, or `fm` unchanged if not applicable.
 * @endinternal
 */
// Can be used for Tau formula and Boolean function
template <NodeType node, bool is_wff = true>
tref push_negation_one_in(tref fm) {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	// Tau formula rules
	if constexpr (is_wff) if (t.child_is(tau::wff_neg)) {
		const tau& ct = t[0][0];
		if (!ct.has_child()) return fm;
		switch (ct[0].value.nt) {
			// !!A ::= A. Without this case to_nnf cannot remove a
			// double negation and depends on the construction hooks
			// doing it -- which squeeze_absorb switches off.
			case tau::wff_neg: return ct[0].first();
			case tau::wff_and: return tau::build_wff_or(
						tau::build_wff_neg(ct[0].first()),
						tau::build_wff_neg(ct[0].second()));
			case tau::wff_or: return tau::build_wff_and(
						tau::build_wff_neg(ct[0].first()),
						tau::build_wff_neg(ct[0].second()));
			case tau::bf_eq: return tau::build_bf_neq(ct[0].first(), ct[0].second());
			case tau::bf_neq: return tau::build_bf_eq(ct[0].first(), ct[0].second());
			case tau::wff_ex: return tau::build_wff_all(ct[0].first(),
						tau::build_wff_neg(ct[0].second()), false);
			case tau::wff_all: return tau::build_wff_ex(ct[0].first(),
						tau::build_wff_neg(ct[0].second()), false);
			case tau::wff_always: return tau::build_wff_sometimes(
						tau::build_wff_neg(ct[0].first()));
			case tau::wff_sometimes: return tau::build_wff_always(
						tau::build_wff_neg(ct[0].first()));
			case tau::bf_lt: return tau::build_bf_nlt(ct[0].first(), ct[0].second());
			case tau::bf_nlt: return tau::build_bf_lt(ct[0].first(), ct[0].second());
			case tau::bf_lteq: return tau::build_bf_nlteq(ct[0].first(), ct[0].second());
			case tau::bf_nlteq: return tau::build_bf_lteq(ct[0].first(), ct[0].second());
			case tau::bf_gt: return tau::build_bf_ngt(ct[0].first(), ct[0].second());
			case tau::bf_ngt: return tau::build_bf_gt(ct[0].first(), ct[0].second());
			case tau::bf_gteq: return tau::build_bf_ngteq(ct[0].first(), ct[0].second());
			case tau::bf_ngteq: return tau::build_bf_gteq(ct[0].first(), ct[0].second());
			// Negated sugar connectives, dualised with the same builders
			// the construction hooks use. See the bare-sugar block below
			// for why these can reach to_nnf at all.
			case tau::wff_imply: return tau::build_wff_and(
						ct[0].first(),
						tau::build_wff_neg(ct[0].second()));
			case tau::wff_rimply: return tau::build_wff_and(
						ct[0].second(),
						tau::build_wff_neg(ct[0].first()));
			case tau::wff_equiv: return tau::build_wff_xor(
						ct[0].first(), ct[0].second());
			case tau::wff_xor: return tau::build_wff_equiv(
						ct[0].first(), ct[0].second());
			case tau::wff_conditional: return tau::build_wff_conditional(
						ct[0].first(),
						tau::build_wff_neg(ct[0].child(1)),
						tau::build_wff_neg(ct[0].child(2)));
			default: return fm;
		}
	}
	// The sugar connectives the construction hooks always rewrite away
	// (`->`, `<-`, `<->`, `^^`, `? :`) can still reach here: a tree built
	// with hooks off keeps them, and a Tau-BA constant is parsed exactly
	// that way -- `tau_spec` sets `reget_with_hooks = false` and, unlike
	// every other parsed formula, a constant never goes through
	// `api::simplify` (whose `reget` is what desugars them everywhere
	// else). Negation normal form means `&&`, `||` and negated atoms and
	// nothing else, and every consumer of it reads the tree that way:
	// `simplify_using_equality` walked into an implication and registered
	// its antecedent as an *asserted* equality of the enclosing conjunctive
	// scope, rewriting and dropping the surrounding conjuncts and returning
	// a formula that was neither implied by nor implying the input
	// (issue #69). Rewrite them here with the hooks' own builders so the
	// contract to_nnf's callers rely on actually holds.
	if constexpr (is_wff) if (t.has_child()) switch (t[0].value.nt) {
		case tau::wff_imply: return tau::build_wff_imply(
					t[0].first(), t[0].second());
		case tau::wff_rimply: return tau::build_wff_rimply(
					t[0].first(), t[0].second());
		case tau::wff_equiv: return tau::build_wff_equiv(
					t[0].first(), t[0].second());
		case tau::wff_xor: return tau::build_wff_xor(
					t[0].first(), t[0].second());
		case tau::wff_conditional: return tau::build_wff_conditional(
					t[0].first(), t[0].child(1), t[0].child(2));
		default: break;
	}
	// Boolean function rules
	if constexpr (!is_wff) if (t.child_is(tau::bf_neg)) {
		const tau& ct = t[0][0];
		if (!ct.has_child()) return fm;
		switch (ct[0].value.nt) {
			// A'' ::= A, the bf dual of the wff_neg case above.
			case tau::bf_neg: return ct[0].first();
			case tau::bf_and: return tau::build_bf_or(
				tau::build_bf_neg(ct[0].first()),
				tau::build_bf_neg(ct[0].second()));
			case tau::bf_or: return tau::build_bf_and(
				tau::build_bf_neg(ct[0].first()),
				tau::build_bf_neg(ct[0].second()));
			case tau::bf_xor: {
				// TODO: strategy for negating first or second argument
				return tau::build_bf_xor(tau::build_bf_neg(ct[0].first()), ct[0].second());
			}
			default: return fm;
		}
	}
	return fm;
}

// Can be used for Tau formula and Boolean function
// -----------------------------------------------------------------------------

/**
 * @internal
 * @brief Expands a single `bf_xor` node: `A XOR B` → `(A & !B) | (!A & B)`.
 * @tparam node Tree node type.
 * @param fm Formula node to expand; must be wrapped in `bf` with a `bf_xor` child.
 * @return Expanded formula, or `fm` unchanged if no `bf_xor` child is present.
 * @endinternal
 */
template<NodeType node>
tref apply_xor_def(tref fm) {
	using tau = tree<node>;
	tau t = tau::get(fm);
	if (t.child_is(tau::bf_xor)) {
		return tau::build_bf_or(
			tau::build_bf_and(t[0].first(),
					tau::build_bf_neg(t[0].second())),
			tau::build_bf_and(tau::build_bf_neg(t[0].first()),
					t[0].second()));
	}
	return fm;
}


/** @internal @copydoc apply_all_xor_def @endinternal */
template<NodeType node>
tref apply_all_xor_def(tref fm) {
	return pre_order<node>(fm).apply_unique(apply_xor_def<node>);
}


/** @internal @copydoc push_negation_in @endinternal */
template <NodeType node, bool is_wff>
tref push_negation_in(tref fm) {
	auto pn = [](tref n) {
		return push_negation_one_in<node, is_wff>(n);
	};
	if constexpr (is_wff) return pre_order<node>(fm)
		.template apply_unique<MemorySlotPre::push_negation_in_m>(
							pn, while_is_formula<node>);
	else return pre_order<node>(fm)
		.template apply_unique<MemorySlotPre::push_negation_in_m>(pn);
}


/**
 * @internal
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
 * @endinternal
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
 * @internal
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
 * @endinternal
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
 * @internal
 * @brief Conjoin two `always`-quantified formulas, aligning their lookbacks.
 *
 * Strips the `always` wrapper from `fm1_aw` and `fm2_aw`, determines their
 * maximum shift, and adjusts the formula with the shorter lookback by adding
 * the difference, then returns their conjunction.
 * @tparam node Tree node type.
 * @param fm1_aw First (possibly `always`-wrapped) formula.
 * @param fm2_aw Second (possibly `always`-wrapped) formula.
 * @return Conjunction of the two formulas with aligned lookbacks.
 * @endinternal
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
 * @internal
 * @brief Collect all positive equalities in `n` of the given BA type and merge them.
 *
 * Finds all `bf_eq` atoms whose BA type matches `type_id`, normalizes each
 * to `f = 0` form, and combines their left-hand sides into a single disjunction
 * `f1 | f2 | ...`. Returns the resulting BF, or `nullptr` if no match is found.
 * @tparam node Tree node type.
 * @param n Formula to search for equalities.
 * @param type_id BA type identifier selecting which equalities to merge.
 * @return Disjunction of all matching left-hand sides, or `nullptr`.
 * @endinternal
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
	// select_top descends through wff_neg, so a bf_eq inside a `!(f = 0)`
	// would be collected here as if it were positive and its negation
	// dropped. Callers must therefore hand this an NNF formula, in which a
	// negated equation is a bf_neq; the assert makes that precondition loud
	// rather than leaving it to the to_nnf ordering around the call site.
	DBG(assert(!tau::get(n).find_top([](tref m) {
		return is<node, tau::wff_neg>(m)
			&& tree<node>::get(m).child_is(tau::bf_eq);
	}));)
	if (trefs eqs = tau::get(n).select_top(match);
		eqs.size() > 0)
	{
		for (tref& eq : eqs) {
			eq = norm_trimmed_equation<node>(eq);
		}
		eqs = tt(eqs) | tt::children | tt::refs;
		// The reshape above can in principle change the vector's size, so
		// re-check before indexing it.
		if (eqs.empty()) {
			LOG_TRACE << "(I) squeeze_positives result: none (reshaped away)";
			return nullptr;
		}
		tref res = tau::build_bf_or(eqs, find_ba_type<node>(eqs[0]));
		LOG_TRACE << "squeeze_positives result: " << LOG_FM(res);
		return res;
	}
	LOG_TRACE << "(I) squeeze_positives result: none";
	return nullptr;
}


/**
 * @internal
 * @brief Replace all free variables in `fm` with `val`.
 *
 * Collects every free variable in `fm` and substitutes each with `val`.
 * Returns `fm` unchanged if it has no free variables.
 * @tparam node Tree node type.
 * @param fm Formula whose free variables are to be replaced.
 * @param val Replacement value; must not be a bare `bf` node.
 * @return Formula with all free variables replaced by `val`.
 * @endinternal
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
 * @internal
 * @brief Function to apply syntactical simplifications to formula in almost
 * linear time in the formula size and the number of paths found in terms
 * @tparam node tree node type
 * @param formula The formula to simplify
 * @return The simplified formula
 * @endinternal
 */
template<NodeType node>
tref syntactic_formula_simplification(tref formula) {
	formula = simplify_using_equality<node>(formula);
	return syntactic_path_simplification<node>(formula);
}
