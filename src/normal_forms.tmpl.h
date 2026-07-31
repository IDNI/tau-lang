// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <list>
#include <numeric>

#include "nso_rr.h"
#include "tau_bdd.h"
#include "normal_forms.h"
#include "tau_bdd.h"
#include "union_find_with_sets.h"
#include "heuristics/bv_predicate_blasting.h"
#include "heuristics/simplify_using_equality.h"

namespace idni::tau_lang {

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "normal_forms"

// Forward declaration for impl-only helper used before normal_forms_transformations.tmpl.h is included.
//template <NodeType node> tref treat_ex_quantified_clause(tref ex_clause, bool& quant_eliminated);

/**
 * @internal
 * @brief Rewrites `!($X = 0)` to `$X != 0` throughout `fm`, the dual of `unequal_to_not_equal`.
 * @tparam node Tree node type.
 * @param fm Formula to transform.
 * @return Formula with negated equalities replaced by disequalities.
 *
 * @par Example
 * @code{.cpp}
 * // !(x = y) becomes x != y
 * tref fm = get_nso_rr("!(x = y).").value().main->get();
 * tref res = not_equal_to_unequal<node_t>(fm);
 * // tau::get(res).to_str() == "x != y"
 * @endcode
 * @endinternal
 */
template <NodeType node>
tref not_equal_to_unequal(tref fm) {
	using tau = tree<node>;
	LOG_TRACE << "not_equal_to_unequal: " << LOG_FM(fm);
	auto not_eq_to_neq = [](tref n) {
		//!($X = 0) ::= $X != 0
		const auto& t = tau::get(n);
		if (t.is(tau::wff_neg))
			if (const auto& e = t[0][0]; e.is(tau::bf_eq))
				return tau::trim(
					tau::build_bf_neq(
						e.first(), e.second()));
		return n;
	};
	tref result = pre_order<node>(fm)
				.apply_unique(not_eq_to_neq, while_is_formula<node>);
	LOG_TRACE << "not_equal_to_unequal: " << LOG_FM(result);
	return result;
}

/**
 * @internal
 * @brief Normalizes comparison operators by rewriting `!=`, `nlteq`, `nlt`, `gteq`, `gt`, `ngteq`, and `ngt` to canonical negated `<`, `<=`, or `=` forms.
 * @tparam node Tree node type.
 * @param fm Formula whose atomic operators are to be normalized.
 * @return Formula with comparison operators rewritten to canonical form.
 *
 * @par Example
 * @code{.cpp}
 * // x != y is rewritten to its canonical negated-equality form !(x = y)
 * tref fm = get_nso_rr("x != y.").value().main->get();
 * tref res = normalize_atomic_formula_operators<node_t>(fm);
 * // tau::get(res).to_str() == "!x = y"   (i.e. !(x = y), the dual of
 * // not_equal_to_unequal's rewrite)
 * @endcode
 *
 * This looks redundant at first glance: non-bv `wff_lt/nlt/lteq/nlteq/
 * gt/ngt/gteq/ngteq` nodes are already decomposed into and/or/eq form by
 * the construction-time hooks (hooks.tmpl.h), and bv-typed occurrences of
 * `bf_neq/gt/gteq/nlt/ngt/nlteq/ngteq` are handled directly by predicate
 * blasting and cvc5 (bv_predicate_blasting.tmpl.h, bv_ba_solver.tmpl.h) --
 * so it's tempting to drop this pass and rely on those. Both attempts
 * regressed the test suite, empirically:
 *  - Removing the pass entirely crashes an assertion in
 *    push_ex_block_into_clause (`!find_top(is<bf_neq>)`) and aborts most
 *    of the satisfiability/solver/splitter/interpreter/api tests: several
 *    downstream matchers (this one, trivial_skolem_ex's bf_eq-only
 *    matcher, the is_atomic filters gating Boole decomposition) hard-
 *    assume only bf_eq/bf_lt/bf_lteq atoms ever reach them.
 *  - Narrowing it to just the bf_neq case (leaving nlteq/nlt/gteq/gt/
 *    ngteq/ngt untouched) stops the crashes but still regresses Release:
 *    test_integration-satisfiability2/4 time out (fragmenting the atom
 *    space hurts pivot selection/memoization in the Boole-decomposition
 *    pipeline enough to blow up what's normally fast), test_integration-
 *    wff_normalization prints "Failed to translate the formula to cvc5:
 *    [t > 3]" (a non-bv atom left unresolved by the generic pipeline
 *    falls through to the cvc5 fallback and cvc5 rejects it), and
 *    test_integration-heuristics-syntactic_path_simplification fails on
 *    formulas containing only `=`/`!=` (to_nnf produces these ordering
 *    node shapes internally even from pure equality negation, so a
 *    neq-only fix doesn't cover it).
 * In short: the hooks and blasting/cvc5 handle *their* construction
 * paths, but this pass also normalizes comparison atoms produced
 * internally by the generic, bv-agnostic quantifier-elimination/NNF
 * machinery (anti_prenex*, onf_wff, to_nnf) -- which is why it can't be
 * removed or narrowed without a broader rewrite of those consumers.
 * @endinternal
 */
template<NodeType node>
tref normalize_atomic_formula_operators(tref fm) {
	using tau = tree<node>;
	LOG_TRACE << "Begin normalize_atomic_formula_operators: " << LOG_FM(fm);
	auto normalize_operators = [](tref n) {
		if (!tau::get(n).is(tau::wff)) return n;
		const tau& c = tau::get(n)[0];
		switch (c.value.nt) {
			case tau::bf_neq:
				return tau::build_wff_neg(
					tau::build_bf_eq(c.first(), c.second()));
			case tau::bf_nlteq:
				return tau::build_bf_lt(c.second(), c.first());
			case tau::bf_nlt:
				return	tau::build_bf_lteq(c.second(), c.first());
			case tau::bf_gteq:
				return tau::build_bf_lteq(c.second(), c.first());
			case tau::bf_gt:
				return tau::build_bf_lt(c.second(), c.first());
			case tau::bf_ngteq:
				return tau::build_bf_lt(c.first(), c.second());
			case tau::bf_ngt:
				return tau::build_bf_lteq(c.first(), c.second());
			default: return n;
		}
	};
	tref result = pre_order<node>(fm)
				.apply_unique(normalize_operators, while_is_formula<node>);
	LOG_TRACE << "End normalize_atomic_formula_operators: " << LOG_FM(result);
	return result;
}

/**
 * @internal
 * @brief Rewrites `>` and `>=` (and their negations `ngt`, `ngteq`) to `<` and `<=` by swapping operands.
 * @tparam node Tree node type.
 * @param fm Formula in which `>` / `>=` operators are to be rewritten.
 * @return Formula with `>` and `>=` replaced by swapped-operand `<` and `<=`.
 *
 * @par Example
 * A raw `bf_gt(a, b)` node (built directly, e.g. via `tau::build_bf_gt`)
 * becomes `bf_lt(b, a)`; a raw `bf_gteq(a, b)` becomes `bf_lteq(b, a)`, and
 * likewise for their negations `bf_ngt`/`bf_ngteq`. No compilable
 * `create_spec`/`get_nso_rr` snippet is given here: for the Boolean-algebra
 * types this codebase parses by default, `>`/`>=` are already desugared to
 * an equivalent AND/XOR form before a `bf_gt`/`bf_gteq` node would ever
 * appear in the parsed tree, so this rewrite is only reachable on
 * hand-built or not-yet-fully-normalized fragments.
 * @endinternal
 */
template<NodeType node>
tref gt_gteq_to_lt_lteq(tref fm) {
	using tau = tree<node>;
	LOG_TRACE << "gt_gteq_to_lt_lteq: " << LOG_FM(fm);
	auto normalize_operators = [](tref n) {
		if (!tau::get(n).is(tau::wff)) return n;
		const tau& c = tau::get(n)[0];
		switch (c.value.nt) {
			case tau::bf_gteq:
				return tau::build_bf_lteq(c.second(), c.first());
			case tau::bf_gt:
				return tau::build_bf_lt(c.second(), c.first());
			case tau::bf_ngteq:
				return tau::build_bf_nlteq(c.second(), c.first());
			case tau::bf_ngt:
				return tau::build_bf_nlt(c.second(), c.first());
			default: return n;
		}
	};
	tref result = pre_order<node>(fm)
				.apply_unique(normalize_operators, while_is_formula<node>);
	LOG_TRACE << "gt_gteq_to_lt_lteq: " << LOG_FM(result);
	return result;
}

/** @internal @copydoc to_nnf @endinternal */
template <NodeType node>
tref to_nnf(tref fm) {
	LOG_TRACE << "to_nnf: " << LOG_FM(fm);
	auto result = push_negation_in<node>(fm);
	LOG_TRACE << "to_nnf result: " << LOG_FM(result);
	return result;
}

/**
 * @internal
 * @brief Reverses normalization of equations of the form `(X XOR Y) = 0` or `(X XOR Y) != 0` back to `X = Y` / `X != Y`.
 * @tparam node Tree node type.
 * @param eq Normalized equation wrapped in `wff`.
 * @return De-normalized equation, or the original node if no XOR pattern is found.
 *
 * @par Example
 * This exactly reverses `norm_trimmed_equation` (normal_forms_transformations.tmpl.h),
 * which turns `X = Y` into `(X XOR Y) = 0` (and `X != Y` into `(X XOR Y) != 0`)
 * for use during equation manipulation (e.g. `normal_forms.tmpl.h:284-315`).
 * Given a node shaped like `(X XOR Y) = 0`, `denorm_equation` returns `X = Y`;
 * given anything else (including an equation whose LHS is not a `bf_xor`,
 * e.g. `x = y` as parsed directly from a spec string, which this codebase's
 * Boolean-algebra printer already renders as `"x = y"` without ever
 * constructing a literal `bf_xor` node), it returns @p eq unchanged. No
 * compilable round-trip is given here since parsing `"x = y."` directly does
 * not produce the `bf_xor`-shaped node this function targets.
 * @endinternal
 */
template<NodeType node>
tref denorm_equation(tref eq) {
	using tau = tree<node>;
	tau e = tau::get(eq);
	if (e.child_is(tau::bf_eq) && e[0][1].equals_0()) {
		const tau& ce = e[0][0];
		if (ce.child_is(tau::bf_xor))
			return tau::build_bf_eq(ce[0].first(), ce[0].second());
	} else if (e.child_is(tau::bf_neq) && e[0][1].equals_0()) {
		const tau& ce = e[0][0];
		if (ce.child_is(tau::bf_xor))
			return tau::build_bf_neq(ce[0].first(), ce[0].second());
	}
	return eq;
}

/**
 * @internal
 * @brief Normalises Boolean algebra constants in a Boolean function term.
 *
 * Pushes negations into BA constants and calls `node::ba::normalize` on each constant node.
 * @tparam node Tree node type.
 * @param fm Boolean function term to normalize.
 * @return Term with all BA constants normalized.
 *
 * @par Example
 * @code{.cpp}
 * // A plain variable term has no BA constants to normalize, so it is
 * // returned unchanged.
 * tref fm = get_nso_rr("x = 0.").value().main->get();
 * tref var = tau::get(fm)[0].first();
 * tref res = normalize_ba<node_t>(var);
 * // tau::get(res).to_str() == "x"
 * @endcode
 * @endinternal
 */
// This function traverses a term fm and normalizes all Boolean algebra constants
template <NodeType node>
tref normalize_ba(tref fm) {
	using tau = tree<node>;
	DBG(LOG_TRACE << "normalize_ba: " << LOG_FM(fm));
	using tau = tree<node>;
	DBG(assert(tau::get(fm).is(tau::bf));)
	auto push_negation = [&](tref n) {
		const tau& t = tau::get(n);
		// Push negation into constants
		if (t.is(tau::bf_neg) && t[0].child_is(tau::ba_constant)) {
			const auto c = t[0][0].get_ba_constant();
			return tau::get_ba_constant(~c, t.get_ba_type());
		}
		// Push negations in
		n = push_negation_one_in<node, false>(n);
		return n;
	};
	auto norm_ba = [](tref n) {
		const tau& t = tau::get(n);
		// Check if node is a constant
		if (!t.is(tau::ba_constant)) return n;
		// Node has a Boolean algebra element
		auto c = t.get_ba_constant();
		auto nc = node::ba::normalize(c);
		if (c == nc) return n;
		return tau::get_ba_constant(nc, t.get_ba_type());
	};
	fm = pre_order<node>(fm).apply_unique(push_negation);
	tref r = pre_order<node>(fm).template apply_unique<normalize_ba_m>(norm_ba);
	DBG(LOG_TRACE << "normalize_ba result: " << LOG_FM(r));
	return r;
}

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "normal_forms"

/**
 * @internal
 * @brief Finds the deepest quantifier inside `n` and returns a pair of (traverser-to-variable, tref-to-quantified-wff).
 *
 * If no quantifier is present returns an empty traverser and `n` itself.
 * @tparam node Tree node type.
 * @param n Formula to search for the innermost quantifier.
 * @return Pair of traverser pointing to the bound variable and tref to the quantified sub-formula.
 *
 * @par Example
 * @code{.cpp}
 * // "ex x (x = 0 && y = 0)" has a single (outermost = innermost) quantifier;
 * // the parser renames the bound variable to "b1" internally.
 * tref fm = get_nso_rr("ex x (x = 0 && y = 0).").value().main->get();
 * auto [var_trav, inner] = get_inner_quantified_wff<node_t>(fm);
 * // var_trav.has_value() == true, tau::get(var_trav.value()).to_str() == "b1"
 * // tau::get(inner).to_str() == "b1 = 0 && y = 0"
 * @endcode
 * @endinternal
 */
// return the inner quantifier or the top wff if the formula is not quantified
template <NodeType node>
std::pair<typename tree<node>::traverser, tref> get_inner_quantified_wff(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	// TODO (LOW) extract to a utils file
	auto quantified = [](const auto& n) -> bool {
		auto const& t = tau::get(n);
		return t.is(tau::wff_ex)
			|| t.is(tau::wff_all)
			|| t.is(tau::wff_sometimes)
			|| t.is(tau::wff_always);
	};
	if (auto quantifier = tau::get(n).find_bottom(quantified); quantifier) {
		return { tt(quantifier) | tau::variable,
			 tt(quantifier) | tau::wff | tt::ref };
	}
	return { tt{}, n };
}

template <NodeType node>
onf_wff<node>::onf_wff(tref _var) {
	using tau = tree<node>;
	const auto& t = tau::get(_var);
	if (!t.is(tau::bf)) var = tau::get(tau::bf, _var);
	else var = _var;
}

template <NodeType node>
tref onf_wff<node>::operator()(tref n) const {
	using tau = tree<node>;
	auto [ _ , nn] = get_inner_quantified_wff<node>(n);
	// We assume that the formula is in DNF. In particular nn is in DNF
	// For each disjunct calculate the onf
	subtree_map<node, tref> changes;
	bool no_disjunction = true;
	for (tref disjunct_ref : tau::get(nn).select_all(is<node, tau::wff_or>))
	{
		no_disjunction = false;
		const auto& disjunct = tau::get(disjunct_ref);
		DBG(assert(disjunct.children_size() == 2);)
		if (!disjunct[0][0].is(tau::wff_or))
			changes[disjunct[0].first()] =
					onf_subformula(disjunct[0].first());
		if (!disjunct[1][0].is(tau::wff_or))
			changes[disjunct[1].first()] =
					onf_subformula(disjunct[1].first());
	}
	if (no_disjunction) changes[nn] = onf_subformula(nn);
	return rewriter::replace<node>(nn, changes);
}

template <NodeType node>
tref onf_wff<node>::onf_subformula(tref n) const {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto invalid = [](tref n) {
		if (tau::get(n).is_term() && is_non_boolean_term<node>(n))
			return true;
		else return false;
	};
	if (tau::get(n).find_top(invalid)) return n;
	auto has_var = [&](const auto& el) {
		return tau::get(el) == tau::get(var);
	};
	const auto& t = tau::get(n);
	tref eq = t.find_bottom(is<node, tau::bf_eq>);
	subtree_map<node, tref> changes;
	if (eq && (tau::get(eq)[0].find_top(has_var)
			|| tau::get(eq)[1].find_top(has_var))) {
		// keep the original (pre-normalization) node as the map key: n
		// contains this node, not the one norm_trimmed_equation rebuilds.
		tref orig_eq = eq;
		eq = norm_trimmed_equation<node>(eq);
		const auto& eq_v = tau::get(eq);
		DBG(assert(eq_v[1][0].is(tau::bf_f));)
		tref f_0 = tt(rewriter::replace<node>(
			eq_v.first(), var, tau::_0(find_ba_type<node>(var))))
				| bf_reduce_canonical<node>() | tt::ref;
		tref f_1 = tt(rewriter::replace<node>(
			tau::build_bf_neg(eq_v.first()), var,tau::_1(find_ba_type<node>(var))))
				| bf_reduce_canonical<node>() | tt::ref;

		changes[orig_eq] = tau::trim(tau::build_bf_interval(
							f_0, var, f_1));
	}
	for (tref orig_neq : t.select_all(is<node, tau::bf_neq>)) {
		if (!tau::get(orig_neq)[0].find_top(has_var)
			&& !tau::get(orig_neq)[1].find_top(has_var)) continue;
		tref neq_ref = norm_trimmed_equation<node>(orig_neq);
		const auto& neq = tau::get(neq_ref);
		DBG(assert(neq[1][0].is(tau::bf_f));)
		tref f_0 = tt(rewriter::replace<node>(
			neq.first(), var,
			tau::_0(find_ba_type<node>(var))))
				| bf_reduce_canonical<node>() | tt::ref;
		tref f_1 = tt(rewriter::replace<node>(
			tau::build_bf_neg(neq.first()), var,
			tau::_1(find_ba_type<node>(var))))
				| bf_reduce_canonical<node>() | tt::ref;
		changes[orig_neq] = tau::trim(tau::build_wff_or(
			tau::build_bf_nlteq(f_0, var),
			tau::build_bf_nlteq(var, f_1)));
	}
	return rewriter::replace<node>(n, changes);
}

/** @internal @copydoc operator| @endinternal */
template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& t, const onf_wff_t<node>& r)
{
	return tt(r(t.value()));
}

