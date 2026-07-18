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


/** @internal @copydoc no_skip @endinternal */
template <NodeType node>
bool no_skip(tref) {
	return false;
}