/** @internal @copydoc onf @endinternal */
template <NodeType node>
tref onf(tref n, tref var) {
	using tau = tree<node>;
	using tt = tau::traverser;
	// FIXME take into account quiantifiers
	return tt(n) | tt::f(to_dnf<node, true>)
		     | tt::f(onf_wff<node>(var))
		     | tt::f(to_dnf<node, true>) | tt::ref;
}

/**
 * @internal
 * @brief Tries to merge two variable assignments that differ in exactly one position (Hamming distance 1) by setting that position to `2` (irrelevant).
 * @param i Current assignment vector to attempt merging with an existing path.
 * @param paths Collection of existing path vectors; updated in place on merge.
 * @param p Number of variables considered (length of the active prefix).
 * @param surface `true` when called at the top level; controls which vector absorbs the merge.
 * @return `true` if a merge occurred (possibly yielding a fully irrelevant path).
 *
 * @par Example
 * With variables `[a, b]`, path vector `{1, 1}` encodes the clause `ab` and
 * `{1, -1}` encodes `ab'` (`1` = positive occurrence, `-1` = negated, `2` =
 * irrelevant). This is exactly the `bf_reduced_dnf(ab|ab')` case (see
 * @ref bf_reduced_dnf and tests/unit/test_normal_forms.cpp:787-794, which
 * asserts the reduced result is `a`):
 * @code{.cpp}
 * std::vector<int_t> i = {1, -1};              // ab'
 * std::vector<std::vector<int_t>> paths = {{1, 1}};  // [ab]
 * bool merged = reduce_paths(i, paths, 2);
 * // merged == true, paths == {{1, 2}}  (i.e. "a", b marked irrelevant)
 * @endcode
 * @endinternal
 */
// Reduce currrent dnf due to update by coeff and variable assignment i
inline bool reduce_paths(std::vector<int_t>& i,
	std::vector<std::vector<int_t>>& paths, int_t p, bool surface = true)
{
	for (size_t j = 0; j < paths.size(); ++j) {
		if (paths[j].empty()) continue;
		// Get Hamming distance between i and path and position of last difference
		// while different irrelevant variables make assignments incompatible
		int_t dist = 0, pos = 0;
		for (int_t k = 0; k < p; ++k) {
			if (i[k] == paths[j][k]) continue;
			else if (dist == 2) break;
			else if (i[k] == 2 || paths[j][k] == 2) { dist = 2; break; }
			else dist += 1, pos = k;
		}
		if (dist == 1) {
			// Remove i from paths if recursion depth is greater 0
			if(!surface) {
				paths[j] = {};
				// Resolve variable
				i[pos] = 2;
				if(std::ranges::all_of(i, [](const auto el) {return el == 2;}))
					return paths = {}, true;
				// Continue with resulting assignment
				reduce_paths(i, paths, p, false);
			} else {
				// Resolve variable
				paths[j][pos] = 2;
				if(std::ranges::all_of(paths[j], [](const auto el) {return el == 2;}))
					return paths = {}, true;
				// Continue with resulting assignment
				reduce_paths(paths[j], paths, p, false);
			}
			return true;
		}
	}
	return false;
}

/**
 * @internal
 * @brief Repeatedly merges any two paths in `paths` that differ in exactly one variable or have a subset/superset relation, simplifying the path set in place.
 * @param paths Path vectors to simplify; updated in place.
 *
 * @par Example
 * With variables `[a, b, c]`, `ac` is `{1, 2, 1}` and `a'b'c` is
 * `{-1, -1, 1}`. `reduce_paths` cannot merge these (they differ in two
 * positions once `b`'s wildcard is accounted for), but `a'b'c` is a subset
 * of the "b unconstrained" shape once `a` is generalized, so `join_paths`
 * absorbs it: `a'b'c` becomes `b'c` (this is the `make_paths_disjoint=false`
 * side of tests/unit/test_normal_forms.cpp:812-830, where the comment traces
 * the same merge on `ac|a'b'c`).
 * @code{.cpp}
 * std::vector<std::vector<int_t>> paths = {{1, 2, 1}, {-1, -1, 1}}; // ac, a'b'c
 * join_paths(paths);
 * // paths == {{1, 2, 1}, {2, -1, 1}}  (i.e. "ac", "b'c")
 * @endcode
 * @endinternal
 */
inline void join_paths(std::vector<std::vector<int_t>>& paths) {
	for (int_t i = 0; i < (int_t)paths.size(); ++i) {
		for (int_t j = 0; j < (int_t)paths.size(); ++j) {
			if (i == j) continue;
			int_t dist = 0, pos = 0;
			bool subset_relation_decided = false, is_i_subset_of_j = true,
				subset_check = true, equal = true;
			for (size_t k=0; k < paths[i].size(); ++k) {
			if (paths[i][k] == paths[j][k]) continue;
			else if (dist == 2) break;
			else if (paths[i][k] == 2) {
				if (!subset_relation_decided) {
					subset_relation_decided = true;
					is_i_subset_of_j = true;
					if (paths[j][k] != 2)
						equal = false;
				} else {
					if (!is_i_subset_of_j) {
						subset_check = false;
						break;
					}
					if (paths[j][k] != 2)
						equal = false;
				}
			}
			else if (paths[j][k] == 2) {
				if (!subset_relation_decided) {
					subset_relation_decided = true;
					is_i_subset_of_j = false;
					// paths[i][k] != 2
					equal = false;
				} else {
					// paths[i][k] != 2
					equal = false;
					if (is_i_subset_of_j) {
						subset_check = false;
						break;
					}
				}
			}
			else dist += 1, pos = k;
		}
		if (subset_check && dist == 1) {
			if (is_i_subset_of_j) {
				// Resovle variable in paths
				paths[j][pos] = 2;
				if (equal) {
					paths.erase(paths.begin()+i);
					--i;
					break;
				}
			} else {
				// Resolve variable in i
				paths[i][pos] = 2;
			}
		} else if (subset_check && dist == 0) {
			// True subset relation between i and j
			if (is_i_subset_of_j) {
				// i -> j
				paths.erase(paths.begin()+j);
				if (i > j) --i;
				--j;
			} else {
				paths.erase(paths.begin()+i);
				if (j > i) --j;
				--i;
				break;
			}
		}
		}
	}
}

// ------------------------------

/**
 * @internal
 * @brief Marks variables in the assignment vector `i` (from position `p+1` onward) as irrelevant (`2`) if they do not appear in the current reduced formula `fm`.
 * @tparam node Tree node type.
 * @param fm Current reduced formula.
 * @param vars Ordered list of all variables.
 * @param i Assignment vector to update; entries are set to `2` for absent variables.
 * @param p Current variable position; positions after `p` are candidates for elimination.
 * @param is_var Predicate that returns `true` for variable nodes.
 *
 * @par Example
 * With `vars = [a, b]` and a reduced formula `fm` that is just the term `a`
 * (i.e. `b` has already dropped out of the reduction), calling this with
 * `p = 0` scans positions after `a` and, finding no `b` in `fm`, sets
 * `i[1] = 2`: an assignment vector `{1, 1}` becomes `{1, 2}`, meaning `b` is
 * irrelevant. This is how `assign_and_reduce` shrinks a fully-assigned path
 * like `ab` down to just `a` whenever the reduced term no longer mentions
 * `b` (see the `bf_reduced_dnf(ab|ab')` example, which shrinks to `a`).
 * @endinternal
 */
// Starting from variable at position p+1 in vars write to i which variables are irrelevant in assignment
template <NodeType node>
void elim_vars_in_assignment(tref fm, const auto& vars, auto& i,
		const int_t p, const auto& is_var) {
	using tau = tree<node>;

	auto cvars = tau::get(fm).select_all(is_var);
	subtree_set<node> cur_vars(
		std::make_move_iterator(cvars.begin()),
		std::make_move_iterator(cvars.end()));

	for (size_t v_iter = p + 1; v_iter < vars.size(); ++v_iter)
		if (!cur_vars.contains(vars[v_iter])) i[v_iter] = 2;
}

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "assign_and_reduce"

/**
 * @internal
 * @brief Recursively enumerates all 0/1 assignments to `vars`, reduces the formula at each assignment, and populates `dnf` with coefficient → path-vector entries.
 * @tparam node Tree node type.
 * @param fm Formula to enumerate assignments over.
 * @param vars Ordered list of variables to assign.
 * @param i Current assignment vector (modified in place during recursion).
 * @param dnf Output map from reduced coefficients to their path vectors.
 * @param is_var Predicate that returns `true` for variable nodes.
 * @param p Current recursion depth (variable index).
 * @param is_wff `true` when `fm` is a well-formed formula, `false` for Boolean function.
 * @return `true` when a tautological (all-irrelevant) path is found.
 *
 * @par Example
 * This is the recursive engine behind `bf_reduced_dnf`: for `fm = ab|ab'`
 * with `vars = [a, b]`, it enumerates `a=0`/`a=1` and, within each,
 * `b=0`/`b=1`, reduces `fm` at each of the 4 corners, and (via
 * `elim_vars_in_assignment` and `reduce_paths`) collapses the two paths
 * where `a=1` (both satisfy `fm`, differing only in `b`) into the single
 * path `{1, 2}` in `dnf` — i.e. the reduced DNF is just `a` (see
 * tests/unit/test_normal_forms.cpp:787-794).
 * @endinternal
 */
// Create assignment in formula and reduce resulting clause
template <NodeType node>
bool assign_and_reduce(tref fm, const trefs& vars, std::vector<int_t>& i,
	auto& dnf, const auto& is_var, int_t p = 0, bool is_wff = false)
{
	using tau = tree<node>;
#ifdef DEBUG
	LOG_TRACE << "Begin assign_and_reduce [" << LOG_NT_COLOR
		<< (is_wff ? "wff" : "bf") << TC.CLEAR() << "]: "
		<< LOG_FM_DUMP(fm);
	for (auto v : vars) LOG_TRACE << "v: " << TAU_TO_STR(v);
	LOG_TRACE << "p: " << p;
#endif // DEBUG
	auto report = [&](bool result) {
		DBG(LOG_TRACE << (result ? "result is ok" : "no result")
				<< " for input: " << LOG_FM(fm);)
		return result;
	};

	// Check if all variables are assigned
	if((int_t) vars.size() == p) {
		tref fm_simp = nullptr;
		if (!is_wff) {
			// Do not add to dnf if the coefficient is 0
			if (tau::get(fm).equals_0()) return report(false);
			// fm is a Boolean function
			// Normalize tau subformulas
			fm_simp = normalize_ba<node>(fm);
			DBG(LOG_TRACE << "normalize_ba result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_0()) return report(false);
			fm_simp = to_dnf<node, false>(fm_simp);
			DBG(LOG_TRACE << "to_dnf result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_0()) return report(false);
			fm_simp = reduce<node>(fm_simp);
			DBG(LOG_TRACE << "reduce result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_0()) return report(false);
		} else {
			if (tau::get(fm).equals_F()) return report(false);
			// fm is a Tau formula
			fm_simp = to_dnf<node, false>(fm);
			DBG(LOG_TRACE << "to_dnf result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_F()) return report(false);
			fm_simp = reduce<node>(fm_simp);
			DBG(LOG_TRACE << "reduce result: " << LOG_FM(fm_simp);)
			if (tau::get(fm_simp).equals_F()) return report(false);
		}
		if (std::ranges::all_of(i, [](const auto el){return el == 2;})){
			//bool t = is<node>(fm->child[0], tau::bf_t);
			return dnf.emplace(fm_simp, std::vector(0, i)), true;
		}

		auto it = dnf.find(fm_simp);
		if (it == dnf.end()) return dnf.emplace(fm_simp,
				std::vector(p == 0 ? 0 : 1, i)), report(false);
		else if (!reduce_paths(i, it->second, p)) {
			// Place coefficient together with variable assignment if no reduction happend
			it->second.push_back(i);
		} else std::erase_if(it->second,
				[](const auto& v) { return v.empty(); });
		return it->second.empty();
	}
	// variable was already eliminated
	if (i[p] == 2) {
		if (assign_and_reduce<node>(fm, vars, i, dnf, is_var, p + 1,
						is_wff)) return report(true);
		i[p] = 0;
		return report(false);
	}
	// Substitute 1 and 0 for v and simplify
	const auto& v = vars[p];
	tref t = is_wff ? tau::_T() : tau::_1(find_ba_type<node>(v));
	tref f = is_wff ? tau::_F() : tau::_0(find_ba_type<node>(v));
	tref fm_v1 = rewriter::replace<node>(fm, v, t);
	tref fm_v0 = rewriter::replace<node>(fm, v, f);

	elim_vars_in_assignment<node>(fm_v1, vars, i, p, is_var);
	if (tau::get(fm_v1) == tau::get(fm_v0)) {
		i[p] = 2;
		if (assign_and_reduce<node>(fm_v1, vars, i, dnf, is_var, p + 1,
						is_wff)) return report(true);
		i[p] = 0;
	} else {
		i[p] = 1;
		if (assign_and_reduce<node>(fm_v1, vars, i, dnf, is_var, p + 1,
						is_wff)) return report(true);
		i[p] = 0;
		elim_vars_in_assignment<node>(fm_v0, vars, i, p, is_var);
		i[p] = -1;
		if (assign_and_reduce<node>(fm_v0, vars, i, dnf, is_var, p + 1,
						is_wff)) return report(true);
		i[p] = 0;
	}
	return report(false);
}

/**
 * @internal
 * @brief Lexicographic comparator for Tau formula variables.
 *
 * Compares two variable nodes by the string representation of their sub-trees.
 * Used to define a canonical variable order for BDD construction and DNF
 * reduction.
 * @tparam node Tree node type.
 *
 * @par Example
 * @code{.cpp}
 * // "x" < "y" lexicographically
 * tref x = tau::get(get_nso_rr("x = 0.").value().main->get())[0].first();
 * tref y = tau::get(get_nso_rr("y = 0.").value().main->get())[0].first();
 * CHECK( lex_var_comp<node_t>(x, y) == true );
 * @endcode
 * @endinternal
 */
template <NodeType node>
auto lex_var_comp = [](tref x, tref y) {
#ifdef TAU_CACHE
	using cache_t = std::map<std::pair<tref, tref>, bool,
				subtree_pair_less<node, tref>>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find({x,y}); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	// TODO (QUESTION) strings have unique id, use .data() instead?
	auto xx = tree<node>::get(x).to_str();
	auto yy = tree<node>::get(y).to_str();
#ifdef TAU_CACHE
	return cache.emplace(std::make_pair(x, y), xx < yy).first->second;
#endif // TAU_CACHE
	return xx < yy;
};

/**
 * @internal
 * @brief Predicate that classifies a wff node as a BDD variable.
 *
 * In BDD-based DNF/CNF reductions of well-formed formulas the following node
 * types are treated as atomic BDD variables: `bf_eq`, `wff_ref`, `wff_ex`,
 * `wff_sometimes`, `wff_always`, `wff_all`, and `constraint`.
 * @tparam node Tree node type.
 * @todo Extend for the full grammar.
 *
 * @par Example
 * @code{.cpp}
 * tref fm = get_nso_rr("x = 0.").value().main->get();
 * CHECK( is_wff_bdd_var<node_t>(fm) == true );  // fm's child is a bf_eq
 * @endcode
 * @endinternal
 */
template <NodeType node>
inline auto is_wff_bdd_var = [](tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	// `n` is a wff wrapper, so the invariant to check is on its child: the
	// caller (dnf_cnf_to_reduced) establishes it with unequal_to_not_equal.
	// Asserting `t.is(bf_neq)` instead would be vacuously true and check
	// nothing.
	DBG(assert(!t.child_is(tau::bf_neq));)
	return t.child_is(tau::bf_eq)
		|| t.child_is(tau::wff_ref)
		|| t.child_is(tau::wff_ex)
		|| t.child_is(tau::wff_sometimes)
		|| t.child_is(tau::wff_always)
		|| t.child_is(tau::wff_all)
		|| t.child_is(tau::constraint);
};

/**
 * @internal
 * @brief Predicate that classifies a bf node as a BDD variable.
 *
 * In BDD-based reductions of Boolean functions the following node types are
 * treated as atomic BDD variables: `variable`, `capture`, `bf_ref`,
 * `ba_constant`, `bf_fall`, and `bf_fex`.
 * @tparam node Tree node type.
 *
 * @par Example
 * @code{.cpp}
 * tref fm = get_nso_rr("x = 0.").value().main->get();
 * tref x = tau::get(fm)[0].first(); // the bf variable x
 * CHECK( is_bf_bdd_var<node_t>(x) == true );
 * @endcode
 * @endinternal
 */
template <NodeType node>
inline auto is_bf_bdd_var = [](tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	return t.child_is(tau::variable)
		|| t.child_is(tau::capture)
		|| t.child_is(tau::bf_ref)
		|| t.child_is(tau::ba_constant)
		|| t.child_is(tau::bf_fall)
		|| t.child_is(tau::bf_fex);
};

// Given a BF b, calculate the Boole normal form (DNF corresponding to the paths to true in the BDD) of b
// where the variable order is given by the function lex_var_comp
/** @internal @copydoc bf_reduced_dnf @endinternal */
template <NodeType node>
tref bf_reduced_dnf(tref fm, bool make_paths_disjoint) {
	using tau = tree<node>;
	LOG_TRACE << "bf_boole_normal_form: " << LOG_FM(fm);
	static auto trace = [&](tref fm) {
		LOG_TRACE << "bf_boole_normal_form result: " << LOG_FM(fm);
		return fm;
	};
	// We do not treat terms that contain a non-Boolean operation
	if (rewriter::find_top<node>(fm, is_non_boolean_term<node>))
		return fm;
	fm = apply_all_xor_def<node>(fm);
	// Function can only be applied to a BF
	const auto& t = tau::get(fm);
	DBG(assert(t.is(tau::bf));)
#ifdef TAU_CACHE
	using cache_t = std::map<std::pair<tref, bool>, tref,
				subtree_pair_less<node, bool>>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(std::make_pair(fm, make_paths_disjoint));
		it != cache.end()) return trace(it->second);
#endif //TAU_CACHE
	// This defines the variable order used to calculate DNF
	// It is made canonical by sorting the variables
	auto is_var = [](tref n) {
		return tau::get(n).child_is(tau::variable);
	};
	auto vars = t.select_top(is_var);
	std::sort(vars.begin(), vars.end(), lex_var_comp<node>);

	std::vector<int_t> i(vars.size()); // Record assignments of vars

	// Resulting DNF - make it ordered for consistency
	// Key is coefficient, value is possible variable assignments for coefficient
	// unordered_tau_map<std::vector<std::vector<int_t>>, BAs...> dnf;
	subtree_map<node, std::vector<std::vector<int_t>>> dnf;

	if (assign_and_reduce<node>(fm, vars, i, dnf, is_var, 0)) {
		assert(dnf.size() == 1);
		return trace(dnf.begin()->first);
	}
	if (dnf.empty()) return trace(_0<node>(find_ba_type<node>(fm)));
	if (!make_paths_disjoint)
		for (auto& [coeff, paths] : dnf) join_paths(paths);

	// Convert map structure dnf back to rewrite tree
	tref reduced_dnf = nullptr;
	bool first = true;
	for (const auto& [coeff, paths] : dnf) {
		const auto& t = tau::get(coeff);
		bool is_one = t[0].is(tau::bf_t);
		if (paths.empty()) {
			DBG(assert(!is_one);)
			if (first) reduced_dnf = coeff;
			else reduced_dnf = tau::build_bf_or(reduced_dnf, coeff);
			continue;
		}
		for (const auto& path : paths) {
			bool first_var = true;
			tref var_path = nullptr;
			for (size_t k = 0; k < vars.size(); ++k) {
				if (path[k] == 2) continue;
				if (first_var) var_path = path[k] == 1 ? vars[k]
					: tau::build_bf_neg(vars[k]),
					first_var = false;
				else var_path = path[k] == 1
					? tau::build_bf_and(var_path, vars[k])
					: tau::build_bf_and(var_path,
						tau::build_bf_neg(vars[k]));
			}
			if (first) reduced_dnf = is_one ? var_path
				: tau::build_bf_and(coeff, var_path),
				first = false;
			else reduced_dnf = is_one
				? tau::build_bf_or(reduced_dnf, var_path)
				: tau::build_bf_or(reduced_dnf,
					tau::build_bf_and(coeff, var_path));
		}
	}
#ifdef TAU_CACHE
	cache.emplace(std::make_pair(fm, make_paths_disjoint), reduced_dnf);
	cache.emplace(std::make_pair(reduced_dnf, make_paths_disjoint),
								reduced_dnf);
#endif //TAU_CACHE
	return trace(reduced_dnf);
}

// The needed class in order to make bf_reduced_dnf work with rule applying process
template <NodeType node>
tref bf_reduce_canonical<node>::operator() (tref fm) const {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	LOG_TRACE << "bf reduce canonical: " << LOG_FM(fm);
	subtree_map<node, tref> changes = {};
	for (tref bf : t.select_top(is<node, tau::bf>)) {
		if (tau::get(bf).child_is(tau::bf_ref)) {
			// Iterate the bf_ref's arguments, not the root's children
			for (tref arg : tau::get(bf)[0][0].select_top(is<node, tau::bf>)) {
				tref dnf = bf_reduced_dnf<node>(arg);
				if (tau::get(dnf) != tau::get(arg))
					changes.emplace(arg, dnf);
			}
		}
		tref dnf = bf_reduced_dnf<node>(bf);
		if (tau::get(dnf) != tau::get(bf)) changes[bf] = dnf;
	}
	tref x = changes.empty()? fm : rewriter::replace<node>(fm, changes);
	LOG_TRACE << "bf reduce canonical result: " << LOG_FM(x);
	return x;
}

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& t,
	const bf_reduce_canonical<node>& r)
{
	return typename tree<node>::traverser(r(t.value()));
}

// template<typename... BAs>
// std::optional<tref> operator|(const std::optional<tref>& fm, const bf_reduce_canonical<BAs...>& r) {
// 	return fm.has_value() ? r(fm.value()) : std::optional<tref>{};
// }

/**
 * @internal
 * @brief Returns `true` if assignment `i` is contained in (i.e., covered by) some path in `paths`.
 *
 * Handles the "irrelevant" (`2`) wildcard in both `i` and path entries.
 * @param i Assignment vector to test.
 * @param paths Existing path vectors; paths matched as a superset are cleared in place.
 * @return `true` if `i` is covered by an existing path.
 * @endinternal
 */
inline bool is_contained_in(const std::vector<int_t>& i, auto& paths) {
	// Check if there is a containment of i in any path of paths
	for (auto& path : paths) {
		bool is_contained = true, is_i_smaller,
			containment_dir_known = false;
		for (int_t k = 0; k < (int_t)i.size(); ++k) {
			if (i[k] == path[k]) continue;
			else if (i[k] == 2) {
				if (containment_dir_known) {
					if (!is_i_smaller) {
						is_contained = false; break; }
				} else {
					containment_dir_known = true;
					is_i_smaller = true;
				}
			}
			else if (path[k] == 2) {
				if (containment_dir_known) {
					if (is_i_smaller) {
						is_contained = false; break; }
				} else {
					containment_dir_known = true;
					is_i_smaller = false;
				}
			}
			else if (i[k] != path[k]) {is_contained = false; break;}
		}
		if (is_contained) {
			if (is_i_smaller) {
				// keep i and delete current path
				path = {};
			} else return true;
		}
	}
	return false;
}

/**
 * @internal
 * @brief Converts a single DNF/CNF clause into an integer assignment vector using the variable position map `var_pos`.
 * @tparam node Tree node type.
 * @param clause Clause node to convert.
 * @param var_pos Map from variable tree references to their index positions.
 * @param wff `true` when the clause is a well-formed formula, `false` for Boolean function.
 * @param is_cnf `true` when processing a CNF clause, `false` for DNF.
 * @return Pair of the assignment vector and a flag indicating whether the clause is trivially decided.
 *
 * @par Example
 * For a DNF clause `ab'` (`bf_and(a, bf_neg(b))`) with `var_pos = {a: 0, b: 1}`,
 * `wff = false`, `is_cnf = false`: the traversal marks position 0 (`a`) as
 * `1` (positive occurrence) and position 1 (`b`) as `-1` (negated
 * occurrence), so the result is `{ {1, -1}, false }` — the vector encodes
 * `ab'` and `false` means the clause is not trivially decided (neither
 * always-true nor always-false).
 * @endinternal
 */
template <NodeType node>
std::pair<std::vector<int_t>, bool> clause_to_vector(tref clause,
	const auto& var_pos, const bool wff, const bool is_cnf)
{
	using tau = tree<node>;
	std::vector<int_t> i(var_pos.size());
	for (size_t k = 0; k < var_pos.size(); ++k) i[k] = 2;
	bool clause_is_decided = false;
	auto var_assigner = [&](tref n) {
		if (clause_is_decided) return false;
		const auto& t = tau::get(n);
		if (!is_cnf && t.is(wff ? tau::wff_f : tau::bf_f)) {
			clause_is_decided = true;
			return false;
		}
		if (is_cnf && t.is(wff ? tau::wff_t : tau::bf_t)) {
			clause_is_decided = true;
			return false;
		}
		if (t.is(wff ? tau::wff_neg : tau::bf_neg)) {
			const auto& v = t[0];
			// Check if v is a T/F or 1/0
			if (v.equals_T() || v.equals_1()) {
				if (!is_cnf) clause_is_decided = true;
				return false;
			} else if (v.equals_F() || v.equals_0()) {
				if (is_cnf) clause_is_decided = true;
				return false;
			}
			auto it = var_pos.find(v.get());
			assert(it != var_pos.end());
			if (i[it->second] == 1) {
				// clause is false for DNF, true for CNF
				clause_is_decided = true;
				return false;
			}
			i[it->second] = -1;
			return false;
		}
		if (auto it = var_pos.find(n); it != var_pos.end()) {
			if (i[it->second] == -1) {
				// clause is false for DNF, true for CNF
				clause_is_decided = true;
				return false;
			}
			i[it->second] = 1;
			return false;
		}
		else return true;
	};
	pre_order<node>(clause).visit_unique(var_assigner);
	return std::make_pair(std::move(i), clause_is_decided);
}

/**
 * @internal
 * @brief Collects all path vectors from the leaves of `new_fm`, deduplicates them, and returns the set.
 *
 * Sets `decided` to `false` if at least one non-trivial path is found.
 * @tparam node Tree node type.
 * @param new_fm Formula (in DNF or CNF) whose leaf clauses are traversed.
 * @param wff `true` when `new_fm` is a well-formed formula, `false` for Boolean function.
 * @param vars Ordered list of BDD variables used to build path vectors.
 * @param decided Output flag; set to `false` when at least one satisfiable clause is encountered.
 * @param is_cnf `true` when `new_fm` is in CNF, `false` for DNF.
 * @param all_reductions Whether to apply `reduce_paths` during collection.
 * @return Deduplicated list of path vectors.
 *
 * @par Example
 * For the DNF formula `ab|ab'` with `vars = [a, b]`, `wff = false`,
 * `is_cnf = false`, `all_reductions = true`: the two leaf clauses `ab` and
 * `ab'` convert (via `clause_to_vector`) to `{1, 1}` and `{1, -1}`; since
 * both are satisfiable, `decided` is set to `false`, and `reduce_paths`
 * merges the second into the first, leaving a single collected path
 * `{1, 2}` — i.e. the DNF reduces to just `a` (see
 * tests/unit/test_normal_forms.cpp:787-794).
 * @endinternal
 */
template <NodeType node>
std::vector<std::vector<int_t>> collect_paths(tref new_fm, bool wff,
	const auto& vars, bool& decided, bool is_cnf, bool all_reductions = true)
{
	using tau = tree<node>;
	std::vector<std::vector<int_t>> paths;
	// unordered_tau_map<int_t, BAs...> var_pos;
	subtree_map<node, int_t> var_pos;
	for (int_t k = 0; k < (int_t) vars.size(); ++k)
		var_pos.emplace(vars[k], k);
	for (tref clause : get_leaves<node>(new_fm, is_cnf
					? (wff ? tau::wff_and : tau::bf_and)
					: (wff ? tau::wff_or  : tau::bf_or))) {
		auto [i, clause_is_decided] =
			clause_to_vector<node>(clause, var_pos, wff, is_cnf);
		if (clause_is_decided) continue;
		// There is at least one satisfiable clause
		decided = false;
		if (std::ranges::all_of(i, [](const auto el) {return el == 2;}))
			return {};
		if (all_reductions) {
			if (!reduce_paths(i, paths, vars.size()))
				paths.emplace_back(std::move(i));
			else {
				std::erase_if(paths,
					[](const auto& v){return v.empty();});
				if (paths.empty()) return {};
			}
		} else paths.emplace_back(std::move(i));
	}
	return paths;
}

/**
 * @internal
 * @brief Reconstructs a reduced DNF (or CNF) formula from a set of paths and a variable list.
 *
 * Handles the `wff` / `bf` distinction and the `is_cnf` flag.
 * @tparam node Tree node type.
 * @param paths Path vectors representing the reduced formula.
 * @param vars Ordered list of BDD variables corresponding to path positions.
 * @param is_cnf `true` to build a CNF, `false` for DNF.
 * @param wff `true` when building a well-formed formula, `false` for Boolean function.
 * @param type_id BA type identifier used for Boolean function constants.
 * @return Reconstructed formula tree.
 *
 * @par Example
 * The reverse of the `collect_paths`/`reduce_paths` example: given
 * `paths = {{1, 2}}` and `vars = [a, b]` with `is_cnf = false`, `wff = false`,
 * position 0 (`1`) contributes the literal `a`, position 1 (`2`, irrelevant)
 * contributes nothing, and the single path becomes the term `a` — the
 * reduced DNF of `ab|ab'` (see @ref bf_reduced_dnf and
 * tests/unit/test_normal_forms.cpp:787-794).
 * @endinternal
 */
template <NodeType node>
tref build_reduced_formula(const auto& paths, const auto& vars, bool is_cnf,
	bool wff, size_t type_id)
{
	using tau = tree<node>;
	if (paths.empty()) return is_cnf ? (wff ? tau::_T() : tau::_1(type_id))
					 : (wff ? tau::_F() : tau::_0(type_id));
	tref reduced_fm = is_cnf ? (wff ? tau::_F() : tau::_0(type_id))
				 : (wff ? tau::_T() : tau::_1(type_id));
	bool first = true;
	for (const auto& path : paths) {
		if (path.empty()) continue;
		bool first_var = true;
		tref var_path = is_cnf  ? (wff ? tau::_F() : tau::_0(type_id))
					: (wff ? tau::_T() : tau::_1(type_id));
		DBG(assert(path.size() == vars.size());)
	for (size_t k = 0; k < vars.size(); ++k) {
		if (path[k] == 2) continue;
		if (first_var) var_path = path[k] == 1 ? vars[k]
			: wff ? tau::build_wff_neg(vars[k])
				: tau::build_bf_neg(vars[k]), first_var = false;
		else {
			if (!is_cnf) {
				if (wff) var_path = path[k] == 1
					? tau::build_wff_and(var_path, vars[k])
					: tau::build_wff_and(var_path,
						tau::build_wff_neg(vars[k]));
				else var_path = path[k] == 1
					? tau::build_bf_and(var_path, vars[k])
					: tau::build_bf_and(var_path,
						tau::build_bf_neg(vars[k]));
			}
			else {
				if (wff) var_path = path[k] == 1
					? tau::build_wff_or(var_path, vars[k])
					: tau::build_wff_or(var_path,
						tau::build_wff_neg(vars[k]));
				else var_path = path[k] == 1
					? tau::build_bf_or(var_path, vars[k])
					: tau::build_bf_or(var_path,
						tau::build_bf_neg(vars[k]));
			}
		}
	}
	if (first) reduced_fm = var_path, first = false;
	else reduced_fm = is_cnf
		? (wff  ? tau::build_wff_and(reduced_fm, var_path)
			: tau::build_bf_and( reduced_fm, var_path))
		: (wff  ? tau::build_wff_or( reduced_fm, var_path)
			: tau::build_bf_or(  reduced_fm, var_path));
	}
	DBG(assert(reduced_fm != nullptr);)
	return not_equal_to_unequal<node>(reduced_fm);
}

/**
 * @internal
 * @brief Converts a formula to either reduced DNF or CNF paths.
 *
 * Pushes negations in, identifies BDD variables, and calls `collect_paths` + `join_paths`.
 * @tparam node Tree node type.
 * @param fm Formula to reduce.
 * @param is_cnf `true` to reduce to CNF paths, `false` for DNF.
 * @return Pair of the deduplicated path vectors and the corresponding BDD variable list.
 *
 * @par Example
 * @code{.cpp}
 * // A contradiction has no satisfiable path; a tautology reduces to a
 * // single (fully irrelevant) path with no BDD variables.
 * tref contradiction = get_nso_rr("x = 0 && x != 0.").value().main->get();
 * auto [paths1, vars1] = dnf_cnf_to_reduced<node_t>(contradiction, false);
 * // paths1.size() == 0, vars1.size() == 0
 *
 * tref tautology = get_nso_rr("x = 0 || x != 0.").value().main->get();
 * auto [paths2, vars2] = dnf_cnf_to_reduced<node_t>(tautology, false);
 * // paths2.size() == 1, vars2.size() == 0
 * @endcode
 * @endinternal
 */
//TODO: decide if to treat xor in bf case
template<NodeType node>
std::pair<std::vector<std::vector<int_t>>, trefs> dnf_cnf_to_reduced(tref fm,
	bool is_cnf) {
	using tau = tree<node>;
	auto smt_replace = [](tref n) {
		if (is_child<node>(n, tau::wff_sometimes)) {
			n = tau::trim2(n); // Remove quantifier
			n = tau::build_wff_neg(n);
			n = syntactic_formula_simplification<node>(n);
			n = tau::build_wff_neg(tau::build_wff_always(n));
			return n;
		} else return n;
	};
	LOG_TRACE << "dnf_cnf_to_reduced: " << LOG_FM(fm);
	bool is_wff = !tau::get(fm).is_term();
	if (is_wff) fm = push_negation_in<node>(fm);
	else fm = push_negation_in<node, false>(fm);
	// Pull negation out of equality
	if (is_wff) {
		// Substitute all sometimes by !always! and push inner equality in
		fm = pre_order<node>(fm).apply_unique(smt_replace);
		fm = unequal_to_not_equal<node>(fm);
	} else fm = apply_all_xor_def<node>(fm); // term case
	trefs vars = is_wff ? tau::get(fm).select_top(is_wff_bdd_var<node>)
			 : tau::get(fm).select_top(is_bf_bdd_var<node>);
	LOG_TRACE << "dnf_cnf_to_reduced / vars.size(): " << vars.size();
	if (vars.empty()) {
		if (tau::get(fm).equals_T() || tau::get(fm).equals_1()) {
			if (is_cnf) return {};
			std::vector<std::vector<int_t>> paths;
			paths.emplace_back();
			return std::make_pair(std::move(paths), std::move(vars));
		} else {
			if (is_cnf) {
				std::vector<std::vector<int_t>> paths;
				paths.emplace_back();
				return std::make_pair(std::move(paths),
						      std::move(vars));
			}
			return {};
		}
	}
	bool decided = true;
	auto paths = collect_paths<node>(fm, is_wff, vars, decided, is_cnf,
					true);
	join_paths(paths);
	if (paths.empty() && !decided) paths.emplace_back();
	return std::make_pair(std::move(paths), std::move(vars));
}

// Assume that fm is in DNF (or CNF -> set is_cnf to true)
/** @internal @copydoc reduce @endinternal */
template<NodeType node, bool is_cnf>
tref reduce(tref fm) {
	using tau = tree<node>;
	bool is_wff = !tau::get(fm).is_term();
	size_t type_id = is_wff ? 0 : find_ba_type<node>(fm);
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(fm); it != end(cache)) return it->second;
#endif // TAU_CACHE
	DBG(LOG_TRACE << "Begin reduce with is_cnf set to " << is_cnf;)
	DBG(LOG_TRACE << "Formula to reduce: " << LOG_FM(fm);)
	// Terms can only contain bf_neg, bf_and, bf_xor and bf_or
	if (!is_wff) {
		if (tau::get(fm).find_top(is_non_boolean_term<node>)) {
			tref res = syntactic_path_simplification<node>(fm);
			// Cache this branch too, like every other exit: bv and
			// tau-constant terms would otherwise be re-simplified on
			// every call.
#ifdef TAU_CACHE
			return cache.emplace(fm, res).first->second;
#endif // TAU_CACHE
			return res;
		}
	}
	auto [paths, vars] = dnf_cnf_to_reduced<node>(fm, is_cnf);
	if (paths.empty()) {
		auto res = is_cnf ? (is_wff ? tau::_T() : tau::_1(type_id))
				  : (is_wff ? tau::_F() : tau::_0(type_id));
#ifdef TAU_CACHE
		return cache.emplace(fm, res).first->second;
#endif // TAU_CACHE
		return res;
	}
	if (paths.size() == 1 && paths[0].empty()) {
		auto res = is_cnf ? (is_wff ? tau::_F() : tau::_0(type_id))
				  : (is_wff ? tau::_T() : tau::_1(type_id));
#ifdef TAU_CACHE
		return cache.emplace(fm, res).first->second;
#endif // TAU_CACHE
		return res;
	}
	auto reduced_fm = build_reduced_formula<node>(paths, vars, is_cnf, is_wff, type_id);
	if (is_wff) reduced_fm = push_negation_in<node>(reduced_fm);
	else reduced_fm = push_negation_in<node, false>(reduced_fm);
	DBG(LOG_TRACE << "End reduce";)
	DBG(LOG_TRACE << "Reduced formula: " << LOG_FM(reduced_fm);)
#ifdef TAU_CACHE
	return cache.emplace(fm, reduced_fm).first->second;
#endif // TAU_CACHE
	return reduced_fm;
}

/**
 * @internal
 * @brief Returns `true` if the sorted sets `v1` and `v2` share at least `i` elements.
 * @tparam node Tree node type.
 * @param i Minimum number of shared elements required.
 * @param v1 First sorted sequence.
 * @param v2 Second sorted sequence.
 * @return `true` if the overlap between `v1` and `v2` is at least `i`.
 * @endinternal
 */
template<NodeType node>
bool is_ordered_overlap_at_least(size_t i, const trefs& v1, const trefs& v2) {
	using tau = tree<node>;
	if (v1.size() < i) return false;
	if (v2.size() < i) return false;
	size_t j = 0, k = 0;
	while (j < v1.size() && k < v2.size()) {
		if (i == 0) return true;
		// Check match
		if (tau::get(v1[j]) == tau::get(v2[k])) --i, ++j, ++k;
		else if (tau::subtree_less(v1[j], v2[k])) ++j;
		else ++k;
	}
	return i == 0;
}

/**
 * @internal
 * @brief Returns the number of shared elements between sorted sets `v1` and `v2`.
 * @tparam node Tree node type.
 * @param v1 First sorted sequence.
 * @param v2 Second sorted sequence.
 * @return Count of elements appearing in both `v1` and `v2`.
 * @endinternal
 */
template<NodeType node>
int_t get_ordered_overlap(const trefs& v1, const trefs& v2) {
	using tau = tree<node>;
	int_t i = 0;
	size_t j = 0, k = 0;
	while (j < v1.size() && k < v2.size()) {
		// Check match
		if (tau::get(v1[j]) == tau::get(v2[k])) ++i, ++j, ++k;
		else if (tau::subtree_less(v1[j], v2[k])) ++j;
		else ++k;
	}
	return i;
}

/** @internal @copydoc wff_reduce_dnf @endinternal */
template <NodeType node>
tref wff_reduce_dnf<node>::operator() (tref fm) const {
	return reduce<node>(fm);
}

/** @internal @copydoc wff_reduce_cnf @endinternal */
template <NodeType node>
tref wff_reduce_cnf<node>::operator() (tref fm) const {
	return reduce<node, true>(fm);
}

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& fm,
	const wff_reduce_dnf<node>& r)
{
	return typename tree<node>::traverser(r(fm.value()));
}

template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& fm,
	const wff_reduce_cnf<node>& r)
{
	return typename tree<node>::traverser(r(fm.value()));
}

/**
 * @internal
 * @brief Distributes a conjunction of two formulas in DNF, producing their product DNF (cross-product of disjuncts).
 *
 * Handles both wff and bf cases.
 * @tparam node Tree node type.
 * @param d1 First DNF formula.
 * @param d2 Second DNF formula.
 * @return Product DNF of `d1` and `d2`.
 *
 * @par Example
 * @code{.cpp}
 * // Each side has a single disjunct here, so the product is their conjunction
 * tref d1 = get_nso_rr("x = 0.").value().main->get();
 * tref d2 = get_nso_rr("y = 0.").value().main->get();
 * tref res = conjunct_dnfs_to_dnf<node_t>(d1, d2);
 * // tau::get(res).to_str() == "x = 0 && y = 0"
 * @endcode
 * @endinternal
 */
template <NodeType node>
tref conjunct_dnfs_to_dnf(tref d1, tref d2) {
	using tau = tree<node>;
	const auto& t = tau::get(d1);
	if (t.is(tau::wff)) {
		DBG(assert(tau::get(d2).is(tau::wff));)
		tref res = tau::_F();
		auto clauses_d1 = get_dnf_wff_clauses<node>(d1);
		auto clauses_d2 = get_dnf_wff_clauses<node>(d2);
		for (tref dis1 : clauses_d1)
			for (tref dis2 : clauses_d2)
				res = tau::build_wff_or(res,
					tau::build_wff_and(dis1, dis2));
		return res;
	} else {
		DBG(assert(t.is(tau::bf) && tau::get(d2).is(tau::bf));)
		tref res = tau::_0(find_ba_type<node>(d1));
		auto clauses_d1 = get_dnf_bf_clauses<node>(d1);
		auto clauses_d2 = get_dnf_bf_clauses<node>(d2);
		for (tref dis1 : clauses_d1)
			for (tref dis2 : clauses_d2)
				res = tau::build_bf_or(res,
					tau::build_bf_and(dis1, dis2));
		return res;
	}
}

/**
 * @internal
 * @brief Distributes a disjunction of two formulas in CNF, producing their product CNF.
 *
 * Handles both wff and bf cases.
 * @tparam node Tree node type.
 * @param c1 First CNF formula.
 * @param c2 Second CNF formula.
 * @return Product CNF of `c1` and `c2`.
 *
 * @par Example
 * @code{.cpp}
 * // Each side has a single conjunct here, so the product is their disjunction
 * tref c1 = get_nso_rr("x = 0.").value().main->get();
 * tref c2 = get_nso_rr("y = 0.").value().main->get();
 * tref res = disjunct_cnfs_to_cnf<node_t>(c1, c2);
 * // tau::get(res).to_str() == "x = 0 || y = 0"
 * @endcode
 * @endinternal
 */
template <NodeType node>
tref disjunct_cnfs_to_cnf(tref c1, tref c2) {
	using tau = tree<node>;
	const auto& t = tau::get(c1);
	if (t.is(tau::wff)) {
		DBG(assert(tau::get(c2).is(tau::wff));)
		tref res = tau::_T();
		auto clauses_c1 = get_cnf_wff_clauses<node>(c1);
		auto clauses_c2 = get_cnf_wff_clauses<node>(c2);
		for (tref dis1 : clauses_c1)
			for (tref dis2 : clauses_c2)
				res = tau::build_wff_and(res,
					tau::build_wff_or(dis1, dis2));
		return res;
	} else {
		DBG(assert(t.is(tau::bf) && tau::get(c2).is(tau::bf));)
		tref res = tau::_1(find_ba_type<node>(c1));
		auto clauses_c1 = get_cnf_bf_clauses<node>(c1);
		auto clauses_c2 = get_cnf_bf_clauses<node>(c2);
		for (tref dis1 : clauses_c1)
			for (tref dis2 : clauses_c2)
				res = tau::build_bf_and(res,
					tau::build_bf_or(dis1, dis2));
		return res;
	}
}

// Conversion to dnf while applying reductions during the process
/** @internal @copydoc to_dnf @endinternal */
template <NodeType node, bool is_wff>
tref to_dnf(tref fm) {
	using tau = tree<node>;
	using tt = tau::traverser;
	LOG_TRACE << "to_dnf: " << LOG_FM(fm);
	auto layer_to_dnf = [](tref n) {
		const auto& t = tau::get(n);
		if constexpr (is_wff) if (t.is(tau::wff)) {
			if (t.child_is(tau::wff_and)) {
				auto conj = conjunct_dnfs_to_dnf<node>(
					t[0].first(), t[0].second());
				// Perform simplification
				if (tau::get(conj) != tau::get(n)) return tt(conj)
					| wff_reduce_dnf<node>() | tt::ref;
				else return n;
			}
		}
		if constexpr (!is_wff) if (t.is(tau::bf)) {
			if (t.child_is(tau::bf_and)) {
				auto conj = conjunct_dnfs_to_dnf<node>(
					t[0].first(), t[0].second());
				// Perform simplification. Structural comparison, as in the
				// wff branch above (see to_cnf for the rationale).
				if (tau::get(conj) != tau::get(n))
					return reduce<node>(conj);
				else return n;
			}
		}
		return n;
	};
	auto pn = [](const auto& n) {
		return push_negation_one_in<node, is_wff>(n);
	};
	tref r;
	if constexpr (is_wff) r = pre_order<node>(fm)
		.template apply_unique<MemorySlotPre::to_dnf_m>(
					pn, while_is_formula<node>, layer_to_dnf);
	else r = pre_order<node>(fm)
		.template apply_unique<MemorySlotPre::to_dnf_m>(
					pn, all, layer_to_dnf);
	LOG_TRACE << "to_dnf result: " << LOG_FM(r);
	return r;
}

/**
 * @internal
 * @brief Converts the top-level temporal layer of `fm` to DNF by distributing `wff_and` over `wff_or`, stopping at temporal quantifiers.
 * @tparam node Tree node type.
 * @param fm Formula whose temporal layer is to be converted to DNF.
 * @return Formula with the temporal layer in DNF.
 * @endinternal
 */
// Conversion of temporal layer to dnf
template <NodeType node>
tref temporal_layer_to_dnf(tref fm) {
	using tau = tree<node>;
	auto layer_to_dnf = [](tref n) {
		const auto& t = tau::get(n);
		if (t.child_is(tau::wff_and)) {
			auto conj = conjunct_dnfs_to_dnf<node>(
				t[0].first(), t[0].second());
			// Perform simplification
			if (tau::get(conj) != tau::get(n))
				return reduce<node>(conj);
			else return n;
		}
		return n;
	};
	auto pn = [](tref n) {
		return push_negation_one_in<node>(n);
	};
	auto visit = [](tref n){
		if (is_temporal_quantifier<node>(n)) return false;
		return true;
	};
	return pre_order<node>(fm).apply_unique(pn, visit, layer_to_dnf);
}

// Conversion to cnf while applying reductions during the process
/** @internal @copydoc to_cnf @endinternal */
template <NodeType node, bool is_wff>
tref to_cnf(tref fm) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto layer_to_cnf = [](tref n) {
		const auto& t = tau::get(n);
		if constexpr (is_wff) if (t.is(tau::wff)) {
			if (t.child_is(tau::wff_or)) {
				auto dis = disjunct_cnfs_to_cnf<node>(
					t[0].first(), t[0].second());
				// Perform simplification
				if (tau::get(dis) != tau::get(n)) return tt(dis)
					| wff_reduce_cnf<node>() | tt::ref;
				else return n;
			}
		}
		if constexpr (!is_wff) if (t.is(tau::bf) && t.child_is(tau::bf_or)) {
				auto dis = disjunct_cnfs_to_cnf<node>(
						t[0].first(), t[0].second());
				// Perform simplification. Compare structurally, as the wff
				// branch above does: `dis` is a freshly built root with no
				// right sibling while `n` generally has one, so a raw tref
				// comparison reports "changed" even for a no-op
				// distribution and pays a full reduce for nothing.
				if (tau::get(dis) != tau::get(n))
					return reduce<node, true>(dis);
				else return n;
			}
		return n;
	};
	auto pn = [](tref n) {
		return push_negation_one_in<node, is_wff>(n);
	};
	if constexpr (is_wff) return pre_order<node>(fm)
		.template apply_unique<MemorySlotPre::to_cnf_m>(
					pn, while_is_formula<node>, layer_to_cnf);
	else return pre_order<node>(fm)
		.template apply_unique<MemorySlotPre::to_cnf_m>(
					pn, all, layer_to_cnf);
}

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "anti_prenex"

/**
 * @internal
 * @brief The procedure tries to detect, using 0/1 substitutions for the provided
 * variable and syntactic comparison, if the atomic formula is equivalent to T
 * or F or independent of the provided variable.
 * @tparam node Type of tree node
 * @param atomic_fm The atomic formula, ie an equation, to simplify
 * @param var The variable to base the simplifications on
 * @return The simplified atomic formula
 * @endinternal
 */
template<NodeType node>
tref syntactic_variable_simplification(tref atomic_fm, tref var) {
	using tau = tree<node>;

	DBG(assert(tau::get(var).is(tau::variable));)

#ifdef TAU_CACHE
	using cache_t = std::unordered_map<std::pair<tref, tref>, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(std::make_pair(tau::trim_right_sibling(atomic_fm),
		tau::trim_right_sibling(var))); it != end(cache))
		return it->second;
#endif // TAU_CACHE

	// Return early if atomic_fm is either T or F
	if (tau::get(atomic_fm).equals_T() || tau::get(atomic_fm).equals_F()) return atomic_fm;

	DBG(LOG_TRACE << "Syntactic_variable_simplification on " << LOG_FM(atomic_fm) << "\n";)
	DBG(LOG_TRACE << "with var: " << LOG_FM(var) << "\n";)
	var = tau::get(tau::bf, var);
	atomic_fm = gt_gteq_to_lt_lteq<node>(atomic_fm);
	atomic_fm = norm_equation<node>(atomic_fm);
	auto atm_type = tau::get(atomic_fm)[0].value.nt;
	tref func1 = tau::get(atomic_fm)[0].first();
	tref func2 = tau::get(atomic_fm)[0].second();
	// Make sure that it works only on Boolean parts by using replace_if
	tref func1_v_0 = rewriter::replace_if<node>(func1, var,
		_0<node>(find_ba_type<node>(var)), while_is_boolean_operation<node>);
	tref func1_v_1 = rewriter::replace_if<node>(func1, var,
		_1<node>(find_ba_type<node>(var)), while_is_boolean_operation<node>);
	// Is func syntactically identically 0
	if (tau::get(func1_v_0).equals_0() && tau::get(func1_v_1).equals_0())
		func1 = tau::_0(find_ba_type<node>(func1));
	// Is func syntactically identically 1
	else if (tau::get(func1_v_0).equals_1() && tau::get(func1_v_1).equals_1())
		func1 = tau::_1(find_ba_type<node>(func1));
	// func is not dependent on var
	else if (tau::get(func1_v_0) == tau::get(func1_v_1) && !contains<node>(func1_v_0, var))
		func1 = rewriter::replace<node>(func1, var,
			_0<node>(find_ba_type<node>(var)));
	if (tau::get(func2).equals_0())
		return denorm_equation<node>(
			tau::get(tau::wff, tau::get(atm_type, func1, func2)));
	// Simplify func2
	tref func2_v_0 = rewriter::replace_if<node>(func2, var,
		_0<node>(find_ba_type<node>(var)), while_is_boolean_operation<node>);
	tref func2_v_1 = rewriter::replace_if<node>(func2, var,
		_1<node>(find_ba_type<node>(var)), while_is_boolean_operation<node>);
	// Is func syntactically identically 0
	if (tau::get(func2_v_0).equals_0() && tau::get(func2_v_1).equals_0())
		func2 = tau::_0(find_ba_type<node>(func2));
	// Is func syntactically identically 1
	else if (tau::get(func2_v_0).equals_1() && tau::get(func2_v_1).equals_1())
		func2 = tau::_1(find_ba_type<node>(func2));
	// func is not dependent on var
	else if (tau::get(func2_v_0) == tau::get(func2_v_1) && !contains<node>(func2_v_0, var))
		func2 = rewriter::replace<node>(func2, var,
			_0<node>(find_ba_type<node>(var)));
	tref res = tau::get(tau::wff, tau::get(atm_type, func1, func2));
	DBG(LOG_TRACE << "Syntactic_variable_simplification result: " << LOG_FM(res) << "\n";)
#ifdef TAU_CACHE
	cache.emplace(std::make_pair(tau::trim_right_sibling(res),
		tau::trim_right_sibling(var)), res);
	return cache.emplace(std::make_pair(tau::trim_right_sibling(atomic_fm),
		tau::trim_right_sibling(var)), res).first->second;
#endif // TAU_CACHE
	return res;
}

/**
 * @internal
 * @brief Comparator for the BDD variable order used during simplification of
 * terms. It is used with std::stable_sort in order to preserve initial order
 * in the found BDD variables.
 * @tparam node Type of tree node
 * @endinternal
 */
template<NodeType node>
auto variable_order_for_simplification = [](tref l, tref r) static {
	// Order is: smaller time point first, if equal
	// constant input < constant output < input < output < other variable
	// while all variables are ordered by var_order
	using tau = tree<node>;
	DBG(assert(tau::get(l).is(tau::variable));)
	DBG(assert(tau::get(r).is(tau::variable));)
	// Reject equal
	if (tau::get(l) == tau::get(r)) return false;
	if (is_io_var<node>(l)) {
		// Check if r is also stream
		if (is_io_var<node>(r)) {
			if (is_io_initial<node>(l)) {
				if (is_io_initial<node>(r)) {
					// Both are initial
					const int_t tl = get_io_time_point<node>(l),
						tr = get_io_time_point<node>(r);
					if (tl < tr) {
						// l is earlier
						return true;
					} else if (tl == tr) {
						// Check input < output
						if (tau::get(l).is_input_variable()) {
							if (tau::get(r).is_input_variable()) {
								// Both input -> compare equal
								return false;
							} else return true;
						} if (tau::get(r).is_input_variable()) {
							// Same time step, r is input and l is not
							return false;
						} else return false;
					} else return false;
				} else return true;
			} else if (is_io_initial<node>(r)) {
				// r initial but l not
				return false;
			} else {
				// Both are non initial
				const int_t sl = get_io_var_shift<node>(l),
					sr = get_io_var_shift<node>(r);
				if (sl > sr) {
					// l is earlier
					return true;
				} else if (sl == sr) {
					// Check input < output
					if (tau::get(l).is_input_variable()) {
						if (tau::get(r).is_input_variable()) {
							// Both input -> compare equal
							return false;
						} else return true;
					} if (tau::get(r).is_input_variable()) {
						// Same time step, r is input and l is not
						return false;
					} else return false;
				} else return false;
			}
		} else return false; // compare equal
	} else return false; // compare equal
};

/**
 * @internal
 * @brief Comparator for the BDD variable order used during simplification of a
 * formula. It is used with std::stable_sort in order to preserve initial order
 * in the found BDD variables.
 * @tparam node Type of tree node
 * @endinternal
 */
template<NodeType node>
auto atm_formula_order_for_simplification = [](tref l, tref r) static {
	// Order is decided by three factors
	// 1) lowest time points in free variables have priority, then
	// 2) lowest highest time points in free variables and last
	// 3) number of free io variables
	DBG(using tau = tree<node>;)
	DBG(assert(tau::get(l).child_is(tau::bf_eq));)
	DBG(assert(tau::get(r).child_is(tau::bf_eq));)
	// For l
	std::pair<bool, int_t> low_t_l {true, 0}, high_t_l {true, 0};
	bool is_high_init = false;
	const trefs& free_vars_l = get_free_vars<node>(l);
	size_t io_free_var_size_l = 0;
	for (tref v : free_vars_l) {
		if (!is_io_var<node>(v)) continue;
		++io_free_var_size_l;
		if (is_io_initial<node>(v)) {
			const int_t t = get_io_time_point<node>(v);
			if (low_t_l.second >= t) {
				low_t_l.first = false;
				low_t_l.second = t;
			}
			if (high_t_l.second <= t || !is_high_init) {
				high_t_l.first = false;
				high_t_l.second = t;
				is_high_init = true;
			}
		} else {
			if (low_t_l.first)
				if (const int_t t = get_io_var_shift<node>(v);
					low_t_l.second < t)
					low_t_l.second = t;
			 if (high_t_l.first)
				if (const int_t t = get_io_var_shift<node>(v);
					high_t_l.second > t || !is_high_init) {
					high_t_l.second = t;
					is_high_init = true;
				}
		}
	}
	// For r
	std::pair<bool, int_t> low_t_r {true, 0}, high_t_r {true, 0};
	is_high_init = false;
	const trefs& free_vars_r = get_free_vars<node>(r);
	size_t io_free_var_size_r = 0;
	for (tref v : free_vars_r) {
		if (!is_io_var<node>(v)) continue;
		++io_free_var_size_r;
		if (is_io_initial<node>(v)) {
			const int_t t = get_io_time_point<node>(v);
			if (low_t_r.second >= t) {
				low_t_r.first = false;
				low_t_r.second = t;
			}
			if (high_t_r.second <= t || !is_high_init) {
				high_t_r.first = false;
				high_t_r.second = t;
				is_high_init = true;
			}
		} else {
			if (low_t_r.first)
				if (const int_t t = get_io_var_shift<node>(v);
					low_t_r.second < t)
					low_t_r.second = t;
			if (high_t_r.first)
				if (const int_t t = get_io_var_shift<node>(v);
					high_t_r.second > t || !is_high_init) {
					high_t_r.second = t;
					is_high_init = true;
				}
		}
	}
	// Check that both have io variables
	if (io_free_var_size_l == 0)
		return false;
	if (io_free_var_size_r == 0) {
		return true;
	}
	// l and r both have at least one io variable
	if (low_t_l.first < low_t_r.first) return true;
	if (low_t_r.first < low_t_l.first) return false;
	// Here low_t_l.first == low_t_r.first
	if (low_t_l.first) {
		// No initial condition, so higher lookback wins
		if (low_t_l.second > low_t_r.second) return true;
		if (low_t_r.second > low_t_l.second) return false;
		// Here lookback is same, compare highest time point
		// We also know that no initial condition is present
		if (high_t_l.second > high_t_r.second) return true;
		if (high_t_r.second > high_t_l.second) return false;
		// Here lookback is also same
		return io_free_var_size_l < io_free_var_size_r;
	} else {
		// Compare initial conditions
		if (low_t_l.second < low_t_r.second) return true;
		if (low_t_r.second < low_t_l.second) return false;
		// Smallest initials are same, compare highest, also initial conditions by assumption
		if (high_t_l.second < high_t_r.second) return true;
		if (high_t_r.second < high_t_l.second) return false;
		// Here highest initials are same
		return io_free_var_size_l < io_free_var_size_r;
	}
};

/**
 * @internal
 * @brief Comparator for the BDD variable order used during anti-prenex algorithm.
 * @tparam node Type of tree node
 * @endinternal
 */
template<NodeType node>
auto atm_formula_order_for_quant_elim(auto& quant_pattern) {
	auto comp = [&quant_pattern](tref l, tref r) {
		// Assignments to variables
		const bool is_assignment_l = is_equational_assignment<node>(l);
		const bool is_assignment_r = is_equational_assignment<node>(r);

		const trefs& free_vars_l = get_free_vars<node>(l);
		int_t min_l = 0, max_l = 0;
		bool is_min_init = false;
		for (tref v : free_vars_l) {
			auto it = quant_pattern.find(v);
			// Not contained, means not quantified
			if (it == quant_pattern.end()) {
				min_l = 0;
				is_min_init = true;
				continue;
			}
			if (it->second < min_l || !is_min_init)
				min_l = it->second, is_min_init = true;
			if (max_l < it->second) max_l = it->second;
		}
		const trefs& free_vars_r = get_free_vars<node>(r);
		int_t min_r = 0, max_r = 0;
		is_min_init = false;
		for (tref v : free_vars_r) {
			auto it = quant_pattern.find(v);
			// Not contained, means not quantified
			if (it == quant_pattern.end()) {
				min_r = 0;
				is_min_init = true;
				continue;
			}
			if (it->second < min_r || !is_min_init)
				min_r = it->second, is_min_init = true;
			if (max_r < it->second) max_r = it->second;
		}
		if (max_l > max_r) return true;
		if (max_r > max_l) return false;
		// Order assignments to front
		if (is_assignment_l && !is_assignment_r) return true;
		if (!is_assignment_l && is_assignment_r) return false;
		if (min_l > min_r) return true;
		if (min_r > min_l) return false;
		// Deterministic tie-breaks: structural comparison instead of
		// raw pointer order (both vectors are subtree_less-sorted)
		if (std::lexicographical_compare(
			free_vars_l.begin(), free_vars_l.end(),
			free_vars_r.begin(), free_vars_r.end(),
			tree<node>::subtree_less)) return true;
		if (std::lexicographical_compare(
			free_vars_r.begin(), free_vars_r.end(),
			free_vars_l.begin(), free_vars_l.end(),
			tree<node>::subtree_less)) return false;
		return tree<node>::subtree_less(l, r);
	};
	return comp;
}

/**
 * @internal
 * @brief Applies syntactic simplifications to an atomic formula, ie an equation.
 * @tparam node Tree node type
 * @param atomic_formula Formula to simplify
 * @return Simplified formula
 * @endinternal
 */
template<NodeType node>
tref syntactic_atomic_formula_simplification(tref atomic_formula) {
	using tau = tree<node>;
	DBG(LOG_TRACE << "Start syntactic_atomic_formula_simplification: "
		<< tau::get(atomic_formula) << "\n";)
	auto atm_type = tau::get(atomic_formula)[0].value.nt;
	// Bring an equation to !(=) 0
	atomic_formula = norm_equation<node>(atomic_formula);
	if (tau::get(atomic_formula).equals_T() ||
		tau::get(atomic_formula).equals_F()) return atomic_formula;
	tref func1 = syntactic_path_simplification<node>(
		tau::get(atomic_formula)[0].first());
	tref func2 = syntactic_path_simplification<node>(
		tau::get(atomic_formula)[0].second());
	// Apply syntactic path simplification
	atomic_formula = tau::get(tau::wff,
		tau::get(atm_type, func1, func2));
	// Bring an equation back to its original form
	atomic_formula = denorm_equation<node>(atomic_formula);
	// Apply syntactic variable simplification for each found free variable
	auto& free_vars = get_free_vars<node>(atomic_formula);
	for (tref v : free_vars) {
		atomic_formula =
			syntactic_variable_simplification<node>(atomic_formula, v);
	}
	DBG(LOG_TRACE << "End syntactic_atomic_formula_simplification: "
		<< tau::get(atomic_formula) << "\n";)
	return atomic_formula;
}

/**
 * @internal
 * @brief Merges two equations `eq1` and `eq2` (both of the form `f (!)= 0`) into a single equation `f1|f2 (!)= 0`.
 *
 * Both inputs must have the same sign.
 * @tparam node Tree node type.
 * @param eq1 First equation (must be `f1 = 0` or `f1 != 0`).
 * @param eq2 Second equation (must be `f2 = 0` or `f2 != 0`, same sign as `eq1`).
 * @return Merged equation `f1|f2 = 0` or `f1|f2 != 0`.
 * @endinternal
 */
// Squeeze two equations equal/unequal to zero into one equation
template <NodeType node>
tref squeeze(tref eq1, tref eq2) {
	using tau = tree<node>;
	// Both equations must be !(=) 0
	DBG(assert(tau::get(eq1)[0][1].equals_0() &&
		tau::get(eq2)[0][1].equals_0());)
	if (tau::get(eq1).child_is(tau::bf_eq)) {
		DBG(assert(tau::get(eq2).child_is(tau::bf_eq));)
		// Squeeze positive equations
		return tau::build_bf_eq_0(
			tau::build_bf_or(tau::trim2(eq1), tau::trim2(eq2)));
	} else if (tau::get(eq1).child_is(tau::bf_neq)) {
		DBG(assert(tau::get(eq2).child_is(tau::bf_neq));)
		// Squeeze negative equations
		return tau::build_bf_neq_0(
		tau::build_bf_or(tau::trim2(eq1), tau::trim2(eq2)));
	}
	DBG(assert(false);)
	return nullptr;
}

/**
 * @internal
 * @brief Applies collected assumptions to an equation `eq` (stack overload with join tracking).
 *
 * Iterates over the top of the assumption stack, squeezes or absorbs matching assumptions into `eq`,
 * and records joins via the union-find `joins` and new assumptions in `additions`.
 * @tparam node Tree node type.
 * @param eq Equation to apply assumptions to; must be in `f (!)= 0` form.
 * @param assms Stack of assumption lists; only the back (top) is used.
 * @param joins Union-find structure tracking which assumptions were merged.
 * @param additions Output list of new assumptions produced during application.
 * @param dual `true` to apply dual semantics (swap squeeze and absorb roles).
 * @return Updated equation.
 * @endinternal
 */
template <NodeType node>
tref apply_assms(tref eq, const auto& assms, auto& joins, trefs& additions, bool dual = false) {
	using tau = tree<node>;
	DBG(assert(tau::get(eq)[0][1].equals_0());)
	const trefs& fv = get_free_vars<node>(eq);
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	bool joined = false;
	for (tref assm : assms.back()) {
		// Check for overlap between current assumption and eq
		const trefs& fv_a = get_free_vars<node>(assm);
		if (const int_t count = get_ordered_overlap<node>(fv, fv_a); count > 1 ||
			(count == 1 && fv.size() == 1)) {
			// Apply assumption
			if (is_eq_pos) {
				if (dual) {
					eq = tau::build_bf_eq_0(tau::build_bf_and(
						tau::trim2(eq), tau::build_bf_neg(assm)));
				} else {
					DBG(LOG_TRACE << "Merging " << tau::get(assm) << " and " << tau::get(tau::trim2(eq)) << "\n";)
					joins.merge(assm, tau::trim2(eq));
					joined = true;
					eq = tau::build_bf_eq_0(tau::build_bf_or(
						tau::trim2(eq), assm));
				}
			} else {
				if (dual) {
					DBG(LOG_TRACE << "Merging " << tau::get(assm) << " and " << tau::get(tau::trim2(eq)) << "\n";)
					joins.merge(assm, tau::trim2(eq));
					joined = true;
					eq = tau::build_bf_neq_0(tau::build_bf_or(
						tau::trim2(eq), assm));
				}
				eq = tau::build_bf_neq_0(tau::build_bf_and(
					tau::trim2(eq), tau::build_bf_neg(assm)));
			}
		} else if (count == 1 && is_eq_pos) {
			DBG(LOG_TRACE << "Merging " << tau::get(assm) << " and " << tau::get(tau::trim2(eq)) << "\n";)
			joins.merge(assm, tau::trim2(eq));
			joined = true;
		}
	}
	if (!joined) if ((is_eq_pos && !dual) || (!is_eq_pos && dual)) {
		DBG(LOG_TRACE << "Addition: " << tau::get(tau::trim2(eq)) << "\n";)
		additions.emplace_back(tau::trim2(eq));
	}
	return eq;
}

/**
 * @internal
 * @brief Applies collected assumptions to an equation `eq` (stack overload).
 *
 * Iterates over the top of the assumption stack, squeezes or absorbs matching assumptions into `eq`.
 * Unlike the join-tracking overload, no union-find or additions list is maintained.
 * @tparam node Tree node type.
 * @param eq Equation to apply assumptions to; must be in `f (!)= 0` form.
 * @param assms Stack of assumption lists; only the back (top) is used.
 * @param dual `true` to apply dual semantics (swap squeeze and absorb roles).
 * @return Updated equation.
 * @endinternal
 */
template <NodeType node>
tref apply_assms(tref eq, const auto& assms, bool dual = false) {
	using tau = tree<node>;
	DBG(assert(tau::get(eq)[0][1].equals_0());)
	const trefs& fv = get_free_vars<node>(eq);
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	for (tref assm : assms.back()) {
		// Check for overlap between current assumption and eq
		const trefs& fv_a = get_free_vars<node>(assm);
		if (const int_t count = get_ordered_overlap<node>(fv, fv_a); count > 1 ||
			(count == 1 && fv.size() == 1)) {
			// Apply assumption
			if (is_eq_pos) {
				if (dual) eq = tau::build_bf_eq_0(tau::build_bf_and(
					tau::trim2(eq), tau::build_bf_neg(assm)));
				else eq = tau::build_bf_eq_0(tau::build_bf_or(
					tau::trim2(eq), assm));
				DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
			} else {
				if (dual) eq = tau::build_bf_neq_0(tau::build_bf_or(
					tau::trim2(eq), assm));
				else eq = tau::build_bf_neq_0(tau::build_bf_and(
					tau::trim2(eq), tau::build_bf_neg(assm)));
				DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
			}
		}
	}
	return eq;
}

/**
 * @internal
 * @brief Applies collected assumptions to an equation `eq` (single-assumption overload with variable guard).
 *
 * Only applies `assm` when the equation contains `var`; uses overlap count to decide whether to squeeze or absorb.
 * @tparam node Tree node type.
 * @param eq Equation to apply assumptions to; must be in `f (!)= 0` form.
 * @param assm Single assumption to apply.
 * @param var Variable that must be present in `eq` for the assumption to be applied.
 * @param dual `true` to apply dual semantics (swap squeeze and absorb roles).
 * @return Updated equation.
 * @endinternal
 */
template <NodeType node>
tref apply_assms(tref eq, tref assm, tref var, const bool dual = false) {
	using tau = tree<node>;
	DBG(assert(tau::get(eq)[0][1].equals_0());)
	const trefs& fv = get_free_vars<node>(eq);
	if (!std::binary_search(fv.begin(), fv.end(), var, tau::subtree_less))
		return eq;
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	// Check for overlap between current assumption and eq
	const trefs& fv_a = get_free_vars<node>(assm);
	if (fv.size() == 1 || is_ordered_overlap_at_least<node>(2,fv, fv_a)) {
		// Apply assumption
		if (is_eq_pos) {
			if (dual) eq = tau::build_bf_eq_0(tau::build_bf_and(
				tau::trim2(eq), tau::build_bf_neg(assm)));
			else eq = tau::build_bf_eq_0(tau::build_bf_or(
				tau::trim2(eq), assm));
			DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
		} else {
			if (dual) eq = tau::build_bf_neq_0(tau::build_bf_or(
				tau::trim2(eq), assm));
			else eq = tau::build_bf_neq_0(tau::build_bf_and(
				tau::trim2(eq), tau::build_bf_neg(assm)));
			DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
		}
	}
	return eq;
}

/**
 * @internal
 * @brief Applies collected assumptions to an equation `eq` (single-assumption overload with updates output).
 *
 * Only applies `assm` when the equation contains `var`; records the equation in `updates` before applying, then squeezes or absorbs as appropriate.
 * @tparam node Tree node type.
 * @param eq Equation to apply assumptions to; must be in `f (!)= 0` form.
 * @param assm Single assumption to apply.
 * @param var Variable that must be present in `eq` for the assumption to be applied.
 * @param updates Output list to which `eq` is appended before application.
 * @param dual `true` to apply dual semantics (swap squeeze and absorb roles).
 * @return Updated equation.
 * @endinternal
 */
template <NodeType node>
tref apply_assms(tref eq, tref assm, tref var, trefs& updates, const bool dual = false) {
	using tau = tree<node>;
	DBG(assert(tau::get(eq)[0][1].equals_0());)
	const trefs& fv = get_free_vars<node>(eq);
	if (!std::binary_search(fv.begin(), fv.end(), var, tau::subtree_less))
		return eq;
	DBG(assert(tau::get(eq).child_is(tau::bf_eq) ||
		tau::get(eq).child_is(tau::bf_neq));)
	const bool is_eq_pos = tau::get(eq).child_is(tau::bf_eq);
	if ((is_eq_pos && !dual) || (!is_eq_pos && dual))
		updates.emplace_back(eq);
	// Check for overlap between current assumption and eq
	const trefs& fv_a = get_free_vars<node>(assm);
	if (fv.size() == 1 || is_ordered_overlap_at_least<node>(2,fv, fv_a)) {
		// Apply assumption
		if (is_eq_pos) {
			if (dual) eq = tau::build_bf_eq_0(tau::build_bf_and(
				tau::trim2(eq), tau::build_bf_neg(assm)));
			else eq = tau::build_bf_eq_0(tau::build_bf_or(
				tau::trim2(eq), assm));
			DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
		} else {
			if (dual) eq = tau::build_bf_neq_0(tau::build_bf_or(
				tau::trim2(eq), assm));
			else eq = tau::build_bf_neq_0(tau::build_bf_and(
				tau::trim2(eq), tau::build_bf_neg(assm)));
			DBG(LOG_TRACE << "From assumption " << tau::get(assm) << " produce " << tau::get(eq) << "\n";)
		}
	}
	return eq;
}

/**
 * @internal
 * @brief Merges joined assumption groups (tracked via a union-find `joins`) and appends new `additions` into the top of the assumption stack.
 * @tparam node Tree node type.
 * @param assms Assumption stack whose back (top) is updated in place.
 * @param joins Union-find structure whose sets drive the merging of existing assumptions.
 * @param additions New assumption terms to append to the top of the stack.
 * @endinternal
 */
template <NodeType node>
void update_assms(auto& assms, auto& joins, trefs& additions) {
	using tau = tree<node>;
	subtree_set<node> excluded;
	trefs& A = assms.back();
	for (size_t i = 0; i < A.size(); ++i) {
		if (excluded.contains(A[i])) {
			A.erase(A.begin()+i);
			DBG(assert(i > 0);)
			--i;
			continue;
		}
		// Get the set of joinable assumptions from joins
		trefs col = joins.get_set(A[i]);
		for (tref c : col) excluded.insert(c);
		DBG(LOG_TRACE << "Updated assumption from " << tau::get(A[i])
			<< " to " << tau::get(tau::build_bf_or(col,
				find_ba_type<node>(A[i]))) << "\n";)
		A[i] = tau::build_bf_or(col, find_ba_type<node>(A[i]));
	}
	for (tref add : additions) {
		DBG(LOG_TRACE << "Added to assumption: " << tau::get(add) << "\n";)
		A.emplace_back(add);
	}
}

/**
 * @internal
 * @brief Squeezes pairs of equations in `eqs[0..primary_end)` that are "connected" according to the predicate, merging them into single equations and updating the end indices.
 * @tparam node Tree node type.
 * @param eqs Vector of equations; modified in place as pairs are merged.
 * @param primary_end Index past the last primary equation; decremented as equations are merged.
 * @param total_end Index past all equations; decremented alongside `primary_end`.
 * @param connected Binary predicate over free-variable lists returning `true` when two equations should be squeezed.
 * @endinternal
 */
// Merges equations in eqs[0..primary_end) that are "connected" per predicate.
// primary_end and total_end are updated in-place as entries are removed.
template<NodeType node>
void squeeze_connected_eqs(trefs& eqs, size_t& primary_end, size_t& total_end,
	auto&& connected) {
	for (size_t i = 0; i + 1 < primary_end; ++i) {
		const trefs& fv1 = get_free_vars<node>(eqs[i]);
		if (fv1.empty()) continue;
		for (size_t j = i + 1; j < primary_end; ++j) {
			const trefs& fv2 = get_free_vars<node>(eqs[j]);
			if (fv2.empty()) continue;
			if (connected(fv1, fv2)) {
				eqs[i] = squeeze<node>(eqs[i], eqs[j]);
				eqs.erase(eqs.begin() + j);
				--j; --primary_end; --total_end;
			}
		}
	}
}

/**
 * @internal
 * @brief Folds equations `eqs[0..eq_end)` using `build_f`, marks the intermediate result in `mark` to prevent revisiting, then folds in remaining equations `eqs[eq_end..size)`.
 * @tparam node Tree node type.
 * @param eqs Vector of equation nodes to fold.
 * @param eq_end Index past the primary equations to fold and mark.
 * @param mark Set of already-visited nodes; updated with the intermediate result.
 * @param build_f Binary combinator (e.g. `build_wff_and` or `build_wff_or`) used to fold equations.
 * @return Combined formula node.
 * @endinternal
 */
// Folds eqs[0..eq_end) with build_f, marks the equation subresult to prevent
// revisiting, then folds in eqs[eq_end..size) and returns the combined node.
template<NodeType node>
tref build_and_mark_result(trefs& eqs, size_t eq_end,
	subtree_unordered_set<node>& mark, auto&& build_f) {
	using tau = tree<node>;
	tref n = eqs[0];
	for (size_t i = 1; i < eq_end; ++i)
		n = build_f(n, eqs[i]);
	mark.insert(n);
	auto cr = tau::get(n).children();
	mark.insert(cr.begin(), cr.end());
	for (size_t i = eq_end; i < eqs.size(); ++i)
		n = build_f(n, eqs[i]);
	return n;
}

/**
 * @internal
 * @brief The procedure collects all = 0 and != 0 equations within scope
 * (conjunction and disjunction respectively) and squeezes
 * those together that share at least one variable. Each squeezed formula A is
 * then integrated into terms in scope that share at least 2 variables including
 * the provided one in the following way:
 * - if A is = 0 and given f = 0, we produce f|A = 0
 * - if A is = 0 and given f != 0, we produce f & A' != 0
 * - if A is != 0 and given f != 0, we produce f|A != 0
 * - if A is != 0 and given f = 0, we produce f & A' = 0
 * @tparam node Tree node type
 * @param formula The formula to apply the procedure to
 * @return The mutated formula
 *
 * @par Example
 * @code{.cpp}
 * // See tests/integration/test_integration-wff_normalization.cpp:116-121.
 * tref fm = get_nso_rr(
 *     "o1[4] = o1[3] && o2[0] != 0 && (o1[3] != o1[1] || o2[2] != 0 || "
 *     "o2[1] = 0) && (o2[1] != 0 || o2[2] = 0) && (o2[2] = 0 || "
 *     "o2[3] = 0 && o1[4] != o1[2]).").value().main->get();
 * tref res = squeeze_absorb<node_t>(fm);
 * // tau::get(res).to_str() ==
 * //   "o2[0]:tau != 0 && o1[4]:tau = o1[3]:tau && "
 * //   "(o2[2]:tau != 0 || o2[1]:tau = 0 || o1[3]:tau != o1[1]:tau) && "
 * //   "(o2[1]:tau != 0 || o2[2]:tau = 0) && "
 * //   "(o2[2]:tau = 0 || o2[3]:tau = 0 && o1[4]:tau != o1[2]:tau)"
 * @endcode
 * @endinternal
 */
template<NodeType node>
tref squeeze_absorb(tref formula) {
	using tau = tree<node>;
	DBG(LOG_DEBUG << "Started squeeze_absorb");
	std::vector<trefs> assms { trefs {} };
	std::vector<trefs> dual_assms { trefs {} };
	subtree_unordered_set<node> mark;
	auto uf_comp = [](tref l, tref r) {
		return tau::subtree_less(l, r);
	};
	// Handles one conjunction/disjunction branch inside f.
	// uf_type: the eq type to union-find (bf_eq for conjs, bf_neq for disjs).
	// other_type: the complementary eq type.
	// pre_stk: assumption stack applied before the update step (dual of update_stk).
	// update_stk: assumption stack updated from primary equations.
	// update_is_dual: dual flag for update_stk; pre_stk is applied with !update_is_dual.
	// build_f: binary combinator (build_wff_and or build_wff_or).
	// Returns nullopt when no equation of interest is found.
	auto process_eq_branch = [&](trefs clauses, size_t uf_type, size_t other_type,
		std::vector<trefs>& pre_stk, std::vector<trefs>& update_stk,
		bool update_is_dual, auto&& build_f) -> std::optional<tref> {
		size_t eq_idx = 0;
		auto uf = union_find_with_sets<decltype(uf_comp), node>(uf_comp);
		for (tref& cl : clauses) {
			const tau& cl_t = tau::get(cl);
			if (cl_t.child_is(other_type) && cl_t[0][1].equals_0()) {
				std::swap(cl, clauses[eq_idx++]);
			} else if (cl_t.child_is(uf_type) && cl_t[0][1].equals_0()) {
				const trefs& fv = get_free_vars<node>(cl);
				for (tref v : fv) uf.merge(fv[0], v);
				std::swap(cl, clauses[eq_idx++]);
			}
		}
		if (eq_idx == 0) return std::nullopt;
		DBG(LOG_TRACE << (update_is_dual ? "Disjuncts" : "Conjuncts") << " with eqs first: ";
			for (tref el : clauses) LOG_TRACE << tau::get(el) << ", ";)
		for (size_t i = 0; i < eq_idx; ++i)
			clauses[i] = apply_assms<node>(clauses[i], pre_stk, !update_is_dual);
		size_t primary_idx = 0;
		for (size_t i = 0; i < eq_idx; ++i)
			if (tau::get(clauses[i]).child_is(uf_type))
				std::swap(clauses[i], clauses[primary_idx++]);
		squeeze_connected_eqs<node>(clauses, primary_idx, eq_idx,
			[&uf](const trefs& fv1, const trefs& fv2) {
				return uf.connected(fv1[0], fv2[0]);
			});
		uf.clear();
		trefs additions;
		for (size_t i = 0; i < primary_idx; ++i)
			clauses[i] = apply_assms<node>(clauses[i], update_stk, uf, additions,
				update_is_dual);
		update_assms<node>(update_stk, uf, additions);
		for (size_t i = primary_idx; i < eq_idx; ++i)
			clauses[i] = apply_assms<node>(clauses[i], update_stk, update_is_dual);
		return build_and_mark_result<node>(clauses, eq_idx, mark,
			std::forward<decltype(build_f)>(build_f));
	};
	auto f = [&](tref n, tref parent) {
		if (!tau::get(n).is(tau::wff)) return n;
		const tau& cn = tau::get(n)[0];
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!cn.is(tau::wff_or)) {
				// Push new assumption to stack
				assms.push_back(assms.back());
			}
		} else if (parent != nullptr && is<node>(parent, tau::wff_and)) {
			if (!cn.is(tau::wff_and)) {
				// Push new assumption to stack
				dual_assms.push_back(dual_assms.back());
			}
		}
		// Try getting conjunctions: = 0 are uf_type (updated), != 0 are other_type.
		if (trefs conjs = get_cnf_wff_clauses<node>(n); conjs.size() > 1) {
			if (auto r = process_eq_branch(std::move(conjs),
					tau::bf_eq, tau::bf_neq, dual_assms, assms, false,
					[](tref l, tref r) { return tau::build_wff_and(l, r); }))
				return *r;
			return n;
		// Try getting disjunctions: != 0 are uf_type (updated), = 0 are other_type.
		} else if (trefs disjs = get_dnf_wff_clauses<node>(n); disjs.size() > 1) {
			if (auto r = process_eq_branch(std::move(disjs),
					tau::bf_neq, tau::bf_eq, assms, dual_assms, true,
					[](tref l, tref r) { return tau::build_wff_or(l, r); }))
				return *r;
			return n;
		} else {
			// No disjuncts or conjuncts present
			// Simply apply dual assumptions and assumptions
			return n;
		}
	};
	auto up = [&](tref n, tref parent) {
		if (!is<node>(n, tau::wff)) return n;
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!is_child<node>(n, tau::wff_or)) {
				assms.pop_back();
			}
		} else if (parent != nullptr && is<node>(parent, tau::wff_and)) {
			if (!is_child<node>(n, tau::wff_and)) {
				dual_assms.pop_back();
			}
		}
		return n;
	};
	auto visit = [&mark](tref n) {
		if (is_quantifier<node>(n)) return false;
		if (is_temporal_quantifier<node>(n)) return false;
		if (mark.contains(n)) return false;
		return is_formula<node>(n);
	};
	// Disable intermediate simplifications for the moment
	tau::use_hooks = false;
	tref res = pre_order<node>(formula).apply(f, visit, up);
	DBG(assert(assms.size() == 1);)
	DBG(assert(dual_assms.size() == 1);)
	// Re-enable intermediate simplifications
	tau::use_hooks = true;
	DBG(LOG_DEBUG << "Ended squeeze_absorb");
	return tau::reget(res);
}

/**
 * @internal
 * @brief The procedure collects all = 0 and != 0 equations within scope
 * (conjunction and disjunction respectively) and squeezes
 * those together that share var. Each squeezed formula A is
 * then integrated into terms in scope that share at least 2 variables including
 * var in the following way:
 * - if A is = 0 and given f = 0, we produce f|A = 0
 * - if A is = 0 and given f != 0, we produce f & A' != 0
 * - if A is != 0 and given f != 0, we produce f|A != 0
 * - if A is != 0 and given f = 0, we produce f & A' = 0
 * @tparam node Tree node type
 * @param formula The formula to apply the procedure to
 * @param var The shared variable for squeeze/absorb
 * @return The mutated formula
 *
 * @par Example
 * @code{.cpp}
 * // See tests/integration/test_integration-wff_normalization.cpp:80-84.
 * tref fm = get_nso_rr(
 *     "(((xy) = 0 || f(x)) && (xy)' = 0) || w = 0.").value().main->get();
 * tref res = squeeze_absorb<node_t>(fm, tau::build_variable("x", 1));
 * // tau::get(res).to_str() == "w = 0 || (xy)' = 0 && f(x)"
 * @endcode
 * @endinternal
*/
template<NodeType node>
tref squeeze_absorb(tref formula, tref var) {
	using tau = tree<node>;
	DBG(LOG_DEBUG << "Started squeeze_absorb on var:" << tau::get(var));
	trefs assms {tau::_0(find_ba_type<node>(var))};
	trefs dual_assms {tau::_0(find_ba_type<node>(var))};
	subtree_unordered_set<node> mark;
	// Handles one conjunction/disjunction branch inside f.
	// primary_type: sort this eq type to front within eqs (squeezed, updated).
	// pre_assm: single assumption applied before the update step (dual of update_assm).
	// update_assm: single assumption updated from primary equations (modified in place).
	// update_is_dual: dual flag for update_assm; pre_assm is applied with !update_is_dual.
	// build_f: binary combinator (build_wff_and or build_wff_or).
	// Returns nullopt when no equation of interest is found.
	auto process_eq_branch_var = [&](trefs clauses, size_t primary_type,
		tref& pre_assm, tref& update_assm, bool update_is_dual,
		auto&& build_f) -> std::optional<tref> {
		size_t eq_idx = 0;
		for (tref& cl : clauses) {
			const tau& cl_t = tau::get(cl);
			if ((cl_t.child_is(tau::bf_neq) || cl_t.child_is(tau::bf_eq))
					&& cl_t[0][1].equals_0())
				std::swap(cl, clauses[eq_idx++]);
		}
		if (eq_idx == 0) return std::nullopt;
		DBG(LOG_TRACE << (update_is_dual ? "Disjuncts" : "Conjuncts") << " with eqs first: ";
			for (tref el : clauses) LOG_TRACE << tau::get(el) << ", ";)
		for (size_t i = 0; i < eq_idx; ++i)
			clauses[i] = apply_assms<node>(clauses[i], pre_assm, var, !update_is_dual);
		size_t primary_idx = 0;
		for (size_t i = 0; i < eq_idx; ++i)
			if (tau::get(clauses[i]).child_is(primary_type))
				std::swap(clauses[i], clauses[primary_idx++]);
		squeeze_connected_eqs<node>(clauses, primary_idx, eq_idx,
			[&var](const trefs& fv1, const trefs& fv2) {
				return std::binary_search(fv1.begin(), fv1.end(),
						var, tau::subtree_less)
				    && std::binary_search(fv2.begin(), fv2.end(),
						var, tau::subtree_less);
			});
		trefs updates;
		for (size_t i = 0; i < primary_idx; ++i)
			clauses[i] = apply_assms<node>(clauses[i], update_assm, var, updates,
				update_is_dual);
		for (tref upd : updates)
			update_assm = tau::build_bf_or(update_assm, tau::trim2(upd));
		for (size_t i = primary_idx; i < eq_idx; ++i)
			clauses[i] = apply_assms<node>(clauses[i], update_assm, var, update_is_dual);
		return build_and_mark_result<node>(clauses, eq_idx, mark,
			std::forward<decltype(build_f)>(build_f));
	};
	auto f = [&](tref n, tref parent) {
		if (!tau::get(n).is(tau::wff)) return n;
		const tau& cn = tau::get(n)[0];
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!cn.is(tau::wff_or)) {
				// Push new assumption to stack
				assms.push_back(assms.back());
			}
		} else if (parent != nullptr && is<node>(parent, tau::wff_and)) {
			if (!cn.is(tau::wff_and)) {
				// Push new assumption to stack
				dual_assms.push_back(dual_assms.back());
			}
		}
		// Try getting conjunctions: = 0 are primary (updated), != 0 are secondary.
		if (trefs conjs = get_cnf_wff_clauses<node>(n); conjs.size() > 1) {
			if (auto r = process_eq_branch_var(std::move(conjs), tau::bf_eq,
					dual_assms.back(), assms.back(), false,
					[](tref l, tref r) { return tau::build_wff_and(l, r); }))
				return *r;
			return n;
		// Try getting disjunctions: != 0 are primary (updated), = 0 are secondary.
		} else if (trefs disjs = get_dnf_wff_clauses<node>(n); disjs.size() > 1) {
			if (auto r = process_eq_branch_var(std::move(disjs), tau::bf_neq,
					assms.back(), dual_assms.back(), true,
					[](tref l, tref r) { return tau::build_wff_or(l, r); }))
				return *r;
			return n;
		} else {
			// No disjuncts or conjuncts present
			// Simply apply dual assumptions and assumptions
			return n;
		}
	};
	auto up = [&](tref n, tref parent) {
		if (!is<node>(n, tau::wff)) return n;
		if (parent != nullptr && is<node>(parent, tau::wff_or)) {
			if (!is_child<node>(n, tau::wff_or)) {
				assms.pop_back();
			}
		} else if (parent != nullptr && is<node>(parent, tau::wff_and)) {
			if (!is_child<node>(n, tau::wff_and)) {
				dual_assms.pop_back();
			}
		}
		return n;
	};
	auto visit = [&mark](tref n) {
		if (is_quantifier<node>(n)) return false;
		if (is_temporal_quantifier<node>(n)) return false;
		if (mark.contains(n)) return false;
		return is_formula<node>(n);
	};
	// Disable intermediate simplifications for the moment
	tau::use_hooks = false;
	tref res = pre_order<node>(formula).apply(f, visit, up);
	DBG(assert(assms.size() == 1);)
	DBG(assert(dual_assms.size() == 1);)
	// Re-enable intermediate simplifications
	tau::use_hooks = true;
	DBG(LOG_DEBUG << "Ended squeeze_absorb");
	return tau::reget(res);
}

/**
 * @internal
 * @brief Do a single Boole decomposition step on term given the provided variable.
 * Assumes that the decomposition is valid.
 * @tparam node Tree node type
 * @param term The term on which to apply Boole decomposition step
 * @param var The variable on which to do Boole decomposition
 * @return The resulting term
 * @endinternal
 */
template<NodeType node>
tref term_boole_decomposition(tref term, tref var) {
	using tau = tree<node>;
	DBG(assert(tau::get(var).is(tau::variable));)
	if (tau::get(term).find_top(is_non_boolean_term<node>)) {
		DBG(LOG_TRACE << "term_boole_decomposition/Non boolean term: "
			<< tau::get(term) << "\n");
		return term;
	}
	var = tau::get(tau::bf, var);
	tref p1 = tau::get(term).replace(var, tau::_1(find_ba_type<node>(var)));
	// Ensure early detection of F
	p1 = syntactic_path_simplification_unsat_on_unchanged_negations<node>(p1);
	tref p2 = tau::get(term).replace(var, tau::_0(find_ba_type<node>(var)));
	// Ensure early detection of F
	p2 = syntactic_path_simplification_unsat_on_unchanged_negations<node>(p2);
	if (tau::get(p1) == tau::get(p2)) return p1;
	// Build Boole decomposition step
	return tau::build_bf_or(
		tau::build_bf_and(var, p1), tau::build_bf_and(tau::build_bf_neg(var), p2)
	);
}

// Note: Recursion depth is bound by the number of variables, which should
// prevent a stack overflow due to tree size in all use cases
/**
 * @internal
 * @brief Recursively do Boole decomposition on term using the provided variables
 * starting at idx.
 * Assumes that the decomposition is valid for all provided variables.
 * @tparam node Tree node type
 * @param term Term on which to do Boole decomposition
 * @param vars The variables to do Boole decomposition on
 * @param idx The current variable index
 * @return The resulting Boole decomposition
 * @endinternal
 */
template<NodeType node>
tref rec_term_boole_decomposition(tref term, const trefs& vars, const int_t idx,
	const bool free_funcs = false) {
	using tau = tree<node>;
	DBG(LOG_TRACE << "Step on " << LOG_FM(term) << "\n";)
	if (tau::get(term).equals_0()) return term;
	if (tau::get(term).equals_1()) return term;
	if (idx == (int_t)vars.size()) {
		if (!free_funcs) {
			term = normalize_ba<node>(term);
			auto func_syms = tau::get(term).select_top(is<node, tau::bf_ref>);
			if (!func_syms.empty()) {
				std::ranges::sort(func_syms, tau::subtree_less);
				term = rec_term_boole_decomposition<node>(term, func_syms, 0, true);
			}
		}
		DBG(LOG_TRACE << "Result: " << LOG_FM(term) << "\n";)
		return term;
	}
	DBG(assert(tau::get(vars[idx]).is(tau::variable) || tau::get(vars[idx]).is(tau::bf_ref));)
	tref p1 = tau::get(term).replace(vars[idx], tau::_1_trimmed(find_ba_type<node>(vars[idx])));
	// Ensure early detection of F
	p1 = syntactic_path_simplification_unsat_on_unchanged_negations<node>(p1);
	tref p2 = tau::get(term).replace(vars[idx], tau::_0_trimmed(find_ba_type<node>(vars[idx])));
	// Ensure early detection of F
	p2 = syntactic_path_simplification_unsat_on_unchanged_negations<node>(p2);
	if (tau::get(p1) == tau::get(p2)) {
		DBG(LOG_TRACE << "Result: " << LOG_FM(p1) << "\n";)
		return rec_term_boole_decomposition<node>(p1, vars, idx + 1);
	}
	p1 = rec_term_boole_decomposition<node>(p1, vars, idx + 1);
	p2 = rec_term_boole_decomposition<node>(p2, vars, idx + 1);
	if (tau::get(p1) == tau::get(p2)) {
		DBG(LOG_TRACE << "Result: " << LOG_FM(p1) << "\n";)
		return p1;
	}
	tref var = tau::get(tau::bf, vars[idx]);
	// Build Boole decomposition
	if (tau::get(p1).equals_1())
		term = tau::build_bf_or(var,  p2);
	else if (tau::get(p2).equals_1())
		term = tau::build_bf_or(p1, tau::build_bf_neg(var));
	else term = tau::build_bf_or(tau::build_bf_and(var, p1),
		tau::build_bf_and(tau::build_bf_neg(var), p2));
	DBG(LOG_TRACE << "Result: " << LOG_FM(term) << "\n";)
	return term;
}

/**
 * @internal
 * @brief Convert term to Boole normal form. Also treats normalization of
 * encountered tau constants.
 * @tparam node Tree node type
 * @param term The term to do the Boole decomposition on
 * @return The resulting Boole decomposition
 * @endinternal
 */
template<NodeType node>
tref term_boole_decomposition(tref term) {
	using tau = tree<node>;

	DBG(LOG_DEBUG << "Term_boole_decomposition on " << LOG_FM(term) << "\n";)
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(term); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	if (tau::get(term).find_top(is_non_boolean_term<node>)) {
		DBG(LOG_TRACE << "term_boole_decomposition/Non boolean term: "
			<< tau::get(term) << "\n");
		[[maybe_unused]] const tref orig = term;
		term = node::ba::simplify_term(term);
		tref res = normalize_ba<node>(term);
		// Cache this branch too: it is the bv/tau-constant path, i.e. the
		// expensive one (simplify_term + normalize_ba), and callers hit it
		// repeatedly through apply_unique_until_change.
#ifdef TAU_CACHE
		cache.emplace(res, res);
		return cache.emplace(orig, res).first->second;
#endif // TAU_CACHE
		return res;
	}
	// Simple cases
	if (tau::get(term).equals_0() || tau::get(term).equals_1())
		return term;
	tref bd = push_negation_in<node, false>(term);
	auto vars = get_free_vars_appearance_order<node>(bd);
	// No free var, so no boole decomposition step
	if (vars.empty()) {
		term = node::ba::simplify_term(term);
		bd = normalize_ba<node>(bd);
		auto func_syms = tau::get(bd).select_top(is<node, tau::bf_ref>);
		std::ranges::sort(func_syms, tau::subtree_less);
		bd = rec_term_boole_decomposition<node>(bd, func_syms, 0);
#ifdef TAU_CACHE
		cache.emplace(bd, bd);
		return cache.emplace(term, bd).first->second;
#endif // TAU_CACHE
		return bd;
	}
	std::ranges::stable_sort(vars, variable_order_for_simplification<node>);
	bd = rec_term_boole_decomposition<node>(bd, vars, 0);
	DBG(LOG_DEBUG << "Term_boole_decomposition result: " << LOG_FM(bd) << "\n";)
#ifdef TAU_CACHE
	cache.emplace(bd, bd);
	return cache.emplace(term, bd).first->second;
#endif // TAU_CACHE
	return bd;
}

// Note: Recursion depth is bound by the number of variables, which should
// prevent a stack overflow due to tree size in all use cases
/**
 * @internal
 * @brief Recursively do Boole decomposition on formula using the provided variables
 * starting at idx.
 * Assumes that the decomposition is valid for all provided variables.
 * @tparam node Tree node type
 * @param formula The formula to do Boole decomposition on
 * @param vars The variable to perform the Boole decomposition on
 * @param idx The current variable index
 * @return The resulting Boole decomposition
 * @endinternal
 */
template<NodeType node>
tref rec_boole_decomposition(tref formula, const trefs& vars, const int_t idx) {
	using tau = tree<node>;
	DBG(LOG_TRACE << "Step on " << LOG_FM(formula) << "\n";)
	if (tau::get(formula).equals_F()) return formula;
	if (tau::get(formula).equals_T()) return formula;
	if (idx == (int_t)vars.size()) {
		DBG(LOG_TRACE << "Result: " << LOG_FM(formula) << "\n";)
		return formula;
	}
	DBG(assert(tau::get(vars[idx]).child_is(tau::bf_eq));)
	tref p1 = tau::get(formula).replace(vars[idx], tau::_T());
	// Ensure early detection of F
	p1 = syntactic_path_simplification_unsat_on_unchanged_negations<node>(p1);
	tref p2 = tau::get(formula).replace(vars[idx], tau::_F());
	// Ensure early detection of F
	p2 = syntactic_path_simplification_unsat_on_unchanged_negations<node>(p2);
	if (tau::get(p1) == tau::get(p2)) {
		DBG(LOG_TRACE << "Result: " << LOG_FM(p1) << "\n";)
		return rec_boole_decomposition<node>(p1, vars, idx + 1);
	}
	p1 = rec_boole_decomposition<node>(p1, vars, idx + 1);
	p2 = rec_boole_decomposition<node>(p2, vars, idx + 1);
	if (tau::get(p1) == tau::get(p2)) {
		DBG(LOG_TRACE << "Result: " << LOG_FM(p1) << "\n";)
		return p1;
	}
	// Build Boole decomposition
	if (tau::get(p1).equals_T())
		formula = tau::build_wff_or(vars[idx], p2);
	else if (tau::get(p2).equals_T())
		formula = tau::build_wff_or(p1, tau::build_wff_neg(vars[idx]));
	else formula = tau::build_wff_or(tau::build_wff_and(vars[idx], p1),
		tau::build_wff_and(tau::build_wff_neg(vars[idx]), p2));
	DBG(LOG_TRACE << "Result: " << LOG_FM(formula) << "\n";)
	return formula;
}

/**
 * @internal
 * This procedure converts the formula to Boole normal form. It also converts all
 * terms to Boole normal form.
 * @tparam node Tree node type
 * @param bnf The formula to convert to Boole normal form
 * @return The resulting Boole normal form
 * @endinternal
 */
template<NodeType node>
tref boole_normal_form(tref formula) {
	using tau = tree<node>;
	DBG(LOG_DEBUG << "Boole_normal_form on " << LOG_FM(formula) << "\n";)
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(formula); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	if (tau::get(formula).equals_T() || tau::get(formula).equals_F())
		return formula;
	// Step 1: Syntactically simplify formula
	tref bnf = syntactic_formula_simplification<node>(formula);
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(bnf) << "\n";)
	// Squeeze and absorb for additional simplifications during term normalization
	// -> causes mayor blow ups
	bnf = squeeze_absorb<node>(bnf);
	// Step 2: Traverse formula, simplify all encountered equations
	auto simp_eqs = [](tref n) {
		if (tau::get(n).child_is(tau::bf_eq)) {
			if (tau::get(n).equals_T() || tau::get(n).equals_F())
				return n;
			tref c1 = tau::get(n)[0].first();
			tref c2 = tau::get(n)[0].second();
			// Apply Boole decomposition
			c1 = term_boole_decomposition<node>(c1);
			c2 = term_boole_decomposition<node>(c2);
			return tau::build_bf_eq(c1, c2);
		} else if (tau::get(n).child_is(tau::bf_neq)) {
			if (tau::get(n).equals_T() || tau::get(n).equals_F())
				return n;
			tref c1 = tau::get(n)[0].first();
			tref c2 = tau::get(n)[0].second();
			// Apply Boole decomposition
			c1 = term_boole_decomposition<node>(c1);
			c2 = term_boole_decomposition<node>(c2);
			return tau::build_bf_neq(c1, c2);
		}
		return n;
	};
	bnf = pre_order<node>(bnf).apply_unique_until_change(simp_eqs, while_is_formula<node>);
	DBG(LOG_DEBUG << "After term_boole_decomposition: " << LOG_FM(bnf) << "\n";)
	// Step 3: Syntactically simplify resulting formula again after normalization of terms
	bnf = syntactic_formula_simplification<node>(bnf);
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(bnf) << "\n";)
	// Step 4: Convert formula to Boole normal form
	// First get atomic formulas without !=
	auto is_atomic = [](tref n) {
		if (!tau::get(n).is(tau::wff)) return false;
		const tau& c = tau::get(n)[0];
		switch (c.value.nt) {
			case tau::bf_eq:
			case tau::bf_lt:
			case tau::bf_lteq: return true;
			default: return false;
		}
	};
	tref eq_bnf = normalize_atomic_formula_operators<node>(bnf);
	trefs atms = rewriter::select_top_until<node>(eq_bnf,
		is_atomic, is_quantifier<node>);
	// No variables for Boole decomposition
	if (atms.empty()) {
#ifdef TAU_CACHE
		cache.emplace(bnf, bnf);
		return cache.emplace(formula, bnf).first->second;
#endif // TAU_CACHE
		return bnf;
	}
	// Sort the BDD variables
	std::ranges::stable_sort(atms, atm_formula_order_for_simplification<node>);
	// Apply Boole decomposition
	eq_bnf = rec_boole_decomposition<node>(eq_bnf, atms, 0);
	// Convert !(=) to != again
	eq_bnf = to_nnf<node>(eq_bnf);
	eq_bnf = simplify_using_equality<node>(eq_bnf);
	DBG(LOG_DEBUG << "Boole_normal_form result: " << LOG_FM(eq_bnf) << "\n";)
#ifdef TAU_CACHE
	cache.emplace(eq_bnf, eq_bnf);
	return cache.emplace(formula, eq_bnf).first->second;
#endif // TAU_CACHE
	return eq_bnf;
}

/** @internal @copydoc term_boole_normal_form @endinternal */
template<NodeType node>
tref term_boole_normal_form(tref formula) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(formula); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	if (tau::get(formula).equals_T() || tau::get(formula).equals_F())
		return formula;
	// Step 1: Syntactically simplify formula
	tref tbnf = syntactic_formula_simplification<node>(formula);
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(tbnf) << "\n";)
	auto simp_eqs = [](tref n) {
		if (tau::get(n).child_is(tau::bf_eq)) {
			if (tau::get(n).equals_T() || tau::get(n).equals_F())
				return n;
			tref c1 = tau::get(n)[0].first();
			tref c2 = tau::get(n)[0].second();
			// Apply Boole decomposition
			c1 = term_boole_decomposition<node>(c1);
			c2 = term_boole_decomposition<node>(c2);
			return tau::build_bf_eq(c1, c2);
		} else if (tau::get(n).child_is(tau::bf_neq)) {
			if (tau::get(n).equals_T() || tau::get(n).equals_F())
				return n;
			tref c1 = tau::get(n)[0].first();
			tref c2 = tau::get(n)[0].second();
			// Apply Boole decomposition
			c1 = term_boole_decomposition<node>(c1);
			c2 = term_boole_decomposition<node>(c2);
			return tau::build_bf_neq(c1, c2);
		}
		return n;
	};
	tbnf = pre_order<node>(tbnf).apply_unique_until_change(simp_eqs, while_is_formula<node>);
	DBG(LOG_DEBUG << "After term_boole_decomposition: " << LOG_FM(tbnf) << "\n";)
	// Step 3: Syntactically simplify resulting formula again after normalization of terms
	tbnf = syntactic_formula_simplification<node>(tbnf);
	DBG(LOG_DEBUG << "After syntactic_formula_simplification: " << LOG_FM(tbnf) << "\n";)
#ifdef TAU_CACHE
	cache.emplace(tbnf, tbnf);
	return cache.emplace(formula, tbnf).first->second;
#endif // TAU_CACHE
	return tbnf;
}

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "to_snf"


/** @internal @copydoc anf @endinternal */
template <NodeType node, size_t type>
tref anf(tref n) {
	// TODO (MEDIUM) write anf (using?)
	// Diagnostics go through the logging channel, not std::cout: this is a
	// library entry point and a caller redirecting stdout must not receive
	// stray text. The input is returned unchanged (see the @warning on the
	// declaration in normal_forms.h).
	LOG_ERROR << "anf is not implemented yet; returning the input unchanged";
	return n;
}

/** @internal @copydoc pnf @endinternal */
template <NodeType node>
tref pnf(tref n) {
	// TODO (MEDIUM) write pnf (using?)
	LOG_ERROR << "pnf is not implemented yet; returning the input unchanged";
	return n;
}

} // namespace idni::tau_lang
